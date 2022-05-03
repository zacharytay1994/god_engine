#pragma once

#include "godOpenGL.h"
#include "Internal/Shader.h"
#include "Internal/Primitives.h"

#include <vector>
#include <queue>

#include "glad/glad.h"
#include <glm/glm/glm.hpp>

namespace god
{
	using OGLEntityID = uint32_t;

	struct OGLRenderData
	{
		glm::vec3 m_position { 0.0f,0.0f,0.0f };
		glm::vec3 m_rotation { 0.0f,0.0f,0.0f };
		glm::vec3 m_scale { 1.0f,1.0f,1.0f };

		friend struct OpenGL;

		GODOPENGL_API OGLRenderData () = default;
		GODOPENGL_API OGLRenderData ( glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale );
	private:
		bool m_active { false };
	};

	struct OpenGL
	{
		GODOPENGL_API OpenGL ( HWND windowHandle , int width , int height );
		GODOPENGL_API ~OpenGL ();

		void GODOPENGL_API ClearColour () const;
		void GODOPENGL_API FrameRender (
			glm::mat4 const& projection ,
			glm::mat4 const& view ,
			glm::vec3 const& camera_position );

		OGLEntityID		GODOPENGL_API	AddCube (
			glm::vec3 const& position = { 0.0f,0.0f,0.0f } ,
			glm::vec3 const& rotation = { 0.0f,0.0f,0.0f } ,
			glm::vec3 const& scale = { 1.0f,1.0f,1.0f } );

		void			GODOPENGL_API	RemoveCube ( OGLEntityID id );
		OGLRenderData	GODOPENGL_API& GetCube ( OGLEntityID id );

		void GODOPENGL_API ResizeViewport ( int width , int height ) const;

		//void GODOPENGL_API Test ();

	private:
		int m_pixel_format { 0 };
		HDC m_window_device_context { nullptr };
		HGLRC m_opengl_rendering_context { nullptr };

		// shaders
		OGLShader m_flat_shader;

		// models
		// cube
		OGLMesh m_cube;

		std::vector<OGLRenderData> m_render_data;
		std::priority_queue<OGLEntityID , std::vector<OGLEntityID> , std::greater<OGLEntityID>> m_free_render_data;
	};
}