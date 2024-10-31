#pragma once

#include <vector>
#include "component.h"
#include "transform.h"

class Entity {
private:
    std::vector<Component *> components;

public:
    Entity() = default;

    void addComponent(Component *component);
    template<class C>
    C *getComponent();

    Transform *getTransform() { return getComponent<Transform>(); }

    void update();
};
