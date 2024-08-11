#pragma once

#ifndef TIMER_H
#define TIMER_H
#include <chrono>

class CustomTimer{
public:
    float runTime;
    bool bPaused;
    std::chrono::system_clock::time_point startTime;

    CustomTimer();

    void Reset();

    void Start();

    void Pause();

    float GetElapsedSeconds();
};


#endif