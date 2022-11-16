#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"
#include "../../../Physics/DiceCallBack.h"

using namespace physx;
namespace god
{
	void RigidDynamicFrameBegin ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& > component )
	{
		( engineResources );
		if ( !entt.m_pause )
		{
			return;
		}//else if entt.m_pause
		Transform& transform = std::get<1> ( component );
		RigidDynamic& rigiddynamic = std::get<2> ( component );

		if ( rigiddynamic.p_RigidDynamic )
		{
			rigiddynamic.p_RigidDynamic->setGlobalPose ( ConvertToPhysXTransform ( transform.m_position , transform.m_rotation ) );
		}

	}



	void RigidDynamicUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& , Renderable3D& > component )
	{
		if ( !entt.m_pause )
		{
			return;
		}//else if entt.m_pause
		EntityData& edata = std::get<0>(component);
		Transform& transform = std::get<1> ( component );
		Renderable3D& renderable = std::get<3> ( component );
		RigidDynamic& rigiddynamic = std::get<2> ( component );
		PhysicsSystem& psys = engineResources.Get<PhysicsSystem>().get();

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem> ().get ().GetPhysics ();
		physx::PxCooking* mCooking = engineResources.Get<PhysicsSystem> ().get ().GetCooking ();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem> ().get ().GetPhysicsScene ();
		Asset3DManager& assetmgr = engineResources.Get<Asset3DManager> ().get ();



		if ( rigiddynamic.initRigidDynamic)
		{

			if ( rigiddynamic.p_material == nullptr )
			{
				rigiddynamic.p_material = mPhysics->createMaterial ( rigiddynamic.StaticFriction , rigiddynamic.DynamicFriction , rigiddynamic.Restitution );
			}

			//exclusive shape (can be modified)
			switch ( rigiddynamic.shapeid )
			{
			case RigidDynamic::Cube:
				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxBoxGeometry ( rigiddynamic.extents.x , rigiddynamic.extents.y , rigiddynamic.extents.z ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;
			case RigidDynamic::Sphere:

				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxSphereGeometry ( rigiddynamic.extents.x ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;
			case RigidDynamic::Capsule:

				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxCapsuleGeometry ( rigiddynamic.extents.x , rigiddynamic.extents.y ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;

			case RigidDynamic::TriangleMesh:
				std::vector<Vertex3D> vertexes = std::get<1> ( assetmgr.Get ( renderable.m_model_id ) ).m_model.m_meshes[ 0 ].m_vertices;
				std::vector< uint32_t> indices = std::get<1> ( assetmgr.Get ( renderable.m_model_id ) ).m_model.m_meshes[ 0 ].m_indices;
				std::vector<glm::vec3> points;
				for ( auto const& vertex : vertexes )
				{
					points.push_back ( vertex.m_position );
				}
				PxTriangleMeshDesc meshDesc;
				meshDesc.points.count = static_cast< physx::PxU32 >( points.size () );
				meshDesc.points.stride = sizeof ( glm::vec3 );
				meshDesc.points.data = &( points.front () );

				meshDesc.triangles.count = static_cast< physx::PxU32 >( indices.size () );
				meshDesc.triangles.stride = 3 * sizeof ( uint32_t );
				meshDesc.triangles.data = &( indices.front () );

				PxDefaultMemoryOutputStream writeBuffer;
				PxTriangleMeshCookingResult::Enum result;
				bool status = mCooking->cookTriangleMesh ( meshDesc , writeBuffer , &result );
				if ( !status )
					std::cerr << "Failed to cook triangle mesh\n";

				PxDefaultMemoryInputData readBuffer ( writeBuffer.getData () , writeBuffer.getSize () );
				rigiddynamic.p_trimesh = mPhysics->createTriangleMesh ( readBuffer );

				PxMeshScale scale ( PxVec3 ( rigiddynamic.extents.x , rigiddynamic.extents.y , rigiddynamic.extents.z ) , PxQuat ( PxIdentity ) );
				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxTriangleMeshGeometry ( rigiddynamic.p_trimesh , scale ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;

			}

			rigiddynamic.p_RigidDynamic = mPhysics->createRigidDynamic ( physx::PxTransform ( transform.m_position.x ,
				transform.m_position.y , transform.m_position.z ) );

			rigiddynamic.mScene = mScene;

			rigiddynamic.p_RigidDynamic->attachShape ( *rigiddynamic.p_shape );
			rigiddynamic.p_RigidDynamic->setAngularVelocity ( physx::PxVec3 ( rigiddynamic.AngularVelocity.x , rigiddynamic.AngularVelocity.y , rigiddynamic.AngularVelocity.z ) , true );
			physx::PxReal mass = 10.f;
			physx::PxRigidBodyExt::setMassAndUpdateInertia ( *rigiddynamic.p_RigidDynamic , mass );
			physx::PxRigidBodyExt::updateMassAndInertia ( *rigiddynamic.p_RigidDynamic , rigiddynamic.Density );

			mScene->addActor ( *rigiddynamic.p_RigidDynamic );
			rigiddynamic.p_RigidDynamic->setContactReportThreshold(0.01f);


			for (auto const& tracked : ContactCallBack)
			{
				if (edata.m_id == tracked.first)
				{
					psys.getContactReportCallback().AddToContactTrack(rigiddynamic.p_RigidDynamic, tracked.second);
				}
			}

			for (auto& [entt1, entt2, fp, actor1, actor2] : psys.getContactReportCallback().getContactPairTrack() )
			{
				if (edata.m_id == entt1)
				{
					actor1 = rigiddynamic.p_RigidDynamic;
				}
				if (edata.m_id == entt2)
				{
					actor2 = rigiddynamic.p_RigidDynamic;
				}
			}
			rigiddynamic.p_RigidDynamic->userData = &edata;

			rigiddynamic.initRigidDynamic = false;

		}//init rigiddynamic

		if (rigiddynamic.Simulation)
		{
			//rigiddynamic.Trigger = false;
			//rigiddynamic.p_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			rigiddynamic.p_RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
		}
		else
		{
			//rigiddynamic.p_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			rigiddynamic.p_RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
		}

		if (rigiddynamic.Trigger)
		{
			rigiddynamic.p_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			rigiddynamic.p_shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}
		else
		{
			rigiddynamic.p_shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
			rigiddynamic.p_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		}

		if (rigiddynamic.Gravity)
		{
			rigiddynamic.p_RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
		}
		else
		{
			rigiddynamic.p_RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		}

		if ( rigiddynamic.locktoscale )
		{
			rigiddynamic.extents = transform.m_scale;
			switch ( rigiddynamic.shapeid )
			{
			case RigidDynamic::Cube:
				rigiddynamic.p_shape->setGeometry ( physx::PxBoxGeometry ( rigiddynamic.extents.x , rigiddynamic.extents.y , rigiddynamic.extents.z ) );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;
			case RigidDynamic::Sphere:
				rigiddynamic.p_shape->setGeometry ( physx::PxSphereGeometry ( rigiddynamic.extents.x ) );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;
			case RigidDynamic::Capsule:
				rigiddynamic.p_shape->setGeometry ( physx::PxCapsuleGeometry ( rigiddynamic.extents.x , rigiddynamic.extents.y ) );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				break;

			case RigidDynamic::TriangleMesh:
				PxMeshScale scale ( PxVec3 ( rigiddynamic.extents.x , rigiddynamic.extents.y , rigiddynamic.extents.z ) , PxQuat ( PxIdentity ) );
				rigiddynamic.p_shape->setGeometry ( physx::PxTriangleMeshGeometry ( rigiddynamic.p_trimesh , scale ) );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
			}

		}



	

	}

	void RigidDynamicFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& > component )
	{
		( entt );
		//update only when physx simulating

		PhysicsSystem& psys = engineResources.Get<PhysicsSystem> ().get ();
		//wait for simulation
		while ( psys.GetisRunning () )
			;
		EntityData& edata = std::get<0>(component);
		Transform& transform = std::get<1> ( component );
		RigidDynamic& rigiddynamic = std::get<2> ( component );
		


		if ( rigiddynamic.p_RigidDynamic )
		{
			//
			if (edata.m_id == 0)
			{
				physx::PxTransform ptransform1 = rigiddynamic.p_RigidDynamic->getGlobalPose();

				ptransform1.p.y -= 0.01f;
				rigiddynamic.p_RigidDynamic->setGlobalPose(ptransform1);

			}
			//
			physx::PxTransform ptransform = rigiddynamic.p_RigidDynamic->getGlobalPose ();

			transform.m_position.x = ptransform.p.x;
			transform.m_position.y = ptransform.p.y;
			transform.m_position.z = ptransform.p.z;

			glm::quat m = ConvertToGlmQuat ( ptransform.q );


			//update transform based on phy system
			transform.m_rotation = glm::degrees ( glm::eulerAngles ( m ) );
		}
	}
}
