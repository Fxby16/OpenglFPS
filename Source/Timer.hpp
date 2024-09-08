#pragma once

#include <chrono>

class Timer{
public:
    Timer(const char* name);
    ~Timer();

    double GetMillisecondsElapsed();
    void PrintTime();

private:
    int Hash(const char* str);

    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
    int m_ID;
    const char* m_Name;
};

extern void ShouldDisplayTimers(bool shouldDisplay);
extern void FreeRemainingTimers();