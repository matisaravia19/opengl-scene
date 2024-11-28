#include "skyLight.h"

#include <iostream>
#include <cmath>
#include <glm/ext/scalar_constants.hpp>

#include "../core/time.h"
#include "shader.h"
#include "../core/dayTimer.h"
#include "../core/entity.h"

#define DAYLIGHT_COLOR glm::vec3(1.0f, 1.0f, 0.9f) * 5.0f
#define MOONLIGHT_COLOR glm::vec3(0.2f, 0.2f, 0.3f)
#define DAWN_COLOR glm::vec3(1.0f, 0.6f, 0.4f) * 2.0f

SkyLight::SkyLight()
        : DirectionalLight(glm::vec3(1.f)) {}

void SkyLight::update() {
    DirectionalLight::update();

    const auto tm = getEntity()->getComponent<DayTimer>()->getCurrentTime();

    // Moonlight has a constant direction
    if (tm < DayTimer::DAWN_START || tm > DayTimer::DUSK_END) {
        color = MOONLIGHT_COLOR; // Blue-ish moonlight
        setDirection(glm::vec3(0.f, -1.f, 0.f));
        return;
    }

    const double offset_time = tm - DayTimer::DAWN_START;
    constexpr double sun_in_sky = DayTimer::DUSK_END - DayTimer::DAWN_START;
    const float angle = static_cast<float>(offset_time / sun_in_sky) * glm::pi<float>();

    // Calculate sun direction based on angle
    setDirection(normalize(glm::vec3(
            std::cos(angle),
            -std::sin(angle),
            0.2f // Slight offset to make the sun not move in a perfect vertical line
    )));

    if (tm < DayTimer::DAWN_END || tm > DayTimer::DUSK_START) {
        color = DAWN_COLOR;
    } else { // Full daylight
        color = DAYLIGHT_COLOR;
    }
}
