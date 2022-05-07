#include "vk_config.h"

#include <vulkan/vulkan.h>

namespace VK_OBJECT
{
	Config::Config ( bool validation , bool renderdoc )
		:
		m_validation ( validation ) ,
		m_renderdoc ( renderdoc )
	{
		// define validation layers
		if ( m_validation )
		{
			m_validation_layers.emplace_back ( "VK_LAYER_KHRONOS_validation" );
		}
		if ( m_renderdoc )
		{
			m_validation_layers.emplace_back ( "VK_LAYER_RENDERDOC_Capture" );
		}

		// define instance extensions
		m_instance_extensions.emplace_back ( "VK_KHR_surface" );
		m_instance_extensions.emplace_back ( "VK_KHR_win32_surface" );
		if ( m_validation )
		{
			m_instance_extensions.emplace_back ( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
		}

		// define device extensions
		m_device_extensions.emplace_back ( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
	}
	bool Config::Validation () const
	{
		return m_validation;
	}
	bool Config::Renderdoc () const
	{
		return m_renderdoc;
	}
	ConfigInfo const& Config::GetValidationLayers () const
	{
		return m_validation_layers;
	}
	ConfigInfo const& Config::GetInstanceExtensions () const
	{
		return m_instance_extensions;
	}
	ConfigInfo const& Config::GetDeviceExtensions () const
	{
		return m_device_extensions;
	}
}