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

	void RigidStaticUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{

		Transform& transform = std::get<1>(component);
		RigidStatic& rigidstatic = std::get<2>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();


		if (rigidstatic.Active == false)
		{
			mScene->removeActor(*rigidstatic.p_RigidStatic);
		}


		if (rigidstatic.updateRigidStatic)
		{
			if (rigidstatic.p_material == nullptr)
			{
				rigidstatic.p_material = mPhysics->createMaterial(rigidstatic.StaticFriction, rigidstatic.DynamicFriction, rigidstatic.Restitution);
			}
			if (rigidstatic.p_shape == nullptr)
			{
				//exclusive shape (can be modified)


				switch (rigidstatic.shapeid)
				{
				case RigidStatic::Cube:
					rigidstatic.p_shape = mPhysics->createShape(physx::PxBoxGeometry(rigidstatic.extents.x, rigidstatic.extents.y, rigidstatic.extents.z), *rigidstatic.p_material, true);
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				case RigidStatic::Sphere:
					rigidstatic.p_shape = mPhysics->createShape(physx::PxSphereGeometry(rigidstatic.extents.x), *rigidstatic.p_material, true);
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				case RigidStatic::Capsule:
					rigidstatic.p_shape = mPhysics->createShape(physx::PxCapsuleGeometry(rigidstatic.extents.x, rigidstatic.extents.y), *rigidstatic.p_material, true);
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;

				}


			}
			if (rigidstatic.p_RigidStatic == nullptr)
			{
				rigidstatic.p_RigidStatic = mPhysics->createRigidStatic(physx::PxTransform(
					transform.m_position.x + rigidstatic.Offset.x,
					transform.m_position.y + rigidstatic.Offset.y,
					transform.m_position.z + rigidstatic.Offset.z));

				rigidstatic.mScene = mScene;
				rigidstatic.p_RigidStatic->attachShape(*rigidstatic.p_shape);

				mScene->addActor(*rigidstatic.p_RigidStatic);

			}
			
			rigidstatic.updateRigidStatic = false;
		}

		if (rigidstatic.locktoscale)
		{
			rigidstatic.extents = transform.m_scale;
			rigidstatic.p_shape->setGeometry(physx::PxBoxGeometry(rigidstatic.extents.x, rigidstatic.extents.y, rigidstatic.extents.z));
		}

		rigidstatic.p_RigidStatic->setGlobalPose(physx::PxTransform(transform.m_position.x + rigidstatic.Offset.x,
			transform.m_position.y + rigidstatic.Offset.y,
			transform.m_position.z + rigidstatic.Offset.z));

	}

	/*
	void RigidStaticFrameEnd(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		//RigidStatic Does not move
	}
	*/


}
