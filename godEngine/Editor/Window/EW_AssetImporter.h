#pragma once
#include "../Editor.h"

#include <godUtility/Utility.h>
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>
#include <godUtility/DateTime.h>

#include "../../imgui/imgui_stdlib.h" 

#include <functional>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_Asset3DImporter : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

		void Clear ();

	private:
		enum class AssetType
		{
			Model ,
			Texture ,
			Count
		};
		AssetType m_asset_type { AssetType::Count };

		//uint32_t	MAX_INPUT_LENGTH { 256 };
		/*std::string ASSET3D_FOLDER { "Assets/GameAssets/Asset3D/" };
		std::string RAW_FOLDER { "Raw/" };
		std::string BUILD_FOLDER { "Build/" };
		std::string MODEL_FOLDER { "Models/" };
		std::string TEXTURE_FOLDER { "Textures/" };
		std::string CONFIG_FILE { "ModelConfig.json" };*/

		std::vector<std::string> m_asset_paths;
		std::vector<std::string> m_asset_names;

		std::string m_error_message { "" };
	};
}

#include "PathConfig.h"
#include "EW_AssetManager.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline void EW_Asset3DImporter<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		using namespace AssetPath;

		ImGui::Begin ( "Asset Importer" );

		if ( ImGui::BeginTabBar ( "Asset Type" ) )
		{
			if ( ImGui::BeginTabItem ( "Models" ) )
			{
				if ( m_asset_type != AssetType::Model )
				{
					m_asset_type = AssetType::Model;
					Clear ();
				}

				if ( ImGui::Button ( "Browse Models" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					//m_model_path = OpenWindowDialog ( L"Model Files" , L"*.obj;*.fbx" );
					m_asset_paths = OpenWindowDialogMulti ( L"Model Files" , L"*.obj;*.fbx" );

					// default asset names
					m_asset_names.resize ( m_asset_paths.size () );
					for ( size_t i = 0; i < m_asset_paths.size (); ++i )
					{
						size_t last_slash = m_asset_paths[ i ].find_last_of ( '\\' ) + 1;
						size_t last_dot = m_asset_paths[ i ].find_last_of ( '.' );
						m_asset_names[ i ] = m_asset_paths[ i ].substr ( last_slash , last_dot - last_slash );

					}
				}

				ImGui::EndTabItem ();
			}

			if ( ImGui::BeginTabItem ( "Textures" ) )
			{
				if ( m_asset_type != AssetType::Texture )
				{
					m_asset_type = AssetType::Texture;
					Clear ();
				}

				if ( ImGui::Button ( "Browse Textures" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					m_asset_paths = OpenWindowDialogMulti ( L"Texture Files" , L"*.png;*.jpg" );

					// default asset names
					m_asset_names.resize ( m_asset_paths.size () );
					for ( size_t i = 0; i < m_asset_paths.size (); ++i )
					{
						size_t last_slash = m_asset_paths[ i ].find_last_of ( '\\' ) + 1;
						size_t last_dot = m_asset_paths[ i ].find_last_of ( '.' );
						m_asset_names[ i ] = m_asset_paths[ i ].substr ( last_slash , last_dot - last_slash );

					}
				}

				ImGui::EndTabItem ();
			}

			ImGui::EndTabBar ();
		}

		ImGui::Separator ();
		if ( m_asset_paths.empty () )
		{
			ImGui::Text ( "- Nothing Selected -" );
		}

		for ( size_t i = 0; i < m_asset_paths.size (); ++i )
		{
			ImGui::PushID ( static_cast< int >( i ) );

			ImGui::Separator ();

			ImGui::BeginChild ( "Paths" , { ImGui::GetWindowWidth () - 20, 75 } , false , ImGuiWindowFlags_HorizontalScrollbar );
			ImGui::Text ( "%d. %s" , i + 1 , m_asset_paths[ i ].c_str () );

			ImGui::InputText ( ":Name" , &m_asset_names[ i ] );
			ImGui::EndChild ();

			ImGui::PopID ();
		}

		/*ImGui::InputText ( "Asset Name" , &m_asset_name );
		ImGui::SameLine ();*/
		/*switch ( m_asset_type )
		{
		case ( AssetType::Model ):
			ImGui::Text ( ( std::string ( "(" ) + Asset3D::m_extension + ")" ).c_str () );
			break;
		}*/

		/*ImGui::PushStyleColor ( ImGuiCol_Text , { 1.0f, 0.0f, 0.0f, 1.0f } );
		if ( m_model_path.empty () )
		{
			ImGui::Text ( "Select a model with browse." );
		}
		if ( m_asset_name.empty () )
		{
			ImGui::Text ( "Give the asset a name." );
		}
		ImGui::PopStyleColor ();*/

		if ( ImGui::Button ( "Import" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			// STEP - Serialze
			rapidjson::Document document;
			document.SetObject ();
			god::ReadJSON ( document , File_ModelsConfig );

			// STEP - Create build version of asset
			switch ( m_asset_type )
			{
			case ( AssetType::Model ):
			{
				for ( size_t i = 0; i < m_asset_paths.size (); ++i )
				{
					rapidjson::Value value;
					value.SetObject ();

					std::string model_path = m_asset_paths[ i ];
					std::string asset_name = m_asset_names[ i ];

					// serialize model file name
					std::string model_name = model_path.substr ( model_path.find_last_of ( '\\' ) + 1 , model_path.size () );
					RapidJSON::JSONifyToValue ( value , document , "Raw" , model_name );

					// serialize model path name
					RapidJSON::JSONifyToValue ( value , document , "Source" , model_path );

					// serialize edit time
					RapidJSON::JSONifyToValue ( value , document , "Last Edited" , GetDateTimeString () );

					if ( document.HasMember ( asset_name.c_str () ) && document[ asset_name.c_str () ].IsArray () )
					{
						// for unique id implementation next time
						//document[ m_asset_name.c_str () ].PushBack ( value , document.GetAllocator () );
						document[ asset_name.c_str () ][ 0 ] = value;
					}
					else
					{
						RapidJSON::JSONifyValues ( document , asset_name , value );
					}

					god::WriteJSON ( document , File_ModelsConfig );

					// STEP - File Setup
					god::FolderHelper::CopyFileToFolder ( model_path , Folder_RawModels );

					// STEP - Build custom from raw files
					Asset3D load_from_raw ( model_path );
					load_from_raw.Serialize ( Folder_BuildModels + asset_name );
				}

				this->Get<EW_AssetManager> ()->ReloadModelConfig ();
				break;
			}
			case ( AssetType::Texture ):
			{
				for ( size_t i = 0; i < m_asset_paths.size (); ++i )
				{
					rapidjson::Value value;
					value.SetObject ();

					std::string asset_path = m_asset_paths[ i ];
					std::string asset_name = m_asset_names[ i ];

					// serialize model file name
					std::string texture_name = asset_path.substr ( asset_path.find_last_of ( '\\' ) + 1 , asset_path.size () );
					RapidJSON::JSONifyToValue ( value , document , "Raw Texture" , texture_name );

					if ( document.HasMember ( asset_name.c_str () ) && document[ asset_name.c_str () ].IsArray () )
					{
						// for unique id implementation next time
						//document[ m_asset_name.c_str () ].PushBack ( value , document.GetAllocator () );
						document[ asset_name.c_str () ][ 0 ] = value;
					}
					else
					{
						RapidJSON::JSONifyValues ( document , asset_name , value );
					}

					god::WriteJSON ( document , File_ModelsConfig );

					// STEP - File Setup
					god::FolderHelper::CopyFileToFolder ( asset_path , ( Folder_RawTextures ).c_str () );
				}
				break;
			}
			}

			Clear ();
		}

		if ( ImGui::Button ( "Close" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			// reset input 
			Clear ();
			this->m_open = false;
		}

		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_Asset3DImporter<EDITOR_RESOURCES>::Clear ()
	{
		m_asset_names.clear ();
		m_asset_paths.clear ();
	}
}