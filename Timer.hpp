#pragma once

#include <deque>
#include <chrono>
#include <functional>

class Timer {
private:
    std::function<void()> callback_;
    std::chrono::high_resolution_clock::time_point expire_time_;

public:
    Timer(const std::function<void()>& callback, std::chrono::high_resolution_clock::time_point expire_time) :
     callback_(callback),
     expire_time_(expire_time) {}

    void operator()() {
        callback_();
    }

    bool IsExpired() {
        return std::chrono::high_resolution_clock::now() >= expire_time_;
    }
};

class TimerManager {
private:
    std::deque<Timer> timers_;
    
public:
    TimerManager() {}
    void AddTimer(long long duration, const std::function<void()>& callback);
    void Update();
};
