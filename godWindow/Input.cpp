#include "pch.h"
#include "Input.h"

#include <windowsx.h>

namespace god
{
	Input::Input ()
	{
		ClearBuffer ( ALL_KEYS_MOUSE );
		m_state_change = false;
	}

	bool Input::KeyIsDown ( UCHAR key ) const
	{
		if ( ValidKeyCode ( key ) )
		{
			return m_keys_down[ key ];
		}
		return false;
	}

	bool Input::KeyWasPressed ( UCHAR key ) const
	{
		if ( ValidKeyCode ( key ) )
		{
			return m_keys_pressed[ key ];
		}
		return false;
	}

	bool Input::KeyUp ( UCHAR key ) const
	{
		if ( ValidKeyCode ( key ) )
		{
			return m_keys_up[ key ];
		}
		return false;
	}

	void Input::SetKeyDown ( WPARAM wParam )
	{
		// make sure key code is within buffer range
		if ( ValidKeyCode ( wParam ) )
		{
			m_state_change = true;
			m_keys_down[ wParam ] = true;

			if ( !m_keys_pressed_once[ wParam ] )
			{
				m_keys_pressed_once[ wParam ] = true;
				m_keys_pressed[ wParam ] = true;
			}

			// additional check for keys with left and right
			switch ( wParam )
			{
			case VK_SHIFT:
				// GetKeyState returns a 8 bit value representing individual key state, 
				// if high-order bit (leftmost bit) == 1, key is down, if == 0, key is up
				// e.g. GetKeyState(VK_LSHIFT) returns 10101100, 1010110 & (masked by) 1000000 is 1000000, which = 128, >0 = true;
				if ( GetKeyState ( VK_LSHIFT ) & 128 )
				{
					m_keys_down[ VK_LSHIFT ] = true;
				}
				if ( GetKeyState ( VK_RSHIFT ) & 128 )
				{
					m_keys_down[ VK_RSHIFT ] = true;
				}
				break;
			case VK_CONTROL:
				if ( GetKeyState ( VK_LCONTROL ) & 128 )
				{
					m_keys_down[ VK_LCONTROL ] = true;
				}
				if ( GetKeyState ( VK_RCONTROL ) & 128 )
				{
					m_keys_down[ VK_RCONTROL ] = true;
				}
				break;
			}
		}
	}

	void Input::SetKeyUp ( WPARAM wParam )
	{
		if ( ValidKeyCode ( wParam ) )
		{
			m_state_change = true;
			m_keys_down[ wParam ] = false;
			m_keys_up[ wParam ] = true;
			m_keys_pressed_once[ wParam ] = false;

			switch ( wParam )
			{
				// weird bug here, if both shifts are down, releasing one does not generate a message?
				// control works fine with the same method, unsure whats wrong
			case VK_SHIFT:
				if ( ( GetKeyState ( VK_LSHIFT ) & 128 ) == 0 )
				{
					m_keys_down[ VK_LSHIFT ] = false;
				}
				if ( ( GetKeyState ( VK_RSHIFT ) & 128 ) == 0 )
				{
					m_keys_down[ VK_RSHIFT ] = false;
				}
				break;
			case VK_CONTROL:
				if ( ( GetKeyState ( VK_LCONTROL ) & 128 ) == 0 )
				{
					m_keys_down[ VK_LCONTROL ] = false;
				}
				if ( ( GetKeyState ( VK_RCONTROL ) & 128 ) == 0 )
				{
					m_keys_down[ VK_RCONTROL ] = false;
				}
				// only release main control if both sides are released
				m_keys_down[ wParam ] = m_keys_down[ VK_LCONTROL ] || m_keys_down[ VK_RCONTROL ] ? true : false;
				break;
			}
		}
	}

	void Input::FillMousePosition ( LPARAM lParam )
	{
		m_mouse_x = GET_X_LPARAM ( lParam );
		m_mouse_y = GET_Y_LPARAM ( lParam );
	}

	void Input::FillMouseRawPosition ( LPARAM lParam )
	{
		// data size
		UINT dw_size = 40;
		// data
		static BYTE lpb[ 40 ];

		GetRawInputData ( ( HRAWINPUT ) lParam , RID_INPUT ,
			lpb , &dw_size , sizeof ( RAWINPUTHEADER ) );

		RAWINPUT* raw = ( RAWINPUT* ) lpb;

		if ( raw->header.dwType == RIM_TYPEMOUSE )
		{
			m_mouse_x = raw->data.mouse.lLastX;
			m_mouse_y = raw->data.mouse.lLastY;
		}
	}

	void Input::EndFrame ()
	{
		ClearBuffer ( KEYS_PRESSED | KEYS_UP );

		// reset mouse buttons
		m_mouse_left_pressed = false;
		m_mouse_left_up = false;
		m_mouse_middle_pressed = false;
		m_mouse_middle_up = false;
		m_mouse_right_pressed = false;
		m_mouse_right_up = false;

		m_state_change = false;
	}

	bool Input::ValidKeyCode ( UCHAR key ) const
	{
		return key < KEYS_ARRAY_LENGTH;
	}

	void Input::ClearBuffer ( UCHAR flag )
	{
		if ( flag & KEYS_DOWN )
		{
			for ( bool& b : m_keys_down )
			{
				b = false;
			}
		}
		if ( flag & KEYS_PRESSED )
		{
			for ( bool& b : m_keys_pressed )
			{
				b = false;
			}
		}
		if ( flag & KEYS_UP )
		{
			for ( bool& b : m_keys_up )
			{
				b = false;
			}
		}
		if ( flag & MOUSE )
		{
			m_mouse_x = 0;
			m_mouse_y = 0;
			m_mouse_raw_x = 0;
			m_mouse_raw_y = 0;
		}
	}
}