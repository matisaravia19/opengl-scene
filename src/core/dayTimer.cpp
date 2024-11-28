#include "dayTimer.h"

#include "./time.h"

void DayTimer::init() {
}

void DayTimer::update() {
    auto settings = &SettingsManager::getSettings();
    if (!settings->dayNightCycle) {
        return;
    }

    settings->dayTime = fmod(settings->dayTime + Time::getDeltaTime() * timeStep, settings->dayLength);
}
