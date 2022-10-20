#include "../pch.h"
#include "GLFWWindow.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#pragma comment (lib, "glfw3.lib")

namespace god
{
	static void glfw_error_callback ( int error , const char* description )
	{
		fprintf ( stderr , "Glfw Error %d: %s\n" , error , description );
	}

	void GLFWFramebufferSizeCallback ( GLFWwindow* window , int width , int height )
	{
		GLFWWindow* my_window = reinterpret_cast< GLFWWindow* >( glfwGetWindowUserPointer ( window ) );
		my_window->m_resized = true;
		my_window->m_width = width;
		my_window->m_height = height;
	}

	void GLFWKeyCallback ( GLFWwindow* window , int key , int scancode , int action , int mods )
	{
		UNREFERENCED_PARAMETER ( mods );
		UNREFERENCED_PARAMETER ( scancode );

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
		UNREFERENCED_PARAMETER ( mods );

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
		UNREFERENCED_PARAMETER ( xOffset );

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

	void GLFWJoystick_callback(int jid, int event)
	{
		//UNREFERENCED_PARAMETER(jid);

		if (event == GLFW_CONNECTED)
		{
			// The joystick was connected
			//setGamepad(true);
			std::cout << "Joystick is connected!\n";

		}
		else if (event == GLFW_DISCONNECTED)
		{
			// The joystick was disconnected
			//setGamepad(false);
			std::cout << "Joystick is disconnected!\n";
		}

	}

	GLFWWindow::GLFWWindow ( uint32_t width , uint32_t height )
		:
		m_width ( width ) ,
		m_height ( height )
	{
		glfwSetErrorCallback ( glfw_error_callback );
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
		glfwSwapInterval ( 0 );

		glfwSetWindowUserPointer ( m_window , this );
		glfwSetFramebufferSizeCallback ( m_window , GLFWFramebufferSizeCallback );
		glfwSetKeyCallback ( m_window , GLFWKeyCallback );
		glfwSetMouseButtonCallback ( m_window , GLFWMouseCallback );
		glfwSetScrollCallback ( m_window , GLFWScrollCallback );

		glfwSetJoystickCallback(GLFWJoystick_callback);

		present = glfwJoystickPresent(GLFW_JOYSTICK_1);
		std::cout << "Joystick/Gamepad status: " << present << "\n";

	}

	GLFWWindow::~GLFWWindow ()
	{
		glfwDestroyWindow ( m_window );
		glfwTerminate ();
	}

	bool GLFWWindow::WindowShouldClose ()
	{
		return glfwWindowShouldClose ( m_window );
	}

	bool GLFWWindow::WindowsMinimized()
	{
		if (m_width == 0 || m_height == 0)
		{
			m_minimized = true;
			return true;
		}

		m_minimized = false;
		return false;
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

		m_gamepad_connected = false;
		
		if (present == 1)
		{
			int axesCount{ 0 };
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			//std::cout << "Number of axes available: " << axesCount << "\n";

			int buttonCount{ 0 };
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

			if (GLFW_PRESS == buttons[0])
				std::cout << "Square Button pressed\n";

			if (GLFW_PRESS == buttons[1])
				std::cout << "X Button pressed\n";

			if (GLFW_PRESS == buttons[2])
				std::cout << "O Button pressed\n";

			if (GLFW_PRESS == buttons[3])
				std::cout << "Triangle Button pressed\n";

			if (GLFW_PRESS == buttons[4])
				std::cout << "L1 Button pressed\n";

			if (GLFW_PRESS == buttons[5])
				std::cout << "R1 Button pressed\n";

			if (GLFW_PRESS == buttons[6])
				std::cout << "L2 Button pressed\n";

			if (GLFW_PRESS == buttons[7])
				std::cout << "R2 Button pressed\n";

			if (GLFW_PRESS == buttons[8])
				std::cout << "8 Button pressed\n";

			if (GLFW_PRESS == buttons[9])
				std::cout << "9 Button pressed\n";

			if (GLFW_PRESS == buttons[10])
				std::cout << "10 Button pressed\n";

			if (GLFW_PRESS == buttons[11])
				std::cout << "11 Button pressed\n";

			if (GLFW_PRESS == buttons[12])
				std::cout << "12 Button pressed\n";

			if (GLFW_PRESS == buttons[13])
				std::cout << "13 Button pressed\n";

			if (GLFW_PRESS == buttons[14])
				std::cout << "14 Button pressed\n";

			if (GLFW_PRESS == buttons[15])
				std::cout << "15 Button pressed\n";

			if (GLFW_PRESS == buttons[16])
				std::cout << "16 Button pressed\n";

			if (GLFW_PRESS == buttons[17])
				std::cout << "17 Button pressed\n";

			if (GLFW_PRESS == buttons[18])
				std::cout << "18 Button pressed\n";

			/*else if (GLFW_RELEASE == buttons[1])
				std::cout << "X Button released\n";*/

			const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
			//std::cout << "Joystick is called " << name << "\n";

		}

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

	GLFWwindow* GLFWWindow::GetGLFWWindow ()
	{
		return m_window;
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

	bool GLFWWindow::WithinWindow ()
	{
		return m_viewport_mouse_x > 0.0f && m_viewport_mouse_x < GetWindowWidth () &&
			m_viewport_mouse_y > 0.0f && m_viewport_mouse_y < GetWindowHeight ();
	}

	double GLFWWindow::ScreenMouseX ()
	{
		double x , y;
		glfwGetCursorPos ( m_window , &x , &y );
		return x;
	}

	double GLFWWindow::ScreenMouseY ()
	{
		double x , y;
		glfwGetCursorPos ( m_window , &x , &y );
		return y;
	}

#define EditorMode
	double GLFWWindow::ViewportMouseX ()
	{
#ifdef EditorMode
		return m_viewport_mouse_x;
#else
		return ScreenMouseX ();
#endif
	}

	double GLFWWindow::ViewportMouseY ()
	{
#ifdef EditorMode
		return m_viewport_mouse_y;
#else
		return ScreenMouseY ();
#endif
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

	bool GLFWWindow::GamepadConnected()
	{
		return m_gamepad_connected;
	}

	bool GLFWWindow::GamepadPressed()
	{
		return m_gamepad_pressed;
	}

	bool GLFWWindow::GamepadReleased()
	{
		return m_gamepad_released;
	}

	void GLFWWindow::SetViewportMouseCoordinates ( double x , double y )
	{
		m_viewport_mouse_x = x;
		m_viewport_mouse_y = y;
	}
}