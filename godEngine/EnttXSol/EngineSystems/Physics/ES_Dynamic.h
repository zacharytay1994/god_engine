#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"
#include "../../../Physics/DiceCallBack.h"



using namespace physx;
namespace god
{

	/**
	 * For the editor to set PhysXTransform when game is paused.
	 *
	 * \param entt
	 * \param engineResources
	 * \param component
	 */
	void RigidDynamicFrameBegin ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& , Renderable3D& > component )
	{
		( engineResources );
		if ( !entt.m_pause )
		{
			return;
		}	//else if entt.m_pause
		Transform& transform = std::get<1> ( component );
		RigidDynamic& rigiddynamic = std::get<2> ( component );

		if ( rigiddynamic.p_RigidDynamic )
		{
			rigiddynamic.p_RigidDynamic->setGlobalPose ( ConvertToPhysXTransform ( transform.m_position , transform.m_rotation ) );
		}
	}


	/**
	 * Runs every update loop.
	 *
	 * \param entt
	 * \param engineResources
	 * \param component
	 */
	void RigidDynamicUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& , Renderable3D& > component )
	{
		( entt );
		( engineResources );
		//EntityData& edata = std::get<0>(component);
		//Transform& transform = std::get<1>(component);
		RigidDynamic& rigiddynamic = std::get<2> ( component );
		//Renderable3D& renderable = std::get<3>(component);
		//PhysicsSystem& psys = engineResources.Get<PhysicsSystem>().get();

		//physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		//physx::PxCooking* mCooking = engineResources.Get<PhysicsSystem>().get().GetCooking();
		//physx::PxScene* mScene = engineResources.Get<PhysicsSystem>().get().GetPhysicsScene();
		//Asset3DManager& assetmgr = engineResources.Get<Asset3DManager>().get();

		//Setting flags
		if ( rigiddynamic.Active != rigiddynamic.Activeflag )
		{
			if ( !rigiddynamic.Active )// Not active
			{
				rigiddynamic.p_RigidDynamic->putToSleep ();
				rigiddynamic.p_RigidDynamic->setActorFlag ( PxActorFlag::eDISABLE_SIMULATION , !rigiddynamic.Active );
				rigiddynamic.Activeflag = rigiddynamic.Active;
			}
			if ( rigiddynamic.Active )//  active
			{
				rigiddynamic.p_RigidDynamic->setActorFlag ( PxActorFlag::eDISABLE_SIMULATION , !rigiddynamic.Active );
				rigiddynamic.p_RigidDynamic->wakeUp ();
				rigiddynamic.Activeflag = rigiddynamic.Active;
			}
		}
		if ( rigiddynamic.Gravity != rigiddynamic.Gravityflag )
		{
			rigiddynamic.p_RigidDynamic->setActorFlag ( PxActorFlag::eDISABLE_GRAVITY , !rigiddynamic.Gravity );
			rigiddynamic.Gravityflag = rigiddynamic.Gravity;
		}
	}

	void RigidDynamicFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& > component )
	{
		( entt );
		//update only when physx simulating

		PhysicsSystem& psys = engineResources.Get<PhysicsSystem> ().get ();
		//wait for simulation
		while ( psys.GetisRunning () );
		//EntityData& edata = std::get<0>(component);
		Transform& transform = std::get<1> ( component );
		RigidDynamic& rigiddynamic = std::get<2> ( component );

		if ( rigiddynamic.p_RigidDynamic )
		{
			physx::PxTransform ptransform = rigiddynamic.p_RigidDynamic->getGlobalPose ();

			transform.m_position.x = ptransform.p.x;
			transform.m_position.y = ptransform.p.y;
			transform.m_position.z = ptransform.p.z;

			glm::quat m = ConvertToGlmQuat ( ptransform.q );

			//update transform based on phy system
			transform.m_rotation = glm::degrees ( glm::eulerAngles ( m ) );
		}
	}

	/**
	 * Initialises RigidDynamic Components and set the PhysXTransform according
	 * to the Transform component.
	 *
	 * \param entt
	 * \param engineResources
	 * \param component
	 */
	void RigidDynamicInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple< EntityData& , Transform& , RigidDynamic& , Renderable3D& > component )
	{
		( entt );
		EntityData& edata = std::get<0> ( component );
		Transform& transform = std::get<1> ( component );
		RigidDynamic& rigiddynamic = std::get<2> ( component );
		Renderable3D& renderable = std::get<3> ( component );
		//PhysicsSystem& psys = engineResources.Get<PhysicsSystem>().get();

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem> ().get ().GetPhysics ();
		physx::PxCooking* mCooking = engineResources.Get<PhysicsSystem> ().get ().GetCooking ();
		physx::PxScene* mScene = engineResources.Get<PhysicsSystem> ().get ().GetPhysicsScene ();
		Asset3DManager& assetmgr = engineResources.Get<Asset3DManager> ().get ();

		if ( rigiddynamic.updateRigidDynamic )
		{
			//std::cout << "init phy obj\n";
			if ( rigiddynamic.locktoscale )
			{
				rigiddynamic.extents = transform.m_scale;
			}
			if ( rigiddynamic.p_material == nullptr )
			{
				rigiddynamic.p_material = mPhysics->createMaterial ( rigiddynamic.StaticFriction , rigiddynamic.DynamicFriction , rigiddynamic.Restitution );
			}
			else
			{
				rigiddynamic.p_material->setStaticFriction ( rigiddynamic.StaticFriction );
				rigiddynamic.p_material->setDynamicFriction ( rigiddynamic.DynamicFriction );
				rigiddynamic.p_material->setRestitution ( rigiddynamic.Restitution );
			}
			//exclusive shape (can be modified)

			physx::PxReal mass = 0.f;

			switch ( rigiddynamic.Shapeid )
			{
			case PhysicsTypes::Cube:
				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxBoxGeometry ( rigiddynamic.extents.x , rigiddynamic.extents.y , rigiddynamic.extents.z ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );

				mass = 2.f * rigiddynamic.extents.x * 2.f * rigiddynamic.extents.y * 2.f * rigiddynamic.extents.z * rigiddynamic.Density;// L^3
				break;
			case PhysicsTypes::Sphere:

				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxSphereGeometry ( rigiddynamic.extents.x ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				mass = ( 4.f / 3.f ) * physx::PxPi * rigiddynamic.extents.x * rigiddynamic.extents.x * rigiddynamic.extents.x;//4/3 * pi * r^3
				break;
			case PhysicsTypes::Capsule:

				rigiddynamic.p_shape = mPhysics->createShape ( physx::PxCapsuleGeometry ( rigiddynamic.extents.x , rigiddynamic.extents.y ) , *rigiddynamic.p_material , true );
				rigiddynamic.p_shape->setMaterials ( &rigiddynamic.p_material , 1 );
				mass = ( ( 4.f / 3.f ) * physx::PxPi * rigiddynamic.extents.x * rigiddynamic.extents.x * rigiddynamic.extents.x )
					+ ( physx::PxPi * rigiddynamic.extents.x * rigiddynamic.extents.x * rigiddynamic.extents.y );//4/3 * pi * r^3 + pi * r^2 * h 
				break;

			case PhysicsTypes::TriangleMesh:
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


			

			rigiddynamic.p_RigidDynamic = mPhysics->createRigidDynamic ( physx::PxTransform (ConvertToPhysXTransform(transform.m_position, transform.m_rotation) ) );

			rigiddynamic.mScene = mScene;

			rigiddynamic.p_RigidDynamic->attachShape ( *rigiddynamic.p_shape );

			mScene->addActor ( *rigiddynamic.p_RigidDynamic );
			rigiddynamic.p_RigidDynamic->setContactReportThreshold ( 0.01f );

			physx::PxRigidBodyExt::setMassAndUpdateInertia ( *rigiddynamic.p_RigidDynamic , mass );
			physx::PxRigidBodyExt::updateMassAndInertia ( *rigiddynamic.p_RigidDynamic , rigiddynamic.Density );

			rigiddynamic.p_RigidDynamic->putToSleep ();
			rigiddynamic.p_RigidDynamic->clearForce ();
			rigiddynamic.p_RigidDynamic->setContactReportThreshold ( 0.1f );
			rigiddynamic.p_RigidDynamic->setAngularVelocity ( physx::PxVec3 ( rigiddynamic.AngularVelocity.x , rigiddynamic.AngularVelocity.y , rigiddynamic.AngularVelocity.z ) , true );
			rigiddynamic.p_RigidDynamic->setLinearVelocity ( physx::PxVec3 ( rigiddynamic.LinearVelocity.x , rigiddynamic.LinearVelocity.y , rigiddynamic.LinearVelocity.z ) , true );
			
			rigiddynamic.p_RigidDynamic->setGlobalPose ( ConvertToPhysXTransform ( transform.m_position , transform.m_rotation ) );
			rigiddynamic.p_RigidDynamic->setMaxLinearVelocity ( 50.f );

			rigiddynamic.m_id = edata.m_id;
			rigiddynamic.p_RigidDynamic->userData = &rigiddynamic;
			rigiddynamic.p_RigidDynamic->setName ( "Dynamic" );

			rigiddynamic.p_RigidDynamic->setActorFlag ( PxActorFlag::eDISABLE_SIMULATION , !rigiddynamic.Active );
			rigiddynamic.p_RigidDynamic->setActorFlag ( PxActorFlag::eDISABLE_GRAVITY , !rigiddynamic.Gravity );
			rigiddynamic.Activeflag = rigiddynamic.Active;
			rigiddynamic.Gravityflag = rigiddynamic.Gravity;

			if ( rigiddynamic.Trigger )
			{
				rigiddynamic.Simulation = false;
				rigiddynamic.p_shape->setFlag ( PxShapeFlag::eSIMULATION_SHAPE , false );
				rigiddynamic.p_shape->setFlag ( PxShapeFlag::eTRIGGER_SHAPE , true );
			}
			else
			{
				rigiddynamic.p_shape->setFlag ( PxShapeFlag::eTRIGGER_SHAPE , false );
			}

			if ( rigiddynamic.Simulation )
			{
				rigiddynamic.Trigger = false;
				rigiddynamic.p_shape->setFlag ( PxShapeFlag::eTRIGGER_SHAPE , false );
				rigiddynamic.p_shape->setFlag ( PxShapeFlag::eSIMULATION_SHAPE , true );

			}
			else
			{
				rigiddynamic.p_shape->setFlag ( PxShapeFlag::eSIMULATION_SHAPE , false );
			}
			rigiddynamic.updateRigidDynamic = false;
		}
	}//init rigiddynamic

}//god
