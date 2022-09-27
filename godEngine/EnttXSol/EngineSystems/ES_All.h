#pragma once

#include "../EnttXSol.h"
#include "ES_Example.h"
#include "ES_Audio.h"
#include "ES_Grid.h"

#include "Physics/ES_Dynamic.h"
#include "Physics/ES_Shape.h"
#include "Physics/ES_Static.h"

#include "Grid/ES_GridManipulate.h"

namespace god
{
	// runs in the middle of a frame
	void EngineSystems ( EnttXSol& enttxsol , EngineResources& engineResources , bool isPause )
	{
		if ( !isPause )
		{
			enttxsol.RunEngineSystem ( engineResources , ExampleSystem );
			enttxsol.RunEngineSystem ( engineResources , AudioSystem );
			enttxsol.RunEngineSystem ( engineResources , GridManipulateSystem );
		}
		
		enttxsol.RunEngineSystem ( engineResources , GridSystem );
		//Physics
		enttxsol.RunEngineSystem(engineResources, ShapeUpdate);
		enttxsol.RunEngineSystem(engineResources, RigidStaticUpdate);
		enttxsol.RunEngineSystem(engineResources, RigidDynamicUpdate);
	}

	// runs at the start of a frame, i.e. runs before EngineSystems()
	void EngineSystemsFrameStart ( EnttXSol& enttxsol , EngineResources& engineResources )
	{
		if ( !enttxsol.m_pause )
		{
			enttxsol.RunEngineSystem ( engineResources , ExampleSystemFrameStart );


		}
		enttxsol.RunEngineSystem(engineResources, RigidStaticFrameBegin);
		enttxsol.RunEngineSystem(engineResources, RigidDynamicFrameBegin);
	}

	// runs at the end of a frame, i.e. runs after EngineSystems()
	// also guaranteed to run after any other major system update.
	void EngineSystemsFrameEnd ( EnttXSol& enttxsol , EngineResources& engineResources )
	{
		if ( !enttxsol.m_pause )
		{
			enttxsol.RunEngineSystem ( engineResources , ExampleSystemFrameEnd );

		}

		//physics
		enttxsol.RunEngineSystem(engineResources, RigidDynamicFrameEnd);
	}

	// runs at the start just after loading the scene
	void EngineSystemsInit ( EnttXSol& enttxsol , EngineResources& engineResources )
	{
		enttxsol.RunEngineSystem ( engineResources , ExampleSystemInit );
		enttxsol.RunEngineSystem ( engineResources , GridManipulateInit );
	}

	// runs at the end before unloading the scene 
	void EngineSystemsCleanup ( EnttXSol& enttxsol , EngineResources& engineResources )
	{
		enttxsol.RunEngineSystem ( engineResources , ExampleSystemCleanup );
	}
}