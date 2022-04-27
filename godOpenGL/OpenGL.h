#pragma once

#include "godOpenGL.h"
#include "glad/glad.h"

namespace god
{
	struct OpenGL
	{
		GODOPENGL_API OpenGL ( HWND windowHandle , int width , int height );
		GODOPENGL_API ~OpenGL ();

		void GODOPENGL_API StartFrame ( bool resized , int width , int height );
		void GODOPENGL_API EndFrame ();

		void GODOPENGL_API ResizeViewport ( int width , int height );

	private:
		int m_pixel_format { 0 };
		HDC m_window_device_context { nullptr };
		HGLRC m_opengl_rendering_context { nullptr };
	};
}