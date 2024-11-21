#include "light.h"

#include "../core/entity.h"
#include "renderer.h"
#include "shader.h"

Light::Light(glm::vec3 color, float intensity) {
    this->color = color;
    this->intensity = intensity;
}

void Light::init() {
    Component::init();
    Renderer::getActive()->registerLight(this);
}

void Light::remove() {
    Component::remove();
    Renderer::getActive()->removeLight(this);
}

DirectionalLight::DirectionalLight(glm::vec3 color, float intensity, glm::vec3 direction) : Light(color, intensity) {
    this->direction = direction;
}

void DirectionalLight::renderDeferred() {
    deferredShader->bind();

    auto renderer = Renderer::getActive();

    deferredShader->setUniform("windowSize", renderer->getWindow()->getSize());
    deferredShader->setUniform("cameraPosition", renderer->getCamera()->getPosition());
    deferredShader->setUniform("lightDirection", direction);
    deferredShader->setUniform("lightColor", color);
    deferredShader->setUniform("intensity", intensity);

    renderer->drawFrameQuad();
}

void DirectionalLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_DIRECTIONAL_LIGHT;
    deferredShader->upload();
}

PointLight::PointLight(glm::vec3 color, float intensity) : Light(color, intensity) {

}

void PointLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_POINT_LIGHT;
    deferredShader->upload();
}

void PointLight::renderDeferred() {
    deferredShader->bind();

    auto renderer = Renderer::getActive();

    deferredShader->setUniform("windowSize", renderer->getWindow()->getSize());
    deferredShader->setUniform("cameraPosition", renderer->getCamera()->getPosition());
    deferredShader->setUniform("lightPosition", getEntity()->getTransform()->getPosition());
    deferredShader->setUniform("lightColor", color);
    deferredShader->setUniform("intensity", intensity);

    renderer->drawFrameQuad();
}

SpotLight::SpotLight(glm::vec3 color, float intensity, glm::vec3 direction, float cutoff, float outerCutoff) :
        Light(color, intensity) {
    this->direction = direction;
    this->cutoff = cutoff;
    this->outerCutoff = outerCutoff;
}

void SpotLight::renderDeferred() {

}