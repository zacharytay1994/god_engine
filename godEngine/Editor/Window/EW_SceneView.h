#pragma once
#include "../Editor.h"

#include "../../imgui/imgui_stdlib.h" 

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_SceneView : EditorWindow<EDITOR_RESOURCES>
	{
		EW_SceneView ( float aspectRatio );
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
	private:
		static constexpr unsigned int NO_TEXTURE = static_cast< unsigned int >( -1 );
	public:
		void SetRenderpassTexture ( unsigned int texture = NO_TEXTURE );
	private:
		unsigned int m_renderpass_texture { NO_TEXTURE };
		float m_aspect_ratio;
		float m_margin_buffer_x { 20.0f };
		float m_margin_buffer_y { 50.0f };
	};
}

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_SceneView<EDITOR_RESOURCES>::EW_SceneView ( float aspectRatio )
		:
		m_aspect_ratio { aspectRatio }
	{
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneView<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		( dt );
		( editorResources );
		ImGui::Begin ( "Scene View" );
		if ( m_renderpass_texture != NO_TEXTURE )
		{
			auto window_size = ImVec2 ( ImGui::GetWindowWidth () - m_margin_buffer_x , ImGui::GetWindowHeight () - m_margin_buffer_y );
			ImVec2 viewport_size;
			if ( window_size.x > window_size.y * m_aspect_ratio )
			{
				viewport_size = { window_size.y * m_aspect_ratio, window_size.y };
				ImGui::SetCursorPosX ( ( ImGui::GetWindowWidth () - viewport_size.x ) / 2.0f );
			}
			else
			{
				viewport_size = { window_size.x, window_size.x * ( 1.0f / m_aspect_ratio ) };
				ImGui::SetCursorPosY ( ( ImGui::GetWindowHeight () - viewport_size.y ) / 2.0f );
			}
			ImGui::Image ( ( void* ) ( static_cast< uint64_t >( m_renderpass_texture ) ) , viewport_size , { 0,1 } , { 1,0 } );
		}
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneView<EDITOR_RESOURCES>::SetRenderpassTexture ( unsigned int texture )
	{
		m_renderpass_texture = texture;
	}
}