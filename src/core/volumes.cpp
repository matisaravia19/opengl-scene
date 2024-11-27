#include "volumes.h"

bool Frustum::intersects(const Sphere &sphere) const {
    for (const auto &plane: planes) {
        if (glm::dot(plane.normal, sphere.center) + plane.distance < -sphere.radius) {
            return false;
        }
    }
    return true;
}

Frustum::Frustum(glm::mat4 viewProjectionMatrix) {
    planes[0].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
    planes[0].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
    planes[0].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
    planes[0].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];

    // Extract the numbers for the LEFT plane
    planes[1].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
    planes[1].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
    planes[1].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
    planes[1].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];

    // Extract the BOTTOM plane
    planes[2].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
    planes[2].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
    planes[2].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
    planes[2].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];

    // Extract the TOP plane
    planes[3].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
    planes[3].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
    planes[3].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
    planes[3].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];

    // Extract the FAR plane
    planes[4].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
    planes[4].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
    planes[4].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
    planes[4].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

    // Extract the NEAR plane
    planes[5].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
    planes[5].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
    planes[5].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
    planes[5].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];

    for (auto &plane: planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}
