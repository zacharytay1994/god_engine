#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"
#include "../../../OpenGL/OpenGL.h"
namespace god
{



	void RayCastDynamic(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidDynamic& > component)
	{
		( entt );
		PhysicsSystem& psystem = engineResources.Get<PhysicsSystem>().get();
		RigidDynamic& rigiddynamic = std::get<2>(component);
		EntityData& edata = std::get<0>(component);
		if (rigiddynamic.p_RigidDynamic == psystem.GetRayCastMouse())
		{
			//std::cout << "raycast: " << edata.m_id << std::endl;
			psystem.setRCMid(edata.m_id);
		}
	}

	void RayCastStatic(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		( entt );
		PhysicsSystem& psystem = engineResources.Get<PhysicsSystem>().get();
		RigidStatic& rigidstatic = std::get<2>(component);
		EntityData& edata = std::get<0>(component);
		if (rigidstatic.p_RigidStatic == psystem.GetRayCastMouse())
		{
			//std::cout << "raycast: " << edata.m_id << std::endl;
			psystem.setRCMid(edata.m_id);
		}

	}

}