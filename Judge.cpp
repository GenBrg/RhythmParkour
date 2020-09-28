#include "Judge.hpp"
#include "PlayMode.hpp"
#include "common_consts.hpp"
#include "Overlay.hpp"

#include <sstream>

static constexpr float kHalfBeatIntervalf = kBeatInterval / 2.0f;
static constexpr long long kHalfBeatInterval = static_cast<long long>(kHalfBeatIntervalf * 1000.0f);

Judge::Judge(PlayMode* playmode) :
playmode_(playmode),
next_beat_idx_(0)
{
    music_start_time_ = std::chrono::high_resolution_clock::now();
}

void Judge::AddBeat(float beat_time)
{
    beat_times_.emplace_back(std::chrono::milliseconds(static_cast<long long>(beat_time * 1000.0f)));
    max_score_ += 3;
}

void Judge::Update(bool jump)
{
    if (next_beat_idx_ >= beat_times_.size()) {
        return;
    }

    auto now = std::chrono::high_resolution_clock::now();
    float time_delta = 0.0f;

    while (true) {
        auto& next_beat_time = beat_times_[next_beat_idx_];

        time_delta = std::chrono::duration< float >(next_beat_time + music_start_time_ - now).count();

        // too early
        if (time_delta > kHalfBeatIntervalf) {
            return;
        }

        // miss
        if (time_delta < -kHalfBeatIntervalf) {
            ++next_beat_idx_;
            playmode_->overlay.AddText("miss", "Miss", glm::vec3(15.0f, 10.0f, 0.0f), 0, kHalfBeatInterval, kGreyColor);
            ++miss_num_;
        } else {
            break;
        }
    }

    // judge
    if (jump) {
        float error = std::abs(time_delta) / kHalfBeatIntervalf;
        if (error <= 0.3f) {
            playmode_->overlay.AddText("perfect", "Perfect", glm::vec3(15.0f, 10.0f, 0.0f), 0, kHalfBeatInterval, kPurpleColor);
            score_ += 3;
            ++perfect_num_;
        } else if (error <= 0.6f) {
            playmode_->overlay.AddText("good", "Good", glm::vec3(15.0f, 10.0f, 0.0f), 0, kHalfBeatInterval, kGreenColor);
            score_ += 2;
            ++good_num_;
        } else {
            playmode_->overlay.AddText("poor", "Poor", glm::vec3(15.0f, 10.0f, 0.0f), 0, kHalfBeatInterval, kRedColor);
            ++score_;
            ++poor_num_;
        }
        ++next_beat_idx_;
    }
}

void Judge::StartTimer()
{
    music_start_time_ = std::chrono::high_resolution_clock::now();
}

void Judge::AddScoreText()
{
    std::ostringstream oss;
    oss << "Score: " << score_;
    playmode_->overlay.AddText("score", oss.str(), glm::vec3(25.0f, 15.0f, 0.0f));
}

void Judge::AddFinalScoreText()
{
    std::string assesment;
    float score_percentage = static_cast<float>(score_) / max_score_;

    if (score_percentage < 0.6f) {
        assesment = "Poor...";
    } else if (score_percentage < 0.8f) {
        assesment = "Good!";
    } else {
        assesment = "Perfect!";
    }

    std::ostringstream final_score_text1;
    std::ostringstream final_score_text2;
    std::ostringstream final_score_text3;
    final_score_text1 << "Score: " << score_ << " / " << max_score_;
    final_score_text2 << "Perfect: " << perfect_num_ << " Good: " << good_num_ << " Poor: " << poor_num_ << " Miss: " << miss_num_;
    final_score_text3 << "Assesment: " << assesment;

    playmode_->overlay.AddText("final_score1", final_score_text1.str(),
                                         glm::vec3(10.0f, 15.0f, 0.0f));
    playmode_->overlay.AddText("final_score2", final_score_text2.str(),
    glm::vec3(10.0f, 12.0f, 0.0f));
    playmode_->overlay.AddText("final_score3", final_score_text3.str(),
    glm::vec3(10.0f, 9.0f, 0.0f));
}
