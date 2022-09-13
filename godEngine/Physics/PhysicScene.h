#pragma once

#include <glm/glm/glm.hpp>
#include "PxPhysicsAPI.h"

namespace god
{
	struct RaycastHit
	{
		uint64_t	m_hitEntity;
		glm::vec3	m_position;
		glm::vec3	m_normal;
		float		m_distance;
	};

	class PhysicScene
	{
	public:

	private:

	};
}