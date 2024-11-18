#pragma once

#include "light.h"
#include "../core/component.h"

class DayCycle : public Component {
    // Ticks 0-11999: Day, 12000-23999: Night
    int tick;
    static constexpr int TICKS_PER_DAY = 24000;
    static constexpr int TICK_RATE = 80;
    DirectionalLight *sun;

public:
    DayCycle();
    ~DayCycle() override;
    void init() override {};
    void update() override;
};