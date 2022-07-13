#pragma once

#include "Component.h"

#include <glm/glm/glm.hpp>

namespace god
{
	struct Position
	{
		float x { 0 };
	};

	struct Velocity
	{
		glm::vec3 m_velocity;
	};
}