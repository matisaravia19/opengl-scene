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
