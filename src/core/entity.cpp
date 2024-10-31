#include "entity.h"

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

template<class C>
C *Entity::getComponent() {
    for (auto &component: components) {
        auto c = dynamic_cast<C *>(component);
        if (c) {
            return c;
        }
    }
    return nullptr;
}
