#pragma once

#include "glm/glm.hpp"

#define VEC3_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define VEC3_DOWN glm::vec3(0.0f, -1.0f, 0.0f)
#define VEC3_FORWARD glm::vec3(0.0f, 0.0f, -1.0f)
#define VEC3_BACK glm::vec3(0.0f, 0.0f, 1.0f)
#define VEC3_RIGHT glm::vec3(1.0f, 0.0f, 0.0f)
#define VEC3_LEFT glm::vec3(-1.0f, 0.0f, 0.0f)

#define VEC3_ZERO glm::vec3(0.0f, 0.0f, 0.0f)
#define VEC3_UNIT_X glm::vec3(1.0f, 0.0f, 0.0f)
#define VEC3_UNIT_Y glm::vec3(0.0f, 1.0f, 0.0f)
#define VEC3_UNIT_Z glm::vec3(0.0f, 0.0f, 1.0f)

#define VEC4_UNIT_W glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)