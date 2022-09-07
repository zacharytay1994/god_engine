#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct EntityData
	{
		uint32_t m_id;
		uint32_t m_parent_id;
	};
	template <>
	inline void NewLuaType<EntityData> ( sol::state& luaState , std::string const& name )
	{
	}

	template<>
	inline void ComponentInspector::operator() < EntityData > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<EntityData , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( EntityData& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Entity Data" );
				ImGui::Separator ();

				ImGui::Text ( "ID: %d , Parent ID: %d" , component.m_id , component.m_parent_id );
			} );
	}

	template<>
	inline void JSONify<EntityData> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , EntityData& component )
	{
	}

	template<>
	inline void DeJSONify<EntityData> ( EngineResources& engineResources , EntityData& component , rapidjson::Value& jsonObj )
	{
	}
}