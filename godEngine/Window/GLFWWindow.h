#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace god
{
	struct GLFWWindow
	{
		bool m_exit { false };

		bool m_fullscreen { false };

		GLFWWindow ( uint32_t width , uint32_t height );
		~GLFWWindow ();

		bool			WindowShouldClose ();
		bool			WindowsMinimized ();
		void			PollEvents ();
		void			SwapWindowBuffers ();

		// getters & setters
		HWND			GetWindowHandle ();
		GLFWwindow* GetGLFWWindow ();
		uint32_t		GetWindowWidth ();
		uint32_t		GetWindowHeight ();
		bool			Resized ();

		// keyboard input
		bool			KeyDown ( int key );
		bool			KeyPressed ( int key );
		bool			KeyUp ( int key );

		// mouse input
		bool			WithinWindow ();

		double			ScreenMouseX ();			// mouse coordinates
		double			ScreenMouseY ();

		double			ViewportMouseX ();
		double			ViewportMouseY ();

		double			MouseOffsetX ();
		double			MouseOffsetY ();

		bool			MouseLDown ( uint32_t priority = 0 );		// left mouse
		bool			MouseLPressed ( uint32_t priority = 0 );
		bool			MouseLUp ( uint32_t priority = 0 );

		bool			MouseRDown ();		// right mouse
		bool			MouseRPressed ();
		bool			MouseRUp ();

		double			MouseScrollUp ();
		double			MouseScrollDown ();

		// gamepad input
		void			setGamepadConnect ( bool connected ) { m_gamepad_connected = connected; }
		void			setGamepadPress ( bool pressed ) { m_gamepad_pressed = pressed; }
		void			setGamepadReleased ( bool released ) { m_gamepad_released = released; }

		bool			GamepadConnected ();
		bool			GamepadPressed ();
		bool			GamepadReleased ();

		enum class GamePadInput : uint8_t
		{
			GAMEPAD_BUTTON_A ,				//0
			GAMEPAD_BUTTON_B ,				//1
			GAMEPAD_BUTTON_X ,				//2
			GAMEPAD_BUTTON_Y ,				//3
			GAMEPAD_BUTTON_LEFT_BUMPER ,		//4
			GAMEPAD_BUTTON_RIGHT_BUMPER ,	//5
			GAMEPAD_BUTTON_BACK ,			//6
			GAMEPAD_BUTTON_START ,			//7
			GAMEPAD_BUTTON_GUIDE ,			//8
			GAMEPAD_BUTTON_LEFT_THUMB ,		//9
			GAMEPAD_BUTTON_RIGHT_THUMB ,		//10
			GAMEPAD_BUTTON_DPAD_UP ,			//11
			GAMEPAD_BUTTON_DPAD_RIGHT ,		//12
			GAMEPAD_BUTTON_DPAD_DOWN ,		//13
			GAMEPAD_BUTTON_DPAD_LEFT ,		//14

			GAMEPAD_BUTTON_LAST = GAMEPAD_BUTTON_DPAD_LEFT ,
			GAMEPAD_BUTTON_CROSS = GAMEPAD_BUTTON_A ,
			GAMEPAD_BUTTON_CIRCLE = GAMEPAD_BUTTON_B ,
			GAMEPAD_BUTTON_SQUARE = GAMEPAD_BUTTON_X ,
			GAMEPAD_BUTTON_TRIANGLE = GAMEPAD_BUTTON_Y
		};

		friend void GLFWFramebufferSizeCallback ( GLFWwindow* window , int width , int height );
		friend void GLFWKeyCallback ( GLFWwindow* window , int key , int scancode , int action , int mods );
		friend void GLFWMouseCallback ( GLFWwindow* window , int button , int action , int mods );
		friend void GLFWScrollCallback ( GLFWwindow* window , double xOffset , double yOffset );

		friend void	GLFWJoystick_callback ( int jid , int event );

		void		SetViewportMouseCoordinates ( double x , double y );
		void		SetWindowTitle ( std::string const& title );

		void UpdateMouseOffset ();

	private:
		GLFWwindow* m_window;
		int m_width { 0 };
		int m_height { 0 };
		int present { 0 };

		bool m_resized { false };
		bool m_minimized { false };

		// glfw input states
		static constexpr uint16_t NUMBER_OF_KEYS { 500 };
		static constexpr uint16_t NUMBER_OF_JOYSTICK_KEYS { 20 };

		std::array<bool , NUMBER_OF_KEYS> m_key_states { false };
		std::array<bool , NUMBER_OF_KEYS> m_previous_key_states { false };

		std::array<bool , NUMBER_OF_JOYSTICK_KEYS> m_joystick_states { false };
		std::array<bool , NUMBER_OF_JOYSTICK_KEYS> m_previous_joystick_states { false };

		bool m_left_mouse { false };
		bool m_previous_left_mouse { false };
		bool m_right_mouse { false };
		bool m_previous_right_mouse { false };

		uint32_t m_left_mouse_logged_pressed_priority { 0 };
		uint32_t m_left_mouse_recent_pressed_priority { 0 };
		uint32_t m_left_mouse_logged_down_priority { 0 };
		uint32_t m_left_mouse_recent_down_priority { 0 };
		uint32_t m_left_mouse_logged_up_priority { 0 };
		uint32_t m_left_mouse_recent_up_priority { 0 };

		bool m_scroll_up { false };
		bool m_scroll_down { false };

		bool m_gamepad_connected { false };
		bool m_gamepad_pressed { false };
		bool m_gamepad_released { false };

		double m_viewport_mouse_x { 0.0f };
		double m_viewport_mouse_y { 0.0f };

		double m_last_mouse_x { 0.0f };
		double m_last_mouse_y { 0.0f };
		double m_mouse_offset_x { 0.0f };
		double m_mouse_offset_y { 0.0f };
	};
}