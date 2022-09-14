#include "../../pch.h"
#include "EC_All.h"

namespace god
{
	std::array<std::string , std::tuple_size_v<EngineComponents::Components>> EngineComponents::m_component_names
	{
		"Entity Data",
		"Transform",
		"Renderable3D",
		"AudioSource", 
		"GridCell"
	};
}