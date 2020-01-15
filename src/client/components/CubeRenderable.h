#pragma once

#include "../gl/textures.h"
#include "ecs/ECS.h"

struct CubeRenderable
{
	ECS_DECLARE_TYPE;

	CubeRenderable() {}

	gl::Texture2d cubeTexture;
	bool hasTexture = false;
};

ECS_DEFINE_TYPE(CubeRenderable);