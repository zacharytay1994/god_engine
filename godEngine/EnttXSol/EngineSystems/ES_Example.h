#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

namespace god
{
	void ExampleSystem ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Update" << std::endl;
	}

	void ExampleSystemFrameStart ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Start Frame" << std::endl;
	}

	void ExampleSystemFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "End Frame" << std::endl;
	}

	void ExampleSystemInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		 //some code here ...
		//std::cout << "Init - Entities with EntityData & Transform." << std::endl;
	}

	void ExampleSystemCleanup ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Cleanup - Entities with EntityData & Transform." << std::endl;
	}
}