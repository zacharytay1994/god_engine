#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"
using namespace physx;
#if 0
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
				rigiddynamic.p_RigidDynamic->setGlobalPose(ConvertToPhysXTransform(transform.m_position, transform.m_rotation));
			}
		}
	}
	void RigidDynamicUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidDynamic&, Renderable3D& > component)
	{
		Transform& transform = std::get<1>(component);
		Renderable3D& renderable = std::get<3>(component);
		RigidDynamic& rigiddynamic = std::get<2>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxCooking* mCooking = engineResources.Get<PhysicsSystem>().get().GetCooking();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();
		Asset3DManager& assetmgr = engineResources.Get<Asset3DManager>().get();

		physx::PxRigidActor* test;
		test->

		if (rigiddynamic.updateRigidDynamic)
		{
			if (rigiddynamic.p_material == nullptr)
			{
				rigiddynamic.p_material = mPhysics->createMaterial(rigiddynamic.StaticFriction, rigiddynamic.DynamicFriction, rigiddynamic.Restitution);
			}
			if (rigiddynamic.p_RigidDynamic == nullptr)
			{

				rigiddynamic.p_RigidDynamic = mPhysics->createRigidDynamic(physx::PxTransform(transform.m_position.x,
					transform.m_position.y, transform.m_position.z));



			}

			if (rigiddynamic.p_shape == nullptr)
			{
				


			}







			rigiddynamic.updateRigidDynamic = false;

		}

		if (rigiddynamic.locktoscale)
		{
			rigiddynamic.extents = transform.m_scale;
			rigiddynamic.p_shape->setGeometry(physx::PxBoxGeometry(rigiddynamic.extents.x, rigiddynamic.extents.y, rigiddynamic.extents.z));
		}

	}

	void RigidDynamicFrameEnd(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidDynamic& > component)
	{
		Transform& transform = std::get<1>(component);
		RigidDynamic& rigiddynamic = std::get<2>(component);
		if (rigiddynamic.Active == false)
		{
			rigiddynamic.mScene->removeActor(*rigiddynamic.p_RigidDynamic);

		}
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

			glm::quat m = ConvertToGlmQuat(ptransform.q);


			//update transform based on phy system
			transform.m_rotation = glm::degrees(glm::eulerAngles(m));
		}
	}
}
#endif