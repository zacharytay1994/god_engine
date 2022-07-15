#pragma once

#include "EC_Transform.h"

#include <tuple>
#include <string>
#include <array>

namespace god
{
	using EngineComponentsTuple = std::tuple<
		Transform
	>;
	static std::array<std::string , std::tuple_size_v<EngineComponentsTuple>> g_EngineComponents
	{
		"Position"
	};
	using EngineComponentType = EngineComponents<EngineComponentsTuple>;
}