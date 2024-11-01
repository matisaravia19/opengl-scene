#pragma once

#include <vector>
#include <string>
#include "component.h"
#include "transform.h"

class Entity {
private:
    std::string name;
    std::vector<Component *> components;

public:
    Entity() = default;
    explicit Entity(std::string name);

    std::string getName();

    void addComponent(Component *component);
    template<class C>
    C *getComponent();

    Transform *getTransform() { return getComponent<Transform>(); }

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
