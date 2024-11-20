#include "skyLight.h"

#include <iostream>

#include "../core/time.h"
#include "shader.h"

SkyLight::SkyLight()
    : DirectionalLight(glm::vec3(1.f), 1.f, glm::vec3(0.f, -1.f, 0.f))
    , tm(0)
{}

void SkyLight::updateUniforms() const {
    Shader::PBR->setUniform("dirLightDir", direction);
    Shader::PBR->setUniform("dirLightColor", color);
}

// TODO: Centralise time, and cutoffs for daytime, nighttime, dusk and dawn.
void SkyLight::update() {
    tm += Time::getDeltaTime() * static_cast<double>(TICK_RATE);
    const int tick = static_cast<int>(tm) % TICKS_PER_DAY;

    std::cout << "tm " << tm << '\n';

    // Moonlight has a constant direction
    if (tick < 4000 || tick > 20000) {
        direction = glm::vec3(0.f, -1.f, 0.f);
        color = glm::vec3(0.2f, 0.2f, 0.3f); // Blue-ish moonlight
        intensity = 0.3f;
        updateUniforms();
        return;
    }

    float offset_tick = static_cast<float>(tick) - 6000.f;
    const float angle = (offset_tick / (16000)) * 180.f;

    // Calculate sun direction based on angle
    const float radians = glm::radians(angle);
    direction = normalize(glm::vec3(
        std::cos(radians),
        -std::sin(radians),
        0.2f  // Slight offset to make the sun not move in a perfect vertical line
    ));

    const float dayProgress = static_cast<float>(tick) / 12000.f;

    if (dayProgress < 6000) { // Dawn
        color = glm::vec3(1.0f, 0.6f, 0.4f);
        intensity = dayProgress * 5.0f;
    } else if (dayProgress > 16000) { // Dusk
        color = glm::vec3(1.0f, 0.6f, 0.4f);
        intensity = (1.0f - dayProgress) * 5.0f;
    } else { // Full daylight
        color = glm::vec3(1.0f, 1.0f, 0.9f);
        intensity = 1.0f;
    }
    updateUniforms();
}
