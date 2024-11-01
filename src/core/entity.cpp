#include "entity.h"

Entity::Entity(std::string name) {
    this->name = std::move(name);
}

std::string Entity::getName() {
    return name;
}

void Entity::addComponent(Component *component) {
    components.push_back(component);
    component->setEntity(this);
    component->init();
}

void Entity::update() {
    for (auto &component: components) {
        component->update();
    }
}
