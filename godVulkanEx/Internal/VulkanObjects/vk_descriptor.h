#pragma once
#include "vk_devices.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace VK_OBJECT
{
	struct Descriptor
	{
		VkDescriptorSetLayout			m_descriptor_set_layout { VK_NULL_HANDLE };
		VkDescriptorPool				m_descriptor_pool { VK_NULL_HANDLE };
		std::vector<VkDescriptorSet>	m_descriptor_sets { VK_NULL_HANDLE };

		Descriptor ( Devices const& devices );
		~Descriptor ();

		void Destroy ();
		void Create ( Devices const& devices );

		void UpdateDescriptorSetWithSampler ( size_t index ,
			VkImageView diffuseImageView , VkSampler diffuseSampler ,
			VkImageView normalImageView , VkSampler normalSampler );

	private:
		VkDevice m_logical_device { VK_NULL_HANDLE };

		void CreateDescriptorSetLayout ( Devices const& devices );

		void CreateDescriptorPool ( Devices const& devices );

		void CreateDescriptorSets ( Devices const& devices );
	};
}