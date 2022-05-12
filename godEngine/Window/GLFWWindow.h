#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace god
{
	struct GLFWWindow
	{
		 GLFWWindow ( uint32_t width , uint32_t height );

		bool			WindowShouldClose ();
		void			PollEvents ();
		void			SwapWindowBuffers ();

		// getters & setters
		HWND			GetWindowHandle ();
		GLFWwindow*		GetGLFWWindow ();
		uint32_t		GetWindowWidth ();
		uint32_t		GetWindowHeight ();
		bool			Resized ();

		// keyboard input
		bool			KeyDown ( int key );
		bool			KeyPressed ( int key );
		bool			KeyUp ( int key );

		// mouse input
		double			MouseX ();			// mouse coordinates
		double			MouseY ();

		bool			MouseLDown ();		// left mouse
		bool			MouseLPressed ();
		bool			MouseLUp ();

		bool			MouseRDown ();		// right mouse
		bool			MouseRPressed ();
		bool			MouseRUp ();

		double			MouseScrollUp ();
		double			MouseScrollDown ();

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