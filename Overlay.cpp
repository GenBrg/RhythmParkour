#include "Overlay.hpp"
#include "PlayMode.hpp"

constexpr float H { 0.1f };

void Text::Draw(DrawLines& lines, float aspect, float ofs)
{
    lines.draw_text(content_,
        glm::vec3(-aspect + anchor_[0] * H, -1.0 + anchor_[1] * H, 0.0),
        glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
        glm::u8vec4(0x00, 0x00, 0x00, 0x00));
    lines.draw_text(content_,
        glm::vec3(-aspect + anchor_[0] * H + ofs, -1.0 + anchor_[1] * H + ofs, 0.0),
        glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
        color_);
}

void Overlay::Draw(float aspect, float ofs)
{
    DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));
    for (auto& p : texts_) {
        p.second.Draw(lines, aspect, ofs);
    }
}

 void Overlay::AddText(const std::string& name, const std::string& content, const glm::vec3& anchor, const glm::u8vec4& color)
 {
     texts_[name] = { content, anchor, color };
 }

void Overlay::AddText(const std::string& name, const std::string& content, const glm::vec3& anchor, long long start_time, const glm::u8vec4& color)
{
    playmode_->timer_manager.AddTimer(start_time, [this, name, content, anchor, color]() {
        texts_[name] = { content, anchor, color };
    });
}

void Overlay::AddText(const std::string& name, const std::string& content, const glm::vec3& anchor, long long start_time, long long end_time, const glm::u8vec4& color)
{
    AddText(name, content, anchor, start_time, color);
    playmode_->timer_manager.AddTimer(end_time, [this, name, content, anchor, color]() {
        texts_.erase(name);
    });
}
