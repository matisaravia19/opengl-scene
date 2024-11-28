#pragma once

#include <vector>
#include "glm/vec3.hpp"

struct Settings {
    bool fullscreen;
    bool showDebugData;
    bool renderGizmos;

    glm::vec3 ambientLight;
    glm::vec3 fogColor;
    float fogDensity;

    bool showShadows;
    float directionalShadowDistance;
};

class SettingsManager {
private:
    static Settings settings;

public:
    static Settings &getSettings();
    static void notifyChanges();
};