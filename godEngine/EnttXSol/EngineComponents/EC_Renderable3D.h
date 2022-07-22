#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct Renderable3D
	{
		int m_model_id { 0 };
	};
	template <>
	inline void NewLuaType<Renderable3D> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<Renderable3D> ( luaState , name ,
			"model_id" , &Renderable3D::m_model_id );
	}
	template<>
	inline void ComponentInspector::operator() < Renderable3D > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<Renderable3D , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( Renderable3D& component , EngineResources& resources )
			{
				auto& asset_manager = resources.Get<Asset3DManager> ();

				ImGui::Separator ();
				ImGui::Text ( "Renderable3D" );
				ImGui::Separator ();

				if ( ImGui::BeginPopup ( "Model Select" ) )
				{
					for ( auto const& asset : asset_manager.get ().GetIDs () )
					{
						if ( ImGui::Selectable ( asset.first.c_str () ) )
						{
							component.m_model_id = asset.second;
							ImGui::CloseCurrentPopup ();
						}
					}
					ImGui::EndPopup ();
				}
				ImGui::Text ( "Model : " );
				if ( ImGui::Button ( asset_manager.get ().GetName ( component.m_model_id ).c_str () , { ImGui::GetWindowWidth (),0 } ) )
				{
					ImGui::OpenPopup ( "Model Select" );
				}
			} );
	}
}