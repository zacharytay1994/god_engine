#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct GridManipulate
	{
		std::string m_grid_root { "" };
		std::string m_highlight_prefab { "None" };
		uint32_t m_highlight_id { static_cast< uint32_t >( -1 ) };
		bool m_clicked { false };
		glm::ivec3 m_last_clicked_cell;
	};

	template <>
	inline void NewLuaType<GridManipulate> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<GridManipulate> ( luaState , name ,
			"clicked" , &GridManipulate::m_clicked ,
			"last_clicked_cell" , &GridManipulate::m_last_clicked_cell );
	}

	template<>
	inline void ComponentInspector::operator() < GridManipulate > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<GridManipulate , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( GridManipulate& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Grid Manipulate" );
				ImGui::Separator ();

				ImGui::InputText ( "Grid Root" , &component.m_grid_root );
				ImGui::InputText ( "Highlight Prefab" , &component.m_highlight_prefab );
			} );
	}

	template<>
	inline void JSONify<GridManipulate> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , GridManipulate& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "grid_root" , component.m_grid_root );
		RapidJSON::JSONifyToValue ( value , document , "highlight_prefab" , component.m_highlight_prefab );
	}

	template<>
	inline void DeJSONify<GridManipulate> ( EngineResources& engineResources , GridManipulate& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_grid_root , "grid_root" );
		AssignIfExist ( jsonObj , component.m_highlight_prefab , "highlight_prefab" );
	}
}