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

    Frustum() = default;
    explicit Frustum(glm::mat4 viewProjectionMatrix);

    bool intersects(const Sphere &sphere) const;
};