#pragma once

#include "EngineComponents.h"
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>

namespace god
{
	/* ENGINE COMPONENTS */
	struct Renderable3D
	{
		int m_model_id { -1 };
		uint32_t m_diffuse_id { 0 };
		uint32_t m_specular_id { 0 };
		float m_shininess { 32.0f };
		float m_emissive { 0.0f };
		bool m_visible { true };

		bool operator==( Renderable3D const& rhs )
		{
			return
				m_model_id == rhs.m_model_id &&
				m_diffuse_id == rhs.m_diffuse_id &&
				m_specular_id == rhs.m_specular_id &&
				m_shininess == rhs.m_shininess &&
				m_emissive == rhs.m_emissive;
		}
	};
	template <>
	inline void NewLuaType<Renderable3D> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<Renderable3D> ( luaState , name ,
			"model_id" , &Renderable3D::m_model_id ,
			"visible" , &Renderable3D::m_visible );
	}
	template<>
	inline void ComponentInspector::operator() < Renderable3D > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& engineResources )
	{
		RegisterInspector<Renderable3D , EngineResources> ( entity , registry , imguiUniqueID , engineResources ,
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

				ImGui::Text ( "- Emissive :" );
				ImGui::DragFloat ( "##Emissive" , &component.m_emissive );
			} );
	}

	template<>
	inline void JSONify<Renderable3D> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , Renderable3D& component )
	{
		// find model uid
		auto model_name = engineResources.Get<Asset3DManager> ().get ().GetName ( component.m_model_id );
		rapidjson::Document model_doc;
		model_doc.SetObject ();
		ReadJSON ( model_doc , "Assets/GameAssets/3DAssets/ModelsConfig.json" );
		if ( model_doc.FindMember ( model_name.c_str () ) != model_doc.MemberEnd () )
		{
			RapidJSON::JSONifyToValue ( value , document , "model uid" , model_doc[ model_name.c_str () ][ 0 ][ "UID" ].GetUint () );
		}
		else
		{
			std::cerr << "EC_Renderable3D::ComponentInspector - Oops something might be wrong, no uid corresponding to model. Ignore this warning for now." << std::endl;
		}

		// find diffuse uid
		auto const& texture_manager = engineResources.Get<OGLTextureManager> ().get ();
		auto diffuse_name = texture_manager.GetName ( component.m_diffuse_id );
		rapidjson::Document texture_doc;
		texture_doc.SetObject ();
		ReadJSON ( texture_doc , "Assets/GameAssets/2DAssets/TexturesConfig.json" );
		if ( texture_doc.FindMember ( diffuse_name.c_str () ) != texture_doc.MemberEnd () )
		{
			RapidJSON::JSONifyToValue ( value , document , "diffuse uid" , texture_doc[ diffuse_name.c_str () ][ 0 ][ "UID" ].GetUint () );
		}
		else
		{
			std::cerr << "EC_Renderable3D::ComponentInspector - Oops something might be wrong, no uid corresponding to diffuse texture. Ignore this warning for now." << std::endl;
		}

		// find specular uid
		auto specular_name = texture_manager.GetName ( component.m_specular_id );
		if ( texture_doc.FindMember ( specular_name.c_str () ) != texture_doc.MemberEnd () )
		{
			RapidJSON::JSONifyToValue ( value , document , "specular uid" , texture_doc[ specular_name.c_str () ][ 0 ][ "UID" ].GetUint () );
		}
		else
		{
			std::cerr << "EC_Renderable3D::ComponentInspector - Oops something might be wrong, no uid corresponding to specular texture. Ignore this warning for now." << std::endl;
		}

		// serialize emissive
		RapidJSON::JSONifyToValue ( value , document , "emissive" , component.m_emissive );
		RapidJSON::JSONifyToValue ( value , document , "visible" , component.m_visible );
	}

	template<>
	inline void DeJSONify<Renderable3D> ( EngineResources& engineResources , Renderable3D& component , rapidjson::Value& jsonObj )
	{
		// find model corresponding to uid
		uint32_t model_uid { 0 };
		AssignIfExist ( jsonObj , model_uid , "model uid" );
		auto const& models = engineResources.Get<Asset3DManager> ().get ().GetResources ();
		int i { 0 };
		for ( auto const& model : models )
		{
			if ( std::get<0> ( model ) == model_uid )
			{
				component.m_model_id = i;
				break;
			}
			++i;
		}

		// find textures corresponding to diffuse and specular uid
		uint32_t diffuse_uid { 0 } , specular_uid { 0 };
		AssignIfExist ( jsonObj , diffuse_uid , "diffuse uid" );
		AssignIfExist ( jsonObj , specular_uid , "specular uid" );
		auto const& textures = engineResources.Get<OGLTextureManager> ().get ().GetResources ();
		i = 0;
		for ( auto const& texture : textures )
		{
			if ( std::get<0> ( texture ) == diffuse_uid )
			{
				component.m_diffuse_id = i;
			}
			if ( std::get<0> ( texture ) == specular_uid )
			{
				component.m_specular_id = i;
			}
			++i;
		}

		AssignIfExist ( jsonObj , component.m_emissive , "emissive" );
		AssignIfExist ( jsonObj , component.m_visible , "visible" );
	}
}