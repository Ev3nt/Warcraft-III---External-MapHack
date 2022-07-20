#include <Windows.h>
#include <iostream>
#include <math.h>
#include <ostream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <Psapi.h>

#include "protect.h"

HWND g_hWnd = NULL;
HWND g_hWarcraftWnd = FindWindow(0, "Warcraft III");
DWORD g_game_dll_base;
HANDLE g_hProcess;

LPDIRECT3D9 g_pd3d = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS g_d3dpp;
LPD3DXLINE g_line;

RECT Form;
float screen_width;
float screen_height;

D3DXMATRIXA16 view_matrix_temp;
D3DXMATRIXA16 view_matrix;

enum offsets {
	unit_list_offset = 0xAB428C,
	unit_between_offset = 0x310,
	unit_table_limit_offset = 0x31FEF,
	unit_flag_is_unit = 0xB8,
	unit_position_offset = 0x288,

	game_ui_offset = 0xACE66C,

	game_matrix_offset = 0xAAE790,
	
	game_matrix_p = 0x850		// 890, 8A0, 8B0
};

BOOL D3DInit(HWND hWnd);
VOID Render();

VOID DrawLine(float x1, float y1, float x2, float y2, float thickness, D3DCOLOR color);
VOID DrawLine(D3DXVECTOR2 src, D3DXVECTOR2 dst, float thickness, D3DCOLOR color);

VOID DrawEspBox(D3DXVECTOR2 top, D3DXVECTOR2 bot, float thickness, D3DCOLOR color);

D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3 pos);

HANDLE GetProcessHandle();
BOOL ReadBytes(LPVOID addr, int num, LPVOID buf);

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

const char* key = "11070123112E8880181D0381091D0381171D0381081D038111C3CC546C1114CF4953D7D48"; // Ev3nt
//const char* key = "1117013311450600135D6354035D5354119F9C746121119C53192DDA4D"; // SasukeMV

//--------------------------------------------------------------------------------------------------

BOOL APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpCmdLine, BOOL nCmdShow)
{
	FILE* console;
	AllocConsole();
	freopen_s(&console, "CONOUT$", "w", stdout);

	// if (!IsValid(key) || !IsValidTime(2021, 5, 4))
	// 	return FALSE;

	WNDCLASSEX dummy_class;
	dummy_class.cbSize = sizeof(dummy_class);
	dummy_class.cbClsExtra = NULL;
	dummy_class.cbWndExtra = NULL;
	dummy_class.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	dummy_class.hCursor = NULL;
	dummy_class.hIcon = NULL;
	dummy_class.hIconSm = NULL;
	dummy_class.hInstance = hInst;
	dummy_class.lpfnWndProc = WndProc;
	dummy_class.lpszClassName = "dummy";
	dummy_class.lpszMenuName = NULL;
	dummy_class.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&dummy_class);
	
	GetWindowRect(g_hWarcraftWnd, &Form);
	RECT rect;
	rect = { Form.left, Form.top, Form.right - Form.left, Form.bottom - Form.top };

	g_hWnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, dummy_class.lpszClassName, dummy_class.lpszClassName, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, (int)rect.right, (int)rect.bottom, NULL, NULL, dummy_class.hInstance, NULL);
	SetLayeredWindowAttributes(g_hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	if (D3DInit(g_hWnd) && (g_hProcess = GetProcessHandle()))
	{
		ShowWindow(g_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(g_hWnd);

		MSG msg;
		while (GetMessage(&msg, NULL, NULL, NULL))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			GetWindowRect(g_hWarcraftWnd, &Form);
			screen_width = (float)(Form.right - Form.left);
			screen_height = (float)(Form.bottom - Form.top);
			rect = { Form.left, Form.top, Form.right - Form.left, Form.bottom - Form.top };

			MoveWindow(g_hWnd, (int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom, TRUE);
		}
	}

	UnregisterClass(dummy_class.lpszClassName, dummy_class.hInstance);
	CloseHandle(g_hProcess);

	return FALSE;
}

//--------------------------------------------------------------------------------------------------

VOID Render()
{
	if (!g_pd3dDevice)
		return;

	g_pd3dDevice->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		if (GetForegroundWindow() == g_hWarcraftWnd)
		{
			int gameui;
			ReadBytes((LPVOID)(g_game_dll_base + game_ui_offset), 4, &gameui);

			if (gameui)
			{
				DWORD unit_list = g_game_dll_base + unit_list_offset;
				DWORD address;
				ReadBytes((LPVOID)(unit_list), 4, &address);

				while (unit_list)
				{
					DWORD flag;

					for (; ; address += unit_between_offset)
						if (ReadBytes((LPVOID)(address + unit_flag_is_unit), 4, &flag))
						{
							if (flag)
							{
								D3DXVECTOR3 position;
								D3DXVECTOR3 screen_position;
								ReadBytes((LPVOID)(address + unit_position_offset), 12, &position);
								position = { position.x, position.y, position.z };

								ReadBytes((LPVOID)(g_game_dll_base + game_matrix_offset), 4, &view_matrix_temp.m);
								ReadBytes((LPVOID)(*(DWORD*)&view_matrix_temp.m + game_matrix_p), 64, &view_matrix_temp.m);

								view_matrix = view_matrix_temp;
								view_matrix[0] = view_matrix_temp[0];
								view_matrix[0] = view_matrix_temp[0];
								view_matrix[1] = view_matrix_temp[4];
								view_matrix[2] = view_matrix_temp[8];
								view_matrix[3] = view_matrix_temp[12];
								view_matrix[4] = view_matrix_temp[1];
								view_matrix[5] = view_matrix_temp[5];
								view_matrix[6] = view_matrix_temp[9];
								view_matrix[7] = view_matrix_temp[13];
								view_matrix[8] = view_matrix_temp[2];
								view_matrix[9] = view_matrix_temp[6];
								view_matrix[10] = view_matrix_temp[10];
								view_matrix[11] = view_matrix_temp[14];
								view_matrix[12] = view_matrix_temp[3];
								view_matrix[13] = view_matrix_temp[7];
								view_matrix[14] = view_matrix_temp[11];
								view_matrix[15] = view_matrix[15];

								screen_position = WorldToScreen(position);

								if (screen_position.z >= 0.01f)
								{
									D3DXVECTOR2 top = { screen_position.x, screen_position.y };
									D3DXVECTOR2 bot = { screen_position.x, screen_position.y - 80 };

									//DrawLine(screen_width / 2, (float)(Form.bottom - Form.top), screen_position.x, screen_position.y, 5, 0XFF00FF00);
									DrawEspBox(top, bot, 5, 0xFFFF0000);
								}
							}
						}
						else
							break;

					ReadBytes((LPVOID)(unit_list), 4, &unit_list);
					address = unit_list;
				}
			}
		}

		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

VOID DrawLine(float x1, float y1, float x2, float y2, float thickness, D3DCOLOR color)
{
	if (!g_line)
		D3DXCreateLine(g_pd3dDevice, &g_line);


	D3DXVECTOR2 vLine[2];
	vLine[0] = D3DXVECTOR2(x1, y1);
	vLine[1] = D3DXVECTOR2(x2, y2);
	g_line->SetWidth(thickness);
	g_line->Draw(vLine, 2, color);
}

VOID DrawLine(D3DXVECTOR2 src, D3DXVECTOR2 dst, float thickness, D3DCOLOR color)
{
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}

VOID DrawEspBox(D3DXVECTOR2 top, D3DXVECTOR2 bot, float thickness, D3DCOLOR color)
{
	D3DXVECTOR2 tl, tr;
	tl.x = top.x - 20;
	tr.x = top.x + 20;
	tl.y = tr.y = top.y;

	D3DXVECTOR2 bl, br;
	bl.x = bot.x - 20;
	br.x = bot.x + 20;
	bl.y = br.y = bot.y;

	DrawLine(tl , tr, thickness, color);
	DrawLine(bl, br, thickness, color);
	DrawLine(tl, bl, thickness, color);
	DrawLine(tr, br, thickness, color);
}

D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3 pos)
{
	float _x = view_matrix[0] * pos.x + view_matrix[1] * pos.y + view_matrix[2] * pos.z + view_matrix[3] - 34.5f;
	float _y = view_matrix[4] * pos.x + view_matrix[5] * pos.y + view_matrix[6] * pos.z + view_matrix[7] + 80.f;

	float w = view_matrix[12] * pos.x + view_matrix[13] * pos.y + view_matrix[14] * pos.z + view_matrix[15];
	
	float x = _x / w;
	float y = _y / w / 1.35f;

	x = (screen_width / 2.f * x) + (x + screen_width / 2.f);
	y = -(screen_height / 2.f * y) + (y + screen_height / 2.f) - 50.f;
	
	return { x,y,w };
}

BOOL D3DInit(HWND hWnd)
{
	if (!(g_pd3d = Direct3DCreate9(D3D_SDK_VERSION)))
		return FALSE;

	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));

	g_d3dpp.BackBufferWidth = (UINT)Form.right;
	g_d3dpp.BackBufferHeight = (UINT)Form.bottom;
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.hDeviceWindow = hWnd;
	g_d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice)))
		return FALSE;

	return TRUE;
}

//--------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT:
		Render();
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, nMsg, wParam, lParam);
	}

	return 0;
}

BOOL ReadBytes(LPVOID addr, int num, LPVOID buf)
{
	if (!ReadProcessMemory(g_hProcess, addr, buf, num, NULL))
		return FALSE;

	return TRUE;
}

HANDLE GetProcessHandle()
{
	HANDLE handle = NULL;
	DWORD pid = 0;

	if (!g_hWarcraftWnd)
		return handle;

	GetWindowThreadProcessId(g_hWarcraftWnd, &pid);
	
	void* tokenHandle;
	TOKEN_PRIVILEGES privilegeToken;
	LookupPrivilegeValue(0, SE_DEBUG_NAME, &privilegeToken.Privileges[0].Luid);
	privilegeToken.PrivilegeCount = 1;
	privilegeToken.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenHandle);
	AdjustTokenPrivileges(tokenHandle, 0, &privilegeToken, sizeof(TOKEN_PRIVILEGES), 0, 0);
	CloseHandle(tokenHandle);

	handle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

	if (!handle)
		return handle;
	
	HMODULE hMods[1024];
	if (EnumProcessModules(handle, hMods, sizeof(hMods), &pid))
		for (unsigned int i = 0; i < (pid / sizeof(HMODULE)); i++)
		{
			char szModName[MAX_PATH];
			if (GetModuleFileNameEx(handle, hMods[i], szModName, sizeof(szModName) / sizeof(char)))
				if (strstr(szModName, "Game.dll"))
				{
					g_game_dll_base = (DWORD)hMods[i];

					break;
				}
		}

	return handle;
}
