#include "dayTimer.h"

#include "./time.h"

void DayTimer::init() {
    Settings::ActiveSettings->subscribe(this);
}

void DayTimer::update() {
    currentTime += Time::getDeltaTime() * timeStep;
    currentTime = fmod(currentTime, DAY_LENGTH);
}

void DayTimer::notify(Settings* settings) {
    // TODO: Set timer from minutes and pause time here.
}
