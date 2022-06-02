#pragma once

#include <godECS/ECS.h>

namespace god
{
	struct ECSManager
	{
		ECS m_ecs;

		void Update ( float dt );
	};
}