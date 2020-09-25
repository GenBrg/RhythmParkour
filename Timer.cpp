#include "Timer.hpp"

void TimerManager::AddTimer(long long duration, const std::function<void()>& callback)
{
    auto expire_time = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(duration);
    timers_.emplace_back(callback, expire_time);
}

void TimerManager::Update()
{
    while (!timers_.empty()) {
        auto& timer = timers_.front();
        
        if (timer.IsExpired()) {
            timer();
            timers_.pop_front();
        } else {
            break;
        }
    }
}


