#pragma once

#include "EC_Example.h"
#include "EC_Transform.h"
#include "EC_Renderable3D.h"
#include "EC_AudioSource.h"

#include <tuple>
#include <string>
#include <array>

namespace god
{
	struct EngineComponents
	{
		using Components = std::tuple<
			ExampleComponent ,
			Transform ,
			Renderable3D ,
			AudioSource
		>;
		static std::array<std::string , std::tuple_size_v<Components>> m_component_names;
	};
}