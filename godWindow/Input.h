#pragma once

#include "godWindow.h"

#include <Windows.h>

namespace god
{
	struct Input
	{
		// mouse states
		int		m_mouse_x { 0 };
		int		m_mouse_y { 0 };						// mouse screen coordinates
		int		m_mouse_raw_x { 0 };
		int		m_mouse_raw_y { 0 };					// high definition mouse data

		bool	m_mouse_left_down { false };			// true if left mouse button down
		bool	m_mouse_left_pressed { false };
		bool	m_mouse_left_up { false };

		bool	m_mouse_middle_down { false };			// true if middle mouse button down
		bool	m_mouse_middle_pressed { false };
		bool	m_mouse_middle_up { false };

		bool	m_mouse_right_down { false };			// true if right mouse button down
		bool	m_mouse_right_pressed { false };
		bool	m_mouse_right_up { false };


		bool	m_mouse_captured { false };				// true if mouse captured
		RAWINPUTDEVICE m_rid[ 1 ];						// for high definition mouse

		Input ();

		// keyboard input
		bool KeyIsDown ( UCHAR key ) const;				// return state of VIRTUAL KEY in keys_down_ buffer
		bool KeyWasPressed ( UCHAR key ) const;			// return if a specified VIRTUAL KEY was pressed in the most recent frame
		bool KeyUp ( UCHAR key ) const;					// return if a specified VIRTUAL KEY was released in the most recent frame

		// get & set
		void SetKeyDown ( WPARAM wParam );				// set key state of VIRTUAL KEY in keys_down_ and keys_pressed buffer
		void SetKeyUp ( WPARAM wParam );				// set key state of VIRTUAL KEY in keys_down_ buffer

		void FillMousePosition ( LPARAM lParam );		// reads and sets mouse_x_ and mouse_y_
		void FillMouseRawPosition ( LPARAM lParam );	// read and sets mouse_raw_x_ and mouse_raw_y_

		void EndFrame ();
	private:
		// definitions
		static constexpr int	KEYS_ARRAY_LENGTH	{ 256 };
		static constexpr UCHAR	KEYS_DOWN			{ 1 };
		static constexpr UCHAR	KEYS_PRESSED		{ 2 };
		static constexpr UCHAR	KEYS_UP				{ 4 };
		static constexpr UCHAR	MOUSE				{ 8 };
		static constexpr UCHAR	ALL_KEYS_MOUSE		{ KEYS_DOWN | KEYS_PRESSED | KEYS_UP | MOUSE };

		bool	m_state_change { false };

		// key states
		bool m_keys_down[ KEYS_ARRAY_LENGTH ] = { false };			// true if specified key is down, false otherwise
		bool m_keys_pressed[ KEYS_ARRAY_LENGTH ] = { false };		// true if specified key was pressed in latest frame, false otherwise, reset to false every frame
		bool m_keys_pressed_once[ KEYS_ARRAY_LENGTH ] = { false };
		bool m_keys_up[ KEYS_ARRAY_LENGTH ] = { false };

		bool ValidKeyCode ( UCHAR key ) const;
		void ClearBuffer ( UCHAR flag );
	};
}