#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void RigidDynamicFrameBegin(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidDynamic& > component)
	{
		if (entt.m_pause)
		{
			Transform& transform = std::get<1>(component);
			RigidDynamic& rigiddynamic = std::get<2>(component);

			if (rigiddynamic.p_RigidDynamic)
			{
				/*
				physx::PxTransform tmp = rigiddynamic.p_RigidDynamic->getGlobalPose();
				tmp.p = transform.m_position;
				tmp.q = ConvertToPhysXQuat({ transform.m_rotation });
				rigiddynamic.p_RigidDynamic->setGlobalPose(tmp) ;
				*/

				rigiddynamic.p_RigidDynamic->setGlobalPose(ConvertToPhysXTransform(transform.m_position, transform.m_rotation));
			}
		}
	}
	void RigidDynamicUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, PhysicsMaterial&, PhysicsShape&, RigidDynamic& > component)
	{
		Transform& transform = std::get<1>(component);
		PhysicsMaterial& phymaterial = std::get<2>(component);
		PhysicsShape& physhape = std::get<3>(component);
		RigidDynamic& rigiddynamic = std::get<4>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();

		if (rigiddynamic.Active == false)
		{
			mScene->removeActor(*rigiddynamic.p_RigidDynamic);
			rigiddynamic.p_RigidDynamic->release();
		}

		if (rigiddynamic.updateRigidDynamic)
		{
			if (rigiddynamic.p_RigidDynamic == nullptr)
			{
				rigiddynamic.p_RigidDynamic = mPhysics->createRigidDynamic(physx::PxTransform(transform.m_position.x,
					transform.m_position.y, transform.m_position.z));

				rigiddynamic.mScene = mScene;

				rigiddynamic.p_RigidDynamic->attachShape(*physhape.p_shape);
				rigiddynamic.p_RigidDynamic->setAngularVelocity(physx::PxVec3(rigiddynamic.AngularVelocity.x, rigiddynamic.AngularVelocity.y, rigiddynamic.AngularVelocity.z), true);
				physx::PxRigidBodyExt::updateMassAndInertia(*rigiddynamic.p_RigidDynamic, rigiddynamic.Density);

				mScene->addActor(*rigiddynamic.p_RigidDynamic);
			}
			else
			{

			}
			std::cout << " rigidstatic Updated\n";
			rigiddynamic.updateRigidDynamic = false;

		}

	}

	void RigidDynamicFrameEnd(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidDynamic& > component)
	{
		Transform& transform = std::get<1>(component);
		RigidDynamic& rigiddynamic = std::get<2>(component);

		if (entt.m_pause)
		{
			return;
		}

		if (rigiddynamic.p_RigidDynamic)
		{
			physx::PxTransform ptransform = rigiddynamic.p_RigidDynamic->getGlobalPose();

			transform.m_position.x = ptransform.p.x;
			transform.m_position.y = ptransform.p.y;
			transform.m_position.z = ptransform.p.z;

			glm::quat m =ConvertToGlmQuat(ptransform.q);
			

			//update transform based on phy system
			transform.m_rotation = glm::degrees(glm::eulerAngles(m));
		}
	}
}
