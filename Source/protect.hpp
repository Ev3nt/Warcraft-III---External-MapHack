#pragma once
#include <Windows.h>
#include <string>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "systeminfo.h"

inline DWORD Gen(std::string str)
{
	DWORD result = 0;

	for (size_t i = 0; i < str.size(); i++)
		result += str[i] % 2;

	return result;
}

inline DWORD Gen(int i)
{
	return Gen(std::to_string(i));
}

inline std::string KEY()
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

inline bool IsValid(LPCSTR key)
{
	SYSTEMTIME time;
	GetSystemTime(&time);

	return !KEY().compare(key) && time.wYear <= 2021 && time.wMonth <= 6 && time.wDay <= 3;
}