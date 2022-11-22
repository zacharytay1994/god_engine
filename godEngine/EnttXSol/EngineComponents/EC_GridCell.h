#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct GridCell
	{
		int32_t m_cell_x { 0 } , m_cell_y { 0 } , m_cell_z { 0 };
		float m_cell_size { 1.0f };

		// not saved, used to change cell in Grid3D
		int32_t m_cell_ox { 0 } , m_cell_oy { 0 } , m_cell_oz { 0 };
		bool m_initialized { false };

		// extra parameters for cells
		bool m_passable { false };
		bool m_steppable { true };

		// flag this to true if wish for the cell to update
		bool m_changed { false };

		bool operator==( GridCell const& rhs )
		{
			return m_cell_x == rhs.m_cell_x &&
				m_cell_y == rhs.m_cell_y &&
				m_cell_z == rhs.m_cell_z &&
				m_cell_size == rhs.m_cell_size;
		}
	};
	template <>
	inline void NewLuaType<GridCell> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<GridCell> ( luaState , name ,
			"x" , &GridCell::m_cell_x ,
			"y" , &GridCell::m_cell_y ,
			"z" , &GridCell::m_cell_z ,
			"cell_size" , &GridCell::m_cell_size ,
			"passable", &GridCell::m_passable,
			"steppable", &GridCell::m_steppable);
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
				ImGui::DragInt ( "X" , &component.m_cell_x );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::DragInt ( "Y" , &component.m_cell_y );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::DragInt ( "Z" , &component.m_cell_z );

				ImGui::Checkbox ( "Passable" , &component.m_passable );
				ImGui::Checkbox ( "Steppable" , &component.m_steppable );
				ImGui::Checkbox ( "Changed" , &component.m_changed );

				ImGui::Text ( "Cell Size: %.3f" , component.m_cell_size );
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
		RapidJSON::JSONifyToValue ( value , document , "passable" , component.m_passable );
		RapidJSON::JSONifyToValue ( value , document , "steppable" , component.m_steppable );
	}

	template<>
	inline void DeJSONify<GridCell> ( EngineResources& engineResources , GridCell& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_cell_x , "cell_x" );
		AssignIfExist ( jsonObj , component.m_cell_y , "cell_y" );
		AssignIfExist ( jsonObj , component.m_cell_z , "cell_z" );
		AssignIfExist ( jsonObj , component.m_cell_size , "cell_size" );
		AssignIfExist ( jsonObj , component.m_passable , "passable" );
		AssignIfExist ( jsonObj , component.m_steppable , "steppable" );
	}
}