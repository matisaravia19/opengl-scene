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
    createShadowMap();
}

void Light::remove() {
    Component::remove();
    Renderer::getActive()->removeLight(this);
}

void Light::createShadowMap() {
    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize.x, shadowMapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::renderShadow(const std::unordered_set<Renderable *> &renderables) {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glViewport(0, 0, shadowMapSize.x, shadowMapSize.y);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    auto projection = getProjectionMatrix();
    auto view = getViewMatrix();

    for (auto &renderable: renderables) {
        renderable->renderShadow(projection, view);
    }
}

void Light::setUniforms() {
    auto renderer = Renderer::getActive();

    deferredShader->setUniform("windowSize", renderer->getWindow()->getSize());
    deferredShader->setUniform("cameraPosition", renderer->getCamera()->getPosition());
    deferredShader->setUniform("lightColor", color);

    deferredShader->setUniform("lightSpaceMatrix", getProjectionMatrix() * getViewMatrix());

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}

void Light::renderDeferred() {
    deferredShader->bind();
    setUniforms();
    Renderer::drawFrameQuad();
}

DirectionalLight::DirectionalLight(const glm::vec3 color)
        : Light(color) {}

void DirectionalLight::setDirection(const glm::vec3 direction) {
    const auto quat = glm::quatLookAt(glm::normalize(direction), glm::vec3(0.f, 1.f, 0.f));
    getEntity()->getTransform()->setRotation(quat);
}

void DirectionalLight::setUniforms() {
    Light::setUniforms();
    deferredShader->setUniform("lightDirection", getEntity()->getTransform()->getForward());
}

void DirectionalLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_DIRECTIONAL_LIGHT;
}

glm::mat4 DirectionalLight::getProjectionMatrix() const {
    return glm::ortho(
            -DIRECTIONAL_LIGHT_SHADOW_DISTANCE,
            DIRECTIONAL_LIGHT_SHADOW_DISTANCE,
            -DIRECTIONAL_LIGHT_SHADOW_DISTANCE,
            DIRECTIONAL_LIGHT_SHADOW_DISTANCE,
            SHADOW_NEAR_PLANE,
            SHADOW_FAR_PLANE
    );
}

glm::mat4 DirectionalLight::getViewMatrix() const {
    auto transform = getEntity()->getTransform();
    auto cameraPosition = Renderer::getActive()->getCamera()->getPosition();
    auto position = cameraPosition - transform->getForward() * DIRECTIONAL_LIGHT_SHADOW_DISTANCE;
    return glm::lookAt(position, cameraPosition, transform->getUp());
}

PointLight::PointLight(glm::vec3 color) : Light(color) {

}

void PointLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_POINT_LIGHT;
    deferredShader->upload();
}

void PointLight::setUniforms() {
    Light::setUniforms();
    deferredShader->setUniform("lightPosition", getEntity()->getTransform()->getPosition());
}

glm::mat4 PointLight::getProjectionMatrix() const {
    return glm::perspective(glm::radians(90.f), 1.f, 1.f, 25.f);
}

glm::mat4 PointLight::getViewMatrix() const {
    auto position = getEntity()->getTransform()->getPosition();
    return glm::lookAt(position, position + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
}

SpotLight::SpotLight(glm::vec3 color, float innerAngle, float outerAngle) : Light(color) {
    this->innerAngle = innerAngle;
    this->innerCutoff = glm::cos(innerAngle);
    this->outerAngle = outerAngle;
    this->outerCutoff = glm::cos(outerAngle);
}

void SpotLight::init() {
    Light::init();
    deferredShader = Shader::DEFERRED_SPOT_LIGHT;
    deferredShader->upload();
}

void SpotLight::setUniforms() {
    Light::setUniforms();
    deferredShader->setUniform("lightPosition", getEntity()->getTransform()->getPosition());
    deferredShader->setUniform("lightDirection", getEntity()->getTransform()->getForward());
    deferredShader->setUniform("innerCutoff", innerCutoff);
    deferredShader->setUniform("outerCutoff", outerCutoff);
}

glm::mat4 SpotLight::getProjectionMatrix() const {
    return glm::perspective(2 * outerAngle, 1.0f, SHADOW_NEAR_PLANE, SHADOW_FAR_PLANE);
}

glm::mat4 SpotLight::getViewMatrix() const {
    auto transform = getEntity()->getTransform();
    return glm::lookAt(transform->getPosition(), transform->getPosition() + transform->getForward(), transform->getUp());
}
