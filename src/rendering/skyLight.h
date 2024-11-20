#pragma once

#include "light.h"

class SkyLight : public DirectionalLight {
    double tm;

    void updateUniforms() const;
public:
    SkyLight();
    void update() override;

    [[nodiscard]] double getTime() const { return tm; }

    static constexpr int TICK_RATE = 760;
    static constexpr int TICKS_PER_DAY = 24000;
};

