#include "vk_devices.h"
#include "vk_helper.h"

#include <iostream>
#include <vector>
#include <set>

namespace VK_OBJECT
{
	Devices::Devices ( VkInstance vkInstance , VkSurfaceKHR vkSurface , ConfigRef const& config )
		:
		m_config ( config )
	{
		if ( !CreatePhysicalDevice ( vkInstance , vkSurface ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Devices - Failed to create physical device!" << std::endl;
			return;
		}
		std::cout << "[SUCCESS] VK_OBJECT::Devices - Physical device created successfully." << std::endl;

		if ( !CreateLogicalDevice ( vkSurface , config ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Devices - Failed to create logical device!" << std::endl;
			return;
		}
		std::cout << "[SUCCESS] VK_OBJECT::Devices - Logical device created successfully." << std::endl;

		m_success = true;
	}

	bool Devices::Success ()
	{
		return m_success;
	}

	bool Devices::CreatePhysicalDevice ( VkInstance vkInstance , VkSurfaceKHR vkSurface )
	{
		// pick a physical device
		uint32_t device_count { 0 };
		vkEnumeratePhysicalDevices ( vkInstance , &device_count , nullptr );
		if ( device_count == 0 )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Devices::CreatePhysicalDevice - Failed to find GPU with vulkan support." << std::endl;
			return false;
		}
		std::vector<VkPhysicalDevice> devices ( device_count );
		vkEnumeratePhysicalDevices ( vkInstance , &device_count , devices.data () );

		// print all devices
		std::cout << "### All physical devices:" << std::endl;
		for ( auto const& physical_device : devices )
		{
			VkPhysicalDeviceProperties device_properties;
			vkGetPhysicalDeviceProperties ( physical_device , &device_properties );
			std::cout << "\t- " << device_properties.deviceName << std::endl;
		}

		for ( auto const& physical_device : devices )
		{
			if ( PhysicalDeviceSuitable ( physical_device , vkSurface ) )
			{
				// device found
				std::cout << "### Suitable Device Found:" << std::endl;
				m_physical_device = physical_device;
				VkPhysicalDeviceProperties device_properties;
				vkGetPhysicalDeviceProperties ( m_physical_device , &device_properties );
				std::cout << "\t- " << device_properties.deviceName << std::endl;
				break;
			}
		}

		if ( m_physical_device == VK_NULL_HANDLE )
		{
			std::cout << "\t- " << "none" << std::endl;
			std::cerr << "[ERROR!] VK_OBJECT::Devices::CreatePhysicalDevice -  Failed to find a suitable GPU for selected operations." << std::endl;
			return false;
		}

		return true;
	}

	bool Devices::PhysicalDeviceSuitable ( VkPhysicalDevice vkPhysicalDevice , VkSurfaceKHR vkSurface )
	{
		VkPhysicalDeviceFeatures supported_features;
		vkGetPhysicalDeviceFeatures ( vkPhysicalDevice , &supported_features );

		return VK_HELPER::GET::QueueFamilies ( vkPhysicalDevice , vkSurface ).IsComplete () &&
			DeviceExtensionsSupported ( vkPhysicalDevice ) &&
			VK_HELPER::CHECK::SwapChainSupport ( vkPhysicalDevice , vkSurface ) &&
			supported_features.samplerAnisotropy;
	}

	bool Devices::DeviceExtensionsSupported ( VkPhysicalDevice vkPhysicalDevice )
	{
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties ( vkPhysicalDevice , &device_properties );
		std::cout << "### Checking device extensions of: " << device_properties.deviceName << std::endl;

		uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties ( vkPhysicalDevice , nullptr , &extension_count , nullptr );
		std::vector<VkExtensionProperties> available_extensions ( extension_count );
		vkEnumerateDeviceExtensionProperties ( vkPhysicalDevice , nullptr , &extension_count , available_extensions.data () );

		return VK_HELPER::CHECK::CompareExtensionsList ( m_config->GetDeviceExtensions () , available_extensions , "device" );
	}

	bool Devices::CreateLogicalDevice ( VkSurfaceKHR vkSurface , ConfigRef const& config )
	{
		VK_HELPER::GET::QueueFamilyIndices indices = VK_HELPER::GET::QueueFamilies ( m_physical_device , vkSurface );

		// create set of queue families to guarantee unique key
		std::set<uint32_t> unique_queue_families = { indices.graphics_family_.value (), indices.present_family_.value () };

		float queue_priority { 1.0f };

		// iterate over families and create queue info
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		for ( auto const& queue_family : unique_queue_families )
		{
			VkDeviceQueueCreateInfo queue_create_info {};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;

			queue_create_infos.push_back ( queue_create_info );
		}

		// device features for logical device
		VkPhysicalDeviceFeatures device_features {};
		device_features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo create_info {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = queue_create_infos.data ();
		create_info.queueCreateInfoCount = static_cast< uint32_t >( queue_create_infos.size () );
		create_info.pEnabledFeatures = &device_features;
		create_info.enabledExtensionCount = static_cast< uint32_t >( config->GetDeviceExtensions ().size () );
		create_info.ppEnabledExtensionNames = config->GetDeviceExtensions ().data ();

		if ( config->Validation () )
		{
			create_info.enabledLayerCount = static_cast< uint32_t >( config->GetValidationLayers ().size () );
			create_info.ppEnabledLayerNames = config->GetValidationLayers ().data ();
		}
		else
		{
			create_info.enabledLayerCount = 0;
		}

		if ( vkCreateDevice ( m_physical_device , &create_info , nullptr , &m_logical_device ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::Devices::CreateLogicalDevice - Failed to create a logical device." << std::endl;
			return false;
		}

		return true;
	}
}