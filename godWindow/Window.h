#pragma once
#define WIN32_LEAN_AND_MEAN

#include "godWindow.h"
#include "Input.h"

#include <Windows.h>
#include <cstdlib>

namespace god
{
	struct Window
	{
		GODWINDOW_API Window ( uint32_t width , uint32_t height , bool fullscreen = false );
		bool GODWINDOW_API WindowShouldClose ();
		void GODWINDOW_API PollEvents ();

		// getters & setters
		uint32_t GODWINDOW_API GetWindowWidth ();
		uint32_t GODWINDOW_API GetWindowHeight ();

		// get input
		bool GODWINDOW_API KeyIsDown ( UCHAR key ) const;
		bool GODWINDOW_API KeyWasPressed ( UCHAR key ) const;
		bool GODWINDOW_API KeyUp ( UCHAR key ) const;

		friend LRESULT WINAPI WinProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam );

	private:
		HWND	m_handle		{ nullptr };
		MSG		m_message;
		int		m_window_width	{ 0 };
		int		m_window_height { 0 };
		bool	m_resized		{ false };
		bool	m_minimised		{ false };
		bool	m_fullscreen	{ false };
		bool	m_to_close		{ false };

		Input	m_input;

		bool CreateWindowClass ( HINSTANCE hInstance , WNDPROC wndProc ) noexcept;
		bool CreateSystemWindow ( HINSTANCE hInstance , HWND& hWnd , bool isFullscreen , int width , int height );
	};
}