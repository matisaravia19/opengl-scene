#include "volumes.h"

bool Frustum::intersects(const Sphere &sphere) const {
    for (const auto &plane: planes) {
        if (glm::dot(plane.normal, sphere.center) + plane.distance < -sphere.radius) {
            return false;
        }
    }
    return true;
}
