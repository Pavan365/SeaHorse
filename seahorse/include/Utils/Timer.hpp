#pragma once
#include "include/Utils/Globals.hpp"
#include <chrono>

// Global timer
class Timer {
private:
    std::chrono::system_clock::time_point start;

public:
    Timer()
        : start(std::chrono::system_clock::now()) {};
    void Start() { start = std::chrono::system_clock::now(); };
    void Stop(std::string msg = "")
    {
        S_INFO(Elapsed(), " seconds, ", msg);
    };
    double Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() / 1e6;
    }
};