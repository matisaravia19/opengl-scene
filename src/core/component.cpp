#include "component.h"

#include <iostream>

Entity *Component::getEntity() {
    return entity;
}

void Component::setEntity(Entity *entity) {
    this->entity = entity;
}

void Component::init() {

}

void Component::update() {

}

void Component::remove() {

}

void DebugComponent::update() {
    std::cout << "Update" << std::endl;
}