#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct Renderable3D
	{
		int m_model_id { -1 };
		uint32_t m_diffuse_id { 0 };
		uint32_t m_specular_id { 0 };
		float m_shininess { 32.0f };
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
				auto& models = resources.Get<Asset3DManager> ();
				auto& textures = resources.Get<OGLTextureManager> ();

				ImGui::Separator ();
				ImGui::Text ( "Renderable3D" );
				ImGui::Separator ();

				if ( ImGui::BeginPopup ( "Model Select" ) )
				{
					for ( auto const& asset : models.get ().GetIDs () )
					{
						if ( ImGui::Selectable ( asset.first.c_str () ) )
						{
							component.m_model_id = asset.second;
							ImGui::CloseCurrentPopup ();
						}
					}
					ImGui::EndPopup ();
				}

				if ( ImGui::BeginPopup ( "Diffuse Select" ) )
				{
					for ( auto const& texture : textures.get ().GetIDs () )
					{
						if ( ImGui::Selectable ( texture.first.c_str () ) )
						{
							component.m_diffuse_id = texture.second;
							ImGui::CloseCurrentPopup ();
						}
					}
					ImGui::EndPopup ();
				}

				if ( ImGui::BeginPopup ( "Specular Select" ) )
				{
					for ( auto const& texture : textures.get ().GetIDs () )
					{
						if ( ImGui::Selectable ( texture.first.c_str () ) )
						{
							component.m_specular_id = texture.second;
							ImGui::CloseCurrentPopup ();
						}
					}
					ImGui::EndPopup ();
				}

				ImGui::Text ( "- Model : " );
				if ( ImGui::Button ( models.get ().GetName ( component.m_model_id ).c_str () , { ImGui::GetWindowWidth (),0 } ) )
				{
					ImGui::OpenPopup ( "Model Select" );
				}

				ImGui::Text ( "Material Properties :" );
				ImGui::Text ( "- Diffuse Map : " );
				ImGui::PushID ( 1 );
				if ( ImGui::Button ( textures.get ().GetName ( component.m_diffuse_id ).c_str () , { ImGui::GetWindowWidth (),0 } ) )
				{
					ImGui::PopID ();
					ImGui::OpenPopup ( "Diffuse Select" );
				}
				else
				{
					ImGui::PopID ();
				}

				ImGui::Text ( "- Specular Map : " );
				ImGui::PushID ( 2 );
				if ( ImGui::Button ( textures.get ().GetName ( component.m_specular_id ).c_str () , { ImGui::GetWindowWidth (),0 } ) )
				{
					ImGui::PopID ();
					ImGui::OpenPopup ( "Specular Select" );
				}
				else
				{
					ImGui::PopID ();
				}

				ImGui::Text ( "- Shininess :" );
				ImGui::InputFloat ( "##Shininess" , &component.m_shininess );
			} );
	}
}