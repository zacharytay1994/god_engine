#pragma once
#include "../Editor.h"

#include <godUtility/Utility.h>
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>
#include <godUtility/DateTime.h>

#include "../../imgui/imgui_stdlib.h" 

#include <string>
#include <iostream>
#include <assert.h>
#include <cstdio>

// to bypass windows.h GetObject() macro definition
#ifdef _MSC_VER
#undef GetObject
#endif

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_AssetManager : EditorWindow<EDITOR_RESOURCES>
	{
		EW_AssetManager ();

		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
		void OnOpen () override;

		void ReloadConfig ();

		bool ModelNameExists ( std::string const& name );

	private:
		rapidjson::Document m_document_models;
		rapidjson::Document m_document_textures;
		std::string m_selected_model { "" };
		std::string m_selected_texture { "" };
		std::string m_to_delete { "" };

		// model editing
		std::string m_edit_name { "" };
		std::string m_edit_current_raw { "" };
		std::string m_edit_raw { "" };
		std::string m_edit_src { "" };
		bool m_edit_new_src { false };
		bool m_flip_model_UVs { true };

		rapidjson::Value& GetModelDocumentValue ( char const* key );
		rapidjson::Value& GetTextureDocumentValue ( char const* key );
	};
}

#include "PathConfig.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_AssetManager<EDITOR_RESOURCES>::EW_AssetManager ()
	{
		ReloadConfig ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_AssetManager<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		ImGui::Begin ( "Asset Manager" );

		if ( ImGui::BeginTabBar ( "AssetType" ) )
		{
			/*! ****************************************************************************************************************
			 * @brief
			 * : Manage models.
			*/
			if ( ImGui::BeginTabItem ( "Models" ) )
			{
				// display models config in a list view
				if ( ImGui::BeginListBox ( "##Models" , { ImGui::GetWindowWidth (), 100.0f } ) )
				{
					if ( m_document_models.IsObject () )
					{
						for ( auto& model : m_document_models.GetObject () )
						{
							if ( m_selected_model.empty () || ImGui::Selectable ( model.name.GetString () , m_selected_model == model.name.GetString () ) )
							{
								m_selected_model = model.name.GetString ();
							}
						}
					}
					ImGui::EndListBox ();
				}

				ImGui::Separator ();
				ImGui::Text ( "Properties: " );
				if ( m_selected_model.empty () )
				{
					ImGui::Text ( "No asset selected." );
				}
				else
				{
					ImGui::BeginChild ( "ModelProperties" , { ImGui::GetWindowWidth () - 20, 150 } , true , ImGuiWindowFlags_HorizontalScrollbar );
					ImGui::Text ( "Model Name : %s" , m_selected_model.c_str () );

					for ( auto& m : m_document_models[ m_selected_model.c_str () ].GetArray ()[ 0 ].GetObject () )
					{
						if ( m.value.IsString () )
						{
							ImGui::Text ( "%s : %s" , m.name.GetString () , m.value.GetString () );
						}
						else if ( m.value.IsNumber () )
						{
							ImGui::Text ( "%s : %u" , m.name.GetString () , static_cast< unsigned long long >( m.value.GetUint () ) );
						}
					}
					ImGui::EndChild ();
				}

				float popup_width { 400.0f };
				float popup_height { 400.0f };
				ImGui::SetNextWindowSize ( { popup_width, 400.0f } );
				if ( ImGui::BeginPopup ( "EditModel" ) )
				{
					ImGui::BeginChild ( "New Properties" , { ImGui::GetWindowWidth () - 15.0f, popup_height - 100.0f } , true , ImGuiWindowFlags_HorizontalScrollbar );

					ImGui::Text ( "New Properties" );
					ImGui::Text ( "Name:" );
					ImGui::SetNextItemWidth ( popup_width - 30.0f );

					bool editable { true };
					if ( m_edit_name != m_selected_model && ModelNameExists ( m_edit_name ) )
					{
						editable = false;
						ImGui::PushStyleColor ( ImGuiCol_Text , { 1.0f, 0.0f, 0.0f, 1.0f } );
						ImGui::Text ( "** Name already used!" );
						ImGui::PopStyleColor ();
					}
					ImGui::InputText ( "##ModelEditName" , &m_edit_name );

					ImGui::Text ( "Current Raw:" );
					ImGui::Text ( m_edit_current_raw.c_str () );

					if ( ImGui::Button ( "Edit Raw" , { popup_width - 30.0f, 0.0f } ) )
					{
						std::string new_src = god::OpenWindowDialog ( L"Model Files" , L"*.obj;*.fbx" );
						if ( !new_src.empty () )
						{
							m_edit_src = new_src;
							m_edit_raw = m_edit_src.substr ( m_edit_src.find_last_of ( '\\' ) + 1 , m_edit_src.size () );
							m_edit_new_src = true;
						}
					}

					ImGui::Text ( "New Raw:" );
					if ( m_edit_new_src )
					{
						ImGui::Text ( m_edit_src.c_str() );
					}
					else
					{
						ImGui::Text ( "- None -" );
					}

					ImGui::EndChild ();

					ImGui::Text ( "Flip UVs :" );
					ImGui::SameLine ();
					ImGui::Checkbox ( "##flipuvs" , &m_flip_model_UVs );

					if ( ImGui::Button ( "Apply" , { popup_width, 0.0f } ) )
					{
						// if new name already exists in config, i.e. representing another model
						if ( editable )
						{
							if ( m_document_models.HasMember ( m_selected_model.c_str () ) )
							{
								// set new name
								rapidjson::Value::MemberIterator value = m_document_models.FindMember ( m_selected_model.c_str () );
								value->name.SetString ( m_edit_name.c_str () , m_document_models.GetAllocator () );
								// update custom model file with new name
								std::rename ( ( AssetPath::Folder_BuildModels + m_selected_model + Asset3D::m_extension ).c_str () ,
									( AssetPath::Folder_BuildModels + m_edit_name + Asset3D::m_extension ).c_str () );
								std::string old_name { m_selected_model };
								m_selected_model = m_edit_name;

								// if diff source, set new raw and source and rebuild model
								bool new_raw { false };
								if ( m_edit_new_src )
								{
									Asset3D load_from_raw ( m_edit_src , false , m_flip_model_UVs );
									if ( load_from_raw.GetSuccessState () )
									{
										GetModelDocumentValue ( "Raw" ).SetString ( m_edit_raw.c_str () , m_document_models.GetAllocator () );
										GetModelDocumentValue ( "Source" ).SetString ( m_edit_src.c_str () , m_document_models.GetAllocator () );

										// serialize to config and copy to raw models folder
										load_from_raw.Serialize ( AssetPath::Folder_BuildModels + m_edit_name );
										god::FolderHelper::CopyFileToFolder ( m_edit_src , AssetPath::Folder_RawModels );
										new_raw = true;
									}
								}

								// update last edited date and time
								GetModelDocumentValue ( "Last Edited" ).SetString ( GetDateTimeString ().c_str () , m_document_models.GetAllocator () );

								// update json
								god::WriteJSON ( m_document_models , AssetPath::File_ModelsConfig );

								// update asset3d resource maager 
								// insert to asset3dmanager edited model
								auto& asset_3d = editorResources.Get<Asset3DManager> ().get ();
								// check if it was already loaded before
								if ( asset_3d.Has ( old_name ) )
								{
									// update with new name
									asset_3d.UpdateName ( old_name , m_edit_name , asset_3d.GetID ( old_name ) );

									// check if it has new raw 
									if ( new_raw )
									{
										asset_3d.SetResource ( asset_3d.GetID ( m_edit_name ) , LoadAsset3D ( AssetPath::Folder_BuildModels + m_edit_name , true ) );
										editorResources.Get<OpenGL> ().get ().UpdateOGLModel ( asset_3d.GetID ( m_edit_name ) , asset_3d );
									}
								}
							}

							ImGui::CloseCurrentPopup ();
						}
					}
					ImGui::EndPopup ();
				}

				if ( ImGui::Button ( "Edit" , { ImGui::GetWindowWidth (), 0.0f } )
					&& !m_selected_model.empty () )
				{
					m_edit_name = m_selected_model;
					m_edit_raw = GetModelDocumentValue ( "Raw" ).GetString ();
					m_edit_current_raw = m_edit_raw;
					m_edit_src = GetModelDocumentValue ( "Source" ).GetString ();
					m_edit_new_src = false;
					m_flip_model_UVs = true;
					ImGui::OpenPopup ( "EditModel" );
				}

				if ( ImGui::BeginPopup ( "ModelConfirmDelete" ) )
				{
					ImGui::Text ( "Delete %s?" , m_selected_model.c_str () );
					if ( ImGui::Button ( "Yes" , { 200.0f, 0.0f } ) )
					{
						m_document_models.RemoveMember ( m_document_models.FindMember ( m_selected_model.c_str () ) );
						god::WriteJSON ( m_document_models , AssetPath::File_ModelsConfig );
						ReloadConfig ();
						m_selected_model = { "" };
					}
					ImGui::EndPopup ();
				}

				if ( !m_selected_model.empty () && ImGui::Button ( "Delete" , { ImGui::GetWindowWidth (), 0.0f } ) )
				{
					ImGui::OpenPopup ( "ModelConfirmDelete" );
				}

				ImGui::EndTabItem ();
			}

			/*! ****************************************************************************************************************
			 * @brief
			 * : Manage Textures
			*/

			if ( ImGui::BeginTabItem ( "Textures" ) )
			{
				// display models config in a list view
				if ( ImGui::BeginListBox ( "##Textures" , { ImGui::GetWindowWidth (), 100.0f } ) )
				{
					if ( m_document_textures.IsObject () )
					{
						for ( auto& textures : m_document_textures.GetObject () )
						{
							if ( m_selected_texture.empty () || ImGui::Selectable ( textures.name.GetString () , m_selected_texture == textures.name.GetString () ) )
							{
								m_selected_texture = textures.name.GetString ();
							}
						}
					}
					ImGui::EndListBox ();
				}


				ImGui::Separator ();
				ImGui::Text ( "Properties: " );
				if ( m_selected_texture.empty () )
				{
					ImGui::Text ( "No asset selected." );
				}
				else
				{
					ImGui::BeginChild ( "TextureProperties" , { ImGui::GetWindowWidth () - 20, 150 } , true , ImGuiWindowFlags_HorizontalScrollbar );
					ImGui::Text ( "Texture Name : %s" , m_selected_texture.c_str () );

					for ( auto& m : m_document_textures[ m_selected_texture.c_str () ].GetArray ()[ 0 ].GetObject () )
					{
						if ( m.value.IsString () )
						{
							ImGui::Text ( "%s : %s" , m.name.GetString () , m.value.GetString () );
						}
						else if ( m.value.IsNumber () )
						{
							ImGui::Text ( "%s : %u" , m.name.GetString () , static_cast< unsigned long long >( m.value.GetUint () ) );
						}
					}
					ImGui::EndChild ();
				}

				float popup_width { 400.0f };
				float popup_height { 400.0f };
				ImGui::SetNextWindowSize ( { popup_width, 400.0f } );
				if ( ImGui::BeginPopup ( "EditTexture" ) )
				{
					ImGui::BeginChild ( "New Properties" , { ImGui::GetWindowWidth () - 15.0f, popup_height - 100.0f } , true , ImGuiWindowFlags_HorizontalScrollbar );

					ImGui::Text ( "New Properties" );
					ImGui::Text ( "Name:" );
					ImGui::SetNextItemWidth ( popup_width - 30.0f );
					ImGui::InputText ( "##TextureEditName" , &m_edit_name );

					ImGui::Text ( "Current Raw:" );
					ImGui::Text ( m_edit_current_raw.c_str () );

					if ( ImGui::Button ( "Edit Raw" , { popup_width - 30.0f, 0.0f } ) )
					{
						std::string new_src = god::OpenWindowDialog ( L"Texture Files" , L"*.png;*.jpg" );
						if ( !new_src.empty () )
						{
							m_edit_src = new_src;
							m_edit_raw = m_edit_src.substr ( m_edit_src.find_last_of ( '\\' ) + 1 , m_edit_src.size () );
							m_edit_new_src = true;
						}
					}

					ImGui::Text ( "New Raw:" );
					if ( m_edit_new_src )
					{
						ImGui::Text ( m_edit_src.c_str () );
					}
					else
					{
						ImGui::Text ( "- None -" );
					}

					ImGui::EndChild ();

					if ( ImGui::Button ( "Apply" , { popup_width, 0.0f } ) )
					{
						if ( m_document_textures.HasMember ( m_selected_texture.c_str () ) )
						{
							auto& ogl_textures = editorResources.Get<OGLTextureManager> ().get ();

							// set new name
							rapidjson::Value::MemberIterator value = m_document_textures.FindMember ( m_selected_texture.c_str () );
							value->name.SetString ( m_edit_name.c_str () , m_document_textures.GetAllocator () );

							// update name in loaded resource
							if ( ogl_textures.Has ( m_selected_texture ) )
							{
								ogl_textures.UpdateName ( m_selected_texture , m_edit_name , ogl_textures.GetID ( m_selected_texture ) );
							}

							m_selected_texture = m_edit_name;


							// if diff source, set new raw and source and rebuild model
							if ( m_edit_new_src )
							{
								GetTextureDocumentValue ( "Raw" ).SetString ( m_edit_raw.c_str () , m_document_textures.GetAllocator () );
								GetTextureDocumentValue ( "Source" ).SetString ( m_edit_src.c_str () , m_document_textures.GetAllocator () );

								if ( ogl_textures.Has ( m_selected_texture ) )
								{
									ogl_textures.Get ( m_edit_name ).Free ();
									ogl_textures.SetResource ( ogl_textures.GetID ( m_edit_name ) , OGLTexture ( AssetPath::Folder_RawTextures + m_edit_raw ) );
								}
							}

							// update last edited date and time
							GetTextureDocumentValue ( "Last Edited" ).SetString ( GetDateTimeString ().c_str () , m_document_textures.GetAllocator () );

							god::FolderHelper::CopyFileToFolder ( m_edit_src , AssetPath::Folder_RawTextures );

							// update json
							god::WriteJSON ( m_document_textures , AssetPath::File_TexturesConfig );
						}

						ImGui::CloseCurrentPopup ();

					}
					ImGui::EndPopup ();
				}

				if ( ImGui::Button ( "Edit" , { ImGui::GetWindowWidth (), 0.0f } )
					&& !m_selected_texture.empty () )
				{
					m_edit_name = m_selected_texture;
					m_edit_raw = GetTextureDocumentValue ( "Raw" ).GetString ();
					m_edit_current_raw = m_edit_raw;
					m_edit_src = GetTextureDocumentValue ( "Source" ).GetString ();
					m_edit_new_src = false;
					ImGui::OpenPopup ( "EditTexture" );
				}

				if ( ImGui::BeginPopup ( "TextureConfirmDelete" ) )
				{
					ImGui::Text ( "Delete %s?" , m_selected_texture.c_str () );
					if ( ImGui::Button ( "Yes" , { 200.0f, 0.0f } ) )
					{
						m_document_textures.RemoveMember ( m_document_textures.FindMember ( m_selected_texture.c_str () ) );
						god::WriteJSON ( m_document_textures , AssetPath::File_TexturesConfig );
						ReloadConfig ();
						m_selected_texture = { "" };
					}
					ImGui::EndPopup ();
				}

				if ( !m_selected_texture.empty () && ImGui::Button ( "Delete" , { ImGui::GetWindowWidth (), 0.0f } ) )
				{
					ImGui::OpenPopup ( "TextureConfirmDelete" );
				}
				ImGui::EndTabItem ();
			}

			ImGui::EndTabBar ();
		}

		if ( ImGui::Button ( "Refresh" , { ImGui::GetWindowWidth (), 0.0f } ) )
		{
			ReloadConfig ();
		}

		if ( ImGui::Button ( "Close" , { ImGui::GetWindowWidth (), 0.0f } ) )
		{
			this->Close ();
		}
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_AssetManager<EDITOR_RESOURCES>::OnOpen ()
	{
		m_selected_model = { "" };
		m_selected_texture = { "" };
		ReloadConfig ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_AssetManager<EDITOR_RESOURCES>::ReloadConfig ()
	{
		m_document_models.SetObject ();
		god::ReadJSON ( m_document_models , AssetPath::File_ModelsConfig );
		m_document_textures.SetObject ();
		god::ReadJSON ( m_document_textures , AssetPath::File_TexturesConfig );
	}

	template<typename EDITOR_RESOURCES>
	inline bool EW_AssetManager<EDITOR_RESOURCES>::ModelNameExists ( std::string const& name )
	{
		if ( m_document_models.IsObject () )
		{
			if ( m_document_models.FindMember ( name.c_str () ) != m_document_models.MemberEnd () )
			{
				return true;
			}
		}
		return false;
	}

	template<typename EDITOR_RESOURCES>
	inline rapidjson::Value& EW_AssetManager<EDITOR_RESOURCES>::GetModelDocumentValue ( char const* key )
	{
		assert ( ( m_document_models.IsObject () && !m_selected_model.empty () )
			&& "EW_AssetManager::GetModelDocumentValue - INVALID QUERY, m_document_models not object or m_selected_model.empty!" );

		if ( m_document_models[ m_selected_model.c_str () ].GetArray ()[ 0 ].HasMember ( key ) )
		{
			return m_document_models[ m_selected_model.c_str () ].GetArray ()[ 0 ][ key ];
		}
		else
		{
			RapidJSON::JSONifyToValue ( m_document_models[ m_selected_model.c_str () ].GetArray ()[ 0 ] , m_document_models , key , std::string ( "" ) );
			return m_document_models[ m_selected_model.c_str () ].GetArray ()[ 0 ][ key ];
		}
	}

	template<typename EDITOR_RESOURCES>
	inline rapidjson::Value& EW_AssetManager<EDITOR_RESOURCES>::GetTextureDocumentValue ( char const* key )
	{
		assert ( ( m_document_textures.IsObject () && !m_selected_texture.empty () )
			&& "EW_AssetManager::GetTextureDocumentValue - INVALID QUERY, m_document_models not object or m_selected_model.empty!" );

		if ( m_document_textures[ m_selected_texture.c_str () ].GetArray ()[ 0 ].HasMember ( key ) )
		{
			return m_document_textures[ m_selected_texture.c_str () ].GetArray ()[ 0 ][ key ];
		}
		else
		{
			RapidJSON::JSONifyToValue ( m_document_textures[ m_selected_texture.c_str () ].GetArray ()[ 0 ] , m_document_textures , key , std::string ( "" ) );
			return m_document_textures[ m_selected_texture.c_str () ].GetArray ()[ 0 ][ key ];
		}
	}
}