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
		GODWINDOW_API ~Window ();

		void GODWINDOW_API InitializeImGui ( HGLRC hglrc );
		void GODWINDOW_API TestImgui ();

		bool		GODWINDOW_API WindowShouldClose ();
		void		GODWINDOW_API PollEvents ();

		// getters & setters
		HWND		GODWINDOW_API GetWindowHandle ();
		uint32_t	GODWINDOW_API GetWindowWidth ();
		uint32_t	GODWINDOW_API GetWindowHeight ();
		bool		GODWINDOW_API Resized ();

		// keyboard input
		bool		GODWINDOW_API KeyIsDown ( UCHAR key ) const;
		bool		GODWINDOW_API KeyWasPressed ( UCHAR key ) const;
		bool		GODWINDOW_API KeyUp ( UCHAR key ) const;

		// mouse input
		int			GODWINDOW_API MouseX ();			// mouse coordinates
		int			GODWINDOW_API MouseY ();

		bool		GODWINDOW_API MouseLDown ();		// left mouse
		bool		GODWINDOW_API MouseLPressed ();
		bool		GODWINDOW_API MouseLUp ();

		bool		GODWINDOW_API MouseMDown ();		// middle mouse
		bool		GODWINDOW_API MouseMPressed ();
		bool		GODWINDOW_API MouseMUp ();

		bool		GODWINDOW_API MouseRDown ();		// right mouse
		bool		GODWINDOW_API MouseRPressed ();
		bool		GODWINDOW_API MouseRUp ();

		friend LRESULT WINAPI WinProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam );

	private:
		HWND	m_handle			{ nullptr };
		MSG		m_message;
		int		m_window_width		{ 0 };
		int		m_window_height		{ 0 };
		bool	m_resized			{ false };
		bool	m_minimised			{ false };
		bool	m_fullscreen		{ false };
		bool	m_to_close			{ false };

		Input	m_input;

		bool CreateWindowClass ( HINSTANCE hInstance , WNDPROC wndProc ) noexcept;
		bool CreateSystemWindow ( HINSTANCE hInstance , HWND& hWnd , bool isFullscreen , int width , int height );

		float m_test { 0.0f };
	};
}