#include "vk_helper.h"

#include <iostream>
#include <set>

namespace VK_HELPER
{
	namespace GET
	{
		QueueFamilyIndices QueueFamilies ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface )
		{
			QueueFamilyIndices indices;

			// get all device queue families
			uint32_t qfp_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties ( physicalDevice , &qfp_count , nullptr );
			std::vector<VkQueueFamilyProperties> queue_families_properties ( qfp_count );
			vkGetPhysicalDeviceQueueFamilyProperties ( physicalDevice , &qfp_count , queue_families_properties.data () );

			// store them in self made queue family struct, i.e. QueueFamilyIndices
			// graphics and present family share the same index
			int i = 0;
			for ( const auto& qfp : queue_families_properties )
			{
				// look for graphics bit
				if ( qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT )
				{
					indices.graphics_family_ = i;
				}

				// look for present support
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR ( physicalDevice , i , surface , &presentSupport );
				if ( presentSupport )
				{
					indices.present_family_ = i;
				}

				// if all families filled, early exit from queue
				if ( indices.IsComplete () )
				{
					break;
				}
				++i;
			}

			return indices;
		}

		SwapChainSupportDetails SwapChainSupportDetails_f ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface )
		{
			SwapChainSupportDetails details;

			// check surface capabilities
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR ( physicalDevice , surface , &details.capabilities_ );

			// check surface formats
			uint32_t format_count;
			vkGetPhysicalDeviceSurfaceFormatsKHR ( physicalDevice , surface , &format_count , nullptr );
			if ( format_count != 0 )
			{
				details.formats_.resize ( format_count );
				vkGetPhysicalDeviceSurfaceFormatsKHR ( physicalDevice , surface , &format_count , details.formats_.data () );
			}

			// check surface present modes
			uint32_t present_modes_count;
			vkGetPhysicalDeviceSurfacePresentModesKHR ( physicalDevice , surface , &present_modes_count , nullptr );
			if ( present_modes_count != 0 )
			{
				details.present_modes_.resize ( present_modes_count );
				vkGetPhysicalDeviceSurfacePresentModesKHR ( physicalDevice , surface , &present_modes_count , details.present_modes_.data () );
			}

			return details;
		}

		VkFormat SupportedFormat ( VkPhysicalDevice vkPhysicalDevice , std::vector<VkFormat> const& candidates , VkImageTiling tiling , VkFormatFeatureFlags features )
		{
			for ( VkFormat format : candidates )
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties ( vkPhysicalDevice , format , &props );

				if ( tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features )
				{
					return format;
				}
				else if ( tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features )
				{
					return format;
				}
			}
			throw std::runtime_error ( "## VK_HELPER::GET::SupportedFormat - Failed to find supported format!" );
		}

		VkFormat DepthFormat ( VkPhysicalDevice vkPhysicalDevice )
		{
			return SupportedFormat ( vkPhysicalDevice ,
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT } ,
				VK_IMAGE_TILING_OPTIMAL ,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		uint32_t MemoryType ( VkPhysicalDevice physicalDevice , uint32_t typeFilter , VkMemoryPropertyFlags properties )
		{
			VkPhysicalDeviceMemoryProperties memory_properties;
			vkGetPhysicalDeviceMemoryProperties ( physicalDevice , &memory_properties );
			for ( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i )
			{
				if ( typeFilter & ( 1 << i ) &&
					( memory_properties.memoryTypes[ i ].propertyFlags & properties ) == properties )
				{
					return i;
				}
			}
			return static_cast< uint32_t >( -1 );
		}
	}

	namespace CHECK
	{
		bool VulkanLayersSupport ( std::vector<char const*> requestedLayers )
		{
			std::cout << "### Requested Layers:" << std::endl;
			for ( const auto& layer : requestedLayers )
			{
				std::cout << "\t- " << layer << std::endl;
			}

			// get available layers
			uint32_t layer_count;
			vkEnumerateInstanceLayerProperties ( &layer_count , nullptr );
			std::vector<VkLayerProperties> available_layers ( layer_count );
			vkEnumerateInstanceLayerProperties ( &layer_count , available_layers.data () );

			std::cout << "### Available Layers:" << std::endl;;
			for ( const auto& layer : available_layers )
			{
				std::cout << "\t- " << layer.layerName << std::endl;
			}

			for ( const auto& requested_layer : requestedLayers )
			{
				bool layer_found = false;
				for ( const auto& layerProperty : available_layers )
				{
					if ( !strcmp ( requested_layer , layerProperty.layerName ) )
					{
						layer_found = true;
					}
				}
				if ( !layer_found )
				{
					std::cerr << "### vkHelper::Check::vkLayersSupported failed! Requested layer not supported." << std::endl;
					std::cerr << "###\t- " << requested_layer << std::endl;
					return false;
				}
			}

			std::cout << "### All requested layers supported!" << std::endl;
			return true;
		}

		bool CompareExtensionsList ( std::vector<char const*> requestedExtensions , std::vector<VkExtensionProperties> availableExtensions , char const* name )
		{
			std::set<std::string> required_extensions ( requestedExtensions.begin () , requestedExtensions.end () );

			// print out requested extensions
			std::cout << "### Requested " << name << " extensions:" << std::endl;
			for ( const auto& extension : required_extensions )
			{
				std::cout << "\t- " << extension << std::endl;
			}

			// print out available extensions
			std::cout << "### Available " << name << " instance extensions:" << std::endl;
			for ( const auto& extension : availableExtensions )
			{
				std::cout << "\t- " << extension.extensionName << std::endl;
			}

			// check extensions
			for ( auto const& extension : availableExtensions )
			{
				required_extensions.erase ( extension.extensionName );
			}
			if ( !required_extensions.empty () )
			{
				std::cerr << "### vkHelper::CheckCompareExtensionsList Failed! Requested " << name << " extensions not supported." << std::endl;
				for ( auto const& extension : required_extensions )
				{
					std::cerr << "\t- " << extension << std::endl;
				}
				return false;
			}
			std::cout << "### All requested " << name << " extensions supported." << std::endl;
			return true;
		}

		bool InstanceExtensionsSupport ( std::vector<char const*> const& extensions )
		{
			// get available extensions
			uint32_t extension_count { 0 };
			//  - get number of supported extensions
			vkEnumerateInstanceExtensionProperties ( nullptr , &extension_count , nullptr );
			//  - allocate container for storing the extensions
			std::vector<VkExtensionProperties> available_extensions ( extension_count );
			//  - query supported extensions details
			vkEnumerateInstanceExtensionProperties ( nullptr , &extension_count , available_extensions.data () );

			return CompareExtensionsList ( extensions , available_extensions , "instance" );
		}

		bool SwapChainSupport ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface )
		{
			SwapChainSupportDetails details = GET::SwapChainSupportDetails_f ( physicalDevice , surface );
			return !details.formats_.empty () && !details.present_modes_.empty ();
			return false;
		}

		bool HasStencilComponent ( VkFormat format )
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}
	}

	namespace DEBUG
	{
		#define VK_ALL_LAYER_MESSAGES
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback ( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity , VkDebugUtilsMessageTypeFlagsEXT messageType , const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData , void* pUserData )
		{
			UNREFERENCED_PARAMETER ( pUserData );
			UNREFERENCED_PARAMETER ( messageType );
			if ( messageSeverity & VK_DEBUG_REPORT_ERROR_BIT_EXT )
			{
				std::cerr << "[ERROR]\n"
					<< "\t[CODE: " << pCallbackData->messageIdNumber << "]\n"
					<< "\t[MESSAGE: " << pCallbackData->pMessage << "]" << std::endl;
			}
			else if ( messageSeverity & VK_DEBUG_REPORT_WARNING_BIT_EXT )
			{
				std::cerr << "[WARNING]\n"
					<< "\t[CODE: " << pCallbackData->messageIdNumber << "]\n"
					<< "\t[MESSAGE: " << pCallbackData->pMessage << "]" << std::endl;
			}
			else
			{
#ifdef VK_ALL_LAYER_MESSAGES
				std::cerr << "[INFO]\n"
					<< "\t[CODE: " << pCallbackData->messageIdNumber << "]\n"
					<< "\t[MESSAGE: " << pCallbackData->pMessage << "]" << std::endl;
#endif
			}
			// should always return false, i.e. not abort function call that triggered this callback
			return VK_FALSE;
		}

		void PopulateDebugMessengerCreateInfo ( VkDebugUtilsMessengerCreateInfoEXT& createInfo )
		{
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			// flag possible problems

			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			// debug call back function defined above 

			createInfo.pfnUserCallback = DebugCallback;

			createInfo.pUserData = nullptr;
		}

		VkResult CreateDebugUtilsMessengerEXT ( VkInstance instance , const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo , const VkAllocationCallbacks* pAllocator , VkDebugUtilsMessengerEXT* pDebugMessenger )
		{
			auto func = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr ( instance , "vkCreateDebugUtilsMessengerEXT" );
			if ( func != nullptr )
			{
				return func ( instance , pCreateInfo , pAllocator , pDebugMessenger );
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void DestroyDebugUtilsMessengerEXT ( VkInstance instance , VkDebugUtilsMessengerEXT debugMessenger , const VkAllocationCallbacks* pAllocator )
		{
			auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr ( instance , "vkDestroyDebugUtilsMessengerEXT" );
			if ( func != nullptr )
			{
				func ( instance , debugMessenger , pAllocator );
			}
			else
			{
				std::cerr << "### vkHelper::Debug::DestroyUtilsMessengerEXT failed! vkDestroyDebugUtilsMessengerEXT func not loaded!" << std::endl;
			}
		}
	}
}