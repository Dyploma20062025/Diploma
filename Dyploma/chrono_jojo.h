#pragma once
#include <chrono>

class JojoChrono {
    std::chrono::steady_clock::time_point start_time;
public:
    void start() {
        start_time = std::chrono::steady_clock::now();
    }
    double stop() {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> ms = end - start_time;
        return ms.count();
    }
};
