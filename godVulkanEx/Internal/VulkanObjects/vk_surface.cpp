#include "vk_surface.h"

#include <iostream>

namespace VK_OBJECT
{
	Surface::Surface ( HWND hwnd , VkInstance vkInstance )
	{
		// get surface creation extension
		auto vkCreateWin32Surface = ( PFN_vkCreateWin32SurfaceKHR ) vkGetInstanceProcAddr ( vkInstance , "vkCreateWin32SurfaceKHR" );
		if ( vkCreateWin32Surface == nullptr )
		{
			std::cerr << "### vkHelper::Create::vkSurfaceWin32 failed! Surface creation extension not loaded!" << std::endl;
			return;
		}

		// surface create info
		VkWin32SurfaceCreateInfoKHR surface_create_info;
		surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_create_info.pNext = nullptr;
		surface_create_info.flags = 0;
		surface_create_info.hinstance = GetModuleHandle ( NULL );
		surface_create_info.hwnd = hwnd;

		// create the surface
		if ( vkCreateWin32Surface != nullptr )
		{
			if ( auto error = vkCreateWin32Surface ( vkInstance , &surface_create_info , nullptr , &m_surface ) )
			{
				std::cerr << "### vkHelper::Create::vkSurfaceWin32 failed! Failed to create win32 surface" << std::endl;
				return;
			}
		}

		m_success = true;
		std::cout << "[SUCCESS] VK_OBJECT::Surface - Created successfully." << std::endl;
	}

	bool Surface::Success ()
	{
		return m_success;
	}
}