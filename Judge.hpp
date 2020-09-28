#pragma once

#include <chrono>
#include <vector>

struct PlayMode;
class Overlay;

class Judge {
private:
    std::chrono::high_resolution_clock::time_point music_start_time_;
    std::vector<std::chrono::milliseconds> beat_times_;
    PlayMode* playmode_;
    size_t next_beat_idx_;

    int score_ { 0 };
    int max_score_ { 0 };

    int perfect_num_ { 0 };
    int good_num_ { 0 };
    int poor_num_ { 0 };
    int miss_num_ { 0 };

public:
    Judge(PlayMode* playmode);
    void AddBeat(float beat_time);
    void Update(bool jump);
    void StartTimer();
    void AddScoreText();
    void AddFinalScoreText();
};
