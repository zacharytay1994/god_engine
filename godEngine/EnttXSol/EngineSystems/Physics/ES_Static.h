#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	/**
	 * For the editor to set PhysXTransform when game is paused.
	 *
	 * \param entt
	 * \param engineResources
	 * \param component
	 */
	void RigidStaticFrameBegin(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		( engineResources );
		if (!entt.m_pause)
		{
			return;
		}	//else if entt.m_pause
		Transform& transform = std::get<1>(component);
		RigidStatic& rigidstatic = std::get<2>(component);

		if (rigidstatic.p_RigidStatic)
		{
			auto posnoffset = glm::vec3(transform.m_position.x + rigidstatic.Offset.x,
				transform.m_position.y + rigidstatic.Offset.y, transform.m_position.z + rigidstatic.Offset.z);

			rigidstatic.p_RigidStatic->setGlobalPose(ConvertToPhysXTransform(posnoffset, transform.m_rotation));
		}
	}

	/**
	 * Runs every update loop.
	 *
	 * \param entt
	 * \param engineResources
	 * \param component
	 */
	void RigidStaticUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		( entt );
		( engineResources );
		//EntityData& edata = std::get<0>(component);
		//Transform& transform = std::get<1>(component);
		RigidStatic& rigidstatic = std::get<2>(component);

		//physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		//physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();
	
		//Setting flags
		if (rigidstatic.Active != rigidstatic.Activeflag)
		{
			if (!rigidstatic.Active)// Not active
			{
				rigidstatic.p_RigidStatic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !rigidstatic.Active);
				rigidstatic.Activeflag = rigidstatic.Active;
			}
			if (rigidstatic.Active)//  active
			{
				rigidstatic.p_RigidStatic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !rigidstatic.Active);
				rigidstatic.Activeflag = rigidstatic.Active;
			}
		}
	}

	//void RigidStaticFrameEnd(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	//{
	//	Transform& transform = std::get<1>(component);
	//	RigidStatic& rigidstatic = std::get<2>(component);

	//}
	

	void RigidStaticInit(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		( entt );
		//EntityData& edata = std::get<0>(component);
		Transform& transform = std::get<1>(component);
		RigidStatic& rigidstatic = std::get<2>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();

		if (rigidstatic.updateRigidStatic)
		{
			if (rigidstatic.p_material == nullptr)
			{
				rigidstatic.p_material = mPhysics->createMaterial(rigidstatic.StaticFriction, rigidstatic.DynamicFriction, rigidstatic.Restitution);
			}
			if (rigidstatic.p_shape == nullptr)
			{
				//exclusive shape (can be modified)
				switch (rigidstatic.Shapeid)
				{
				case PhysicsTypes::Cube:
					rigidstatic.p_shape = mPhysics->createShape(physx::PxBoxGeometry(rigidstatic.extents.x, rigidstatic.extents.y, rigidstatic.extents.z), *rigidstatic.p_material, true);
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				case PhysicsTypes::Sphere:
					rigidstatic.p_shape = mPhysics->createShape(physx::PxSphereGeometry(rigidstatic.extents.x), *rigidstatic.p_material, true);
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				case PhysicsTypes::Capsule:
					rigidstatic.p_shape = mPhysics->createShape(physx::PxCapsuleGeometry(rigidstatic.extents.x, rigidstatic.extents.y), *rigidstatic.p_material, true);
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				}
			}
			if (rigidstatic.p_RigidStatic == nullptr)
			{
				auto posnoffset = glm::vec3(transform.m_position.x + rigidstatic.Offset.x,
					transform.m_position.y + rigidstatic.Offset.y, transform.m_position.z + rigidstatic.Offset.z);

				rigidstatic.p_RigidStatic = mPhysics->createRigidStatic(ConvertToPhysXTransform(posnoffset, transform.m_rotation) );
				rigidstatic.mScene = mScene;
				rigidstatic.p_RigidStatic->attachShape(*rigidstatic.p_shape);
				mScene->addActor(*rigidstatic.p_RigidStatic);
			}
			//rigidstatic.p_RigidStatic->setActorFlag()
			rigidstatic.p_RigidStatic->userData = &rigidstatic;
			rigidstatic.p_RigidStatic->setName("Static");
			if (rigidstatic.locktoscale)
			{
				rigidstatic.extents = transform.m_scale;
				switch (rigidstatic.Shapeid)
				{
				case PhysicsTypes::Cube:
					rigidstatic.p_shape->setGeometry(physx::PxBoxGeometry(rigidstatic.extents.x, rigidstatic.extents.y, rigidstatic.extents.z));
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				case PhysicsTypes::Sphere:
					rigidstatic.p_shape->setGeometry(physx::PxSphereGeometry(rigidstatic.extents.x));
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				case PhysicsTypes::Capsule:
					rigidstatic.p_shape->setGeometry(physx::PxCapsuleGeometry(rigidstatic.extents.x, rigidstatic.extents.y));
					rigidstatic.p_shape->setMaterials(&rigidstatic.p_material, 1);
					break;
				}
			}
			rigidstatic.updateRigidStatic = false;
		}
	}//init static

}//god
