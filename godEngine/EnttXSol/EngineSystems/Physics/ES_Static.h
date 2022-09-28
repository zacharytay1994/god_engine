#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void RigidStaticFrameBegin(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		if (entt.m_pause)
		{
			Transform& transform = std::get<1>(component);
			RigidStatic& rigidstatic = std::get<2>(component);

			if (rigidstatic.p_RigidStatic)
			{
				rigidstatic.p_RigidStatic->setGlobalPose(ConvertToPhysXTransform(transform.m_position, transform.m_rotation));
			}
		}
	}

	void RigidStaticUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, PhysicsMaterial&, PhysicsShape&, RigidStatic& > component)
	{

		Transform& transform = std::get<1>(component);
		PhysicsMaterial& phymaterial = std::get<2>(component);
		PhysicsShape& physhape = std::get<3>(component);
		RigidStatic& rigidstatic = std::get<4>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();


		if (rigidstatic.Active == false)
		{
			mScene->removeActor(*rigidstatic.p_RigidStatic);
		}


		if (rigidstatic.updateRigidStatic)
		{
			if (rigidstatic.p_RigidStatic == nullptr)
			{
				rigidstatic.p_RigidStatic = mPhysics->createRigidStatic(physx::PxTransform(
					transform.m_position.x + rigidstatic.Offset.x,
					transform.m_position.y + rigidstatic.Offset.y,
					transform.m_position.z + rigidstatic.Offset.z));

				rigidstatic.mScene = mScene;
				rigidstatic.p_RigidStatic->attachShape(*physhape.p_shape);

				mScene->addActor(*rigidstatic.p_RigidStatic);

			}
			else
			{
				rigidstatic.p_RigidStatic->setGlobalPose(physx::PxTransform(transform.m_position.x + rigidstatic.Offset.x,
					transform.m_position.y + rigidstatic.Offset.y,
					transform.m_position.z + rigidstatic.Offset.z));
			}
			//std::cout << " rigid static Updated\n";
			rigidstatic.updateRigidStatic = false;
		}


	}

	/*
	void RigidStaticFrameEnd(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		//RigidStatic Does not move
	}
	*/


}
