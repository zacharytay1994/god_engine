#pragma once

#include "../../godVulkanEx.h"

#define VK_USE_PLATFORM_WIN32_KHR 1
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace VK_HELPER
{
	static constexpr int MAX_FRAMES_IN_FLIGHT { 3 };

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities_;
		std::vector<VkSurfaceFormatKHR> formats_;
		std::vector<VkPresentModeKHR> present_modes_;
	};

	namespace GET
	{
		/*!
		 * @brief object that checks if all queue families are ready
		*/
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family_;
			std::optional<uint32_t> present_family_;

			bool IsComplete ()
			{
				return graphics_family_.has_value ()
					&& present_family_.has_value ();
			}
		};

		QueueFamilyIndices GODVULKANEX_API QueueFamilies (
			VkPhysicalDevice	physicalDevice ,
			VkSurfaceKHR		surface
		);

		SwapChainSupportDetails	SwapChainSupportDetails_f (
			VkPhysicalDevice physicalDevice ,
			VkSurfaceKHR surface
		);

		VkFormat SupportedFormat ( VkPhysicalDevice vkPhysicalDevice , std::vector<VkFormat> const& candidates , VkImageTiling tiling , VkFormatFeatureFlags features );

		VkFormat DepthFormat ( VkPhysicalDevice vkPhysicalDevice );

		uint32_t MemoryType (
			VkPhysicalDevice		physicalDevice ,
			uint32_t				typeFilter ,
			VkMemoryPropertyFlags	properties
		);
	}

	namespace CHECK
	{
		/*!
		 * @brief checks vkLayersSupport
		*/
		bool VulkanLayersSupport (
			std::vector<char const*> requestedLayers
		);

		bool CompareExtensionsList (
			std::vector<char const*> requestedExtensions ,
			std::vector<VkExtensionProperties> availableExtensions ,
			char const* name
		);

		/*!
		 * @brief checks InstanceExtensionsSupport
		*/
		bool InstanceExtensionsSupport (
			std::vector<char const*> const& extensions
		);

		/*!
		 * @brief checks SwapChainSupport
		*/
		bool SwapChainSupport (
			VkPhysicalDevice		physicalDevice ,
			VkSurfaceKHR			surface
		);

		bool HasStencilComponent (
			VkFormat				format
		);
	}

	namespace DEBUG
	{
		/*!
		 * @brief debug callback
		*/
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback (
			VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity ,
			VkDebugUtilsMessageTypeFlagsEXT				messageType ,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData ,
			void* pUserData
		);

		/*!
		 * @brief populates debug messenger create info
		*/
		void	PopulateDebugMessengerCreateInfo (
			VkDebugUtilsMessengerCreateInfoEXT& createInfo
		);

		/*!
		 * @brief creates debug utils messenger
		*/
		VkResult CreateDebugUtilsMessengerEXT (
			VkInstance									instance ,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo ,
			const VkAllocationCallbacks* pAllocator ,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		);

		/*!
		 * @brief destroys debug messenger
		*/
		void	DestroyDebugUtilsMessengerEXT (
			VkInstance									instance ,
			VkDebugUtilsMessengerEXT					debugMessenger ,
			const VkAllocationCallbacks* pAllocator
		);
	}
}