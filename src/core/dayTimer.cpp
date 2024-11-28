#include "dayTimer.h"

#include "./time.h"

void DayTimer::init() {
}

void DayTimer::update() {
    currentTime += Time::getDeltaTime() * timeStep;
    currentTime = fmod(currentTime, DAY_LENGTH);
}
