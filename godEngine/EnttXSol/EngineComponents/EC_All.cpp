#include "../../pch.h"
#include "EC_All.h"

namespace god
{
	std::array<std::string , std::tuple_size_v<EngineComponents::Components>> EngineComponents::m_component_names
	{
		"ExampleComponent",
		"Transform",
		"Renderable3D",
		"RigidBody"
	};
}