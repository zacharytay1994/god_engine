#pragma once

#include "godWindow.h"

#include <Windows.h>

namespace god
{
	struct Input
	{
		Input ();

		// keyboard check
		bool KeyIsDown ( UCHAR key ) const;				// return state of VIRTUAL KEY in keys_down_ buffer
		bool KeyWasPressed ( UCHAR key ) const;			// return if a specified VIRTUAL KEY was pressed in the most recent frame
		bool KeyUp ( UCHAR key ) const;					// return if a specified VIRTUAL KEY was released in the most recent frame

		// get & set
		void SetKeyDown ( WPARAM wparam );				// set key state of VIRTUAL KEY in keys_down_ and keys_pressed buffer
		void SetKeyUp ( WPARAM wparam );				// set key state of VIRTUAL KEY in keys_down_ buffer

		void EndFrame ();
	private:
		// definitions
		static constexpr int	KEYS_ARRAY_LENGTH	{ 256 };
		static constexpr UCHAR	KEYS_DOWN			{ 1 };
		static constexpr UCHAR	KEYS_PRESSED		{ 2 };
		static constexpr UCHAR	KEYS_UP				{ 4 };
		static constexpr UCHAR	MOUSE				{ 8 };
		static constexpr UCHAR	ALL_KEYS_MOUSE		{ KEYS_DOWN | KEYS_PRESSED | KEYS_UP | MOUSE };

		// key and mouse states
		// key states
		bool m_keys_down[ KEYS_ARRAY_LENGTH ] = { false };		// true if specified key is down, false otherwise
		bool m_keys_pressed[ KEYS_ARRAY_LENGTH ] = { false };	// true if specified key was pressed in latest frame, false otherwise, reset to false every frame
		bool m_keys_up[ KEYS_ARRAY_LENGTH ] = { false };

		// text record
		std::string m_text_in	{ "" };							// user entered text
		char		m_char_in	{ 0 };							// last character entered
		bool		m_new_line	{ true };						// true on start of new line

		// mouse states
		int		m_mouse_x				{ 0 };
		int		m_mouse_y				{ 0 };					// mouse screen coordinates
		int		m_mouse_raw_x			{ 0 };
		int		m_mouse_raw_y			{ 0 };					// high definition mouse data
		bool	m_mouse_captured		{ false };				// true if mouse captured
		bool	m_mouse_left_button		{ false };				// true if left mouse button down
		bool	m_mouse_middle_button	{ false };				// true if middle mouse button down
		bool	m_mouse_right_button	{ false };				// true if right mouse button down
		bool	m_mouse_x1_button		{ false };				// true if x1 mouse button down
		bool	m_mouse_x2_button		{ false };				// true if x2 mouse button down
		bool	m_state_change			{ false };
		RAWINPUTDEVICE m_rid[ 1 ];								// for high definition mouse

		bool ValidKeyCode ( UCHAR key ) const;
		void ClearBuffer ( UCHAR flag );
	};
}