#include "skyLight.h"

#include <iostream>
#include <cmath>
#include <glm/ext/scalar_constants.hpp>

#include "../core/time.h"
#include "shader.h"
#include "../core/dayTimer.h"
#include "../core/entity.h"

SkyLight::SkyLight()
    : DirectionalLight(glm::vec3(1.f), 1.f, glm::vec3(0.f, -1.f, 0.f))
{}

void SkyLight::updateUniforms() const {
    Shader::PBR->setUniform("dirLightDir", direction);
    Shader::PBR->setUniform("dirLightColor", color);
}

void SkyLight::update() {
    const auto tm = getEntity()->getComponent<DayTimer>()->getCurrentTime();

    // Moonlight has a constant direction
    if (tm < DayTimer::DAWN_START || tm > DayTimer::DUSK_END) {
        direction = glm::vec3(0.f, -1.f, 0.f);
        color = glm::vec3(0.2f, 0.2f, 0.3f); // Blue-ish moonlight
        intensity = 0.3f;
        updateUniforms();
        return;
    }

    const double offset_time = tm - DayTimer::DAWN_START;
    constexpr double sun_in_sky = DayTimer::DUSK_END - DayTimer::DAWN_START;
    const float angle = static_cast<float>(offset_time / sun_in_sky) * glm::pi<float>();

    // Calculate sun direction based on angle
    direction = normalize(glm::vec3(
        std::cos(angle),
        -std::sin(angle),
        0.2f  // Slight offset to make the sun not move in a perfect vertical line
    ));

    const auto day_progress = static_cast<float>(tm / DayTimer::DAY_LENGTH);

    if (tm < DayTimer::DAWN_END) {
        color = glm::vec3(1.f, .4f, .2f);
        intensity = day_progress * 5.0f;
    } else if (tm > DayTimer::DUSK_START) {
        color = glm::vec3(1.f, .4f, .2f);
        intensity = (1.0f - day_progress) * 5.0f;
    } else { // Full daylight
        color = glm::vec3(1.0f, 1.0f, 0.9f);
        intensity = 1.0f;
    }
    updateUniforms();
}
