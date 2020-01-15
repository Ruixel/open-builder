#pragma once

#include "../maths.h"
#include "../gl/vertex_array.h"
#include "../gl/shader.h"
#include "../components/CubeRenderable.h"
#include "common/components/Transform.h"
#include "ecs/ECS.h"
#include "common/debug.h"

#include <iostream>

struct RenderEvent
{
	ECS_DECLARE_TYPE;
};
ECS_DEFINE_TYPE(RenderEvent);

class CubeRenderSystem : public ECS::EntitySystem,
	public ECS::EventSubscriber<RenderEvent>
{
public:
	CubeRenderSystem(gl::VertexArray* cube, gl::UniformLocation modelLocation) {
		m_cube = cube;
		m_shaderModelLocation = modelLocation;

		m_errorTexture.create("skins/error");
	}

	// Make this system respond to the Render event
	virtual void configure(ECS::World* world) override {
		world->subscribe<RenderEvent>(this);
	}

	virtual void unconfigure(ECS::World* world) override {
		world->unsubscribeAll(this);
	}
	
	// Draw cube on render request
	virtual void receive(ECS::World* world, const RenderEvent& event) override {
		// Bind cube for drawing
		auto drawable = m_cube->getDrawable();
		drawable.bind();

		world->each<CubeRenderable, Transform>([&](ECS::Entity* ent, 
			ECS::ComponentHandle<CubeRenderable> cr, 
			ECS::ComponentHandle<Transform> transform) {

			// Load texture, if not load default error texture
			if (cr->hasTexture)
				cr->cubeTexture.bind();
			else
				m_errorTexture.bind();
			
			// Generate model matrix from the transform component
			glm::mat4 modelMatrix{ 1.0f };
			translateMatrix(modelMatrix, transform->position);
			rotateMatrix(modelMatrix, transform->rotation);

			// Load matrix into shader
			gl::loadUniform(m_shaderModelLocation, modelMatrix);

			// Draw cube
			drawable.draw();
		});
	}

private:
	gl::VertexArray* m_cube;
	gl::UniformLocation m_shaderModelLocation;

	gl::Texture2d m_errorTexture;
};