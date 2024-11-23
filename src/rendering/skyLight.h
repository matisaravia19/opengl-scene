#pragma once

#include "light.h"

class SkyLight : public DirectionalLight {
    void updateUniforms() const;
public:
    SkyLight();
    void update() override;
};

