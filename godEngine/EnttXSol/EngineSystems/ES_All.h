#pragma once

#include "../EnttXSol.h"
#include "ES_Example.h"

namespace god
{
	void EngineSystems ( EnttXSol& enttxsol )
	{
		enttxsol.RunEngineSystem ( ExampleSystem );
	}
}