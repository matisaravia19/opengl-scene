#pragma once

#include <vector>
#include <string>
#include "component.h"
#include "transform.h"

class Entity {
private:
    std::string name;
    std::vector<Component *> components;
    bool active = false;

public:
    Entity() = default;
    explicit Entity(std::string name);
    ~Entity();

    std::string getName();

    void addComponent(Component *component);
    template<class C>
    C *getComponent();

    template<class C>
    void removeComponent();

    Transform *getTransform() { return getComponent<Transform>(); }

    void spawn();
    void update();
};

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

template<class C>
void Entity::removeComponent() {
    for (auto it = components.begin(); it != components.end(); it++) {
        auto c = dynamic_cast<C *>(*it);
        if (c) {
            (*it)->remove();
            components.erase(it);
            delete c;
            return;
        }
    }
}