#include "pch.h"
#include "OpenGL.h"

#pragma comment (lib, "opengl32.lib")

#include <iostream>

namespace god
{
	OpenGL::OpenGL ( HWND windowHandle , int width , int height )
		:
		m_window_device_context { GetDC ( windowHandle ) }
	{
		// customize pixel format
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof ( PIXELFORMATDESCRIPTOR ),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	//Flags
			PFD_TYPE_RGBA,												// The kind of framebuffer. RGBA or palette.
			32,															// Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,															// Number of bits for the depthbuffer
			8,															// Number of bits for the stencilbuffer
			0,															// Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		// select pixel format
		m_pixel_format = ChoosePixelFormat ( m_window_device_context , &pfd );
		SetPixelFormat ( m_window_device_context , m_pixel_format , &pfd );

		// create opengl context
		m_opengl_rendering_context = wglCreateContext ( m_window_device_context );
		wglMakeCurrent ( m_window_device_context , m_opengl_rendering_context );

		// load opengl functions with glad
		if ( !gladLoadGL () )
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport ( width , height );

		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL ()
	{
		wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::StartFrame ( bool resized , int width , int height )
	{
		if ( resized )
		{
			ResizeViewport ( width , height );
		}

		glClearColor ( 0.2f , 0.3f , 0.3f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT );
	}

	void OpenGL::EndFrame ()
	{
		SwapBuffers ( m_window_device_context );
	}

	void OpenGL::ResizeViewport ( int width , int height )
	{
		glViewport ( 0 , 0 , width , height );
		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
	}
}