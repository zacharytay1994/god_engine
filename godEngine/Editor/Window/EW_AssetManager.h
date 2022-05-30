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

// to bypass windows.h GetObject() macro definition
#ifdef _MSC_VER
#undef GetObject
#endif

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_AssetManager : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
		void OnOpen () override;

		void ReloadModelConfig ();

	private:
		rapidjson::Document m_document_models;
		std::string m_selected_asset { "" };
		std::string m_to_delete { "" };

		// model editing
		std::string m_edit_name { "" };
		std::string m_edit_raw { "" };
		std::string m_edit_src { "" };
		bool m_edit_new_src { false };

		rapidjson::Value& GetDocumentValue ( char const* key );
	};
}

#include "PathConfig.h"

namespace god
{
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
							if ( m_selected_asset.empty () || ImGui::Selectable ( model.name.GetString () , m_selected_asset == model.name.GetString () ) )
							{
								m_selected_asset = model.name.GetString ();
							}
						}
					}
					ImGui::EndListBox ();
				}

				ImGui::Separator ();
				ImGui::Text ( "Properties: " );
				if ( m_selected_asset.empty () )
				{
					ImGui::Text ( "No asset selected." );
				}
				else
				{
					ImGui::BeginChild ( "ModelProperties" , { ImGui::GetWindowWidth () - 20, 150 } , true , ImGuiWindowFlags_HorizontalScrollbar );
					ImGui::Text ( "Model Name : %s" , m_selected_asset.c_str () );

					for ( auto& m : m_document_models[ m_selected_asset.c_str () ].GetArray ()[ 0 ].GetObject () )
					{
						ImGui::Text ( "%s : %s" , m.name.GetString () , m.value.GetString () );
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
					ImGui::InputText ( "##ModelEditName" , &m_edit_name );

					ImGui::Text ( "Raw:" );
					ImGui::Text ( m_edit_raw.c_str () );

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

					ImGui::EndChild ();

					if ( ImGui::Button ( "Apply" , { popup_width, 0.0f } ) )
					{
						if ( m_document_models.HasMember ( m_selected_asset.c_str () ) )
						{
							// set new name
							rapidjson::Value::MemberIterator value = m_document_models.FindMember ( m_selected_asset.c_str () );
							value->name.SetString ( m_edit_name.c_str () , m_document_models.GetAllocator () );
							m_selected_asset = m_edit_name;

							// if diff source, set new raw and source and rebuild model
							if ( m_edit_new_src )
							{
								GetDocumentValue ( "Raw" ).SetString ( m_edit_raw.c_str () , m_document_models.GetAllocator () );
								GetDocumentValue ( "Source" ).SetString ( m_edit_src.c_str () , m_document_models.GetAllocator () );

								Asset3D load_from_raw ( m_edit_src );
								load_from_raw.Serialize ( AssetPath::Folder_BuildModels + m_edit_name );
							}

							// update last edited date and time
							GetDocumentValue ( "Last Edited" ).SetString ( GetDateTimeString ().c_str () , m_document_models.GetAllocator () );

							god::FolderHelper::CopyFileToFolder ( m_edit_src , AssetPath::Folder_RawModels );

							// update json
							god::WriteJSON ( m_document_models , AssetPath::File_ModelsConfig );
						}

						ImGui::CloseCurrentPopup ();

					}
					ImGui::EndPopup ();
				}

				if ( ImGui::Button ( "Edit" , { ImGui::GetWindowWidth (), 0.0f } )
					&& !m_selected_asset.empty () )
				{
					m_edit_name = m_selected_asset;
					m_edit_raw = GetDocumentValue ( "Raw" ).GetString ();
					m_edit_src = GetDocumentValue ( "Source" ).GetString ();
					m_edit_new_src = false;
					ImGui::OpenPopup ( "EditModel" );
				}

				if ( ImGui::BeginPopup ( "ModelConfirmDelete" ) )
				{
					ImGui::Text ( "Delete %s?" , m_selected_asset.c_str () );
					if ( ImGui::Button ( "Yes" , { 200.0f, 0.0f } ) )
					{
						m_document_models.RemoveMember ( m_document_models.FindMember ( m_selected_asset.c_str () ) );
						god::WriteJSON ( m_document_models , AssetPath::File_ModelsConfig );
						ReloadModelConfig ();
						m_selected_asset = { "" };
					}
					ImGui::EndPopup ();
				}

				if ( !m_selected_asset.empty () && ImGui::Button ( "Delete" , { ImGui::GetWindowWidth (), 0.0f } ) )
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
				ImGui::EndTabItem ();
			}

			ImGui::EndTabBar ();
		}

		if ( ImGui::Button ( "Refresh" , { ImGui::GetWindowWidth (), 0.0f } ) )
		{
			ReloadModelConfig ();
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
		m_selected_asset = { "" };
		ReloadModelConfig ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_AssetManager<EDITOR_RESOURCES>::ReloadModelConfig ()
	{
		m_document_models.SetObject ();
		god::ReadJSON ( m_document_models , AssetPath::File_ModelsConfig );
	}

	template<typename EDITOR_RESOURCES>
	inline rapidjson::Value& EW_AssetManager<EDITOR_RESOURCES>::GetDocumentValue ( char const* key )
	{
		assert ( ( m_document_models.IsObject () && !m_selected_asset.empty () ) 
			&& "EW_AssetManager::GetDocumentValue - INVALID QUERY, m_document_models not object or m_selected_asset.empty!" );

		if ( m_document_models[ m_selected_asset.c_str () ].GetArray ()[ 0 ].HasMember ( key ) )
		{
			return m_document_models[ m_selected_asset.c_str () ].GetArray ()[ 0 ][ key ];
		}
		else
		{
			RapidJSON::JSONifyToValue ( m_document_models[ m_selected_asset.c_str () ].GetArray ()[ 0 ] , m_document_models , key , std::string ( "" ) );
			return m_document_models[ m_selected_asset.c_str () ].GetArray ()[ 0 ][ key ];
		}
	}
}