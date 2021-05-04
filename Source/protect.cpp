#include "protect.h"

#include <iostream>
#include <vector>
#include <memory>
#define CURL_STATICLIB
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#include <curl.h>

#include "systeminfo.h"

DWORD Gen(std::string str)
{
	DWORD result = 0;

	for (size_t i = 0; i < str.size(); i++)
		result += str[i] % 2;

	return result;
}

DWORD Gen(int i)
{
	return Gen(std::to_string(i));
}

std::string KEY()
{
	std::unique_ptr<CPUInfoDelegate> cpuInfo = std::make_unique<CPUInfoDelegate>();
	std::vector<CPUInfo> cpuInfoVector{ cpuInfo->cpuInfoVector() };

	//------------------------------------------------------------------------------------------
	std::string key = "";

	char BUFFER[MAX_PATH];
	//------------------------------------------------------------------------------------------

	for (std::vector<CPUInfo>::const_iterator iter = cpuInfoVector.begin(); iter != cpuInfoVector.end(); iter++) {
		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->cpuNumber() + 1));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->name()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->manufacturer()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->numberOfCores()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->architecture()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->L2CacheSize()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->L3CacheSize()));
		key += BUFFER;
	}

	std::unique_ptr<MotherboardInfoDelegate> moboInfo = std::make_unique<MotherboardInfoDelegate>();
	std::vector<MotherboardInfo> moboInfoVector = moboInfo->motherboardInfoVector();

	ZeroMemory(BUFFER, sizeof(BUFFER));
	sprintf(BUFFER, "%01X", Gen(moboInfo->numberOfMotherboardInfoItems()));
	key += BUFFER;

	for (std::vector<MotherboardInfo>::const_iterator iter = moboInfoVector.begin(); iter != moboInfoVector.end(); iter++) {
		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->motherboardNumber() + 1));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->name()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->manufacturer()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->chipset()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->serialNumber()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->version()));
		key += BUFFER;
	}

	std::unique_ptr<RAMInfoDelegate> ramInfo = std::make_unique<RAMInfoDelegate>();
	std::vector<RAMInfo> ramInfoVector = ramInfo->ramInfoVector();

	ZeroMemory(BUFFER, sizeof(BUFFER));
	sprintf(BUFFER, "%01X", Gen(ramInfo->numberOfRAMInfoItems()));
	key += BUFFER;

	for (std::vector<RAMInfo>::const_iterator iter = ramInfoVector.begin(); iter != ramInfoVector.end(); iter++) {
		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->ramNumber() + 1));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->name()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->manufacturer()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->capacity()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->serialNumber()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->formFactor()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->partNumber()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->memoryType()));
		key += BUFFER;
	}

	std::unique_ptr<GPUInfoDelegate> gpuInfo = std::make_unique<GPUInfoDelegate>();
	std::vector<GPUInfo> gpuInfoVector = gpuInfo->gpuInfoVector();

	ZeroMemory(BUFFER, sizeof(BUFFER));
	sprintf(BUFFER, "%01X", Gen(gpuInfo->numberOfGPUInfoItems()));
	key += BUFFER;

	for (std::vector<GPUInfo>::const_iterator iter = gpuInfoVector.begin(); iter != gpuInfoVector.end(); iter++) {
		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->gpuNumber() + 1));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->name()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->manufacturer()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->caption()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->adapterRAM()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->driverVersion()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->videoArchitecture()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->videoModeDescription()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->videoProcessor()));
		key += BUFFER;
	}

	std::unique_ptr<OSInfoDelegate> osInfo = std::make_unique<OSInfoDelegate>();
	std::vector<OSInfo> osInfoVector = osInfo->osInfoVector();

	ZeroMemory(BUFFER, sizeof(BUFFER));
	sprintf(BUFFER, "%01X", Gen(osInfo->numberOfOSInfoItems()));
	key += BUFFER;

	for (std::vector<OSInfo>::const_iterator iter = osInfoVector.begin(); iter != osInfoVector.end(); iter++) {
		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->osNumber() + 1));
		key += BUFFER;

		// Если ключ изменился за этот день, закомментируйте эту хрень, что снизу.
		/*ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->name()));
		key += BUFFER;*/
		// Вот до сюда.

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->manufacturer()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->caption()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->version()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->currentUser()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->installDate()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->buildNumber()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->bootDevice()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->totalVirtualMemory()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->totalVisibleMemory()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->totalSwapSize()));
		key += BUFFER;

		ZeroMemory(BUFFER, sizeof(BUFFER));
		sprintf(BUFFER, "%01X", Gen(iter->serialNumber()));
		key += BUFFER;
	}

	std::cout << "Key: " << key.c_str() << std::endl;
	
	return key;
}

size_t write_to_string(void* ptr, size_t size, size_t count, void* stream) {
	((std::string*)stream)->append((char*)ptr, 0, size * count);
	return size * count;
}

bool IsValidTime(int year, int month, int day)
{
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://showcase.api.linx.twenty57.net/UnixTime/tounix?date=now");

	std::string response;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (response.size() != 12)
		return false;

	long long time_long = std::stoi(response.substr(1, response.size() - 2));
	tm* time = localtime(&time_long);

	SYSTEMTIME times;
	times.wYear = 1900 + time->tm_year;
	times.wMonth = time->tm_mon + 1;
	times.wDay = time->tm_mday;

	std::cout << 1900 + time->tm_year << "." << time->tm_mon + 1 << "." << time->tm_mday << " " << time->tm_hour << ":" << time->tm_min << ":" << time->tm_sec << std::endl;

	if (times.wYear >= year && times.wMonth >= month && times.wDay >= day)
		return false;

	return true;
}

bool IsValid(LPCSTR key)
{
	return !KEY().compare(key);
}