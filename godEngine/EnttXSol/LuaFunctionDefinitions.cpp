#include "../pch.h"
#include "LuaFunctionDefinitions.h"

#include "EnttXSol.h"
#include "EngineComponents/EC_All.h"
#include "../Window/DeltaTimer.h"

#include <sol/sol.hpp>

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#include <glm/gtc/random.hpp>
#pragma warning(default: 4201)

#include <functional>

namespace god
{
	void RegisterLuaCPP ( EnttXSol& entt , EngineResources& engineResources , MainVariables& mainVariables )
	{
		// glm::vec3
		entt.RegisterLuaType<glm::vec3> ( "vec3" ,
			"x" , &glm::vec3::x ,
			"y" , &glm::vec3::y ,
			"z" , &glm::vec3::z );

		// glm::ivec3
		entt.RegisterLuaType<glm::ivec3> ( "ivec3" ,
			"x" , &glm::ivec3::x ,
			"y" , &glm::ivec3::y ,
			"z" , &glm::ivec3::z );

		// ChangeScene(sceneName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "ChangeScene" ,
			[&entt , &mainVariables]( std::string const& s , bool b )->void
			{
				auto& [scene_to_change , play_on_change] = mainVariables;
				scene_to_change = s;
				play_on_change = b;
			}
		);

		// Camera
		entt.RegisterLuaType<Camera> ( "Camera" ,
			"position"	, &Camera::m_position	, 
			"lookat"	, &Camera::m_look_at	);


		// GetComponent(e,componentName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( entt.m_identifier_GetScriptComponent ,
			[&entt]( entt::entity e , std::string const& s )->sol::table&
			{
#if _DEBUG
				if ( !entt.GetStorage<sol::table> ( s ).contains ( e ) )
				{
					std::cout << "LUASCRIPTERROR - Trying to get script component [" << s << "] from entity [" << static_cast< int >( e ) << "] that does not exist." << std::endl;
				}
#endif
				return entt.GetStorage<sol::table> ( s ).get ( e );
			}
		);

		// AddScriptComponent(e,componentName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "AttachScriptComponent" ,
			[&entt]( entt::entity e , std::string const& component )->void
			{
				if ( !entt.AttachComponent ( e , component ) )
				{
					std::cerr << "RemoveScriptComponent:: ERROR - failed to attach " << component << "from " << static_cast< int >( e ) << "!" << std::endl;
				}
			}
		);

		// RemoveScriptComponent(e,componentName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "RemoveScriptComponent" ,
			[&entt]( entt::entity e , std::string const& component )->void
			{
				if ( !entt.RemoveComponent ( e , component ) )
				{
					std::cerr << "RemoveScriptComponent:: ERROR - failed to remove " << component << "from " << static_cast< int >( e ) << "!" << std::endl;
				}
			}
		);

		// AddScriptSystem(e,componentName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "AttachScriptSystem" ,
			[&entt]( entt::entity e , std::string const& system )->void
			{
				entt.AttachScriptSystem<EngineComponents> ( entt.GetEngineComponent<EntityData> ( e )->m_id , system );
			}
		);

		// GetEntity(entityName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GetEntity" ,
			[&entt]( std::string const& entityName )->int
			{
				// potential area for optimization looking for entity of name
				for ( uint32_t i = 0; i < entt.m_entities.Size (); ++i )
				{
					if ( entt.m_entities.Valid ( i ) && entt.m_entities[ i ].m_name == entityName )
					{
						return static_cast< int >( entt.m_entities[ i ].m_id );
					}
				}
				return -1;
			}
		);

		// GetDeltaTime()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GetDeltaTime" ,
			[]()->float
			{
				return DeltaTimer::m_dt;
			}
		);

		// GetPathToCell(e,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GetPath" ,
			[&entt , &engineResources]( entt::entity e , int x , int y , int z )-> std::vector<glm::ivec3>
			{
				GridCell* grid_cell = entt.GetEngineComponent<GridCell> ( e );
				EntityData* entity_data = entt.GetEngineComponent<EntityData> ( e );
				if ( grid_cell && entity_data )
				{
					auto& grid = engineResources.Get<EntityGrid> ().get ();
					auto path = grid[ entity_data->m_parent_id ].GetPathAStar (
						grid_cell->m_cell_size ,
						{ grid_cell->m_cell_x, grid_cell->m_cell_y, grid_cell->m_cell_z } ,
						{ x, y, z } ,
						0 , 0
					);

					std::vector<glm::ivec3> out;
					for ( auto const& coordinate : path )
					{
						out.push_back ( { std::get<0> ( coordinate ),std::get<1> ( coordinate ) ,std::get<2> ( coordinate ) } );
					}
					return out;
				}
				return std::vector<glm::ivec3> ();
			}
		);

		entt.RegisterLuaFunction ( "GetPath3D" ,
			[&entt , &engineResources]( entt::entity e , int x , int y , int z , int maxDown , int maxUp , int verticalHeuristic )-> std::vector<glm::ivec3>
			{
				GridCell* grid_cell = entt.GetEngineComponent<GridCell> ( e );
				EntityData* entity_data = entt.GetEngineComponent<EntityData> ( e );
				if ( grid_cell && entity_data )
				{
					auto& grid = engineResources.Get<EntityGrid> ().get ();
					auto path = grid[ entity_data->m_parent_id ].GetPathAStar (
						grid_cell->m_cell_size ,
						{ grid_cell->m_cell_x, grid_cell->m_cell_y, grid_cell->m_cell_z } ,
						{ x, y, z } ,
						maxDown , maxUp , verticalHeuristic
					);

					std::vector<glm::ivec3> out;
					for ( auto const& coordinate : path )
					{
						out.push_back ( { std::get<0> ( coordinate ),std::get<1> ( coordinate ) ,std::get<2> ( coordinate ) } );
					}
					return out;
				}
				return std::vector<glm::ivec3> ();
			}
		);

		// HasComponent(e,name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "HasComponent" ,
			[&entt]( entt::entity e , std::string const& name )->bool
			{
				return entt.HasComponent ( e , name );
			}
		);

		// EntitiesWithEngineComponents(name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "EntitiesWithEngineComponent" ,
			[&entt]( std::string const& name )->std::vector<entt::entity>
			{
				std::vector<entt::entity> entities;
				entt.GetEntitiesWithEngineComponent ( name , entities );
				return entities;
			}
		);

		// EntitiesWithScriptComponents(name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "EntitiesWithScriptComponent" ,
			[&entt]( std::string const& name )->std::vector<entt::entity>
			{
				std::vector<entt::entity> entities;
				entt.GetEntitiesWithScriptComponent ( name , entities );
				return entities;
			}
		);

		// CheckKeyDown(key)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "CheckKeyDown" ,
			[&engineResources]( int key )->bool
			{
				auto& window = engineResources.Get<GLFWWindow> ().get ();
				return window.KeyDown ( key );
			}
		);

		// CheckKeyPress(key)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "CheckKeyPress" ,
			[&engineResources]( int key )->bool
			{
				auto& window = engineResources.Get<GLFWWindow> ().get ();
				return window.KeyPressed ( key );
			}
		);

		// CheckLeftMousePress()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "CheckLeftMousePress" ,
			[&engineResources]()->bool
			{
				auto& window = engineResources.Get<GLFWWindow> ().get ();
				return window.MouseLPressed ();
			}
		);

		// CheckRightMousePress()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "CheckRightMousePress" ,
			[&engineResources]()->bool
			{
				auto& window = engineResources.Get<GLFWWindow> ().get ();
				return window.MouseRPressed ();
			}
		);

		// GenerateRandomProbability()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GenerateRandomProbability" ,
			[]()->float
			{
				return glm::linearRand ( 0.0f , 1.0f );
			}
		);

		// GenerateRandomNumberInRange(minValue, maxValue)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GenerateRandomNumberInRange" ,
			[]( int minValue , int maxValue )->int
			{
				return glm::linearRand ( minValue , maxValue );
			}
		);

		// srand()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "srand" ,
			[]()->void
			{
				srand ( static_cast< unsigned int >( time ( 0 ) ) );
			}
		);

		// DrawLine(x0,y0,z0,x1,y1,z1,r,g,b,a,lineWidth)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "DrawLine" ,
			[&engineResources]( float x0 , float y0 , float z0 , float x1 , float y1 , float z1 , float r , float g , float b , float a , float lineWidth )->void
			{
				engineResources.Get<OpenGL> ().get ().DrawLine ( { x0,y0,z0 } , { x1,y1,z1 } , { r,g,b,a } , lineWidth );
			}
		);

		// InstancePrefabNow(name,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabNow" ,
			[&entt , &engineResources]( std::string const& name , float x , float y , float z )->entt::entity
			{
				auto e = entt.InstancePrefab ( engineResources , name );
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					transform->m_position = { x, y, z };
				}
				return entt.m_entities[ e ].m_id;
			}
		);

		// InstancePrefabParentedNow(parent,name,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabParentedNow" ,
			[&entt , &engineResources]( entt::entity parent , std::string const& name , float x , float y , float z )->entt::entity
			{
				auto e = entt.InstancePrefab ( engineResources , name , entt.GetEngineComponent<EntityData> ( parent )->m_id );
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					transform->m_position = { x, y, z };
				}
				return entt.m_entities[ e ].m_id;
			}
		);

		// InstancePrefabOnGridNow(name,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabOnGridNow" ,
			[&entt , &engineResources]( std::string const& name , float x , float y , float z )->entt::entity
			{
				auto e = entt.InstancePrefab ( engineResources , name );
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					transform->m_position = { x, y, z };
				}
				entt.AttachComponent<GridCell> ( e );
				GridCell* grid_cell = entt.GetEngineComponent<GridCell> ( e );
				grid_cell->m_cell_x = static_cast< uint32_t >( x );
				grid_cell->m_cell_y = static_cast< uint32_t >( y );
				grid_cell->m_cell_z = static_cast< uint32_t >( z );
				return entt.m_entities[ e ].m_id;
			}
		);

		// InstancePrefabOnGridNow(name,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabParentedOnGridNow" ,
			[&entt , &engineResources]( entt::entity parent , std::string const& name , float x , float y , float z )->entt::entity
			{
				auto e = entt.InstancePrefab ( engineResources , name , entt.GetEngineComponent<EntityData> ( parent )->m_id );
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					transform->m_position = { x, y, z };
				}
				entt.AttachComponent<GridCell> ( e );
				GridCell* grid_cell = entt.GetEngineComponent<GridCell> ( e );
				grid_cell->m_cell_x = static_cast< uint32_t >( x );
				grid_cell->m_cell_y = static_cast< uint32_t >( y );
				grid_cell->m_cell_z = static_cast< uint32_t >( z );
				return entt.m_entities[ e ].m_id;
			}
		);

		// InstancePrefab(name,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefab" ,
			[&entt]( std::string const& name , float x , float y , float z )
			{
				entt.QueueInstancePrefab ( name , x , y , z );
			}
		);

		// InstancePrefabParented(parent,name,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabParented" ,
			[&entt , &engineResources]( entt::entity e , std::string const& name , float x , float y , float z )
			{
				entt.QueueInstancePrefab ( name , x , y , z , entt.GetEngineComponent<EntityData> ( e )->m_id );
			}
		);

		// InstancePrefabOnGrid(name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabOnGrid" ,
			[&entt]( std::string const& name , int x , int y , int z )
			{
				entt.QueueInstancePrefab ( name , static_cast< float >( x ) , static_cast< float >( y ) , static_cast< float >( z ) , EnttXSol::Entities::Null , true );
			}
		);

		// InstancePrefabParentedOnGrid(name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabParentedOnGrid" ,
			[&entt]( entt::entity e , std::string const& name , int x , int y , int z )
			{
				entt.QueueInstancePrefab ( name , static_cast< float >( x ) , static_cast< float >( y ) , static_cast< float >( z ) , entt.GetEngineComponent<EntityData> ( e )->m_id , true );
			}
		);

		// RemoveInstance(e)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "RemoveInstance" ,
			[&entt]( entt::entity e )
			{
				entt.QueueDelete ( entt.GetEngineComponent<EntityData> ( e )->m_id );
				//entt.RemoveEntity ( engineResources.Get<EntityGrid> ().get () , entt.GetEngineComponent<EntityData> ( e )->m_id );
			}
		);

		// FindCameraObject()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "FindCameraObject" ,
			[&engineResources]()->god::Camera&
			{
				return engineResources.Get<Camera> ().get ();
			}
		);

		// SetCameraPosition(x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetCameraPosition" ,
			[&engineResources]( float x , float y , float z )->void
			{
				engineResources.Get<Camera> ().get ().SetPosition ( { x, y, z } );
			}
		);

		// SetCameraLookAt(x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetCameraLookAt" ,
			[&engineResources]( float x , float y , float z )->void
			{
				engineResources.Get<Camera> ().get ().SetLookAt ( { x, y, z } );
			}
		);

		// SetCameraNextPosition(x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetCameraNextPosition" ,
			[&engineResources]( float x , float y , float z )->void
			{
				engineResources.Get<Camera> ().get ().SetNextPosition ( { x, y, z } );
			}
		);

		// SetCameraNextLookAt(x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetCameraNextLookAt" ,
			[&engineResources]( float x , float y , float z )->void
			{
				engineResources.Get<Camera> ().get ().SetNextLookAt ( { x, y, z } );
			}
		);

		// SetCameraMoveSpeed(speed)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetCameraMoveSpeed" ,
			[&engineResources]( float speed )->void
			{
				engineResources.Get<Camera> ().get ().m_camera_move_speed = speed;
			}
		);

		// SetCameraPanSpeed(speed)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetCameraPanSpeed" ,
			[&engineResources]( float speed )->void
			{
				engineResources.Get<Camera> ().get ().m_camera_pan_speed = speed;
			}
		);

		// Sin(value)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "Sin" ,
			[]( float value )->float
			{
				return glm::sin ( value );
			}
		);

		// Abs(value)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "Abs" ,
			[]( float value )->float
			{
				return glm::abs ( value );
			}
		);

		// LerpVec3(x0,y0,z0,x1,y1,z1)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "LerpVec3" ,
			[]( float x0 , float y0 , float z0 , float x1 , float y1 , float z1 , float lerpVal )->glm::vec3
			{
				glm::vec3 out;
				out.x = std::lerp ( x0 , x1 , lerpVal );
				out.y = std::lerp ( y0 , y1 , lerpVal );
				out.z = std::lerp ( z0 , z1 , lerpVal );
				return out;
			}
		);

		// EntityName(e)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "EntityName" ,
			[&entt]( entt::entity e )->std::string&
			{
				return entt.m_entities[ entt.GetEngineComponent<EntityData> ( e )->m_id ].m_name;
			}
		);

		// SetTransformPosition(e,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetTransformPosition" ,
			[&entt , &engineResources]( entt::entity e , float x , float y , float z )
			{
				while ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () )
					;

				if ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () == false )
				{
					if ( entt.HasComponent ( e , "RigidDynamic" ) && entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic )
					{
						entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic->setGlobalPose ( ConvertToPhysXTransform ( { x, y, z } , entt.GetEngineComponent<Transform> ( e )->m_rotation ) );
					}
				}
			}
		);

		// SetTransformRotation(e,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetTransformRotation" ,
			[&entt , &engineResources]( entt::entity e , float x , float y , float z )
			{
				while ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () )
					;

				if ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () == false )
				{
					if ( entt.HasComponent ( e , "RigidDynamic" ) && entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic )
					{
						entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic->setGlobalPose ( ConvertToPhysXTransform ( entt.GetEngineComponent<Transform> ( e )->m_position , { x, y, z } ) );
					}
				}
			}
		);

		// SetVelocity(e,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "SetVelocity" ,
			[&entt , &engineResources]( entt::entity e , float x , float y , float z )
			{
				while ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () )
					;

				if ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () == false )
				{
					if ( entt.HasComponent ( e , "RigidDynamic" ) && entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic )
					{
						entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic->clearForce ();
						entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic->setLinearVelocity ( ConvertToPhysXVector ( { x, y, z } ) );
					}
				}
			}
		);

		// AddForce(e,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "AddForce" ,
			[&entt , &engineResources]( entt::entity e , float x , float y , float z )
			{
				while ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () )
					;

				if ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () == false )
				{
					if ( entt.HasComponent ( e , "RigidDynamic" ) && entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic )
					{
						entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic->setActorFlag ( PxActorFlag::eDISABLE_SIMULATION , false );
						entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic->addForce ( ConvertToPhysXVector ( { x, y, z } ) );
					}
				}
			}
		);

		// FreezeObject(e)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "FreezeObject" ,
			[&entt , &engineResources]( entt::entity e , bool freeze )
			{
				while ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () )
					;

				if ( engineResources.Get<PhysicsSystem> ().get ().GetisRunning () == false )
				{
					if ( entt.HasComponent ( e , "RigidDynamic" ) && entt.GetEngineComponent<RigidDynamic> ( e )->p_RigidDynamic )
					{

						entt.GetEngineComponent<RigidDynamic>(e)->Active = !freeze;
					}
				}
			}
		);

		// Child(e, index)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "Child" ,
			[&entt]( entt::entity e , unsigned index )->entt::entity
			{
				if ( index >= entt.m_entities[ entt.GetEngineComponent<EntityData> ( e )->m_id ].m_children.size () )
					std::cout << "Child at index " << index << " does not exist!" << std::endl;
				else
					return entt.m_entities[ entt.m_entities[ entt.GetEngineComponent<EntityData> ( e )->m_id ].m_children[ index ] ].m_id;
				return entt::null;
			}
		);

		// ChangeTexture(e, texture name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "ChangeTexture" ,
			[&entt , &engineResources]( entt::entity e , std::string texture_name )
			{
				Renderable3D* r = entt.GetEngineComponent<Renderable3D> ( e );
				if ( r )
				{
					r->m_diffuse_id = engineResources.Get<OGLTextureManager> ().get ().GetID ( texture_name );
				}
			}
		);

		// ChangeModel(e, model name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "ChangeModel" ,
			[&entt , &engineResources]( entt::entity e , std::string model_name )
			{
				Renderable3D* r = entt.GetEngineComponent<Renderable3D> ( e );
				if ( r )
				{
					r->m_model_id = engineResources.Get<Asset3DManager> ().get ().GetID ( model_name );
				}
			}
		);

		// TextureName(e)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "TextureName" ,
			[&entt , &engineResources]( entt::entity e )->std::string
			{
				Renderable3D* r = entt.GetEngineComponent<Renderable3D> ( e );
				if ( r )
				{
					return engineResources.Get<OGLTextureManager> ().get ().GetName ( r->m_diffuse_id );
				}
				return std::string ();
			}
		);

		// WorldPosition(e)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "WorldPosition" ,
			[&entt]( entt::entity e )->glm::vec3
			{
				return glm::vec3 { entt.GetEngineComponent<Transform> ( e )->m_parent_transform * glm::vec4{ entt.GetEngineComponent<Transform> ( e )->m_position, 1.0f } };
			}
		);
	}
}