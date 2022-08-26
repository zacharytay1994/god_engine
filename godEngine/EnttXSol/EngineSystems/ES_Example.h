#pragma once

#include "../EngineComponents/EC_All.h"

namespace god
{
	void ExampleSystem ( Transform& transform )
	{
		( transform );
		// some code here ...
		//std::cout << transform.m_position.x << std::endl;
		//transform.m_rotation.y += 0.002f;
	}
}