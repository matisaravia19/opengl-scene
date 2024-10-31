#include "light.h"

Light::Light(glm::vec3 color, float intensity) {
    this->color = color;
    this->intensity = intensity;
}

DirectionalLight::DirectionalLight(glm::vec3 color, float intensity, glm::vec3 direction) : Light(color, intensity) {
    this->direction = direction;
}

PointLight::PointLight(glm::vec3 color, float intensity) : Light(color, intensity) {

}

SpotLight::SpotLight(glm::vec3 color, float intensity, glm::vec3 direction, float cutoff, float outerCutoff) :
        Light(color, intensity) {
    this->direction = direction;
    this->cutoff = cutoff;
    this->outerCutoff = outerCutoff;
}