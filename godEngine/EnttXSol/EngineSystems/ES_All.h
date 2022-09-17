#pragma once

#include "../EnttXSol.h"
#include "ES_Example.h"
#include "ES_Grid.h"
#include "ES_Physics.h"

namespace god
{
	void EngineSystems ( EnttXSol& enttxsol , EngineResources& engineResources , bool isPause )
	{
		
		if ( !isPause )
		{
			enttxsol.RunEngineSystem ( engineResources , ExampleSystem );
			
		}
		enttxsol.RunEngineSystem ( engineResources , GridSystem );


		//Physics
		enttxsol.RunEngineSystem(engineResources, UpdateBase);
		enttxsol.RunEngineSystem(engineResources, UpdateStatic);
		enttxsol.RunEngineSystem(engineResources, UpdateDynamic);
		//
	}
}