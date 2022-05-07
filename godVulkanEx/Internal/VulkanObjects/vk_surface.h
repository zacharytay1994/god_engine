#pragma once

#define VK_USE_PLATFORM_WIN32_KHR 1
#include <vulkan/vulkan.h>
#include <Windows.h>

namespace VK_OBJECT
{
	struct Surface
	{
		VkSurfaceKHR m_surface { VK_NULL_HANDLE };

		Surface ( HWND hwnd , VkInstance vkInstance );

		bool Success ();

	private:
		bool m_success { false };
	};
}