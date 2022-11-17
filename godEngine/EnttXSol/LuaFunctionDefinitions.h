#pragma once

#include "../Editor/EngineResources.h"

#include <tuple>
#include <string>

namespace god
{
	using MainVariables = std::tuple<std::string , bool>;
	struct EnttXSol;
	void RegisterLuaCPP ( EnttXSol& entt , EngineResources& engineResources , MainVariables& mainVariables );
}