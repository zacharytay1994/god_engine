#pragma once

#include "../EnttXSol.h"
#include "ES_Example.h"
#include "ES_Audio.h"
#include "ES_Grid.h"

namespace god
{
	void EngineSystems ( EnttXSol& enttxsol , EngineResources& engineResources , bool isPause )
	{
		if ( !isPause )
		{
			enttxsol.RunEngineSystem ( engineResources , ExampleSystem );
		}
		enttxsol.RunEngineSystem ( engineResources , GridSystem );
	}
}