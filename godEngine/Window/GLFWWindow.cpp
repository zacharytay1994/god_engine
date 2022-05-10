#include "../pch.h"
#include "GLFWWindow.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#pragma comment (lib, "glfw3.lib")

namespace god
{
	void GLFWFramebufferSizeCallback ( GLFWwindow* window , int width , int height )
	{
		GLFWWindow* my_window = reinterpret_cast< GLFWWindow* >( glfwGetWindowUserPointer ( window ) );
		my_window->m_resized = true;
		my_window->m_width = width;
		my_window->m_height = height;
	}

	void GLFWKeyCallback ( GLFWwindow* window , int key , int scancode , int action , int mods )
	{
		GLFWWindow* my_window = reinterpret_cast< GLFWWindow* >( glfwGetWindowUserPointer ( window ) );
		if ( action == GLFW_PRESS )
		{
			my_window->m_key_states[ key ] = true;
		}
		else if ( action == GLFW_RELEASE )
		{
			my_window->m_key_states[ key ] = false;
		}
	}

	void GLFWMouseCallback ( GLFWwindow* window , int button , int action , int mods )
	{
		GLFWWindow* my_window = reinterpret_cast< GLFWWindow* >( glfwGetWindowUserPointer ( window ) );
		if ( button == GLFW_MOUSE_BUTTON_LEFT )
		{
			if ( action == GLFW_PRESS )
			{
				my_window->m_left_mouse = true;
			}
			else if ( action == GLFW_RELEASE )
			{
				my_window->m_left_mouse = false;
			}
		}
		else if ( button == GLFW_MOUSE_BUTTON_RIGHT )
		{
			if ( action == GLFW_PRESS )
			{
				my_window->m_right_mouse = true;
			}
			else if ( action == GLFW_RELEASE )
			{
				my_window->m_right_mouse = false;
			}
		}
	}

	void GLFWScrollCallback ( GLFWwindow* window , double xOffset , double yOffset )
	{
		GLFWWindow* my_window = reinterpret_cast< GLFWWindow* >( glfwGetWindowUserPointer ( window ) );
		if ( yOffset > 0.1 )
		{
			my_window->m_scroll_up = true;
		}
		else if ( yOffset < 0.1 )
		{
			my_window->m_scroll_down = true;
		}
	}

	GLFWWindow::GLFWWindow ( uint32_t width , uint32_t height , bool fullscreen )
		:
		m_width ( width ) ,
		m_height ( height )
	{
		glfwInit ();
		glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR , 4 );
		glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR , 5 );
		glfwWindowHint ( GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE );

		m_window = glfwCreateWindow ( m_width , m_height , "God Engine" , NULL , NULL );
		if ( m_window == NULL )
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate ();
			return;
		}
		glfwMakeContextCurrent ( m_window );

		glfwSetWindowUserPointer ( m_window , this );

		glfwSetFramebufferSizeCallback ( m_window , GLFWFramebufferSizeCallback );
		glfwSetKeyCallback ( m_window , GLFWKeyCallback );
		glfwSetMouseButtonCallback ( m_window , GLFWMouseCallback );
		glfwSetScrollCallback ( m_window , GLFWScrollCallback );
	}

	bool GLFWWindow::WindowShouldClose ()
	{
		return glfwWindowShouldClose ( m_window );
	}

	void GLFWWindow::PollEvents ()
	{
		// reset states
		m_previous_key_states = m_key_states;
		m_previous_left_mouse = m_left_mouse;
		m_previous_right_mouse = m_right_mouse;
		m_scroll_up = false;
		m_scroll_down = false;
		m_resized = false;
		glfwPollEvents ();
	}

	void GLFWWindow::SwapWindowBuffers ()
	{
		glfwSwapBuffers ( m_window );
	}

	HWND GLFWWindow::GetWindowHandle ()
	{
		return glfwGetWin32Window ( m_window );
	}

	uint32_t GLFWWindow::GetWindowWidth ()
	{
		return m_width;
	}

	uint32_t GLFWWindow::GetWindowHeight ()
	{
		return m_height;
	}

	bool GLFWWindow::Resized ()
	{
		return m_resized;
	}

	bool GLFWWindow::KeyDown ( int key )
	{
		return m_key_states[ key ];
	}

	bool GLFWWindow::KeyPressed ( int key )
	{
		return m_key_states[ key ] && !m_previous_key_states[ key ];
	}

	bool GLFWWindow::KeyUp ( int key )
	{
		return !m_key_states[ key ] && m_previous_key_states[ key ];
	}

	int GLFWWindow::MouseX ()
	{
		double x , y;
		glfwGetCursorPos ( m_window , &x , &y );
		return x;
	}

	int GLFWWindow::MouseY ()
	{
		double x , y;
		glfwGetCursorPos ( m_window , &x , &y );
		return y;
	}

	bool GLFWWindow::MouseLDown ()
	{
		return m_left_mouse;
	}

	bool GLFWWindow::MouseLPressed ()
	{
		return m_left_mouse && !m_previous_left_mouse;
	}

	bool GLFWWindow::MouseLUp ()
	{
		return !m_left_mouse && m_previous_left_mouse;
	}

	bool GLFWWindow::MouseRDown ()
	{
		return m_right_mouse;
	}

	bool GLFWWindow::MouseRPressed ()
	{
		return m_right_mouse && !m_previous_right_mouse;
	}

	bool GLFWWindow::MouseRUp ()
	{
		return !m_right_mouse && m_previous_right_mouse;
	}

	double GLFWWindow::MouseScrollUp ()
	{
		return m_scroll_up;
	}

	double GLFWWindow::MouseScrollDown ()
	{
		return m_scroll_down;
	}
}