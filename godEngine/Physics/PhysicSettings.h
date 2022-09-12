#pragma once

namespace god
{
	struct PhysicSettings
	{
		float fixedDT = 1.f / 60.f;
		glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };
	};
}