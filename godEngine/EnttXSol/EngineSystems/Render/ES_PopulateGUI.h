#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include <godUtility/Math.h>

namespace god
{
	void PopulateGUI ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , Renderable3D& , GUIObject& , ActiveComponent&> components )
	{
		auto [entity_data , transform , renderable , gui_object , active] = components;

		// return if not valid
		if ( !entt.m_entities.Valid ( entity_data.m_id ) )
		{
			return;
		}

		Scene& scene = engineResources.Get<Scene> ().get ();

		bool changed { false };
		glm::mat4 parentTransform { 1.0f };
		if ( entt.m_entities[ entity_data.m_id ].m_parent_id != EnttXSol::Entities::Null )
		{
			//changed = entt.m_entities[ entity_data.m_id ].m_parent_changed;
			parentTransform = entt.m_entities[ entity_data.m_id ].m_parent_transform;
			changed = transform.m_parent_transform != parentTransform;
		}

		bool world_changed { false };
		if ( changed )
		{
			transform.m_parent_transform = parentTransform;
			world_changed = true;
		}
		if ( transform.m_changed )
		{
			glm::mat4 model_transform = BuildModelMatrixRotDegrees ( transform.m_position , transform.m_rotation , transform.m_scale );
			transform.m_local_transform = model_transform;
			world_changed = true;
		}
		if ( world_changed )
		{
			transform.m_world_transform = transform.m_parent_transform * transform.m_local_transform;
		}

		// add to scene
		if ( renderable.m_model_id != -1 && gui_object.m_active && renderable.m_visible )
		{
			Scene::InstancedRenderData render_data { static_cast< uint32_t >( renderable.m_model_id ) ,
				renderable.m_diffuse_id , renderable.m_specular_id , gui_object.m_alpha };
			render_data.m_spritesheet_data = renderable.m_spritesheet_data;
			scene.Add2DInstancedObject ( render_data , transform.m_world_transform );

			if ( GUIText* p_gui_text = entt.GetEngineComponent<GUIText>( entity_data.m_id ) /*m_registry.storage<GUIText> ().contains ( m_entities[ e ].m_id )*/ )
			{
				Fonts& fonts = engineResources.Get<Fonts> ().get ();
				auto const& characters = fonts.GetFont ( "Arial" ).GetCharacters ( Fonts::DEFAULT_FONT_SIZE );
				GUIText& gui_text = *p_gui_text/* m_registry.get<GUIText> ( m_entities[ e ].m_id )*/;
				std::stringstream ss;
				ss << gui_text.m_text;
				float scale { 1.0f / 100.0f * gui_text.m_size };

				float x { -1.0f } , y { 1.0f - gui_text.m_padding_top };
				std::string word , sentence;
				float sentence_length { 0.0f };
				float space_length = static_cast< float >( characters[ static_cast< uint32_t >( ' ' ) ].m_advance );
				float sentence_max_length { 2.0f - gui_text.m_padding_left - gui_text.m_padding_right };
				float sentence_left = -1.0f + gui_text.m_padding_left;
				float sentence_right = -1.0f - gui_text.m_padding_right;

				float scale_y { scale * ( transform.m_world_transform[ 0 ].x / transform.m_world_transform[ 1 ].y ) };

				while ( ss >> word )
				{
					word.push_back ( ' ' );
					float word_length { 0.0f };
					for ( auto const& c : word )
					{
						word_length += characters[ static_cast< uint32_t >( c ) ].m_advance;
					}
					if ( ( sentence_length + word_length ) * scale < sentence_max_length )
					{
						// add word to sentence if not exceed right
						sentence_length += word_length;
						sentence.append ( word );
					}
					else
					{
						// set starting x
						switch ( gui_text.m_alignment )
						{
						case ( static_cast< int >( TextAlignment::LEFT ) ):
						{
							x = sentence_left;
							break;
						}
						case ( static_cast< int >( TextAlignment::CENTER ) ):
						{
							x = -1.0f + ( 2.0f - ( sentence_length - space_length ) * scale ) / 2.0f;
							break;
						}
						case ( static_cast< int >( TextAlignment::RIGHT ) ):
						{
							x = sentence_right + ( 2.0f - ( sentence_length - space_length ) * scale );
							break;
						}
						}

						// draw sentence
						for ( auto const& c : sentence )
						{
							auto& ch = characters[ static_cast< uint32_t >( c ) ];

							float xpos = x + ( ch.m_bearing.x + ch.m_size.x / 2.0f ) * scale;
							float ypos = y + ( ch.m_bearing.y - ch.m_size.y / 2.0f ) * scale_y;

							float w = ch.m_size.x * scale / 2.0f;
							float h = ch.m_size.y * scale_y / 2.0f;

							glm::mat4 character_transform = BuildModelMatrixRotDegrees ( { xpos,ypos,1 } , { 0,0,0 } , { w,-h,1 } );

							scene.AddCharacter ( { static_cast< uint32_t >( renderable.m_model_id ) ,
								ch.m_texture_ID , renderable.m_specular_id , renderable.m_shininess } , transform.m_world_transform * character_transform );

							x += ch.m_advance * scale;
						}

						// new line and reset sentence
						y -= 100.0f * scale_y;
						sentence = word;
						sentence_length = word_length;
					}
				}
				// set starting x
				switch ( gui_text.m_alignment )
				{
				case ( static_cast< int >( TextAlignment::LEFT ) ):
				{
					x = sentence_left;
					break;
				}
				case ( static_cast< int >( TextAlignment::CENTER ) ):
				{
					x = -1.0f + ( 2.0f - ( sentence_length - space_length ) * scale ) / 2.0f;
					break;
				}
				case ( static_cast< int >( TextAlignment::RIGHT ) ):
				{
					x = sentence_right + ( 2.0f - ( sentence_length - space_length ) * scale );
					break;
				}
				}

				// draw sentence
				for ( auto const& c : sentence )
				{
					auto& ch = characters[ static_cast< uint32_t >( c ) ];

					float xpos = x + ( ch.m_bearing.x + ch.m_size.x / 2.0f ) * scale;
					float ypos = y + ( ch.m_bearing.y - ch.m_size.y / 2.0f ) * scale_y;

					float w = ch.m_size.x * scale / 2.0f;
					float h = ch.m_size.y * scale_y / 2.0f;

					glm::mat4 character_transform = BuildModelMatrixRotDegrees ( { xpos,ypos,1 } , { 0,0,0 } , { w,-h,1 } );

					scene.AddCharacter ( { static_cast< uint32_t >( renderable.m_model_id ) ,
						ch.m_texture_ID , renderable.m_specular_id , renderable.m_shininess } , transform.m_world_transform * character_transform );

					x += ch.m_advance * scale;
				}
			}
		}

		if ( gui_object.m_active )
		{
			// populate scene with children
			for ( auto const& child : entt.m_entities[ entity_data.m_id ].m_children )
			{
				//entt.m_entities[ child ].m_parent_changed = world_changed;
				entt.m_entities[ child ].m_parent_transform = transform.m_world_transform;
				//RecursivePopulateScene<S , T , R> ( scene , fonts , child , world_changed , transform.m_world_transform , cameraPosition );
			}
		}
	}
}
