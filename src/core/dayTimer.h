#pragma once

#include "component.h"
#include "settings.h"

class DayTimer : public Component {
    double currentTime;
    double timeStep;

    static constexpr double DAWN = 6.f;
    static constexpr double DUSK = 18.f;
    static constexpr double TRANSITION = 4.f; // time it takes to transition between day and night.
public:
    static constexpr double DAY_LENGTH = 24.f;
    static constexpr double DAWN_START = DAWN - TRANSITION * .5;
    static constexpr double DAWN_END = DAWN_START + TRANSITION;
    static constexpr double DUSK_START = DUSK - TRANSITION * .5;
    static constexpr double DUSK_END = DUSK_START + TRANSITION;

    DayTimer() : currentTime(DAWN), timeStep(0.5f) {}

    void init() override;
    void update() override;

    [[nodiscard]] double getTimeStep() const { return timeStep; }

    [[nodiscard]] double getCurrentTime() const { return currentTime; }

    void setDayLength(double minutes);
};
