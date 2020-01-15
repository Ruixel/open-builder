#pragma once

#include "ecs/ECS.h"
#include "glm/glm.hpp"

struct Transform {
	ECS_DECLARE_TYPE;

	Transform(glm::vec3& pos, glm::vec3& rot) : position(pos), rotation(rot) {}
	Transform() {};

	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
};

ECS_DEFINE_TYPE(Transform);