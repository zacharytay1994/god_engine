#pragma once

#include "../EnttXSol.h"
#include "ES_Example.h"
#include "ES_Grid.h"

#include "ES_AudioSource.h"
#include "ES_AudioListener.h"

#include "Physics/ES_Dynamic.h"
//#include "Physics/ES_RayCast.h"
#include "Physics/ES_Static.h"
#include "Physics/ES_Debug.h"

#include "Render/ES_Transform.h"
#include "Render/ES_FaceCamera.h"

#include "Grid/ES_GridManipulate.h"

#include "GUI/ES_GUIObject.h"

#include "../../Window/SystemTimer.h"

namespace god
{
	// runs in the middle of a frame
	void EngineSystems ( EnttXSol& enttxsol , EngineResources& engineResources , bool isPause )
	{
		// render
		enttxsol.RunEngineSystem ( engineResources , TransformDirty );
		enttxsol.RunEngineSystem ( engineResources , FaceCamera );

		if ( !isPause )
		{
			SystemTimer::StartTimeSegment ( "ExampleSystem" );
			enttxsol.RunEngineSystem ( engineResources , ExampleSystem );
			SystemTimer::EndTimeSegment ( "ExampleSystem" );
			SystemTimer::StartTimeSegment ( "GridManipulateSystem" );
			enttxsol.RunEngineSystem ( engineResources , GridManipulateSystem );
			SystemTimer::EndTimeSegment ( "GridManipulateSystem" );
		}

		// gui
		SystemTimer::StartTimeSegment ( "GUIObjectUpdate" );
		enttxsol.RunEngineSystem ( engineResources , GUIObjectUpdate );
		SystemTimer::EndTimeSegment ( "GUIObjectUpdate" );
		SystemTimer::StartTimeSegment ( "GridSystem" );
		enttxsol.RunEngineSystem ( engineResources , GridSystem );
		SystemTimer::EndTimeSegment ( "GridSystem" );

		//Physics
		if ( isPause )
		{
			SystemTimer::StartTimeSegment ( "RigidStaticUpdate" );
			enttxsol.RunEngineSystem ( engineResources , RigidStaticUpdate );
			SystemTimer::EndTimeSegment ( "RigidStaticUpdate" );
			SystemTimer::StartTimeSegment ( "RigidDynamicUpdate" );
			enttxsol.RunEngineSystem ( engineResources , RigidDynamicUpdate );
			SystemTimer::EndTimeSegment ( "RigidDynamicUpdate" );
		}
	}

	// runs at the start of a frame, i.e. runs before EngineSystems()
	void EngineSystemsFrameStart ( EnttXSol& enttxsol , EngineResources& engineResources )
	{
		if ( !enttxsol.m_pause )
		{
			SystemTimer::StartTimeSegment ( "ExampleSystemFrameStart" );
			enttxsol.RunEngineSystem ( engineResources , ExampleSystemFrameStart );
			SystemTimer::EndTimeSegment ( "ExampleSystemFrameStart" );

		}

		SystemTimer::StartTimeSegment ( "AudioSourceSystem" );
		enttxsol.RunEngineSystem ( engineResources , AudioSourceSystem );
		SystemTimer::EndTimeSegment ( "AudioSourceSystem" );

		if ( enttxsol.m_pause )
		{
			SystemTimer::StartTimeSegment ( "RigidStaticFrameBegin" );
			enttxsol.RunEngineSystem ( engineResources , RigidStaticFrameBegin );
			SystemTimer::EndTimeSegment ( "RigidStaticFrameBegin" );
			SystemTimer::StartTimeSegment ( "RigidDynamicFrameBegin" );
			enttxsol.RunEngineSystem ( engineResources , RigidDynamicFrameBegin );
			SystemTimer::EndTimeSegment ( "RigidDynamicFrameBegin" );
		}
	}

	// runs at the end of a frame, i.e. runs after EngineSystems()
	// also guaranteed to run after any other major system update.
	void EngineSystemsFrameEnd ( EnttXSol& enttxsol , EngineResources& engineResources )
	{
		//physics
		SystemTimer::StartTimeSegment ( "Physics Frame End" );
		enttxsol.RunEngineSystem ( engineResources , DebugDynamic );
		enttxsol.RunEngineSystem ( engineResources , DebugStatic );

		SystemTimer::EndTimeSegment ( "Physics Frame End" );

		if ( !enttxsol.m_pause )
		{
			enttxsol.RunEngineSystem ( engineResources , ExampleSystemFrameEnd );
			SystemTimer::StartTimeSegment ( "RigidDynamicFrameEnd" );
			enttxsol.RunEngineSystem ( engineResources , RigidDynamicFrameEnd );
			SystemTimer::EndTimeSegment ( "RigidDynamicFrameEnd" );

			SystemTimer::StartTimeSegment("AudioListenerSystem");
			enttxsol.RunEngineSystem(engineResources, AudioListenerSystem);
			SystemTimer::EndTimeSegment("AudioListenerSystem");
		}

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