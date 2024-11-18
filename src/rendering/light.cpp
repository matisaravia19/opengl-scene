#include "light.h"

Light::Light(glm::vec3 color, float intensity) {
    this->color = color;
    this->intensity = intensity;
}


void Light::setColor(const glm::vec3 color) {
    this->color = color;
}
glm::vec3 Light::getColor() const {
    return color;
}
void Light::setIntensity(const float intensity) {
    this->intensity = intensity;
}
float Light::getIntensity() const {
    return intensity;
}

DirectionalLight::DirectionalLight(const glm::vec3 color, const float intensity, const glm::vec3 direction)
    : Light(color, intensity)
    , direction(direction)
{}

void DirectionalLight::setDirection(const glm::vec3 direction) {
    this->direction = direction;
}
glm::vec3 DirectionalLight::getDirection() const {
    return direction;
}

PointLight::PointLight(glm::vec3 color, float intensity) : Light(color, intensity) {

}

SpotLight::SpotLight(glm::vec3 color, float intensity, glm::vec3 direction, float cutoff, float outerCutoff) :
        Light(color, intensity) {
    this->direction = direction;
    this->cutoff = cutoff;
    this->outerCutoff = outerCutoff;
}