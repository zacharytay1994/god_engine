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
				rigiddynamic.p_RigidDynamic->setGlobalPose(ConvertToPhysXTransform(transform.m_position, transform.m_rotation));
			}
		}
	}
	void RigidDynamicUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&,  RigidDynamic& > component)
	{
		Transform& transform = std::get<1>(component);

		RigidDynamic& rigiddynamic = std::get<2>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();



		
		if (rigiddynamic.updateRigidDynamic)
		{
			if (rigiddynamic.p_material == nullptr)
			{
				rigiddynamic.p_material = mPhysics->createMaterial(rigiddynamic.StaticFriction, rigiddynamic.DynamicFriction, rigiddynamic.Restitution);
			}
			if (rigiddynamic.p_shape == nullptr)
			{
				//exclusive shape (can be modified)


				switch (rigiddynamic.shapeid)
				{
				case RigidDynamic::Cube:
					rigiddynamic.p_shape = mPhysics->createShape(physx::PxBoxGeometry(rigiddynamic.extents.x, rigiddynamic.extents.y, rigiddynamic.extents.z), *rigiddynamic.p_material, true);
					rigiddynamic.p_shape->setMaterials(&rigiddynamic.p_material, 1);
					break;
				case RigidDynamic::Sphere:
					rigiddynamic.p_shape = mPhysics->createShape(physx::PxSphereGeometry(rigiddynamic.extents.x), *rigiddynamic.p_material, true);
					rigiddynamic.p_shape->setMaterials(&rigiddynamic.p_material, 1);
					break;
				case RigidDynamic::Capsule:
					rigiddynamic.p_shape = mPhysics->createShape(physx::PxCapsuleGeometry(rigiddynamic.extents.x, rigiddynamic.extents.y), *rigiddynamic.p_material, true);
					rigiddynamic.p_shape->setMaterials(&rigiddynamic.p_material, 1);
					break;
				case RigidDynamic::TriangleMesh:
					/*
					using namespace physx;
					
					PxTriangleMeshGeometry triGeom;
					triGeom.triangleMesh = triangleMesh;
					physx::PxTriangleMeshGeometry()
					rigiddynamic.p_shape = mPhysics->createShape(physx::PxCapsuleGeometry(rigiddynamic.extents.x, rigiddynamic.extents.y), *rigiddynamic.p_material, true);
					rigiddynamic.p_shape->setMaterials(&rigiddynamic.p_material, 1);
					*/
					break;
				}
			}

			physx::PxRigidDynamic* meshActor = mPhysics->createRigidDynamic(physx::PxTransform(1.0f,1.f,1.f));
			physx::PxShape* meshShape;
			if (meshActor)
			{
				meshActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

			

				physx::PxTriangleMeshGeometry triGeom;
				triGeom.triangleMesh = triangleMesh;
				meshShape = physx::PxRigidActorExt::createExclusiveShape(*meshActor, triGeom,
				*rigiddynamic.p_material);
				mScene->addActor(*meshActor);

				PxConvexMeshGeometry convexGeom = PxConvexMeshGeometry(convexBox);
				convexShape = PxRigidActorExt::createExclusiveShape(*meshActor, convexGeom,
					defaultMaterial);



			if (rigiddynamic.p_RigidDynamic == nullptr)
			{
				rigiddynamic.p_RigidDynamic = mPhysics->createRigidDynamic(physx::PxTransform(transform.m_position.x,
					transform.m_position.y, transform.m_position.z));

				rigiddynamic.mScene = mScene;

				rigiddynamic.p_RigidDynamic->attachShape(*rigiddynamic.p_shape);
				rigiddynamic.p_RigidDynamic->setAngularVelocity(physx::PxVec3(rigiddynamic.AngularVelocity.x, rigiddynamic.AngularVelocity.y, rigiddynamic.AngularVelocity.z), true);
				physx::PxRigidBodyExt::updateMassAndInertia(*rigiddynamic.p_RigidDynamic, rigiddynamic.Density);

				mScene->addActor(*rigiddynamic.p_RigidDynamic);
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

			glm::quat m =ConvertToGlmQuat(ptransform.q);
			

			//update transform based on phy system
			transform.m_rotation = glm::degrees(glm::eulerAngles(m));
		}
	}
}
