#include "light.h"

#include "../core/entity.h"
#include "renderer.h"
#include "shader.h"

Light::Light(glm::vec3 color) {
    this->color = color;
}
void Light::setColor(const glm::vec3 color) {
    this->color = color;
}
glm::vec3 Light::getColor() const {
    return color;
}

void Light::init() {
    Component::init();
    Renderer::getActive()->registerLight(this);
}

void Light::remove() {
    Component::remove();
    Renderer::getActive()->removeLight(this);
}

DirectionalLight::DirectionalLight(glm::vec3 color) : Light(color) {

}
void DirectionalLight::setDirection(const glm::vec3 direction) {
    this->direction = direction;
}
glm::vec3 DirectionalLight::getDirection() const {
    return direction;
}

void DirectionalLight::renderDeferred() {
    deferredShader->bind();

    auto renderer = Renderer::getActive();

    deferredShader->setUniform("windowSize", renderer->getWindow()->getSize());
    deferredShader->setUniform("cameraPosition", renderer->getCamera()->getPosition());
    deferredShader->setUniform("lightDirection", getEntity()->getTransform()->getForward());
    deferredShader->setUniform("lightColor", color);

    renderer->drawFrameQuad();
}

void DirectionalLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_DIRECTIONAL_LIGHT;
    deferredShader->upload();
}

PointLight::PointLight(glm::vec3 color) : Light(color) {

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

    renderer->drawFrameQuad();
}

SpotLight::SpotLight(glm::vec3 color, float innerAngle, float outerAngle) : Light(color) {
    this->innerCutoff = glm::cos(innerAngle);
    this->outerCutoff = glm::cos(outerAngle);
}

void SpotLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_SPOT_LIGHT;
    deferredShader->upload();
}

void SpotLight::renderDeferred() {
    deferredShader->bind();

    auto renderer = Renderer::getActive();

    deferredShader->setUniform("windowSize", renderer->getWindow()->getSize());
    deferredShader->setUniform("cameraPosition", renderer->getCamera()->getPosition());
    deferredShader->setUniform("lightPosition", getEntity()->getTransform()->getPosition());
    deferredShader->setUniform("lightDirection", getEntity()->getTransform()->getForward());
    deferredShader->setUniform("lightColor", color);
    deferredShader->setUniform("innerCutoff", innerCutoff);
    deferredShader->setUniform("outerCutoff", outerCutoff);

    renderer->drawFrameQuad();
}
