#include "component.h"

#include <iostream>

Entity *Component::getEntity() {
    return entity;
}

void Component::setEntity(Entity *entity) {
    this->entity = entity;
}

void Component::init() {
    lifeState = ComponentLifeState::INITIALIZED;
}

void Component::update() {

}

void Component::remove() {
    lifeState = ComponentLifeState::REMOVED;
}

ComponentLifeState Component::getLifeState() {
    return lifeState;
}

void DebugComponent::update() {
    Component::update();
    std::cout << "Update" << std::endl;
}