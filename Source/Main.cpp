#include <Windows.h>
#include <iostream>
#include <math.h>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <Psapi.h>

typedef struct {
	float x;
	float y;
	float z;
} CameraPosition, * PCameraPosition;

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

CameraPosition camera;

enum offsets {
	unit_list_offset = 0xAB428C,
	unit_between_offset = 0x310,
	unit_table_limit_offset = 0x31FEF,
	unit_flag_is_unit = 0xB8,
	unit_position_offset = 0x288,

	camera_position_offset = 0xAB554C,
	camera_z_offset = 0xAAE794,

	game_ui_offset = 0xACE66C,

	game_matrix_offset = 0xAAE790,
	//game_matrix_offset = 0x7DBA1C,
	
	game_matrix_p = 0x850		// 890, 8A0, 8B0
	//game_matrix_p = 0xAC4
};

BOOL D3DInit(HWND hWnd);
VOID Render();

VOID DrawLine(float x1, float y1, float x2, float y2, float thickness, D3DCOLOR color);
VOID DrawLine(D3DXVECTOR2 src, D3DXVECTOR2 dst, float thickness, D3DCOLOR color);

VOID DrawEspBox(D3DXVECTOR2 top, D3DXVECTOR2 bot, float thickness, D3DCOLOR color);
int world_to_screen(float* from, float* to);

D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3 pos);

HANDLE GetProcessHandle();
BOOL ReadBytes(LPVOID addr, int num, LPVOID buf);

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

//--------------------------------------------------------------------------------------------------

BOOL APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpCmdLine, BOOL nCmdShow)
{
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
			/*Form.right -= 35;
			Form.bottom -= 150;*/
		}
	}

	UnregisterClass(dummy_class.lpszClassName, dummy_class.hInstance);
	CloseHandle(g_hProcess);

	return FALSE;
}

//--------------------------------------------------------------------------------------------------

VOID Render()
{
	if (g_pd3dDevice == NULL)
		return;

	g_pd3dDevice->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		int gameui;
		ReadBytes((LPVOID)(g_game_dll_base + game_ui_offset), 4, &gameui);

		if (gameui != 0)
		{
			DWORD unit_list = g_game_dll_base + unit_list_offset;
			DWORD address;
			ReadBytes((LPVOID)(unit_list), 4, &address);

			while (unit_list != 0)
			{
				DWORD flag;

				for (; ; address += unit_between_offset)
					if (ReadBytes((LPVOID)(address + unit_flag_is_unit), 4, &flag))
					{
						if (flag)
						{
							D3DXVECTOR3 position = { 0, 0, 0};
							D3DXVECTOR3 screen_position;
							ReadBytes((LPVOID)(address + unit_position_offset), 12, &position);
							//ReadBytes((LPVOID)(g_game_dll_base + camera_position_offset), 8, &camera);
							position = { position.x, position.y, position.z};

							ReadBytes((LPVOID)(g_game_dll_base + game_matrix_offset), 4, &view_matrix_temp.m);
							ReadBytes((LPVOID)(*(DWORD*)&view_matrix_temp.m + game_matrix_p), 64, &view_matrix_temp.m);
							//ReadBytes((LPVOID)((DWORD)0x10763550), 64, &view_matrix.m);

							view_matrix = view_matrix_temp;
							view_matrix[0] = view_matrix_temp[0];// *0.785f;
							view_matrix[0] = view_matrix_temp[0];
							view_matrix[1] = view_matrix_temp[4];
							view_matrix[2] = view_matrix_temp[8];
							view_matrix[3] = view_matrix_temp[12];// *0.784f;// - 25.19f;
							view_matrix[4] = view_matrix_temp[1];
							view_matrix[5] = view_matrix_temp[5];// *1.164f;
							view_matrix[6] = view_matrix_temp[9];// *1.165f;
							view_matrix[7] = view_matrix_temp[13];// *1.163f;// - 38.05f;
							view_matrix[8] = view_matrix_temp[2];
							view_matrix[9] = view_matrix_temp[6];// *0.965f;
							view_matrix[10] = view_matrix_temp[10];// *0.976f;
							view_matrix[11] = view_matrix_temp[14];// *1.07f;// + 104.71f;
							view_matrix[12] = view_matrix_temp[3];
							view_matrix[13] = view_matrix_temp[7];
							view_matrix[14] = view_matrix_temp[11];
							view_matrix[15] = view_matrix[15];

							

							/*view_matrix[0] -= 0.15f;
							view_matrix[5] += 0.30f;
							view_matrix[6] += 0.20f;
							view_matrix[9] -= 0.02f;
							view_matrix[10] += 0.02f;*/

							//world_to_screen(position, screen_position);
							screen_position = WorldToScreen(position);
							
							if (screen_position.z >= 0.01f)
							{
								D3DXVECTOR2 top = { screen_position.x, screen_position.y };
								D3DXVECTOR2 bot = { screen_position.x, screen_position.y - 100 };

								DrawLine(screen_width / 2, (float)(Form.bottom - Form.top), screen_position.x, screen_position.y, 5, 0XFF00FF00);
								DrawEspBox(top, bot, 5, 0xFFFF0000);
							}
							//DrawLine((Form.right - Form.left) / 2, Form.bottom - Form.top, screen_position.x - Form.left, screen_position.y - Form.top - 100, 5, 0XFF00FF00);
							/*D3DXVECTOR3 position;
							ReadBytes((LPVOID)(address + unit_position_offset), 12, &position);
							
							float fov = 2500.f;
							float coefficient_x = 0.74f;
							float coefficient_y = 0.425f;

							ReadBytes((LPVOID)(g_game_dll_base + camera_position_offset), 8, &camera);
							ReadBytes((LPVOID)(g_game_dll_base + camera_z_offset), 4, &camera.z);
							ReadBytes((LPVOID)(*(DWORD*)&camera.z + 0xE0), 4, &camera.z);
							RECT camerarect = { (LONG)(camera.x - fov / 2), (LONG)(camera.y + fov / 2), (LONG)(camera.x + fov / 2), (LONG)(camera.y - fov / 2) };

							if (!(position.x > camerarect.left && position.x < camerarect.right && position.y > camerarect.bottom && position.y < camerarect.top))
								continue;

							float zc = 1460.f;
							float zu = camera.z + position.z;
							float xc = camera.x;
							float xu = position.x;

							float x2 = (-xc + xu);

							float x = (x2 * zc) / zu;

							float yc = camera.y;
							float yu = position.y;

							float y2 = (yc - yu);

							float y = (y2 * (zc - 740)) / zu;

							DrawLine(Form.z / 2, Form.w / 2, x, y, 5, 0XFFFF0000);

							/*int height = 50;
							D3DXVECTOR2 hitbox = { Form.z / 2 + x, Form.w / 2 + y };*/

							/*D3DXVECTOR2 top = { Form.z / 2 + (-camera.x + position.x) * coefficient_x, Form.w / 2 - 100 + (camera.y - position.y) * coefficient_y };
							D3DXVECTOR2 bot = { Form.z / 2 + (-camera.x + position.x) * coefficient_x, Form.w / 2 - 100 + (camera.y - position.y) * coefficient_y + 100 };*/
							/*D3DXVECTOR2 top = { hitbox.x, hitbox.y };
							D3DXVECTOR2 bot = { hitbox.x, hitbox.y + 5 };
							DrawEspBox(top, bot, 1, 0xFF00FFFF);

							DrawLine(Form.z / 2, Form.w / 2, Form.z / 2 + 225, Form.w / 2, 3, 0xFFFF0000);

							ID3DXFont* d3dFont = 0;
							D3DXCreateFont(g_pd3dDevice, 25, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Bahnschrift", &d3dFont);

							char buffer[MAX_PATH] = { 0 };
							sprintf_s(buffer, "Camera X: %f\nCamera Y: %f\nCamera Z: %f", camera.x, camera.y, camera.z);

							RECT rect = { (LONG)(Form.z / 2), (LONG)(Form.w / 2), (LONG)Form.z, (LONG)Form.w };
							d3dFont->DrawText(0, buffer, sizeof(buffer), &rect, 0, D3DCOLOR_ARGB(255, 255, 0, 0));

							d3dFont->Release();*/
						}
					}
					else
						break;

				ReadBytes((LPVOID)(unit_list), 4, &unit_list);
				address = unit_list;
			}
		}

		/*ID3DXFont* d3dFont = 0;
		D3DXCreateFont(g_pd3dDevice, 50, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Bahnschrift", &d3dFont);

		RECT rect = { (LONG)Form.z / 2, (LONG)Form.w /2, (LONG)Form.z, (LONG)Form.w };
		//RECT rect = { (LONG)Form.x, (LONG)Form.y, (LONG)Form.z, (LONG)Form.w };

		char buffer[MAX_PATH] = { 0 };

		float x;
		float y;
		if (ReadBytes((LPVOID)(g_game_dll_base + 0x00AB554C), 4, &x))
			sprintf_s(buffer, "Camera X: %f", x);
		else
			sprintf_s(buffer, "readprocessmemory failed. %08X\n", GetLastError());

		d3dFont->DrawText(0, buffer, sizeof(buffer), &rect, 0, D3DCOLOR_ARGB(255, 255, 0, 0));

		if (ReadBytes((LPVOID)(g_game_dll_base + 0x00AB5550), 4, &y))
			sprintf_s(buffer, "Camera Y: %f", y);
		else
			sprintf_s(buffer, "readprocessmemory failed. %08X\n", GetLastError());

		rect.top += 50;

		d3dFont->DrawText(0, buffer, sizeof(buffer), &rect, 0, D3DCOLOR_ARGB(255, 255, 0, 0));

		g_pd3dDevice->EndScene();

		d3dFont->Release();*/
	}
	g_pd3dDevice->EndScene();

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

/*D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3 pos)
{
	float _x = view_matrix[0] * pos.x + view_matrix[1] * pos.y + view_matrix[2] * pos.z + view_matrix[3];
	float _y = view_matrix[4] * pos.x + view_matrix[5] * pos.y + view_matrix[6] * pos.z + view_matrix[7];

	float w = view_matrix[12] * pos.x + view_matrix[13] * pos.y + view_matrix[14] * pos.z + view_matrix[15];

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = screen_width * 0.5f;
	float y = screen_height * 0.5f;

	x += 0.5f * _x * screen_width + 0.5f;
	y -= 0.5f * _y * screen_height + 0.5f;

	return { x,y,w };
}*/

D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3 pos)
{
	float _x = view_matrix[0] * pos.x + view_matrix[1] * pos.y + view_matrix[2] * pos.z + view_matrix[3];
	float _y = view_matrix[4] * pos.x + view_matrix[5] * pos.y + view_matrix[6] * pos.z + view_matrix[7]; // »√–¿… “”“ —  Œ›‘‘»÷≈Õ“¿Ã»

	float w = view_matrix[12] * pos.x + view_matrix[13] * pos.y + view_matrix[14] * pos.z + view_matrix[15];
	
	float x = _x / w;
	float y = _y / w / 1.32f;

	x = (screen_width / 2.f * x) + (x + screen_width / 2.f) - 20.f;
	y = -(screen_height / 2.f * y) + (y + screen_height / 2.f) - 30.f;
	
	return { x,y,w };
}

/*int world_to_screen(float* from, float* to)
{
	float w = 0.0f;
	to[0] = view_matrix.m[0][0] * from[0] + view_matrix.m[0][1] * from[1] + view_matrix.m[0][2] * from[2] + view_matrix.m[0][3];
	to[1] = view_matrix.m[1][0] * from[0] + view_matrix.m[1][1] * from[1] + view_matrix.m[1][2] * from[2] + view_matrix.m[1][3];
	w = view_matrix.m[3][0] * from[0] + view_matrix.m[3][1] * from[1] + view_matrix.m[3][2] * from[2] + view_matrix.m[3][3];
	if (w < 0.01f)
		return 0;
	float invw = 1.0f / w;
	to[0] *= invw;
	to[1] *= invw;
	int width = (int)(Form.right - Form.left);
	int height = (int)(Form.bottom - Form.top);
	float x = width / 2;
	float y = height / 2;
	x += 0.5 * to[0] * width + 0.5;
	y -= 0.5 * to[1] * height + 0.5;
	to[0] = x + Form.left;
	to[1] = y + Form.top;
	return 1;
}*/

/*D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3 pos)
{
	

	float w = view_matrix[12] * pos.x + view_matrix[13] * pos.y + view_matrix[14] * pos.z + view_matrix[15];

	float inv_w = 1.f / w;

	float x = (screen_width * 0.5f) + (0.5f * ((view_matrix[0] * pos.x + view_matrix[1] * pos.y + view_matrix[2] * pos.z + view_matrix[3]) * inv_w) * screen_width + 0.5f);
	float y = (screen_height * 0.5f) - (0.5f * ((view_matrix[4] * pos.x + view_matrix[5] * pos.y + view_matrix[6] * pos.z + view_matrix[7]) * inv_w) * screen_height + 0.5f);

	return { x,y,w };
}*/

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