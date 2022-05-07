#pragma once

#include "vk_config.h"

#include <vulkan/vulkan.h>

namespace VK_OBJECT
{
	struct Instance
	{
		ConfigRef const				m_config;
		VkInstance					m_instance { VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT	m_debug_messenger { VK_NULL_HANDLE };

		Instance (
			char const* appName ,
			ConfigRef const& config
		);

	private:
		bool m_success { false };

		bool CreateInstance (
			char const* appName
		);

		bool CreateDebugMessenger ();
	};
}