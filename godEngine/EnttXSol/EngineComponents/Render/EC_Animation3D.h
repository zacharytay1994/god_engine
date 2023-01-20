#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct SkeleAnim3D
	{
		std::string m_animation { "None" };
		uint32_t m_animation_id { static_cast< uint32_t >( -1 ) };
		uint32_t m_animator_id { static_cast< uint32_t >( -1 ) };

		uint32_t m_diffuse_id { 0 };
		uint32_t m_specular_id { 0 };
		float m_shininess { 32.0f };
		float m_emissive { 0.0f };
		bool m_visible { true };

		bool m_initialized { false };

		bool operator==( SkeleAnim3D const& rhs )
		{
			return
				m_animation == rhs.m_animation &&
				m_diffuse_id == rhs.m_diffuse_id &&
				m_specular_id == rhs.m_specular_id &&
				m_shininess == rhs.m_shininess &&
				m_emissive == rhs.m_emissive;
		}
	};
	template <>
	inline void NewLuaType<SkeleAnim3D> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<SkeleAnim3D> ( luaState , name ,
			"animation" , &SkeleAnim3D::m_animation ,
			"visible" , &SkeleAnim3D::m_visible );
	}
	template<>
	inline void ComponentInspector::operator() < SkeleAnim3D > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<SkeleAnim3D , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( SkeleAnim3D& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "SkeleAnim3D" );
				ImGui::Separator ();

				ImGui::InputText ( "Animation" , &component.m_animation );

				auto& textures = resources.Get<OGLTextureManager> ();

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
	inline void JSONify<SkeleAnim3D> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , SkeleAnim3D& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "animation" , component.m_animation );

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
	inline void DeJSONify<SkeleAnim3D> ( EngineResources& engineResources , SkeleAnim3D& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_animation , "animation" );

		// find textures corresponding to diffuse and specular uid
		uint32_t diffuse_uid { 0 } , specular_uid { 0 };
		AssignIfExist ( jsonObj , diffuse_uid , "diffuse uid" );
		AssignIfExist ( jsonObj , specular_uid , "specular uid" );
		auto const& textures = engineResources.Get<OGLTextureManager> ().get ().GetResources ();
		int i { 0 };
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