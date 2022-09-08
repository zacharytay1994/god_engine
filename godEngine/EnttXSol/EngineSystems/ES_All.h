#pragma once

#include "../EnttXSol.h"
#include "ES_Example.h"
#include "ES_Grid.h"

namespace god
{
	void EngineSystems ( EnttXSol& enttxsol )
	{
		enttxsol.RunEngineSystem ( ExampleSystem );
		enttxsol.RunEngineSystem ( GridSystem );
	}
}