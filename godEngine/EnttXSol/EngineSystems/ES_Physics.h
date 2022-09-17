#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

namespace god
{
	void UpdateBase(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, PhysicsMaterial& , PhysicsShape& > component)
	{
		Transform& transform = std::get<1>(component);
		PhysicsMaterial& phymaterial = std::get<2>(component);
		PhysicsShape& physhape = std::get<3>(component);
		
		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		//std::map < std::string, physx::PxMaterial*> & materialcont = engineResources.Get<PhysicsSystem>().get().GetMaterialContainer();

		//Update Physics Material Properties
		if (phymaterial.updatematerial)
		{
			if (phymaterial.p_material == nullptr)
			{
				phymaterial.p_material = mPhysics->createMaterial(phymaterial.StaticFriction, phymaterial.DynamicFriction, phymaterial.Restitution);
			}
			else
			{
				phymaterial.p_material->setDynamicFriction(phymaterial.DynamicFriction);
				phymaterial.p_material->setStaticFriction(phymaterial.StaticFriction);
				phymaterial.p_material->setRestitution(phymaterial.Restitution);


			}
			std::cout << " Material Updated\n";
			phymaterial.updatematerial = false;

		}
		
		//Update Physics Material Properties
		if (physhape.updatePhysicsShape)
		{
			if (physhape.p_shape == nullptr)
			{
				
				physhape.p_shape = mPhysics->createShape(physx::PxBoxGeometry(physhape.extents.x, physhape.extents.y, physhape.extents.z), *phymaterial.p_material);
			}
			else
			{
				//As Attached Shapes cannot be modified
				physhape.p_shape->release();
				physhape.p_shape = mPhysics->createShape(physx::PxBoxGeometry(physhape.extents.x, physhape.extents.y, physhape.extents.z), *phymaterial.p_material);
		
			}
			std::cout << " Shape Updated\n";
			physhape.updatePhysicsShape = false;
		}
	}

	void UpdateStatic(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, PhysicsMaterial&, PhysicsShape&, RigidStatic& > component)
	{
		Transform& transform = std::get<1>(component);
		PhysicsMaterial& phymaterial = std::get<2>(component);
		PhysicsShape& physhape = std::get<3>(component);
		RigidStatic& rigidstatic = std::get<4>(component);


		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();
		//std::map < std::string, physx::PxMaterial*> & materialcont = engineResources.Get<PhysicsSystem>().get().GetMaterialContainer();

		if (rigidstatic.updateRigidStatic)
		{
			if (rigidstatic.p_RigidStatic == nullptr)
			{
				rigidstatic.p_RigidStatic = mPhysics->createRigidStatic(physx::PxTransform(transform.m_position.x + rigidstatic.Offset.x,
					transform.m_position.y + rigidstatic.Offset.y,
					transform.m_position.z + rigidstatic.Offset.z));

				rigidstatic.p_RigidStatic->attachShape(*physhape.p_shape);

				mScene->addActor(*rigidstatic.p_RigidStatic);

			}
			else
			{
				rigidstatic.p_RigidStatic->setGlobalPose(physx::PxTransform(transform.m_position.x + rigidstatic.Offset.x,
					transform.m_position.y + rigidstatic.Offset.y,
					transform.m_position.z + rigidstatic.Offset.z));

				rigidstatic.p_RigidStatic->attachShape(*physhape.p_shape);
			
		
			}

			std::cout << " rigidstatic Updated\n";
			rigidstatic.updateRigidStatic = false;
		}


	}
	void UpdateDynamic(EnttXSol & entt, EngineResources & engineResources, std::tuple< EntityData&, Transform&, PhysicsMaterial&, PhysicsShape&, RigidDynamic& > component)
	{
		Transform& transform = std::get<1>(component);
		PhysicsMaterial& phymaterial = std::get<2>(component);
		PhysicsShape& physhape = std::get<3>(component);
		RigidDynamic& rigiddynamic = std::get<4>(component);


		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();
		//std::map < std::string, physx::PxMaterial*> & materialcont = engineResources.Get<PhysicsSystem>().get().GetMaterialContainer();

		if (rigiddynamic.updateRigidDynamic)
		{
			if (rigiddynamic.p_RigidDynamic == nullptr)
			{
				rigiddynamic.p_RigidDynamic = mPhysics->createRigidDynamic(physx::PxTransform(transform.m_position.x,
					transform.m_position.y,transform.m_position.z));

				rigiddynamic.p_RigidDynamic->attachShape(*physhape.p_shape);

				mScene->addActor(*rigiddynamic.p_RigidDynamic);
			}
			else
			{



			}
			std::cout << " rigidstatic Updated\n";
			rigiddynamic.updateRigidDynamic = false;

		}
		physx::PxTransform pos = rigiddynamic.p_RigidDynamic->getGlobalPose();
		transform.m_position.x = pos.p.x;
		transform.m_position.y = pos.p.y;
		transform.m_position.z = pos.p.z;

	}


}