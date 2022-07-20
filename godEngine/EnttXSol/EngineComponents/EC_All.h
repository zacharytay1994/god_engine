#pragma once

#include "EC_Example.h"
#include "EC_Transform.h"
#include "EC_Renderable3D.h"

#include <tuple>
#include <string>
#include <array>

namespace god
{
	/*using EngineComponentsTuple = std::tuple<
		ExampleComponent ,
		Transform ,
		Renderable3D
	>;
	static std::array<std::string , std::tuple_size_v<EngineComponentsTuple>> g_EngineComponents
	{
		"ExampleComponent",
		"Transform",
		"Renderable3D"
	};
	using EngineComponentType = EngineComponents<EngineComponentsTuple>;*/

	struct EngineComponentDefinitions
	{
		using Components = std::tuple<
			ExampleComponent ,
			Transform ,
			Renderable3D
		>;
		static std::array<std::string , std::tuple_size_v<Components>> m_component_names;
	};
}