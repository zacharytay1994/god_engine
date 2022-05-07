#include "vk_vulkan_instance.h"
#include "vk_helper.h"

#include <iostream>

namespace VK_OBJECT
{
	Instance::Instance (
		char const* appName ,
		ConfigRef const& config )
		:
		m_config ( config )
	{
		// create instance
		if ( !CreateInstance ( appName ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Instance - Failed to create vulkan instance!" << std::endl;
			return;
		}

		// create debug messenger
		if ( m_config->Validation () )
		{
			if ( !CreateDebugMessenger () )
			{
				std::cerr << "[ERROR!] VK_OBJECT::Instance - Failed to create debug messenger!" << std::endl;
				return;
			}
		}

		m_success = true;
		std::cout << "[SUCCESS] VK_OBJECT::Instance - Created successfully." << std::endl;
	}

	bool Instance::CreateInstance (
		char const* appName )
	{
		// app info
		VkApplicationInfo app_info {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = appName;
		app_info.applicationVersion = VK_MAKE_VERSION ( 1 , 0 , 0 );
		app_info.pEngineName = "Engine";
		app_info.engineVersion = VK_MAKE_VERSION ( 1 , 0 , 0 );
		app_info.apiVersion = VK_API_VERSION_1_0;

		// create info for app info
		VkInstanceCreateInfo create_info {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		// validation layer settings
		create_info.enabledLayerCount = static_cast< uint32_t >( m_config->GetValidationLayers ().size () );
		create_info.ppEnabledLayerNames = m_config->GetValidationLayers ().size () > 0 ? m_config->GetValidationLayers ().data () : nullptr;

		create_info.enabledExtensionCount = static_cast< uint32_t >( m_config->GetInstanceExtensions ().size () );
		create_info.ppEnabledExtensionNames = m_config->GetInstanceExtensions ().size () > 0 ? m_config->GetInstanceExtensions ().data () : nullptr;

		create_info.pNext = nullptr;

		if ( m_config->Validation () )
		{
			// set debug info
			VkDebugUtilsMessengerCreateInfoEXT debug_create_info {};
			VK_HELPER::DEBUG::PopulateDebugMessengerCreateInfo ( debug_create_info );
			create_info.pNext = ( VkDebugUtilsMessengerCreateInfoEXT* ) &debug_create_info;
		}

		if ( vkCreateInstance ( &create_info , nullptr , &m_instance ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Instance::Create - Failed to create VkInstance!" << std::endl;
			return false;
		}

		return true;
	}

	bool Instance::CreateDebugMessenger ()
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info {};
		VK_HELPER::DEBUG::PopulateDebugMessengerCreateInfo ( debug_create_info );

		if ( VK_HELPER::DEBUG::CreateDebugUtilsMessengerEXT ( m_instance , &debug_create_info , nullptr , &m_debug_messenger ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Instance::Create - Failed to set up debug messenger!" << std::endl;
			return false;
		}

		return true;
	}
}