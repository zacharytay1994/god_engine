#include "pch.h"
#include "Window.h"

namespace god
{
	// Custom windows procedure
	LRESULT WINAPI WinProc ( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam )
	{
		Window* window { nullptr };
		window = ( Window* ) GetWindowLongPtr ( hwnd , GWLP_USERDATA );

		switch ( msg )
		{
		case WM_CLOSE:
			window->m_to_close = true;
			return 0;
		case WM_DESTROY:
			PostQuitMessage ( 88888 );
			return 0;
			// keyboard input
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			window->m_input.SetKeyDown ( wParam );
			return 0;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			window->m_input.SetKeyUp ( wParam );
			return 0;
			// mouse input
		case WM_MOUSEMOVE:
			window->m_input.FillMousePosition ( lParam );
			return 0;
		case WM_INPUT:
			window->m_input.FillMouseRawPosition ( lParam );
			return 0;
		case WM_LBUTTONDOWN:
			window->m_input.m_mouse_left_down = true;
			window->m_input.m_mouse_left_pressed = true;
			window->m_input.FillMousePosition ( lParam );
			return 0;
		case WM_LBUTTONUP:
			window->m_input.m_mouse_left_down = false;
			window->m_input.m_mouse_left_up = true;
			window->m_input.FillMousePosition ( lParam );
			return 0;
		case WM_MBUTTONDOWN:
			window->m_input.m_mouse_middle_down = true;
			window->m_input.m_mouse_middle_pressed = true;
			window->m_input.FillMousePosition ( lParam );
			return 0;
		case WM_MBUTTONUP:
			window->m_input.m_mouse_middle_down = false;
			window->m_input.m_mouse_middle_up = true;
			window->m_input.FillMousePosition ( lParam );
			return 0;
		case WM_RBUTTONDOWN:
			window->m_input.m_mouse_right_down = true;
			window->m_input.m_mouse_right_pressed = true;
			window->m_input.FillMousePosition ( lParam );
			return 0;
		case WM_RBUTTONUP:
			window->m_input.m_mouse_right_down = false;
			window->m_input.m_mouse_right_up = true;
			window->m_input.FillMousePosition ( lParam );
			return 0;
			// update width and height of client area upon resize
		case WM_SIZE:
			if ( window )
			{
				window->m_resized = true;
				window->m_window_width = LOWORD ( lParam );
				window->m_window_height = HIWORD ( lParam );
			}
			return 0;
		}

		return DefWindowProc ( hwnd , msg , wParam , lParam );
	}

	Window::Window ( uint32_t width , uint32_t height , bool fullscreen )
		:
		m_fullscreen ( fullscreen )
	{
		const auto hInstance = GetModuleHandle ( nullptr );

		if ( !CreateWindowClass ( hInstance , WinProc ) )
		{
			return;
		}

		if ( !CreateSystemWindow ( hInstance , m_handle , m_fullscreen , width , height ) )
		{
			return;
		}

		if ( m_handle )
		{
			SetWindowLongPtr ( m_handle , GWLP_USERDATA , reinterpret_cast< LONG_PTR >( this ) );
		}

		m_resized = false;

		std::cout << "godWindow constructed." << std::endl;
	}

	bool Window::WindowShouldClose ()
	{
		return m_to_close;
	}

	void Window::PollEvents ()
	{
		// reset window states
		m_input.EndFrame ();
		m_resized = false;

		// process messages
		if ( PeekMessage ( &m_message , NULL , 0 , 0 , PM_REMOVE ) )
		{
			TranslateMessage ( &m_message );
			DispatchMessage ( &m_message );
		}
	}

	HWND Window::GetWindowHandle ()
	{
		return m_handle;
	}

	uint32_t Window::GetWindowWidth ()
	{
		return m_window_width;
	}

	uint32_t Window::GetWindowHeight ()
	{
		return m_window_height;
	}

	bool Window::Resized ()
	{
		return m_resized;
	}

	bool Window::KeyIsDown ( UCHAR key ) const
	{
		return m_input.KeyIsDown ( key );
	}

	bool Window::KeyWasPressed ( UCHAR key ) const
	{
		return m_input.KeyWasPressed ( key );
	}

	bool Window::KeyUp ( UCHAR key ) const
	{
		return m_input.KeyUp ( key );
	}

	int Window::MouseX ()
	{
		return m_input.m_mouse_x;
	}

	int Window::MouseY ()
	{
		return m_input.m_mouse_y;
	}

	bool Window::MouseLDown ()
	{
		return m_input.m_mouse_left_down;
	}

	bool Window::MouseLPressed ()
	{
		return m_input.m_mouse_left_pressed;
	}

	bool Window::MouseLUp ()
	{
		return m_input.m_mouse_left_up;
	}

	bool Window::MouseMDown ()
	{
		return m_input.m_mouse_middle_down;
	}

	bool Window::MouseMPressed ()
	{
		return m_input.m_mouse_middle_pressed;
	}

	bool Window::MouseMUp ()
	{
		return m_input.m_mouse_middle_up;
	}
	bool Window::MouseRDown ()
	{
		return m_input.m_mouse_right_down;
	}
	bool Window::MouseRPressed ()
	{
		return m_input.m_mouse_right_pressed;
	}
	bool Window::MouseRUp ()
	{
		return m_input.m_mouse_right_up;
	}

	bool Window::CreateWindowClass ( HINSTANCE hInstance , WNDPROC wndProc ) noexcept
	{
		// check if already registered the class
		{
			WNDCLASSA C {};
			if ( GetClassInfoA ( hInstance , "VULKANCLASS" , &C ) )
			{
				std::cerr << "godWindow, CreateWindowClass - Window class already exists." << std::endl;
				return false;
			}
		}

		// define window class
		WNDCLASSEX wnd_class;
		wnd_class.cbSize = sizeof ( WNDCLASSEX );
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		wnd_class.lpfnWndProc = wndProc;
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hInstance = hInstance;
		wnd_class.hIcon = LoadIcon ( NULL , IDI_APPLICATION );
		wnd_class.hCursor = LoadCursor ( NULL , IDC_ARROW );
		wnd_class.hbrBackground = static_cast< HBRUSH >( GetStockObject ( WHITE_BRUSH ) );
		wnd_class.lpszMenuName = nullptr;
		wnd_class.lpszClassName = TEXT ( "VULKANCLASS" );
		wnd_class.hIconSm = LoadIcon ( NULL , IDI_WINLOGO );

		// register the window class and check if its successful
		if ( !RegisterClassEx ( &wnd_class ) )
		{
			std::cerr << "godWindow, CreateWindowClass - Window class not registered." << std::endl;
			return false;
		}

		return true;
	}

	bool Window::CreateSystemWindow ( HINSTANCE hInstance , HWND& hWnd , bool isFullscreen , int width , int height )
	{
		// get resolution 
		const int screen_width = GetSystemMetrics ( SM_CXSCREEN );
		const int screen_height = GetSystemMetrics ( SM_CYSCREEN );

		m_window_width = width;
		m_window_height = height;

		if ( isFullscreen )
		{
			DEVMODE dm_screen_settings {};
			memset ( &dm_screen_settings , 0 , sizeof ( dm_screen_settings ) );

			dm_screen_settings.dmSize = sizeof ( dm_screen_settings );
			dm_screen_settings.dmPelsWidth = screen_width;
			dm_screen_settings.dmPelsHeight = screen_height;
			dm_screen_settings.dmBitsPerPel = 32;
			dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// if current resolution not fullscreen, make it fullscreen
			if ( ( width != screen_width ) && ( height != screen_height ) )
			{
				if ( ChangeDisplaySettings ( &dm_screen_settings , CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
				{
					std::cerr << "godWindow, CreateWindowClass - Display settings not changed successfully." << std::endl;
					return false;
				}
			}
		}

		// compute window flags
		const DWORD dw_ex_stye = isFullscreen
			? WS_EX_APPWINDOW
			: WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		const DWORD dw_style = isFullscreen
			? WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			: WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		// window rectangle
		RECT window_rect;
		if ( isFullscreen )
		{
			window_rect.left = static_cast< long >( 0 );
			window_rect.right = static_cast< long >( screen_width );
			window_rect.top = static_cast< long >( 0 );
			window_rect.bottom = static_cast< long >( screen_height );
		}
		else
		{
			window_rect.left = static_cast< long >( screen_width ) / 2 - width / 2;
			window_rect.right = static_cast< long >( width );
			window_rect.top = static_cast< long >( screen_height ) / 2 - height / 2;
			window_rect.bottom = static_cast< long >( height );
		}

		if ( !AdjustWindowRectEx ( &window_rect , dw_style , FALSE , dw_ex_stye ) )
		{
			std::cerr << "Failed to adjust window rect." << std::endl;
		}

		// create window
		hWnd = CreateWindowEx
		(
			0
			, TEXT ( "VULKANCLASS" )
			, TEXT ( "VULKANWINDOW" )
			, dw_style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			, window_rect.left
			, window_rect.top
			, window_rect.right
			, window_rect.bottom
			, nullptr
			, nullptr
			, hInstance
			, nullptr
		);

		// get window width and height of client area
		RECT client_rect;
		GetClientRect ( hWnd , &client_rect );
		m_window_width = client_rect.right;
		m_window_height = client_rect.bottom;

		// check if window successfully created
		if ( !hWnd )
		{
			std::cerr << "godWindow, CreateWindowClass - Failed to create system window." << std::endl;
			return false;
		}

		ShowWindow ( hWnd , SW_SHOW );
		SetForegroundWindow ( hWnd );
		SetFocus ( hWnd );

		return true;
	}
}