#pragma once

#include "vk_config.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace VK_OBJECT
{
	struct Devices
	{
		ConfigRef const		m_config;
		VkPhysicalDevice	m_physical_device { VK_NULL_HANDLE };
		VkDevice			m_logical_device { VK_NULL_HANDLE };

		Devices ( VkInstance vkInstance , VkSurfaceKHR vkSurface , ConfigRef const& config );

		bool Success ();

	private:
		bool				m_success { false };

		/*
			PHYSICAL DEVICE CREATION
		*/

		bool CreatePhysicalDevice ( VkInstance vkInstance , VkSurfaceKHR vkSurface );

		bool PhysicalDeviceSuitable ( VkPhysicalDevice vkPhysicalDevice , VkSurfaceKHR vkSurface );

		bool DeviceExtensionsSupported ( VkPhysicalDevice vkPhysicalDevice );

		/*
			LOGICAL DEVICE CREATION
		*/

		bool CreateLogicalDevice ( VkSurfaceKHR vkSurface , ConfigRef const& config );
	};
}