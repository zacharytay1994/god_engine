#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct GridCell
	{
		int32_t m_cell_x { 0 } , m_cell_y { 0 } , m_cell_z { 0 };
		float m_cell_size { 1.0f };
	};
	template <>
	inline void NewLuaType<GridCell> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<GridCell> ( luaState , name ,
			"cell_x" , &GridCell::m_cell_x ,
			"cell_y" , &GridCell::m_cell_y ,
			"cell_z" , &GridCell::m_cell_z ,
			"cell_size" , &GridCell::m_cell_size );
	}
	template<>
	inline void ComponentInspector::operator() < GridCell > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<GridCell , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( GridCell& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Grid Cell" );
				ImGui::Separator ();

				auto width = ImGui::GetWindowWidth ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::DragInt ( "Cell X" , &component.m_cell_x );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::DragInt ( "Cell Y" , &component.m_cell_y );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::DragInt ( "Cell Z" , &component.m_cell_z );
			} );
	}

	template<>
	inline void JSONify<GridCell> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , GridCell& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "cell_x" , component.m_cell_x );
		RapidJSON::JSONifyToValue ( value , document , "cell_y" , component.m_cell_y );
		RapidJSON::JSONifyToValue ( value , document , "cell_z" , component.m_cell_z );
		RapidJSON::JSONifyToValue ( value , document , "cell_size" , component.m_cell_size );
	}

	template<>
	inline void DeJSONify<GridCell> ( EngineResources& engineResources , GridCell& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_cell_x , "cell_x" );
		AssignIfExist ( jsonObj , component.m_cell_x , "cell_y" );
		AssignIfExist ( jsonObj , component.m_cell_z , "cell_z" );
		AssignIfExist ( jsonObj , component.m_cell_size , "cell_size" );
	}
}