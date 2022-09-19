#include "../pch.h"
#include "LuaFunctionDefinitions.h"

#include "EnttXSol.h"
#include "../Window/DeltaTimer.h"

#include <sol/sol.hpp>

namespace god
{
	void RegisterLuaFunctions ( EnttXSol& entt )
	{
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
					if ( entt.m_entities[ i ].m_name == entityName )
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
	}
}