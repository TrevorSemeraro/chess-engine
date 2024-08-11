#include <chrono>
#include "timer.h"
#include "pch.h"

CustomTimer::CustomTimer() {
    this->bPaused = false;
    this->runTime = 0;
    this->startTime = std::chrono::system_clock::now();
}

void CustomTimer::Reset() {
    this->runTime = 0;
    this->bPaused = false;
}

void CustomTimer::Start() {
    if (this->bPaused) {
        this->startTime = std::chrono::system_clock::now();
    }
    this->bPaused = false;
}

void CustomTimer::Pause() {
    if (!this->bPaused) {
        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = endTime - startTime;

        this->runTime += elapsed_seconds.count();
    }
    this->bPaused = true;
}

float CustomTimer::GetElapsedSeconds() {
    if (!bPaused) {
        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = endTime - startTime;

        return runTime + elapsed_seconds.count();
    }
    return runTime;
}
