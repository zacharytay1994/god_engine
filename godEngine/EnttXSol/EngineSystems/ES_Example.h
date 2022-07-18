#pragma once

#include "../EngineComponents/EC_All.h"

namespace god
{
	void ExampleSystem ( Transform& transform )
	{
		transform.m_position.x += 0.01f;
		std::cout << transform.m_position.x << std::endl;
	}
}