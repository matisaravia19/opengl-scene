#include "settings.h"
#include "window.h"

Settings SettingsManager::settings = {
        .fullscreen = false,
        .showDebugData = false,
        .renderGizmos = false,
        .ambientLight = glm::vec3(0.1f),
        .fogColor = glm::vec3(1.0f),
        .fogDensity = 0.01f,
        .showShadows = true,
        .directionalShadowDistance = 100.0f,
        .treeDistance = 100.0f,
        .dayNightCycle = true,
        .dayLength = 24.0f,
        .dayTime = 0.0f,
};

Settings &SettingsManager::getSettings() {
    return settings;
}

void SettingsManager::notifyChanges() {
    Window::getActive()->setFullscreen(settings.fullscreen);
}
