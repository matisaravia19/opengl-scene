#pragma once

#include "glm/glm.hpp"

struct Sphere {
    glm::vec3 center;
    float radius;

    Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {}
};

struct Plane {
    glm::vec3 normal;
    float distance;
};

struct Frustum {
    Plane planes[6];

    bool intersects(const Sphere &sphere) const;
};