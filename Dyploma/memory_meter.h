#ifndef MEMORY_METER_H
#define MEMORY_METER_H

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>

inline size_t getCurrentMemoryKB() {
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return info.WorkingSetSize / 1024;
}

#else
#include <unistd.h>
#include <fstream>
#include <string>

inline size_t getCurrentMemoryKB() {
    std::ifstream statm("/proc/self/status");
    std::string line;
    while (std::getline(statm, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            size_t kb = 0;
            sscanf(line.c_str(), "VmRSS: %lu kB", &kb);
            return kb;
        }
    }
    return 0;
}
#endif

#endif
