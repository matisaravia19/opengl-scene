#include "dayCycle.h"

#include "../core/time.h"

DayCycle::DayCycle()
    : tick(0)
{
    sun = new DirectionalLight(glm::vec3(1.f), 1.f, glm::vec3(0.f, -1.f, 0.f));
}

DayCycle::~DayCycle() {
    delete sun;
}

void DayCycle::update() {
    const auto deltaTime = Time::getDeltaTime();
    tick = static_cast<int>((tick + deltaTime * TICK_RATE)) % TICKS_PER_DAY;

    // Moonlight has a constant direction
    if (tick >= 12000) {
        sun->setDirection(glm::vec3(0.f, -1.f, 0.f));
        sun->setColor(glm::vec3(0.2f, 0.2f, 0.3f)); // Blue-ish moonlight
        sun->setIntensity(0.3f);
        return;
    }

    const float angle = (static_cast<float>(tick) / TICKS_PER_DAY) * 180.f;

    // Calculate sun direction based on angle
    const float radians = glm::radians(angle);
    const glm::vec3 sunDir = normalize(glm::vec3(
        std::cos(radians),
        -std::sin(radians),
        0.2f  // Slight offset to make the sun not move in a perfect vertical line
    ));

    const float dayProgress = static_cast<float>(tick) / 12000.f;

    glm::vec3 sunColour;
    float intensity;

    if (dayProgress < 0.2f) { // Dawn
        sunColour = glm::vec3(1.0f, 0.6f, 0.4f);
        intensity = dayProgress * 5.0f;
    } else if (dayProgress > 0.8f) { // Dusk
        sunColour = glm::vec3(1.0f, 0.6f, 0.4f);
        intensity = (1.0f - dayProgress) * 5.0f;
    } else { // Full daylight
        sunColour = glm::vec3(1.0f, 1.0f, 0.9f);
        intensity = 1.0f;
    }

    sun->setDirection(sunDir);
    sun->setColor(sunColour);
    sun->setIntensity(intensity);
}
