#include <vector>
#include <string>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <ctime>
#include <cstdio>
#include <cassert>
#include <functional>
#include <cstdlib>

#define PATH_MAX 128

class SystemCommand
{
public:

    SystemCommand();
    SystemCommand(std::initializer_list<std::string> command);
    SystemCommand(const std::string& command);
    SystemCommand(const std::string& command, int maxOutput);

    void execute();
    void executeWithoutPipe();
    std::vector<std::string> executeAndWaitForOutputAsVector();
    std::string executeAndWaitForOutputAsString();
    void insertIntoCommand(int position, const std::string& stringToInsert);
    void insertIntoCommand(int position, char charToInsert);
    void printCommand();

    int maxOutput();

    std::string command();
    bool hasError();
    int returnValue();
    int sizeOfOutput();

    std::string outputAsString();
    std::vector<std::string> outputAsVector();

    void setCommand(const std::string& command);
    void appendToCommand(const std::string& stringToAppend);
    void setMaxOutput(int maxOutput);


private:
    int _returnValue;
    std::string _command;
    int _maxOutput;
    bool _hasError;
    int _sizeOfOutput;
    bool _repeatCommand;
    std::ifstream _readFromFile;
    std::vector<std::string> _outputAsVector;
    const static int  _DEFAULT_MAX_OUTPUT;

    void verifyValidMaxOutput();
    void stripShellControlCharactersFromCommand();
    void systemCommandLaunch(bool withPipe);

    FILE* popenHandler(const std::string& directory, const std::string& fileMode);
    int pcloseHandler(FILE* filePtr);

    const bool _WITH_PIPE = true;
    const bool _WITHOUT_PIPE = false;

    std::string stripAllFromString(const std::string& stringToStrip, const std::string& whatToStrip);
    std::string stripFromString(const std::string& stringToStrip, const std::string& whatToStrip);
    void stripPipeFromCommand();

    template <typename T>
    std::string toString(const T& thingToConvert)
    {
        std::stringstream stringTransfer;
        std::string returnString;
        stringTransfer << thingToConvert;
        stringTransfer >> returnString;
        return returnString;
    }

    void addFormattedThing(std::vector<std::string>& container, const std::string& thingToAdd, const std::function <std::string(const std::string&)>& lambdaFunction) {
        container.push_back(lambdaFunction(thingToAdd));
    }

    template <typename T>
    void logDataMember(const std::string& nameOfDataMemberToLog, const T& dataMemberToLog) { std::cout << nameOfDataMemberToLog << " = " << toString(dataMemberToLog) << std::endl; }

    template <typename T>
    void logDataMember(const T& dataMemberToLog) { std::cout << toString(dataMemberToLog) << std::endl; }

    void logString(const std::string& stringToLog) { std::cout << stringToLog << std::endl; }
};

class CPUInfo
{
public:
    CPUInfo::CPUInfo(const std::vector<std::string>& rawData, int cpuNumber);
    std::string name() const;
    std::string manufacturer() const;
    std::string numberOfCores() const;
    std::string architecture() const;
    std::string L2CacheSize() const;
    std::string L3CacheSize() const;
    std::string currentTemperature() const;
    std::string currentClockSpeed() const;
    int cpuNumber() const;

private:
    std::string _name;
    std::string _manufacturer;
    std::string _numberOfCores;
    std::string _architecture;
    std::string _L2CacheSize;
    std::string _L3CacheSize;
    int _cpuNumber;

    int kelvinToCelcius(int tempInKelvin) const;
    std::string getArchitecture(std::string& dataWidth) const;

    template <typename T>
    std::string toString(const T& convert) const
    {
        std::stringstream transfer;
        std::string returnString;
        transfer << convert;
        transfer >> returnString;
        return returnString;
    }

    static const std::string TEMPERATURE_QUERY_STRING;
    static const std::string TEMPERATURE_ERROR_IDENTIFIER_STRING;
    static const std::string NAME_IDENTIFIER_STRING;
    static const std::string NUMBER_OF_CORES_IDENTIFIER_STRING;
    static const std::string MANUFACTURER_IDENTIFIER_STRING;
    static const std::string CLOCK_SPEED_QUERY_STRING;
    static const std::string CURRENT_CLOCK_SPEED_IDENTIFIER_STRING;
    static const std::string ARCHITECTURE_IDENTIFIER_STRING;
    static const std::string L2_CACHE_SIZE_IDENTIFIER_STRING;
    static const std::string L3_CACHE_SIZE_IDENTIFIER_STRING;
};

class CPUInfoDelegate
{
public:
    CPUInfoDelegate();
    std::vector<CPUInfo> cpuInfoVector() const;
    int numberOfCPUInfoItems() const;
private:
    std::vector<CPUInfo> _cpuInfoVector;
    int _numberOfCPUInfoItems;

    void determineNumberOfCPUInfoItems(const std::vector<std::string>& data);
    static const std::string CPU_INFO_QUERY_STRING;
    static const std::string CPU_INSTANCE_QUERY_STRING;
    static const std::string CPU_INFO_END_IDENTIFIER_STRING;
};

class GPUInfo
{
public:
    GPUInfo::GPUInfo(const std::vector<std::string>& rawData, int cpuNumber);
    std::string name() const;
    std::string manufacturer() const;
    std::string caption() const;
    std::string adapterRAM() const;
    std::string refreshRate() const;
    std::string driverVersion() const;
    std::string videoArchitecture() const;
    std::string videoMemoryType() const;
    std::string videoModeDescription() const;
    std::string videoProcessor() const;
    int gpuNumber() const;

private:
    std::string _name;
    std::string _manufacturer;
    std::string _caption;
    std::string _adapterRAM;
    std::string _refreshRate;
    std::string _driverVersion;
    std::string _videoArchitecture;
    std::string _videoMemoryType;
    std::string _videoModeDescription;
    std::string _videoProcessor;
    int _gpuNumber;

    std::string getVideoArchitecture(const std::string& videoArchitectureString) const;
    std::string getVideoMemoryType(const std::string& videoMemoryTypeString) const;

    template <typename T>
    std::string toString(const T& convert) const
    {
        std::stringstream transfer;
        std::string returnString;
        transfer << convert;
        transfer >> returnString;
        return returnString;
    }

    static const std::string NVIDIA_IDENTIFIER_STRING;
    static const std::string INTEL_IDENTIFIER_STRING;
    static const std::string AMD_IDENTIFIER_STRING;
    static const std::string NAME_IDENTIFIER_STRING;
    static const std::string MANUFACTURER_IDENTIFIER_STRING;
    static const std::string ADAPTER_RAM_IDENTIFIER_STRING;
    static const std::string REFRESH_RATE_IDENTIFIER_STRING;
    static const std::string DRIVER_VERSION_IDENTIFIER_STRING;
    static const std::string VIDEO_ARCHITECTURE_IDENTIFIER_STRING;
    static const std::string VIDEO_MEMORY_TYPE_IDENTIFIER_STRING;
    static const std::string VIDEO_MODE_DESCRIPTION_IDENTIFIER_STRING;
    static const std::string VIDEO_PROCESSOR_IDENTIFIER_STRING;
    static const std::string CAPTION_IDENTIFIER_STRING;
};

class GPUInfoDelegate
{
public:
    GPUInfoDelegate();
    std::vector<GPUInfo> gpuInfoVector() const;
    int numberOfGPUInfoItems() const;
private:
    std::vector<GPUInfo> _gpuInfoVector;
    int _numberOfGPUInfoItems;

    void determineNumberOfGPUInfoItems(const std::vector<std::string>& data);
    static const std::string GPU_INFO_QUERY_STRING;
    static const std::string GPU_INSTANCE_QUERY_STRING;
    static const std::string GPU_INFO_END_IDENTIFIER_STRING;
};

class MotherboardInfo
{
public:
    MotherboardInfo(const std::vector<std::string>& rawData, int motherboardNumber);
    std::string name() const;
    std::string manufacturer() const;
    std::string chipset() const;
    std::string serialNumber() const;
    std::string version() const;
    int motherboardNumber() const;

private:
    std::string _name;
    std::string _manufacturer;
    std::string _chipset;
    std::string _serialNumber;
    std::string _version;
    int _motherboardNumber;

    template <typename T>
    std::string toString(const T& convert) const
    {
        std::stringstream transfer;
        std::string returnString;
        transfer << convert;
        transfer >> returnString;
        return returnString;
    }

    void determineChipset();

    static const std::string CHIPSET_QUERY_STRING;
    static const std::string NAME_IDENTIFIER_STRING;
    static const std::string MANUFACTURER_IDENTIFIER_STRING;
    static const std::string CHIPSET_IDENTIFIER_STRING;
    static const std::string CHIPSET_END_IDENTIFIER_STRING;
    static const std::string SERIAL_NUMBER_IDENTIFIER_STRING;
    static const std::string VERSION_IDENTIFIER_STRING;
};

class MotherboardInfoDelegate
{
public:
    MotherboardInfoDelegate();
    std::vector<MotherboardInfo> motherboardInfoVector() const;
    int numberOfMotherboardInfoItems() const;

private:
    std::vector<MotherboardInfo> _motherboardInfoVector;
    int _numberOfMotherboardInfoItems;

    void determineNumberOfMotherboardInfoItems(const std::vector<std::string>& data);

    static const std::string MOTHERBOARD_INFO_QUERY_STRING;
    static const std::string MOTHERBOARD_INSTANCE_QUERY_STRING;
    static const std::string MOTHERBOARD_INFO_END_IDENTIFIER_STRING;
};

class OSInfo
{
public:
    OSInfo::OSInfo(const std::vector<std::string>& rawData, int OSNumber);

    std::string name() const;
    std::string manufacturer() const;
    std::string architecture() const;
    std::string caption() const;
    std::string version() const;
    std::string currentUser() const;
    std::string installDate() const;
    std::string buildNumber() const;
    std::string lastBootUpTime() const;
    std::string bootDevice() const;
    std::string serialNumber() const;
    std::string totalVirtualMemory() const;
    std::string totalVisibleMemory() const;
    std::string totalSwapSize() const;
    int osNumber() const;

    std::string freePhysicalMemory() const;
    std::string freeVirtualMemory() const;
    std::string freePagingFileSpace() const;
    std::string usedPagingFileSpace() const;
    std::string currentDateTime() const;

private:
    std::string _name;
    std::string _manufacturer;
    std::string _architecture;
    std::string _caption;
    std::string _version;
    std::string _currentUser;
    std::string _installDate;
    std::string _buildNumber;
    std::string _lastBootUpTime;
    std::string _bootDevice;
    std::string _serialNumber;
    std::string _totalVirtualMemory;
    std::string _totalVisibleMemory;
    std::string _totalSwapSize;

    int _osNumber;

    template <typename T>
    std::string toString(const T& convert) const
    {
        std::stringstream transfer;
        std::string returnString;
        transfer << convert;
        transfer >> returnString;
        return returnString;
    }

    static const std::string NAME_IDENTIFIER_STRING;
    static const std::string MANUFACTURER_IDENTIFIER_STRING;
    static const std::string ARCHITECTURE_IDENTIFIER_STRING;
    static const std::string CAPTION_INDENTIFIER_STRING;
    static const std::string VERSION_IDENTIFIER_STRING;
    static const std::string CURRENT_USER_IDENTIFIER_STRING;
    static const std::string INSTALL_DATE_IDENTIFIER_STRING;
    static const std::string BUILD_NUMBER_IDENTIFIER_STRING;
    static const std::string BOOT_DEVICE_IDENTIFIER_STRING;
    static const std::string TOTAL_VIRTUAL_MEMORY_SIZE_IDENTIFIER_STRING;
    static const std::string TOTAL_VISIBLE_MEMORY_SIZE_IDENTIFIER_STRING;
    static const std::string LAST_BOOT_UP_TIME_IDENTIFIER_STRING;
    static const std::string SERIAL_NUMBER_IDENTIFIER_STRING;
    static const std::string TOTAL_SWAP_SIZE_IDENTIFIER_STRING;

    static const std::string FREE_PHYSICAL_MEMORY_IDENTIFIER_STRING;
    static const std::string FREE_PHYSICAL_MEMORY_QUERY_STRING;

    static const std::string FREE_VIRTUAL_MEMORY_IDENTIFIER_STRING;
    static const std::string FREE_VIRTUAL_MEMORY_QUERY_STRING;

    static const std::string FREE_SPACE_IN_PAGING_FILES_IDENTIFIER_STRING;
    static const std::string FREE_SPACE_IN_PAGING_FILES_QUERY_STRING;

    static const std::string USED_SPACE_IN_PAGING_FILE_IDENTIFIER_STRING;
    static const std::string USED_SPACE_IN_PAGING_FILE_QUERY_STRING;

    static const std::string CURRENT_DATE_TIME_IDENTIFIER_STRING;
    static const std::string CURRENT_DATE_TIME_QUERY_STRING;

    static const int KILOBYTES_PER_MEGABYTE;

};

class OSInfoDelegate
{
public:
    OSInfoDelegate();
    std::vector<OSInfo> osInfoVector() const;
    int numberOfOSInfoItems() const;
private:
    std::vector<OSInfo> _osInfoVector;
    int _numberOfOSInfoItems;

    void determineNumberOfOSInfoItems(const std::vector<std::string>& data);
    static const std::string OS_INFO_QUERY_STRING;
    static const std::string OS_INSTANCE_QUERY_STRING;
    static const std::string OS_INFO_END_IDENTIFIER_STRING;
};

class RAMInfo
{
public:
    RAMInfo(const std::vector<std::string>& rawData, int ramNumber);

    std::string name() const;
    std::string manufacturer() const;
    std::string capacity() const;
    std::string serialNumber() const;
    std::string formFactor() const;
    std::string partNumber() const;
    std::string memoryType() const;
    std::string clockSpeed() const;
    int ramNumber() const;

private:

    std::string _name;
    std::string _manufacturer;
    std::string _capacity;
    std::string _serialNumber;
    std::string _formFactor;
    std::string _partNumber;
    std::string _memoryType;
    std::string _clockSpeed;
    int _ramNumber;

    std::string getMemoryType(const std::string& memoryTypeString) const;
    std::string getFormFactor(const std::string& formFactorString) const;

    template<typename T>
    std::string toString(const T& convert) const
    {
        std::stringstream transfer;
        std::string returnString;
        transfer << convert;
        transfer >> returnString;
        return returnString;
    }

    static const std::string NAME_IDENTIFIER_STRING;
    static const std::string MANUFACTURER_IDENTIFIER_STRING;
    static const std::string CAPACITY_IDENTIFIER_STRING;
    static const std::string SERIAL_NUMBER_IDENTIFIER_STRING;
    static const std::string FORM_FACTOR_IDENTIFIER_STRING;
    static const std::string PART_NUMBER_IDENTIFIER_STRING;
    static const std::string CLOCK_SPEED_IDENTIFIER_STRING;
    static const std::string MEMORY_TYPE_IDENTIFIER_STRING;
    static const std::string BACKUP_CLOCK_SPEED_IDENTIFIER_STRING;
    static const int BYTES_PER_MEGABYTE;
};

class RAMInfoDelegate
{
public:
    RAMInfoDelegate();

    std::vector<RAMInfo> ramInfoVector() const;
    int numberOfRAMInfoItems() const;
private:
    std::vector<RAMInfo> _ramInfoVector;
    int _numberOfRAMInfoItems;

    void determineNumberOfRAMInfoItems(const std::vector<std::string>& data);

    static const std::string RAM_INFO_QUERY_STRING;
    static const std::string RAM_INSTANCE_QUERY_STRING;
    static const std::string RAM_INFO_END_IDENTIFIER_STRING;

};