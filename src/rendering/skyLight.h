#pragma once

#include "light.h"

class SkyLight : public DirectionalLight {
public:
    SkyLight();
    void update() override;
};

