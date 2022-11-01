#include "../pch.h"
#include "LuaFunctionDefinitions.h"

#include "EnttXSol.h"
#include "EngineComponents/EC_All.h"
#include "../Window/DeltaTimer.h"

#include <sol/sol.hpp>
#include <glm/glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <functional>

namespace god
{
	void RegisterLuaCPP ( EnttXSol& entt , EngineResources& engineResources )
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

		// Camera
		entt.RegisterLuaType<Camera> ( "Camera" ,
			"position" , &Camera::m_position );

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
						{ x, y, z }
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
				srand ( time ( 0 ) );
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
				entt.QueueInstancePrefab ( name , x , y , z , EnttXSol::Entities::Null , true );
			}
		);

		// InstancePrefabParentedOnGrid(name)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "InstancePrefabParentedOnGrid" ,
			[&entt]( entt::entity e , std::string const& name , int x , int y , int z )
			{
				entt.QueueInstancePrefab ( name , x , y , z , entt.GetEngineComponent<EntityData> ( e )->m_id , true );
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
		entt.RegisterLuaFunction("SetTransformPosition",
			[&entt, &engineResources](entt::entity e, float x, float y, float z)->void
			{
				if (engineResources.Get<PhysicsSystem>().get().GetisRunning() == false)
				{
					if (entt.HasComponent(e, "RigidDynamic") && entt.GetEngineComponent<RigidDynamic>(e)->p_RigidDynamic)
					{
						entt.GetEngineComponent<RigidDynamic>(e)->p_RigidDynamic->setGlobalPose(ConvertToPhysXTransform({ x, y, z }, entt.GetEngineComponent<Transform>(e)->m_rotation));
					}
				}
			}
		);

		// FreezeObject(e, bool)
		// ==============================================================================================
		entt.RegisterLuaFunction("FreezeObject",
			[&entt, &engineResources](entt::entity e, bool freeze)->void
			{
				if (engineResources.Get<PhysicsSystem>().get().GetisRunning() == false)
				{
					if (entt.HasComponent(e, "RigidDynamic") && entt.GetEngineComponent<RigidDynamic>(e)->p_RigidDynamic)
					{
						entt.GetEngineComponent<RigidDynamic>(e)->p_RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, freeze);
					}
				}
			}
		);
	}
}