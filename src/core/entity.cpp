#include "entity.h"

void Entity::addComponent(Component *component) {
    components.push_back(component);
    component->init();
}

void Entity::update() {
    for (auto &component : components) {
        component->update();
    }
}
