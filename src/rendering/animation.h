#pragma once


struct Animation {
    std::string name;
    float duration;
    float ticksPerSecond;
    std::vector <std::pair<std::string, glm::mat4>> keyframes;
};
