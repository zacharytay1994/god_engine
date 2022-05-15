#pragma once
#include "../Editor.h"

#include <godUtility/Utility.h>
#include <godUtility/FileIO.h>

#include "../../imgui/imgui_stdlib.h" 

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_Asset3DImporter : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

	private:
		static constexpr uint32_t MAX_INPUT_LENGTH { 256 };
		std::string m_model_path;
		std::string m_asset_name;
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

		if ( ImGui::Button ( "Browse" ) )
		{
			m_model_path = OpenWindowDialog (L"Model Files", L"*.obj;*.fbx");
		}
		ImGui::SameLine ();
		ImGui::InputText ( "Model Path" , &m_model_path );
		
		ImGui::Separator ();
		ImGui::InputText ( "Asset Name" , &m_asset_name );

		if ( ImGui::Button ( "Test Deserialize" ) )
		{
			Asset3D asset;
			asset.Deserialize ();
		}

		if ( ImGui::Button ( "Create Asset" ) )
		{
			Asset3D testasset ( m_model_path );
			testasset.Serialize ();
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