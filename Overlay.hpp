#pragma once

#include "DrawLines.hpp"

#include <unordered_map>
#include <string>

struct PlayMode;

class Text {
private:
    std::string content_;
    glm::vec3 anchor_;
    glm::u8vec4 color_;

public:
    Text() = default;
    Text(const std::string& content, const glm::vec3& anchor, const glm::u8vec4& color) :
    content_(content),
    anchor_(anchor),
    color_(color)
    {}

    void Draw(DrawLines& lines, float aspect, float ofs);
};

class Overlay {
private:
    std::unordered_map<std::string, Text> texts_;
    PlayMode* playmode_;

public:
    Overlay(PlayMode* playmode) : playmode_(playmode) {}
    void Draw(float aspect, float ofs);
    void AddText(const std::string& name, const std::string& content, const glm::vec3& anchor, const glm::u8vec4& color = glm::u8vec4(0xff, 0xff, 0xff, 0x00));
    void AddText(const std::string& name, const std::string& content, const glm::vec3& anchor, long long start_time, const glm::u8vec4& color = glm::u8vec4(0xff, 0xff, 0xff, 0x00));
    void AddText(const std::string& name, const std::string& content, const glm::vec3& anchor, long long start_time, long long end_time, const glm::u8vec4& color = glm::u8vec4(0xff, 0xff, 0xff, 0x00));
};
