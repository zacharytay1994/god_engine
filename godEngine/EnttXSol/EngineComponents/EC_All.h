#pragma once

#include "EC_Example.h"
#include "EC_Transform.h"

#include <tuple>
#include <string>
#include <array>

namespace god
{
	using EngineComponentsTuple = std::tuple<
		ExampleComponent ,
		Transform
	>;
	static std::array<std::string , std::tuple_size_v<EngineComponentsTuple>> g_EngineComponents
	{
		"ExampleComponent",
		"Transform"
	};
	using EngineComponentType = EngineComponents<EngineComponentsTuple>;
}