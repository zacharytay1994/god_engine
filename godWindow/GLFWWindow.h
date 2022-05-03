#pragma once

#include "godWindow.h"

#include <GLFW/glfw3.h>

#include <array>

namespace god
{
	struct GLFWWindow
	{
		GODWINDOW_API GLFWWindow ( uint32_t width , uint32_t height , bool fullscreen = false );

		bool		GODWINDOW_API WindowShouldClose ();
		void		GODWINDOW_API PollEvents ();
		void		GODWINDOW_API SwapWindowBuffers ();

		// getters & setters
		HWND		GODWINDOW_API GetWindowHandle ();
		uint32_t	GODWINDOW_API GetWindowWidth ();
		uint32_t	GODWINDOW_API GetWindowHeight ();
		bool		GODWINDOW_API Resized ();

		// keyboard input
		bool		GODWINDOW_API KeyDown ( int key );
		bool		GODWINDOW_API KeyPressed ( int key );
		bool		GODWINDOW_API KeyUp ( int key );

		// mouse input
		int			GODWINDOW_API MouseX ();			// mouse coordinates
		int			GODWINDOW_API MouseY ();

		bool		GODWINDOW_API MouseLDown ();		// left mouse
		bool		GODWINDOW_API MouseLPressed ();
		bool		GODWINDOW_API MouseLUp ();

		bool		GODWINDOW_API MouseRDown ();		// right mouse
		bool		GODWINDOW_API MouseRPressed ();
		bool		GODWINDOW_API MouseRUp ();

		double		GODWINDOW_API MouseScrollUp ();
		double		GODWINDOW_API MouseScrollDown ();

		friend void GLFWFramebufferSizeCallback ( GLFWwindow* window , int width , int height );
		friend void GLFWKeyCallback ( GLFWwindow* window , int key , int scancode , int action , int mods );
		friend void GLFWMouseCallback ( GLFWwindow* window , int button , int action , int mods );
		friend void GLFWScrollCallback ( GLFWwindow* window , double xOffset , double yOffset );

	private:
		GLFWwindow* m_window;
		int m_width { 0 };
		int m_height { 0 };
		bool m_resized { false };

		// glfw input states
		static constexpr uint16_t NUMBER_OF_KEYS { 500 };
		std::array<bool , NUMBER_OF_KEYS> m_key_states { false };
		std::array<bool , NUMBER_OF_KEYS> m_previous_key_states { false };

		bool m_left_mouse { false };
		bool m_previous_left_mouse { false };
		bool m_right_mouse { false };
		bool m_previous_right_mouse { false };

		bool m_scroll_up { false };
		bool m_scroll_down { false };
	};
}