#pragma once
#include "../Editor.h"

#include <godUtility/Utility.h>
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>

#include "../../imgui/imgui_stdlib.h" 

#include <functional>
#include <chrono>
#include <ctime>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_Asset3DImporter : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

	private:
		enum class AssetType
		{
			Model ,
			Count
		};
		AssetType m_asset_type { AssetType::Count };

		uint32_t	MAX_INPUT_LENGTH { 256 };
		std::string ASSET3D_FOLDER { "Assets/GameAssets/Asset3D/" };
		std::string RAW_FOLDER { "Raw/" };
		std::string BUILD_FOLDER { "Build/" };
		std::string MODEL_FOLDER { "Models/" };
		std::string CONFIG_FILE { "Asset3DConfig.json" };

		std::string m_model_path;
		std::string m_asset_name;

		std::string m_error_message { "" };
	};
}

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline void EW_Asset3DImporter<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		ImGui::Begin ( "Asset3D Importer" );

		if ( ImGui::BeginTabBar ( "Asset Type" ) )
		{
			if ( ImGui::BeginTabItem ( "Model" ) )
			{
				ImGui::InputText ( "File Path" , &m_model_path );
				ImGui::SameLine ();
				if ( ImGui::Button ( "Browse" ) )
				{
					m_model_path = OpenWindowDialog ( L"Model Files" , L"*.obj;*.fbx" );
				}

				m_asset_type = AssetType::Model;

				ImGui::EndTabItem ();
			}

			ImGui::EndTabBar ();
		}

		ImGui::Separator ();

		ImGui::InputText ( "Asset Name" , &m_asset_name );
		ImGui::SameLine ();
		switch ( m_asset_type )
		{
		case ( AssetType::Model ):
			ImGui::Text ( ( std::string ( "(" ) + Asset3D::m_extension + ")" ).c_str () );
			break;
		}

		ImGui::PushStyleColor ( ImGuiCol_Text , { 1.0f, 0.0f, 0.0f, 1.0f } );
		if ( m_model_path.empty () )
		{
			ImGui::Text ( "Select a model with browse." );
		}
		if ( m_asset_name.empty () )
		{
			ImGui::Text ( "Give the asset a name." );
		}
		ImGui::PopStyleColor ();

		if ( ImGui::Button ( "Import" )
			&& !m_asset_name.empty ()
			&& !m_model_path.empty () )
		{
			ImGui::Text ( "Loading..." );

			// STEP - Serialze
			rapidjson::Document document;
			document.SetObject ();
			god::ReadJSON ( document , ASSET3D_FOLDER + CONFIG_FILE );

			rapidjson::Value value;
			value.SetObject ();

			// serialize model file name
			std::string model_name = m_model_path.substr ( m_model_path.find_last_of ( '\\' ) + 1 , m_model_path.size () );
			RapidJSON::JSONifyToValue ( value , document , "Model" , model_name );

			if ( document.HasMember ( m_asset_name.c_str () ) && document[ m_asset_name.c_str () ].IsArray () )
			{
				//document[ m_asset_name.c_str () ].PushBack ( value , document.GetAllocator () );
				document[ m_asset_name.c_str () ][ 0 ] = value;
			}
			else
			{
				RapidJSON::JSONifyValues ( document , m_asset_name , value );
			}

			god::WriteJSON ( document , ASSET3D_FOLDER + CONFIG_FILE );

			// STEP - File Setup
			god::FolderHelper::CopyFileToFolder ( m_model_path , ( ASSET3D_FOLDER + RAW_FOLDER + MODEL_FOLDER ).c_str () );

			// STEP - Create build version of asset
			switch ( m_asset_type )
			{
			case ( AssetType::Model ):
				Asset3D load_from_raw ( m_model_path );
				load_from_raw.Serialize ( ASSET3D_FOLDER + BUILD_FOLDER + MODEL_FOLDER + m_asset_name );
				break;
			}
		}

		if ( ImGui::Button ( "Close" ) )
		{
			// reset input 
			m_model_path.clear ();
			m_asset_name.clear ();

			this->m_open = false;
		}

		ImGui::End ();
	}
}