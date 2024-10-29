#include "component.h"

#include <iostream>

void Component::init() {

}

void Component::update() {

}

void Component::remove() {

}

void DebugComponent::update() {
    std::cout << "Update" << std::endl;
}