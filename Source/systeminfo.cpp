#include "systeminfo.h"

const std::string CPUInfo::TEMPERATURE_QUERY_STRING = "wmic /namespace:\\\\root\\wmi PATH MSAcpi_ThermalZoneTemperature get CurrentTemperature";
const std::string CPUInfo::TEMPERATURE_ERROR_IDENTIFIER_STRING = "ERROR:";
const std::string CPUInfo::CLOCK_SPEED_QUERY_STRING = "wmic cpu get /format:list | findstr /R /C:CurrentClockSpeed=";
const std::string CPUInfo::CURRENT_CLOCK_SPEED_IDENTIFIER_STRING = "CurrentClockSpeed=";
const std::string CPUInfo::NAME_IDENTIFIER_STRING = "Name=";
const std::string CPUInfo::NUMBER_OF_CORES_IDENTIFIER_STRING = "NumberOfCores=";
const std::string CPUInfo::MANUFACTURER_IDENTIFIER_STRING = "Manufacturer=";
const std::string CPUInfo::ARCHITECTURE_IDENTIFIER_STRING = "DataWidth=";
const std::string CPUInfo::L2_CACHE_SIZE_IDENTIFIER_STRING = "L2CacheSize=";
const std::string CPUInfo::L3_CACHE_SIZE_IDENTIFIER_STRING = "L3CacheSize=";

CPUInfo::CPUInfo(const std::vector<std::string>& rawData, int cpuNumber) :
    _name{ "" },
    _manufacturer{ "" },
    _numberOfCores{ "" },
    _architecture{ "" },
    _L2CacheSize{ "" },
    _L3CacheSize{ "" },
    _cpuNumber{ cpuNumber }
{
    for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {

        //Name
        if ((iter->find(NAME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NAME_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NAME_IDENTIFIER_STRING);
            this->_name = iter->substr(foundPosition + NAME_IDENTIFIER_STRING.length());
        }

        //Manufacturer
        if ((iter->find(MANUFACTURER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(MANUFACTURER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(MANUFACTURER_IDENTIFIER_STRING);
            this->_manufacturer = iter->substr(foundPosition + MANUFACTURER_IDENTIFIER_STRING.length());
        }

        //Number Of Cores
        if ((iter->find(NUMBER_OF_CORES_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NUMBER_OF_CORES_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NUMBER_OF_CORES_IDENTIFIER_STRING);
            this->_numberOfCores = iter->substr(foundPosition + NUMBER_OF_CORES_IDENTIFIER_STRING.length());
        }

        //Architecture
        if ((iter->find(ARCHITECTURE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(ARCHITECTURE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(ARCHITECTURE_IDENTIFIER_STRING);
            std::string dataWidth = iter->substr(foundPosition + ARCHITECTURE_IDENTIFIER_STRING.length());
            this->_architecture = getArchitecture(dataWidth);
        }

        //L2 Cache Size
        if ((iter->find(L2_CACHE_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(L2_CACHE_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(L2_CACHE_SIZE_IDENTIFIER_STRING);
            this->_L2CacheSize = iter->substr(foundPosition + L2_CACHE_SIZE_IDENTIFIER_STRING.length()) + "KB";
            if (this->_L2CacheSize == "KB") {
                this->_L2CacheSize = "";
            }
        }

        //L3 Cache Size
        if ((iter->find(L3_CACHE_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(L3_CACHE_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(L3_CACHE_SIZE_IDENTIFIER_STRING);
            this->_L3CacheSize = iter->substr(foundPosition + L3_CACHE_SIZE_IDENTIFIER_STRING.length()) + "KB";
            if (this->_L3CacheSize == "KB") {
                this->_L3CacheSize = "";
            }
        }
    }
    //In case any of these values are missing or don't get assigned
    if (this->_name == "") {
        this->_name = "Unknown";
    }
    if (this->_manufacturer == "") {
        this->_manufacturer = "Unknown";
    }
    if (this->_numberOfCores == "") {
        this->_numberOfCores = "Unknown";
    }
    if (this->_architecture == "") {
        this->_architecture = "Unknown";
    }
    if (this->_L2CacheSize == "") {
        this->_L2CacheSize = "Unknown";
    }
    if (this->_L3CacheSize == "") {
        this->_L3CacheSize = "Unknown";
    }
}

std::string CPUInfo::name() const
{
    return this->_name;
}

std::string CPUInfo::manufacturer() const
{
    return this->_manufacturer;
}

std::string CPUInfo::numberOfCores() const
{
    return this->_numberOfCores;
}

std::string CPUInfo::architecture() const
{
    return this->_architecture;
}

std::string CPUInfo::L2CacheSize() const
{
    return this->_L2CacheSize;
}

std::string CPUInfo::L3CacheSize() const
{
    return this->_L3CacheSize;
}

int CPUInfo::cpuNumber() const
{
    return this->_cpuNumber;
}

std::string CPUInfo::currentClockSpeed() const
{
    std::string clockSpeed{ "" };
    SystemCommand systemCommand{ CLOCK_SPEED_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        if (raw.empty()) {
            clockSpeed = "Unknown";
        }
        int cpuInfoNumber = 0;
        for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
            if (cpuInfoNumber == this->_cpuNumber) {
                if ((iter->find(CURRENT_CLOCK_SPEED_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CURRENT_CLOCK_SPEED_IDENTIFIER_STRING) == 0)) {
                    size_t foundPosition = iter->find(CURRENT_CLOCK_SPEED_IDENTIFIER_STRING);
                    clockSpeed = iter->substr(foundPosition + CURRENT_CLOCK_SPEED_IDENTIFIER_STRING.length()) + "MHz";
                }
            }
            cpuInfoNumber++;
        }
    }
    else {
        clockSpeed = "Unknown";
    }
    if ((clockSpeed == "MHz") || (clockSpeed == "")) {
        clockSpeed = "Unknown";
    }
    return clockSpeed;
}

std::string CPUInfo::currentTemperature() const
{
    //NOTE: THIS IS NOT SUPPORTED BY ALL COMPUTERS!!!
    std::string temperature{ "" };
    SystemCommand systemCommand{ TEMPERATURE_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        for (auto iter = raw.begin(); iter != raw.end(); iter++) {
            if (iter->find(TEMPERATURE_ERROR_IDENTIFIER_STRING)) {
                temperature = "Unknown";
            }
            else {
                std::string rawTemp = *(raw.begin() + 1);
                try {
                    int tempInKelvin = std::stoi(rawTemp);
                    int tempInCelcius = kelvinToCelcius(tempInKelvin);
                    temperature = toString(tempInCelcius) + "C";
                }
                catch (std::exception& e) {
                    (void)e;
                    temperature = "Unknown";
                }
            }
        }
    }
    else {
        temperature = "Unknown";
    }
    return temperature;
}


int CPUInfo::kelvinToCelcius(int tempInKelvin) const
{
    return tempInKelvin - 273;
}

std::string CPUInfo::getArchitecture(std::string& dataWidth) const
{
    try {
        int dataWidthInt = std::stoi(dataWidth);
        switch (dataWidthInt) {
        case 32: return "x86";
        case 64: return "x86_64";
        default: return "Unknown";
        }
    }
    catch (std::exception& e) {
        (void)e;
        return "Unknown";
    }
}

const std::string CPUInfoDelegate::CPU_INFO_QUERY_STRING = "wmic cpu get /format: list";
const std::string CPUInfoDelegate::CPU_INSTANCE_QUERY_STRING = "AssetTag=";
const std::string CPUInfoDelegate::CPU_INFO_END_IDENTIFIER_STRING = "VoltageCaps=";

CPUInfoDelegate::CPUInfoDelegate() :
    _numberOfCPUInfoItems{ 0 }
{
    SystemCommand systemCommand{ CPU_INFO_QUERY_STRING };
    systemCommand.execute();
    std::vector<std::string> tempVector = systemCommand.outputAsVector();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw = { systemCommand.outputAsVector() };
        determineNumberOfCPUInfoItems(raw);
        std::vector<std::string> singleCPUInfoItem;
        std::vector<std::string>::const_iterator iter = raw.begin();
        int cpuNumber = 0;
        while (cpuNumber < this->_numberOfCPUInfoItems) {
            while (iter->find(CPU_INFO_END_IDENTIFIER_STRING) == std::string::npos) {
                if ((*iter != "") && (*iter != "\r")) {
                    singleCPUInfoItem.push_back(*iter);
                }
                iter++;
            }
            singleCPUInfoItem.push_back(*iter);
            this->_cpuInfoVector.emplace_back(singleCPUInfoItem, cpuNumber);
            singleCPUInfoItem.clear();
            iter++;
            cpuNumber++;
        }
    }
}

void CPUInfoDelegate::determineNumberOfCPUInfoItems(const std::vector<std::string>& data)
{
    for (auto iter = data.begin(); iter != data.end(); iter++) {
        if (iter->find(CPU_INSTANCE_QUERY_STRING) != std::string::npos) {
            this->_numberOfCPUInfoItems++;
        }
    }
}

int CPUInfoDelegate::numberOfCPUInfoItems() const
{
    return this->_numberOfCPUInfoItems;
}

std::vector<CPUInfo> CPUInfoDelegate::cpuInfoVector() const
{
    return this->_cpuInfoVector;
}

const std::string GPUInfo::NVIDIA_IDENTIFIER_STRING = "NVIDIA";
const std::string GPUInfo::INTEL_IDENTIFIER_STRING = "INTEL";
const std::string GPUInfo::AMD_IDENTIFIER_STRING = "AMD";
const std::string GPUInfo::NAME_IDENTIFIER_STRING = "Name=";
const std::string GPUInfo::MANUFACTURER_IDENTIFIER_STRING = "AdapterCompatibility=";
const std::string GPUInfo::ADAPTER_RAM_IDENTIFIER_STRING = "AdapterRAM=";
const std::string GPUInfo::REFRESH_RATE_IDENTIFIER_STRING = "CurrentRefreshRate=";
const std::string GPUInfo::DRIVER_VERSION_IDENTIFIER_STRING = "DriverVersion=";
const std::string GPUInfo::VIDEO_ARCHITECTURE_IDENTIFIER_STRING = "VideoArchitecture=";
const std::string GPUInfo::VIDEO_MEMORY_TYPE_IDENTIFIER_STRING = "VideoMemoryType";
const std::string GPUInfo::VIDEO_MODE_DESCRIPTION_IDENTIFIER_STRING = "VideoModeDescription=";
const std::string GPUInfo::VIDEO_PROCESSOR_IDENTIFIER_STRING = "VideoProcessor=";
const std::string GPUInfo::CAPTION_IDENTIFIER_STRING = "Caption=";

GPUInfo::GPUInfo(const std::vector<std::string>& rawData, int gpuNumber) :
    _name{ "" },
    _manufacturer{ "" },
    _caption{ "" },
    _adapterRAM{ "" },
    _refreshRate{ "" },
    _driverVersion{ "" },
    _videoArchitecture{ "" },
    _videoMemoryType{ "" },
    _videoModeDescription{ "" },
    _videoProcessor{ "" },
    _gpuNumber{ gpuNumber }
{
    for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {

        //Name
        if ((iter->find(NAME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NAME_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NAME_IDENTIFIER_STRING);
            this->_name = iter->substr(foundPosition + NAME_IDENTIFIER_STRING.length());
        }

        //Manufacturer
        if ((iter->find(MANUFACTURER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(MANUFACTURER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(MANUFACTURER_IDENTIFIER_STRING);
            this->_manufacturer = iter->substr(foundPosition + MANUFACTURER_IDENTIFIER_STRING.length());
        }

        //Caption
        if ((iter->find(CAPTION_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CAPTION_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(CAPTION_IDENTIFIER_STRING);
            this->_caption = iter->substr(foundPosition + CAPTION_IDENTIFIER_STRING.length());
        }

        //Adapter RAM
        if ((iter->find(ADAPTER_RAM_IDENTIFIER_STRING) != std::string::npos) && (iter->find(ADAPTER_RAM_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(ADAPTER_RAM_IDENTIFIER_STRING);
            std::string capacityString = iter->substr(foundPosition + ADAPTER_RAM_IDENTIFIER_STRING.length());
            long long int capacity{ 0 };
            try {
                capacity = std::stoll(capacityString);
                this->_adapterRAM = toString(capacity / 1000000) + "MB (" + toString(capacity) + " Bytes)";
            }
            catch (std::exception& e) {
                (void)e;
                this->_adapterRAM = capacityString + " Bytes";
            }
        }

        //Refresh Rate
        if ((iter->find(REFRESH_RATE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(REFRESH_RATE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(REFRESH_RATE_IDENTIFIER_STRING);
            this->_refreshRate = iter->substr(foundPosition + REFRESH_RATE_IDENTIFIER_STRING.length()) + "MHz";
            if (this->_refreshRate == "MHz") {
                this->_refreshRate = "";
            }

        }

        //Driver Version
        if ((iter->find(DRIVER_VERSION_IDENTIFIER_STRING) != std::string::npos) && (iter->find(DRIVER_VERSION_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(DRIVER_VERSION_IDENTIFIER_STRING);
            this->_driverVersion = iter->substr(foundPosition + DRIVER_VERSION_IDENTIFIER_STRING.length());
        }

        //Video Architecture
        if ((iter->find(VIDEO_ARCHITECTURE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(VIDEO_ARCHITECTURE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(VIDEO_ARCHITECTURE_IDENTIFIER_STRING);
            std::string videoArchitectureString = iter->substr(foundPosition, VIDEO_ARCHITECTURE_IDENTIFIER_STRING.length());
            this->_videoArchitecture = getVideoArchitecture(videoArchitectureString);
        }

        //Video Memory Type
        if ((iter->find(VIDEO_MEMORY_TYPE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(VIDEO_MEMORY_TYPE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(VIDEO_MEMORY_TYPE_IDENTIFIER_STRING);
            std::string videoMemoryTypeString = iter->substr(foundPosition, VIDEO_MEMORY_TYPE_IDENTIFIER_STRING.length());
            this->_videoMemoryType = getVideoMemoryType(videoMemoryTypeString);
        }

        //Video Mode Description
        if ((iter->find(VIDEO_MODE_DESCRIPTION_IDENTIFIER_STRING) != std::string::npos) && (iter->find(VIDEO_MODE_DESCRIPTION_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(VIDEO_MODE_DESCRIPTION_IDENTIFIER_STRING);
            this->_videoModeDescription = iter->substr(foundPosition + VIDEO_MODE_DESCRIPTION_IDENTIFIER_STRING.length());
        }

        //Video Processor
        if ((iter->find(VIDEO_PROCESSOR_IDENTIFIER_STRING) != std::string::npos) && (iter->find(VIDEO_PROCESSOR_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(VIDEO_PROCESSOR_IDENTIFIER_STRING);
            this->_videoProcessor = iter->substr(foundPosition + VIDEO_PROCESSOR_IDENTIFIER_STRING.length());
        }
    }
    //In case any of these values are missing or don't get assigned
    if (this->_name == "") {
        this->_name = "Unknown";
    }
    if (this->_manufacturer == "") {
        this->_manufacturer = "Unknown";
    }
    if (this->_caption == "") {
        this->_caption = "Unknown";
    }
    if (this->_adapterRAM == "") {
        this->_adapterRAM = "Unknown";
    }
    if (this->_refreshRate == "") {
        this->_refreshRate = "Unknown";
    }
    if (this->_driverVersion == "") {
        this->_driverVersion = "Unknown";
    }
    if (this->_videoArchitecture == "") {
        this->_videoArchitecture = "Unknown";
    }
    if (this->_videoMemoryType == "") {
        this->_videoMemoryType = "Unknown";
    }
    if (this->_videoModeDescription == "") {
        this->_videoModeDescription = "Unknown";
    }
    if (this->_videoProcessor == "") {
        this->_videoProcessor = "Unknown";
    }
}

std::string GPUInfo::getVideoArchitecture(const std::string& videoArchitectureString) const
{
    int videoArch{ 2 };
    try {
        videoArch = std::stoi(videoArchitectureString);
    }
    catch (std::exception& e) {
        (void)e;
        videoArch = 2;
    }
    //As per https://msdn.microsoft.com/en-us/library/aa394512(v=vs.85).aspx
    switch (videoArch) {
    case 1: return "Other";
    case 2: return "Unknown";
    case 3: return "CGA";
    case 4: return "EGA";
    case 5: return "VGA";
    case 6: return "SVGA";
    case 7: return "MDA";
    case 8: return "HGC";
    case 9: return "MCGA";
    case 10: return "8514A";
    case 11: return "XGA";
    case 12: return "Linear Frame Buffer";
    case 160: return "PC - 98";
    default: return "Unknown";
    }
}

std::string GPUInfo::getVideoMemoryType(const std::string& videoMemoryTypeString) const
{
    int videoMemoryType{ 2 };
    try {
        videoMemoryType = std::stoi(videoMemoryTypeString);
    }
    catch (std::exception& e) {
        (void)e;
        videoMemoryType = 2;
    }
    switch (videoMemoryType) {
    case 1: return "Other";
    case 2: return "Unknown";
    case 3: return "VRAM";
    case 4: return "DRAM";
    case 5: return "SRAM";
    case 6: return "WRAM";
    case 7: return "EDO_RAM";
    case 8: return "Burst Synchronous DRAM";
    case 9: return "Pipelined Burst SRAM";
    case 10: return "CDRAM";
    case 11: return "3DRAM";
    case 12: return "SDRAM";
    case 13: return "SGRAM";
    default: return "Unknown";
    }
}

std::string GPUInfo::name() const
{
    return this->_name;
}

std::string GPUInfo::manufacturer() const
{
    return this->_manufacturer;
}

std::string GPUInfo::caption() const
{
    return this->_caption;
}

std::string GPUInfo::adapterRAM() const
{
    return this->_adapterRAM;
}

std::string GPUInfo::refreshRate() const
{
    return this->_refreshRate;
}

std::string GPUInfo::driverVersion() const
{
    return this->_driverVersion;
}

std::string GPUInfo::videoArchitecture() const
{
    return this->_videoArchitecture;
}

std::string GPUInfo::videoProcessor() const
{
    return this->_videoProcessor;
}

std::string GPUInfo::videoMemoryType() const
{
    return this->_videoMemoryType;
}

std::string GPUInfo::videoModeDescription() const
{
    return this->_videoModeDescription;
}

int GPUInfo::gpuNumber() const
{
    return this->_gpuNumber;
}

const std::string GPUInfoDelegate::GPU_INFO_QUERY_STRING = "wmic path Win32_VideoController get /format: list";
const std::string GPUInfoDelegate::GPU_INSTANCE_QUERY_STRING = "CurrentBitsPerPixel=";
const std::string GPUInfoDelegate::GPU_INFO_END_IDENTIFIER_STRING = "VideoProcessor=";

GPUInfoDelegate::GPUInfoDelegate() :
    _numberOfGPUInfoItems{ 0 }
{
    SystemCommand systemCommand{ GPU_INFO_QUERY_STRING };
    systemCommand.execute();
    std::vector<std::string> tempVector = systemCommand.outputAsVector();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw = { systemCommand.outputAsVector() };
        determineNumberOfGPUInfoItems(raw);
        std::vector<std::string> singleGPUInfoItem;
        std::vector<std::string>::const_iterator iter = raw.begin();
        int gpuNumber = 0;
        while (gpuNumber < this->_numberOfGPUInfoItems) {
            while (iter->find(GPU_INFO_END_IDENTIFIER_STRING) == std::string::npos) {
                if ((*iter != "") && (*iter != "\r")) {
                    singleGPUInfoItem.push_back(*iter);
                }
                iter++;
            }
            singleGPUInfoItem.push_back(*iter);
            this->_gpuInfoVector.emplace_back(singleGPUInfoItem, gpuNumber);
            singleGPUInfoItem.clear();
            iter++;
            gpuNumber++;
        }
    }
}

void GPUInfoDelegate::determineNumberOfGPUInfoItems(const std::vector<std::string>& data)
{
    for (auto iter = data.begin(); iter != data.end(); iter++) {
        if (iter->find(GPU_INSTANCE_QUERY_STRING) != std::string::npos) {
            this->_numberOfGPUInfoItems++;
        }
    }
}

int GPUInfoDelegate::numberOfGPUInfoItems() const
{
    return this->_numberOfGPUInfoItems;
}

std::vector<GPUInfo> GPUInfoDelegate::gpuInfoVector() const
{
    return this->_gpuInfoVector;
}

const std::string MotherboardInfo::CHIPSET_QUERY_STRING = "wmic path Win32_PnPEntity get /format:list | findstr /R /C:\"Chipset\"";
const std::string MotherboardInfo::NAME_IDENTIFIER_STRING = "Product=";
const std::string MotherboardInfo::MANUFACTURER_IDENTIFIER_STRING = "Manufacturer=";
const std::string MotherboardInfo::CHIPSET_IDENTIFIER_STRING = "Description=";
const std::string MotherboardInfo::CHIPSET_END_IDENTIFIER_STRING = "Chipset";
const std::string MotherboardInfo::SERIAL_NUMBER_IDENTIFIER_STRING = "SerialNumber=";
const std::string MotherboardInfo::VERSION_IDENTIFIER_STRING = "Version=";

MotherboardInfo::MotherboardInfo(const std::vector<std::string>& rawData, int motherboardNumber) :
    _name{ "" },
    _manufacturer{ "" },
    _chipset{ "" },
    _serialNumber{ "" },
    _version{ "" },
    _motherboardNumber{ motherboardNumber }
{
    determineChipset();
    for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {
        //Name
        if (iter->find(NAME_IDENTIFIER_STRING) != std::string::npos) {
            size_t foundPosition = iter->find(NAME_IDENTIFIER_STRING);
            this->_name = iter->substr(foundPosition + NAME_IDENTIFIER_STRING.length());
        }

        //Manufacturer
        if (iter->find(MANUFACTURER_IDENTIFIER_STRING) != std::string::npos) {
            size_t foundPosition = iter->find(MANUFACTURER_IDENTIFIER_STRING);
            this->_manufacturer = iter->substr(foundPosition + MANUFACTURER_IDENTIFIER_STRING.length());
        }

        //Serial Number
        if (iter->find(SERIAL_NUMBER_IDENTIFIER_STRING) != std::string::npos) {
            size_t foundPosition = iter->find(SERIAL_NUMBER_IDENTIFIER_STRING);
            this->_serialNumber = iter->substr(foundPosition + SERIAL_NUMBER_IDENTIFIER_STRING.length());
        }

        //Motherboard Version
        if (iter->find(VERSION_IDENTIFIER_STRING) != std::string::npos) {
            size_t foundPosition = iter->find(VERSION_IDENTIFIER_STRING);
            this->_version = iter->substr(foundPosition + VERSION_IDENTIFIER_STRING.length());
        }
    }
    if (this->_name == "") {
        this->_name = "Unknown";
    }
    if (this->_manufacturer == "") {
        this->_manufacturer = "Unknown";
    }
    if (this->_serialNumber == "") {
        this->_serialNumber = "Unknown";
    }
    if (this->_version == "") {
        this->_version = "Unknown";
    }
}

std::string MotherboardInfo::name() const
{
    return this->_name;
}

std::string MotherboardInfo::manufacturer() const
{
    return this->_manufacturer;
}

std::string MotherboardInfo::chipset() const
{
    return this->_chipset;
}

std::string MotherboardInfo::serialNumber() const
{
    return this->_serialNumber;
}

std::string MotherboardInfo::version() const
{
    return this->_version;
}

int MotherboardInfo::motherboardNumber() const
{
    return this->_motherboardNumber;
}

void MotherboardInfo::determineChipset()
{
    SystemCommand systemCommand{ CHIPSET_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> rawData = systemCommand.outputAsVector();
        for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {
            if (iter->find(CHIPSET_IDENTIFIER_STRING) != std::string::npos) {
                size_t foundPosition = iter->find(CHIPSET_IDENTIFIER_STRING);
                size_t endPosition = iter->find(CHIPSET_END_IDENTIFIER_STRING);
                this->_chipset = iter->substr(foundPosition + CHIPSET_IDENTIFIER_STRING.length(), endPosition - CHIPSET_END_IDENTIFIER_STRING.length() + 2);
            }
        }
    }
}

const std::string MotherboardInfoDelegate::MOTHERBOARD_INFO_QUERY_STRING = "wmic baseboard get /format: list";
const std::string MotherboardInfoDelegate::MOTHERBOARD_INSTANCE_QUERY_STRING = "RequiresDaughterBoard=";
const std::string MotherboardInfoDelegate::MOTHERBOARD_INFO_END_IDENTIFIER_STRING = "Width=";

MotherboardInfoDelegate::MotherboardInfoDelegate() :
    _numberOfMotherboardInfoItems{ 0 }
{
    SystemCommand systemCommand{ MOTHERBOARD_INFO_QUERY_STRING };
    systemCommand.execute();
    std::vector<std::string> tempVector = systemCommand.outputAsVector();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw = { systemCommand.outputAsVector() };
        determineNumberOfMotherboardInfoItems(raw);
        std::vector<std::string> singleMotherboardInfoItem;
        std::vector<std::string>::const_iterator iter = raw.begin();
        int motherboardNumber = 0;
        while (motherboardNumber < this->_numberOfMotherboardInfoItems) {
            while (iter->find(MOTHERBOARD_INFO_END_IDENTIFIER_STRING) == std::string::npos) {
                if ((*iter != "") && (*iter != "\r")) {
                    singleMotherboardInfoItem.push_back(*iter);
                }
                iter++;
            }
            singleMotherboardInfoItem.push_back(*iter);
            this->_motherboardInfoVector.emplace_back(singleMotherboardInfoItem, motherboardNumber);
            singleMotherboardInfoItem.clear();
            iter++;
            motherboardNumber++;
        }
    }
}

void MotherboardInfoDelegate::determineNumberOfMotherboardInfoItems(const std::vector<std::string>& data)
{
    for (auto iter = data.begin(); iter != data.end(); iter++) {
        if (iter->find(MOTHERBOARD_INSTANCE_QUERY_STRING) != std::string::npos) {
            this->_numberOfMotherboardInfoItems++;
        }
    }
}

std::vector<MotherboardInfo> MotherboardInfoDelegate::motherboardInfoVector() const
{
    return this->_motherboardInfoVector;
}

int MotherboardInfoDelegate::numberOfMotherboardInfoItems() const
{
    return this->_numberOfMotherboardInfoItems;
}

const std::string OSInfo::ARCHITECTURE_IDENTIFIER_STRING = "OSArchitecture=";
const std::string OSInfo::CAPTION_INDENTIFIER_STRING = "Caption=";
const std::string OSInfo::NAME_IDENTIFIER_STRING = "Name=";
const std::string OSInfo::MANUFACTURER_IDENTIFIER_STRING = "Manufacturer=";
const std::string OSInfo::VERSION_IDENTIFIER_STRING = "Version=";
const std::string OSInfo::CURRENT_USER_IDENTIFIER_STRING = "RegisteredUser=";
const std::string OSInfo::INSTALL_DATE_IDENTIFIER_STRING = "InstallDate=";
const std::string OSInfo::BUILD_NUMBER_IDENTIFIER_STRING = "BuildNumber=";
const std::string OSInfo::LAST_BOOT_UP_TIME_IDENTIFIER_STRING = "LastBootUpTime=";
const std::string OSInfo::BOOT_DEVICE_IDENTIFIER_STRING = "BootDevice=";
const std::string OSInfo::SERIAL_NUMBER_IDENTIFIER_STRING = "SerialNumber=";
const std::string OSInfo::TOTAL_VIRTUAL_MEMORY_SIZE_IDENTIFIER_STRING = "TotalVirtualMemorySize=";
const std::string OSInfo::TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING = "TotalVisibleMemorySize=";
const std::string OSInfo::TOTAL_SWAP_SIZE_IDENTIFIER_STRING = "TotalSwapSize=";

const std::string OSInfo::FREE_PHYSICAL_MEMORY_IDENTIFIER_STRING = "FreePhysicalMemory=";
const std::string OSInfo::FREE_PHYSICAL_MEMORY_QUERY_STRING = "wmic os get /format:list | findstr /R /C:FreePhysicalMemory=";

const std::string OSInfo::FREE_VIRTUAL_MEMORY_IDENTIFIER_STRING = "FreeVirtualMemory=";
const std::string OSInfo::FREE_VIRTUAL_MEMORY_QUERY_STRING = "wmic os get /format:list | findstr /R /C:FreeVirtualMemory=";

const std::string OSInfo::FREE_SPACE_IN_PAGING_FILES_IDENTIFIER_STRING = "FreeSpaceInPagingFiles=";
const std::string OSInfo::FREE_SPACE_IN_PAGING_FILES_QUERY_STRING = "wmic os get /format:list | findstr /R /C:FreeSpaceInPagingFiles=";

const std::string OSInfo::USED_SPACE_IN_PAGING_FILE_IDENTIFIER_STRING = "SizeStoredInPagingFiles=";
const std::string OSInfo::USED_SPACE_IN_PAGING_FILE_QUERY_STRING = "wmic os get /format:list | findstr /R /C:SizeStoredInPagingFiles=";

const std::string OSInfo::CURRENT_DATE_TIME_IDENTIFIER_STRING = "LocalDateTime=";
const std::string OSInfo::CURRENT_DATE_TIME_QUERY_STRING = "wmic os get /format:list | findstr /R /C:LocalDateTime=";

const int OSInfo::KILOBYTES_PER_MEGABYTE = 1000;



OSInfo::OSInfo(const std::vector<std::string>& rawData, int osNumber) :
    _name{ "" },
    _manufacturer{ "" },
    _architecture{ "" },
    _caption{ "" },
    _version{ "" },
    _currentUser{ "" },
    _installDate{ "" },
    _buildNumber{ "" },
    _lastBootUpTime{ "" },
    _bootDevice{ "" },
    _serialNumber{ "" },
    _totalVirtualMemory{ "" },
    _totalVisibleMemory{ "" },
    _totalSwapSize{ "" },
    _osNumber{ osNumber }

{
    for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {

        //Name
        if ((iter->find(NAME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NAME_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NAME_IDENTIFIER_STRING);
            this->_name = iter->substr(foundPosition + NAME_IDENTIFIER_STRING.length());
        }

        //Manufacturer
        if ((iter->find(MANUFACTURER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(MANUFACTURER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(MANUFACTURER_IDENTIFIER_STRING);
            this->_manufacturer = iter->substr(foundPosition + MANUFACTURER_IDENTIFIER_STRING.length());
        }

        //Architecture
        if ((iter->find(ARCHITECTURE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(ARCHITECTURE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(ARCHITECTURE_IDENTIFIER_STRING);
            this->_architecture = iter->substr(foundPosition + ARCHITECTURE_IDENTIFIER_STRING.length());
        }

        //Caption
        if ((iter->find(CAPTION_INDENTIFIER_STRING) != std::string::npos) && (iter->find(CAPTION_INDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(CAPTION_INDENTIFIER_STRING);
            this->_caption = iter->substr(foundPosition + CAPTION_INDENTIFIER_STRING.length());
        }

        //Version
        if ((iter->find(VERSION_IDENTIFIER_STRING) != std::string::npos) && (iter->find(VERSION_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(VERSION_IDENTIFIER_STRING);
            this->_version = iter->substr(foundPosition + VERSION_IDENTIFIER_STRING.length());
        }

        //Current User
        if ((iter->find(CURRENT_USER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CURRENT_USER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(CURRENT_USER_IDENTIFIER_STRING);
            this->_currentUser = iter->substr(foundPosition + CURRENT_USER_IDENTIFIER_STRING.length());
        }

        //Install Date
        if ((iter->find(INSTALL_DATE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(INSTALL_DATE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(INSTALL_DATE_IDENTIFIER_STRING);
            this->_installDate = iter->substr(foundPosition + INSTALL_DATE_IDENTIFIER_STRING.length());
        }

        //Build Number
        if ((iter->find(BUILD_NUMBER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(BUILD_NUMBER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(BUILD_NUMBER_IDENTIFIER_STRING);
            this->_buildNumber = iter->substr(foundPosition + BUILD_NUMBER_IDENTIFIER_STRING.length());
        }

        //Last Boot Up Time
        if ((iter->find(LAST_BOOT_UP_TIME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(LAST_BOOT_UP_TIME_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(LAST_BOOT_UP_TIME_IDENTIFIER_STRING);
            this->_lastBootUpTime = iter->substr(foundPosition + LAST_BOOT_UP_TIME_IDENTIFIER_STRING.length());
        }

        //Boot Device
        if ((iter->find(BOOT_DEVICE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(BOOT_DEVICE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(BOOT_DEVICE_IDENTIFIER_STRING);
            this->_bootDevice = iter->substr(foundPosition + BOOT_DEVICE_IDENTIFIER_STRING.length());
        }

        //Serial Number
        if ((iter->find(SERIAL_NUMBER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(SERIAL_NUMBER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(SERIAL_NUMBER_IDENTIFIER_STRING);
            this->_serialNumber = iter->substr(foundPosition + SERIAL_NUMBER_IDENTIFIER_STRING.length());
        }

        //Total Virtual Memory
        if ((iter->find(TOTAL_VIRTUAL_MEMORY_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(TOTAL_VIRTUAL_MEMORY_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(TOTAL_VIRTUAL_MEMORY_SIZE_IDENTIFIER_STRING);
            std::string totalVirtualMemoryString = iter->substr(foundPosition + TOTAL_VIRTUAL_MEMORY_SIZE_IDENTIFIER_STRING.length());
            if (totalVirtualMemoryString == "") {
                this->_totalVirtualMemory = "";
                continue;
            }
            else {
                long long int totalVirtualMemory{ 0 };
                try {
                    totalVirtualMemory = std::stoll(totalVirtualMemoryString);
                    this->_totalVirtualMemory = toString(totalVirtualMemory / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(totalVirtualMemory) + " KB)";
                }
                catch (std::exception& e) {
                    (void)e;
                    this->_totalVirtualMemory = totalVirtualMemoryString + " KB";
                }
            }
        }

        //Total Visible Memory
        if ((iter->find(TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING);
            std::string totalVisibleMemoryString = iter->substr(foundPosition + TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING.length());
            if (totalVisibleMemoryString == "") {
                this->_totalVisibleMemory = "";
                continue;
            }
            else {
                long long int totalVisibleMemory{ 0 };
                try {
                    totalVisibleMemory = std::stoll(totalVisibleMemoryString);
                    this->_totalVisibleMemory = toString(totalVisibleMemory / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(totalVisibleMemory) + " KB)";
                }
                catch (std::exception& e) {
                    (void)e;
                    this->_totalVisibleMemory = totalVisibleMemoryString + " KB";
                }
            }
        }

        //Total Swap Size
        if ((iter->find(TOTAL_SWAP_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(TOTAL_SWAP_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING);
            std::string totalSwapSizeString = iter->substr(foundPosition + TOTAL_SWAP_SIZE_IDENTIFIER_STRING.length());
            if (totalSwapSizeString == "") {
                this->_totalSwapSize = "";
                continue;
            }
            else {
                long long int totalSwapSize{ 0 };
                try {
                    totalSwapSize = std::stoll(totalSwapSizeString);
                    this->_totalVisibleMemory = toString(totalSwapSize / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(totalSwapSize) + " KB)";
                }
                catch (std::exception& e) {
                    (void)e;
                    this->_totalSwapSize = totalSwapSizeString + " KB";
                }
            }
        }
    }
    //In case any of these values are missing or don't get assigned
    if (this->_name == "") {
        this->_name = "Unknown";
    }
    if (this->_manufacturer == "") {
        this->_manufacturer = "Unknown";
    }
    if (this->_architecture == "") {
        this->_architecture = "Unknown";
    }
    if (this->_caption == "") {
        this->_caption = "Unknown";
    }
    if (this->_version == "") {
        this->_version = "Unknown";
    }
    if (this->_currentUser == "") {
        this->_currentUser = "Unknown";
    }
    if (this->_installDate == "") {
        this->_installDate = "Unknown";
    }
    if (this->_buildNumber == "") {
        this->_buildNumber = "Unknown";
    }
    if (this->_lastBootUpTime == "") {
        this->_lastBootUpTime = "Unknown";
    }
    if (this->_bootDevice == "") {
        this->_bootDevice = "Unknown";
    }
    if (this->_serialNumber == "") {
        this->_serialNumber = "Unknown";
    }
    if ((this->_totalVirtualMemory == "") || (this->_totalVirtualMemory == " KB")) {
        this->_totalVirtualMemory = "Unknown";
    }
    if ((this->_totalVisibleMemory == "") || (this->_totalVisibleMemory == " KB")) {
        this->_totalVisibleMemory = "Unknown";
    }
    if ((this->_totalSwapSize == "") || (this->_totalSwapSize == " KB")) {
        this->_totalSwapSize = "Unknown";
    }
}

std::string OSInfo::name() const
{
    return this->_name;
}

std::string OSInfo::manufacturer() const
{
    return this->_manufacturer;
}

std::string OSInfo::architecture() const
{
    return this->_architecture;
}

std::string OSInfo::caption() const
{
    return this->_caption;
}
std::string OSInfo::version() const
{
    return this->_version;
}

std::string OSInfo::currentUser() const
{
    return this->_currentUser;
}

std::string OSInfo::installDate() const
{
    return this->_installDate;
}

std::string OSInfo::buildNumber() const
{
    return this->_buildNumber;
}

std::string OSInfo::lastBootUpTime() const
{
    return this->_lastBootUpTime;
}

std::string OSInfo::bootDevice() const
{
    return this->_bootDevice;
}

std::string OSInfo::serialNumber() const
{
    return this->_serialNumber;
}

std::string OSInfo::totalVirtualMemory() const
{
    return this->_totalVirtualMemory;
}

std::string OSInfo::totalVisibleMemory() const
{
    return this->_totalVisibleMemory;
}

std::string OSInfo::totalSwapSize() const
{
    return this->_totalSwapSize;
}

int OSInfo::osNumber() const
{
    return this->_osNumber;
}

std::string OSInfo::freePhysicalMemory() const
{
    std::string freePhysicalMemory{ "" };
    SystemCommand systemCommand{ FREE_PHYSICAL_MEMORY_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        if (raw.empty()) {
            freePhysicalMemory = "Unknown";
        }
        int osInfoNumber = 0;
        for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
            if (osInfoNumber == this->_osNumber) {
                if ((iter->find(FREE_PHYSICAL_MEMORY_IDENTIFIER_STRING) != std::string::npos) && (iter->find(FREE_PHYSICAL_MEMORY_IDENTIFIER_STRING) == 0)) {
                    size_t foundPosition = iter->find(FREE_PHYSICAL_MEMORY_IDENTIFIER_STRING);
                    std::string freePhysicalMemoryString = iter->substr(foundPosition + FREE_PHYSICAL_MEMORY_IDENTIFIER_STRING.length());
                    if (freePhysicalMemoryString == "") {
                        freePhysicalMemory = "";
                        continue;
                    }
                    else {
                        long long int freePhysicalMemoryInt{ 0 };
                        try {
                            freePhysicalMemoryInt = std::stoll(freePhysicalMemoryString);
                            freePhysicalMemory = toString(freePhysicalMemoryInt / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(freePhysicalMemoryInt) + " KB)";
                        }
                        catch (std::exception& e) {
                            (void)e;
                            freePhysicalMemory = freePhysicalMemoryString + " KB";
                        }
                    }
                }
            }
            osInfoNumber++;
        }
    }
    else {
        freePhysicalMemory = "Unknown";
    }
    if ((freePhysicalMemory == "") || (freePhysicalMemory == " KB")) {
        freePhysicalMemory = "Unknown";
    }
    return freePhysicalMemory;
}

std::string OSInfo::freeVirtualMemory() const
{
    std::string freeVisibleMemory{ "" };
    SystemCommand systemCommand{ FREE_VIRTUAL_MEMORY_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        if (raw.empty()) {
            freeVisibleMemory = "Unknown";
        }
        int osInfoNumber = 0;
        for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
            if (osInfoNumber == this->_osNumber) {
                if ((iter->find(FREE_VIRTUAL_MEMORY_IDENTIFIER_STRING) != std::string::npos) && (iter->find(FREE_VIRTUAL_MEMORY_IDENTIFIER_STRING) == 0)) {
                    size_t foundPosition = iter->find(FREE_VIRTUAL_MEMORY_IDENTIFIER_STRING);
                    std::string freeVisibleMemoryString = iter->substr(foundPosition + FREE_VIRTUAL_MEMORY_IDENTIFIER_STRING.length());
                    if (freeVisibleMemoryString == "") {
                        freeVisibleMemory = "";
                        continue;
                    }
                    else {
                        long long int freeVisibleMemoryInt{ 0 };
                        try {
                            freeVisibleMemoryInt = std::stoll(freeVisibleMemoryString);
                            freeVisibleMemory = toString(freeVisibleMemoryInt / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(freeVisibleMemoryInt) + " KB)";
                        }
                        catch (std::exception& e) {
                            (void)e;
                            freeVisibleMemory = freeVisibleMemoryString + " KB";
                        }
                    }
                }
            }
            osInfoNumber++;
        }
    }
    else {
        freeVisibleMemory = "Unknown";
    }
    if ((freeVisibleMemory == "") || (freeVisibleMemory == " KB")) {
        freeVisibleMemory = "Unknown";
    }
    return freeVisibleMemory;
}

std::string OSInfo::freePagingFileSpace() const
{
    std::string freePagingFileSpace{ "" };
    SystemCommand systemCommand{ FREE_SPACE_IN_PAGING_FILES_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        if (raw.empty()) {
            freePagingFileSpace = "Unknown";
        }
        int osInfoNumber = 0;
        for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
            if (osInfoNumber == this->_osNumber) {
                if ((iter->find(FREE_SPACE_IN_PAGING_FILES_IDENTIFIER_STRING) != std::string::npos) && (iter->find(FREE_SPACE_IN_PAGING_FILES_IDENTIFIER_STRING) == 0)) {
                    size_t foundPosition = iter->find(FREE_SPACE_IN_PAGING_FILES_IDENTIFIER_STRING);
                    std::string freePagingFileSpaceString = iter->substr(foundPosition + FREE_SPACE_IN_PAGING_FILES_IDENTIFIER_STRING.length());
                    if (freePagingFileSpaceString == "") {
                        freePagingFileSpace = "";
                        continue;
                    }
                    else {
                        long long int freePagingFileSpaceInt{ 0 };
                        try {
                            freePagingFileSpaceInt = std::stoll(freePagingFileSpaceString);
                            freePagingFileSpace = toString(freePagingFileSpaceInt / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(freePagingFileSpaceInt) + " KB)";
                        }
                        catch (std::exception& e) {
                            (void)e;
                            freePagingFileSpace = freePagingFileSpaceString + " KB";
                        }
                    }
                }
            }
            osInfoNumber++;
        }
    }
    else {
        freePagingFileSpace = "Unknown";
    }
    if ((freePagingFileSpace == "") || (freePagingFileSpace == " KB")) {
        freePagingFileSpace = "Unknown";
    }
    return freePagingFileSpace;
}

std::string OSInfo::usedPagingFileSpace() const
{
    std::string usedPagingFileSpace{ "" };
    SystemCommand systemCommand{ USED_SPACE_IN_PAGING_FILE_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        if (raw.empty()) {
            usedPagingFileSpace = "Unknown";
        }
        int osInfoNumber = 0;
        for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
            if (osInfoNumber == this->_osNumber) {
                if ((iter->find(USED_SPACE_IN_PAGING_FILE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(USED_SPACE_IN_PAGING_FILE_IDENTIFIER_STRING) == 0)) {
                    size_t foundPosition = iter->find(USED_SPACE_IN_PAGING_FILE_IDENTIFIER_STRING);
                    std::string usedPagingFileSpaceString = iter->substr(foundPosition + USED_SPACE_IN_PAGING_FILE_IDENTIFIER_STRING.length());
                    if (usedPagingFileSpaceString == "") {
                        usedPagingFileSpace = "";
                        continue;
                    }
                    else {
                        long long int usedPagingFileSpaceInt{ 0 };
                        try {
                            usedPagingFileSpaceInt = std::stoll(usedPagingFileSpaceString);
                            usedPagingFileSpace = toString(usedPagingFileSpaceInt / KILOBYTES_PER_MEGABYTE) + "MB (" + toString(usedPagingFileSpaceInt) + " KB)";
                        }
                        catch (std::exception& e) {
                            (void)e;
                            usedPagingFileSpace = usedPagingFileSpaceString + " KB";
                        }
                    }
                }
            }
            osInfoNumber++;
        }
    }
    else {
        usedPagingFileSpace = "Unknown";
    }
    if ((usedPagingFileSpace == "") || (usedPagingFileSpace == " KB")) {
        usedPagingFileSpace = "Unknown";
    }
    return usedPagingFileSpace;
}

std::string OSInfo::currentDateTime() const
{
    std::string currentDateTime{ "" };
    SystemCommand systemCommand{ CURRENT_DATE_TIME_QUERY_STRING };
    systemCommand.execute();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw{ systemCommand.outputAsVector() };
        if (raw.empty()) {
            currentDateTime = "Unknown";
        }
        int osInfoNumber = 0;
        for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
            if (osInfoNumber == this->_osNumber) {
                if ((iter->find(CURRENT_DATE_TIME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CURRENT_DATE_TIME_IDENTIFIER_STRING) == 0)) {
                    size_t foundPosition = iter->find(CURRENT_DATE_TIME_IDENTIFIER_STRING);
                    std::string currentDateTimeString = iter->substr(foundPosition + CURRENT_DATE_TIME_IDENTIFIER_STRING.length());

                }
            }
            osInfoNumber++;
        }
    }
    else {
        currentDateTime = "Unknown";
    }
    if (currentDateTime == "") {
        currentDateTime = "Unknown";
    }
    return currentDateTime;
}

const std::string OSInfoDelegate::OS_INFO_QUERY_STRING = "wmic os get /format:list";
const std::string OSInfoDelegate::OS_INSTANCE_QUERY_STRING = "PortableOperatingSystem=";
const std::string OSInfoDelegate::OS_INFO_END_IDENTIFIER_STRING = "WindowsDirectory=";

OSInfoDelegate::OSInfoDelegate() :
    _numberOfOSInfoItems{ 0 }
{
    SystemCommand systemCommand{ OS_INFO_QUERY_STRING };
    systemCommand.execute();
    std::vector<std::string> tempVector = systemCommand.outputAsVector();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw = { systemCommand.outputAsVector() };
        determineNumberOfOSInfoItems(raw);
        std::vector<std::string> singleOSInfoItem;
        std::vector<std::string>::const_iterator iter = raw.begin();
        int osNumber = 0;
        while (osNumber < this->_numberOfOSInfoItems) {
            while (iter->find(OS_INFO_END_IDENTIFIER_STRING) == std::string::npos) {
                if ((*iter != "") && (*iter != "\r")) {
                    singleOSInfoItem.push_back(*iter);
                }
                iter++;
            }
            singleOSInfoItem.push_back(*iter);
            this->_osInfoVector.emplace_back(singleOSInfoItem, osNumber);
            singleOSInfoItem.clear();
            iter++;
            osNumber++;
        }
    }
}

void OSInfoDelegate::determineNumberOfOSInfoItems(const std::vector<std::string>& data)
{
    for (auto iter = data.begin(); iter != data.end(); iter++) {
        if (iter->find(OS_INSTANCE_QUERY_STRING) != std::string::npos) {
            this->_numberOfOSInfoItems++;
        }
    }
}

int OSInfoDelegate::numberOfOSInfoItems() const
{
    return this->_numberOfOSInfoItems;
}

std::vector<OSInfo> OSInfoDelegate::osInfoVector() const
{
    return this->_osInfoVector;
}

const std::string RAMInfo::NAME_IDENTIFIER_STRING = "DeviceLocator=";
const std::string RAMInfo::MANUFACTURER_IDENTIFIER_STRING = "Manufacturer=";
const std::string RAMInfo::CAPACITY_IDENTIFIER_STRING = "Capacity=";
const std::string RAMInfo::SERIAL_NUMBER_IDENTIFIER_STRING = "SerialNumber=";
const std::string RAMInfo::FORM_FACTOR_IDENTIFIER_STRING = "FormFactor=";
const std::string RAMInfo::PART_NUMBER_IDENTIFIER_STRING = "PartNumber=";
const std::string RAMInfo::MEMORY_TYPE_IDENTIFIER_STRING = "MemoryType=";
const std::string RAMInfo::CLOCK_SPEED_IDENTIFIER_STRING = "ConfiguredClockSpeed=";
const std::string RAMInfo::BACKUP_CLOCK_SPEED_IDENTIFIER_STRING = "Speed=";

const int RAMInfo::BYTES_PER_MEGABYTE = 1000000;

RAMInfo::RAMInfo(const std::vector<std::string>& rawData, int ramNumber) :
    _name{ "" },
    _manufacturer{ "" },
    _capacity{ "" },
    _serialNumber{ "" },
    _formFactor{ "" },
    _partNumber{ "" },
    _memoryType{ "" },
    _clockSpeed{ "" },
    _ramNumber{ ramNumber }
{
    std::string backupClockSpeed{ "" };
    for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {

        //Name
        if ((iter->find(NAME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NAME_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NAME_IDENTIFIER_STRING);
            this->_name = iter->substr(foundPosition + NAME_IDENTIFIER_STRING.length());
        }

        //Manufacturer
        if ((iter->find(MANUFACTURER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(MANUFACTURER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(MANUFACTURER_IDENTIFIER_STRING);
            this->_manufacturer = iter->substr(foundPosition + MANUFACTURER_IDENTIFIER_STRING.length());
        }

        //Capacity
        if ((iter->find(CAPACITY_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CAPACITY_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(CAPACITY_IDENTIFIER_STRING);
            std::string capacityString = iter->substr(foundPosition + CAPACITY_IDENTIFIER_STRING.length());
            if (capacityString == "") {
                this->_capacity = "";
                continue;
            }
            else {
                long long int capacity{ 0 };
                try {
                    capacity = std::stoll(capacityString);
                    this->_capacity = toString(capacity / BYTES_PER_MEGABYTE) + "MB (" + toString(capacity) + " Bytes)";
                }
                catch (std::exception& e) {
                    (void)e;
                    this->_capacity = capacityString + " Bytes";
                }
            }
        }

        //Serial Number
        if ((iter->find(SERIAL_NUMBER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(SERIAL_NUMBER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(SERIAL_NUMBER_IDENTIFIER_STRING);
            this->_serialNumber = iter->substr(foundPosition + SERIAL_NUMBER_IDENTIFIER_STRING.length());
        }

        //Form Factor (SODIMM, DIMM, etc)
        if ((iter->find(FORM_FACTOR_IDENTIFIER_STRING) != std::string::npos) && (iter->find(FORM_FACTOR_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(FORM_FACTOR_IDENTIFIER_STRING);
            std::string formFactorString = iter->substr(foundPosition + FORM_FACTOR_IDENTIFIER_STRING.length());
            this->_formFactor = getFormFactor(formFactorString);
        }

        //Part Number
        if ((iter->find(PART_NUMBER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(PART_NUMBER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(PART_NUMBER_IDENTIFIER_STRING);
            this->_partNumber = iter->substr(foundPosition + PART_NUMBER_IDENTIFIER_STRING.length());
        }

        //Memory Type (DRAM, SDRAM, etc)
        if ((iter->find(MEMORY_TYPE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(MEMORY_TYPE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(MEMORY_TYPE_IDENTIFIER_STRING);
            std::string memoryTypeString = iter->substr(foundPosition + MEMORY_TYPE_IDENTIFIER_STRING.length());
            this->_memoryType = getMemoryType(memoryTypeString);
        }

        //Clock Speed
        if ((iter->find(CLOCK_SPEED_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CLOCK_SPEED_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(CLOCK_SPEED_IDENTIFIER_STRING);
            this->_clockSpeed = iter->substr(foundPosition + CLOCK_SPEED_IDENTIFIER_STRING.length()) + "MHz";
            if (this->_clockSpeed == "MHz") {
                this->_clockSpeed = "";
            }
        }

        //Backup clock speed
        if ((iter->find(BACKUP_CLOCK_SPEED_IDENTIFIER_STRING) != std::string::npos) && (iter->find(BACKUP_CLOCK_SPEED_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(BACKUP_CLOCK_SPEED_IDENTIFIER_STRING);
            backupClockSpeed = iter->substr(foundPosition + BACKUP_CLOCK_SPEED_IDENTIFIER_STRING.length()) + "MHz";
            if (backupClockSpeed == "MHz") {
                backupClockSpeed = "";
            }
        }
    }
    //In case any of these values are missing or don't get assigned
    if (this->_name == "") {
        this->_name = "Unknown";
    }
    if (this->_manufacturer == "") {
        this->_manufacturer = "Unknown";
    }
    if ((this->_capacity == "") || (this->_capacity == " Bytes")) {
        this->_capacity = "Unknown";
    }
    if (this->_serialNumber == "") {
        this->_serialNumber = "Unknown";
    }
    if (this->_partNumber == "") {
        this->_partNumber = "Unknown";
    }
    if ((this->_clockSpeed == "") && (backupClockSpeed == "")) {
        this->_clockSpeed = "Unknown";
    }
    else if (this->_clockSpeed == "") {
        this->_clockSpeed = backupClockSpeed;
    }
}

std::string RAMInfo::name() const
{
    return this->_name;
}

std::string RAMInfo::manufacturer() const
{
    return this->_manufacturer;
}

std::string RAMInfo::capacity() const
{
    return this->_capacity;
}

std::string RAMInfo::serialNumber() const
{
    return this->_serialNumber;
}

std::string RAMInfo::formFactor() const
{
    return this->_formFactor;
}

std::string RAMInfo::partNumber() const
{
    return this->_partNumber;
}

std::string RAMInfo::memoryType() const
{
    return this->_memoryType;
}

std::string RAMInfo::clockSpeed() const
{
    return this->_clockSpeed;
}

int RAMInfo::ramNumber() const
{
    return this->_ramNumber;
}

std::string RAMInfo::getMemoryType(const std::string& memoryTypeString) const
{
    int memoryType{ 0 };
    try {
        memoryType = std::stoi(memoryTypeString);
    }
    catch (std::exception& e) {
        (void)e;
        memoryType = 0;
    }
    //As per https://msdn.microsoft.com/en-us/library/aa394347(v=vs.85).aspx
    switch (memoryType) {
    case 0: return "Unknown";
    case 1: return "Other";
    case 2: return "DRAM";
    case 3: return "Syncronous DRAM";
    case 4: return "Cache DRAM";
    case 5: return "EDO";
    case 6: return "EDRAM";
    case 7: return "VRAM";
    case 8: return "SRAM";
    case 9: return "RAM";
    case 10: return "ROM";
    case 11: return "Flash";
    case 12: return "EEPROM";
    case 13: return "FEPROM";
    case 14: return "EPROM";
    case 15: return "CDRAM";
    case 16: return "3DRAM";
    case 17: return "SDRAM";
    case 18: return "SGRAM";
    case 19: return "RDRAM";
    case 20: return "DDR1";
    case 21: return "DDR2";
    case 22: return "DDR2 - FBDIMM";
    case 23: return "DDR2 - FBDIMM";
    case 24: return "DDR3";
    case 25: return "FBD2";
    default: return "Unknown";
    }
}

std::string RAMInfo::getFormFactor(const std::string& formFactorString) const
{
    int formFactor{ 0 };
    try {
        formFactor = std::stoi(formFactorString);
    }
    catch (std::exception& e) {
        (void)e;
        formFactor = 0;
    }
    //As per https://msdn.microsoft.com/en-us/library/aa394347(v=vs.85).aspx
    switch (formFactor) {
    case 0: return "Unknown";
    case 1: return "Other";
    case 2: return "SIP";
    case 3: return "DIP";
    case 4: return "ZIP";
    case 5: return "SOJ";
    case 6: return "Proprietary";
    case 7: return "SIMM";
    case 8: return "DIMM";
    case 9: return "TSOP";
    case 10: return "PGA";
    case 11: return "RIMM";
    case 12: return "SODIMM";
    case 13: return "SRIMM";
    case 14: return "SMD";
    case 15: return "SSMP";
    case 16: return "QFD";
    case 17: return "TQFP";
    case 18: return "SOIC";
    case 19: return "LCC";
    case 20: return "PLCC";
    case 21: return "BGA";
    case 22: return "FPBGA";
    case 23: return "LGA";
    default: return "Unknown";
    }
}

const std::string RAMInfoDelegate::RAM_INFO_QUERY_STRING = "wmic memorychip get /format: list";
const std::string RAMInfoDelegate::RAM_INSTANCE_QUERY_STRING = "ConfiguredClockSpeed=";
const std::string RAMInfoDelegate::RAM_INFO_END_IDENTIFIER_STRING = "Version=";

RAMInfoDelegate::RAMInfoDelegate() :
    _numberOfRAMInfoItems{ 0 }
{
    SystemCommand systemCommand{ RAM_INFO_QUERY_STRING };
    systemCommand.execute();
    std::vector<std::string> tempVector = systemCommand.outputAsVector();
    if (!systemCommand.hasError()) {
        std::vector<std::string> raw = { systemCommand.outputAsVector() };
        determineNumberOfRAMInfoItems(raw);
        std::vector<std::string> singleRAMInfoItem;
        std::vector<std::string>::const_iterator iter = raw.begin();
        int ramNumber = 0;
        while (ramNumber < this->_numberOfRAMInfoItems) {
            while (iter->find(RAM_INFO_END_IDENTIFIER_STRING) == std::string::npos) {
                if ((*iter != "") && (*iter != "\r")) {
                    singleRAMInfoItem.push_back(*iter);
                }
                iter++;
            }
            singleRAMInfoItem.push_back(*iter);
            this->_ramInfoVector.emplace_back(singleRAMInfoItem, ramNumber);
            singleRAMInfoItem.clear();
            iter++;
            ramNumber++;
        }
    }
}

void RAMInfoDelegate::determineNumberOfRAMInfoItems(const std::vector<std::string>& data)
{
    for (auto iter = data.begin(); iter != data.end(); iter++) {
        if (iter->find(RAM_INSTANCE_QUERY_STRING) != std::string::npos) {
            this->_numberOfRAMInfoItems++;
        }
    }
}

int RAMInfoDelegate::numberOfRAMInfoItems() const
{
    return this->_numberOfRAMInfoItems;
}

std::vector<RAMInfo> RAMInfoDelegate::ramInfoVector() const
{
    return this->_ramInfoVector;
}

const int SystemCommand::_DEFAULT_MAX_OUTPUT = 2147483647;

SystemCommand::SystemCommand(std::initializer_list<std::string> command) :
    _command{ *std::begin(command) },
    _maxOutput{ _DEFAULT_MAX_OUTPUT },
    _hasError{ false },
    _repeatCommand{ false }
{

}

SystemCommand::SystemCommand() :
    _command{ "" },
    _maxOutput{ _DEFAULT_MAX_OUTPUT },
    _hasError{ false },
    _repeatCommand{ false }
{

}

SystemCommand::SystemCommand(const std::string& command) :
    _command{ command },
    _maxOutput{ _DEFAULT_MAX_OUTPUT },
    _hasError{ false },
    _repeatCommand{ false }
{

}

SystemCommand::SystemCommand(const std::string& command, int maxOutput) :
    _command{ command },
    _maxOutput{ maxOutput },
    _hasError{ false },
    _repeatCommand{ false }
{

}


void SystemCommand::printCommand()
{
    std::cout << this->_command << std::endl;
}

void SystemCommand::insertIntoCommand(int position, const std::string& stringToInsert)
{
    if (static_cast<unsigned int>(position) > this->_command.length()) {
        return;
    }
    this->_command.insert(position, stringToInsert);
    this->_repeatCommand = false;
}

void SystemCommand::insertIntoCommand(int position, char charToInsert)
{
    std::string temp = "";
    temp += charToInsert;
    this->insertIntoCommand(position, temp);
}

int SystemCommand::maxOutput()
{
    return this->_maxOutput;
}

std::string SystemCommand::command()
{
    return this->_command;
}

bool SystemCommand::hasError()
{
    return this->_hasError;
}

void SystemCommand::appendToCommand(const std::string& stringToAppend)
{
    _command += stringToAppend;
    this->_repeatCommand = false;
}

int SystemCommand::returnValue()
{
    return this->_returnValue;
}

void SystemCommand::setCommand(const std::string& command)
{
    this->_repeatCommand = false;
    this->_command = command;
    this->_hasError = false;
    this->_sizeOfOutput = 0;
    this->_outputAsVector.clear();
}

void SystemCommand::setMaxOutput(int maxOutput)
{
    this->_maxOutput = maxOutput;
}

std::string SystemCommand::outputAsString()
{
    std::string returnString = "";
    for (std::vector<std::string>::const_iterator iter = _outputAsVector.begin(); iter != _outputAsVector.end(); iter++) {
        returnString += (*iter);
    }
    return returnString;
}

void SystemCommand::stripShellControlCharactersFromCommand()
{
    //TODO: Implement
    return;
}

std::vector<std::string> SystemCommand::outputAsVector()
{
    return this->_outputAsVector;
}

void SystemCommand::execute()
{
    this->systemCommandLaunch(_WITH_PIPE);
}

std::vector<std::string> SystemCommand::executeAndWaitForOutputAsVector()
{
    this->systemCommandLaunch(_WITH_PIPE);
    return this->_outputAsVector;
}

void SystemCommand::executeWithoutPipe()
{
    this->systemCommandLaunch(_WITHOUT_PIPE);
}

std::string SystemCommand::stripAllFromString(const std::string& stringToStrip, const std::string& whatToStrip)
{
    std::string returnString = stringToStrip;
    if (returnString.find(whatToStrip) == std::string::npos) {
        return returnString;
    }
    while (returnString.find(whatToStrip) != std::string::npos) {
        returnString = stripFromString(returnString, whatToStrip);
    }
    return returnString;
}


std::string SystemCommand::stripFromString(const std::string& stringToStrip, const std::string& whatToStrip)
{
    std::string returnString = stringToStrip;
    if (returnString.find(whatToStrip) == std::string::npos) {
        return returnString;
    }
    size_t foundPosition = stringToStrip.find(whatToStrip);
    if (foundPosition == 0) {
        returnString = returnString.substr(whatToStrip.length());
    }
    else if (foundPosition == (returnString.length() - whatToStrip.length())) {
        returnString = returnString.substr(0, foundPosition);
    }
    else {
        returnString = returnString.substr(0, foundPosition) + returnString.substr(foundPosition + whatToStrip.length());
    }
    return returnString;
}

void SystemCommand::stripPipeFromCommand()
{
#if defined(_WIN32) && defined(_MSC_VER)
    _command = stripAllFromString(_command, "2>&1");
    _command = stripAllFromString(_command, ">");
#else
    _command = stripAllFromString(_command, "2>&1");
    _command = stripAllFromString(_command, ">");
#endif
}


std::string SystemCommand::executeAndWaitForOutputAsString()
{
    this->systemCommandLaunch(_WITH_PIPE);
    return this->outputAsString();
}

int SystemCommand::sizeOfOutput()
{
    return this->_sizeOfOutput;
}

void SystemCommand::verifyValidMaxOutput()
{
    if ((_maxOutput <= 8) || (_maxOutput > std::numeric_limits<int>::max())) {
        _maxOutput = _DEFAULT_MAX_OUTPUT;
    }
}

FILE* SystemCommand::popenHandler(const std::string& directory, const std::string& fileMode)
{
#if defined(_WIN32) && defined(_MSC_VER)
    return _popen(directory.c_str(), fileMode.c_str());
#else
    return popen(directory.c_str(), fileMode.c_str());
#endif
}

int SystemCommand::pcloseHandler(FILE* filePtr)
{
#if defined(_WIN32) && defined(_MSC_VER)
    int returnValue = _pclose(filePtr);
    return returnValue;
#else
    int returnValue = pclose(filePtr);
    return returnValue == -1 ? -1 : returnValue / 256;
#endif
}

void SystemCommand::systemCommandLaunch(bool withPipe)
{
    stripPipeFromCommand();
    if (this->_repeatCommand) {
        this->_hasError = false;
        this->_sizeOfOutput = 0;
        this->_outputAsVector.clear();
    }
    else {
        this->_repeatCommand = true;
    }
    this->_command += " 2>&1"; //Merges stderror with stdout
    verifyValidMaxOutput();
    if (withPipe) {
        FILE* fp;
        char path[PATH_MAX];

        fp = popenHandler(this->_command.c_str(), "r");
        if (fp == NULL) {
            std::cout << "ERROR: Failed to execute command \"" << this->_command << "\"" << std::endl;
            this->_returnValue = -1;
            return;
        }
        int outputSize{ 0 };
        while ((fgets(path, PATH_MAX, fp) != NULL) && (outputSize <= this->_maxOutput)) {
            std::string formattedPath{ std::string(path) };
            addFormattedThing(this->_outputAsVector, formattedPath, [](const std::string& stringToStrip) -> std::string
                {
                    std::string returnString{ stringToStrip };
                    std::vector<std::string> newLines{ "\r\n", "\n\r", "\n" };
                    for (std::vector<std::string>::const_iterator iter = newLines.begin(); iter != newLines.end(); iter++) {
                        if (returnString.find(*iter) != std::string::npos) {
                            size_t foundPosition = returnString.find(*iter);
                            returnString = returnString.substr(0, foundPosition);
                        }
                    }
                    return returnString;
                });
            outputSize += sizeof(*(std::end(_outputAsVector) - 1));
        }
        this->_returnValue = pcloseHandler(fp);
    }
    else {
        this->_returnValue = system(this->_command.c_str());
    }
    this->_hasError = (this->_returnValue != 0);
}