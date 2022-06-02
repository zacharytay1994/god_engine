#include "../pch.h"
#include "ECSManager.h"

#include "Systems/CommonSystems.h"

namespace god
{
	void ECSManager::Update ( float dt )
	{
		m_ecs.RunSystem ( UpdatePosition );
	}
}
