#include "vk_descriptor.h"
#include "vk_helper.h"

#include <stdexcept>
#include <array>

namespace VK_OBJECT
{
	Descriptor::Descriptor ( Devices const& devices )
		:
		m_logical_device { devices.m_logical_device }
	{
		// create descriptor set layout
		CreateDescriptorSetLayout ( devices );
		CreateDescriptorPool ( devices );
		CreateDescriptorSets ( devices );
	}

	Descriptor::~Descriptor ()
	{
		Destroy ();
		vkDestroyDescriptorSetLayout ( m_logical_device , m_descriptor_set_layout , nullptr );
	}

	void Descriptor::Destroy ()
	{
		vkDestroyDescriptorPool ( m_logical_device , m_descriptor_pool , nullptr );
	}

	void Descriptor::Create ( Devices const& devices )
	{
		// create descriptor set layout
		CreateDescriptorPool ( devices );
		CreateDescriptorSets ( devices );
	}

	void Descriptor::UpdateDescriptorSetWithSampler ( size_t index , 
		VkImageView diffuseImageView , VkSampler diffuseSampler , 
		VkImageView normalImageView , VkSampler normalSampler )
	{
		VkDescriptorImageInfo diffuse_imageInfo {};
		diffuse_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		diffuse_imageInfo.imageView = diffuseImageView;
		diffuse_imageInfo.sampler = diffuseSampler;

		std::array<VkWriteDescriptorSet , 2> descriptor_write {};
		descriptor_write[ 0 ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write[ 0 ].dstSet = m_descriptor_sets[ index ];
		descriptor_write[ 0 ].dstBinding = 0;
		descriptor_write[ 0 ].dstArrayElement = 0;
		descriptor_write[ 0 ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write[ 0 ].descriptorCount = 1;
		descriptor_write[ 0 ].pImageInfo = &diffuse_imageInfo;

		VkDescriptorImageInfo normal_imageInfo {};
		normal_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		normal_imageInfo.imageView = normalImageView;
		normal_imageInfo.sampler = normalSampler;

		descriptor_write[ 1 ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write[ 1 ].dstSet = m_descriptor_sets[ index ];
		descriptor_write[ 1 ].dstBinding = 1;
		descriptor_write[ 1 ].dstArrayElement = 0;
		descriptor_write[ 1 ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write[ 1 ].descriptorCount = 1;
		descriptor_write[ 1 ].pImageInfo = &normal_imageInfo;

		vkUpdateDescriptorSets ( m_logical_device , static_cast< uint32_t >( descriptor_write.size () ) , descriptor_write.data () , 0 , nullptr );
	}

	void Descriptor::CreateDescriptorSetLayout ( Devices const& devices )
	{
		std::array<VkDescriptorSetLayoutBinding , 2> samplerLayoutBinding {};
		samplerLayoutBinding[ 0 ].binding = 0;
		samplerLayoutBinding[ 0 ].descriptorCount = 1;
		samplerLayoutBinding[ 0 ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding[ 0 ].pImmutableSamplers = nullptr;
		samplerLayoutBinding[ 0 ].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding[ 0 ].pImmutableSamplers = nullptr; // Optional

		samplerLayoutBinding[ 1 ].binding = 1;
		samplerLayoutBinding[ 1 ].descriptorCount = 1;
		samplerLayoutBinding[ 1 ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding[ 1 ].pImmutableSamplers = nullptr;
		samplerLayoutBinding[ 1 ].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding[ 1 ].pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast< uint32_t >( samplerLayoutBinding.size () );;
		layoutInfo.pBindings = samplerLayoutBinding.data ();

		if ( vkCreateDescriptorSetLayout ( devices.m_logical_device , &layoutInfo , nullptr , &m_descriptor_set_layout ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to create descriptor set layout!" );
		}
	}

	void Descriptor::CreateDescriptorPool ( Devices const& devices )
	{
		std::array<VkDescriptorPoolSize , 2> pool_size {};
		pool_size[ 0 ].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_size[ 0 ].descriptorCount = static_cast< uint32_t >( VK_HELPER::MAX_FRAMES_IN_FLIGHT );

		pool_size[ 1 ].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_size[ 1 ].descriptorCount = static_cast< uint32_t >( VK_HELPER::MAX_FRAMES_IN_FLIGHT );

		VkDescriptorPoolCreateInfo pool_info {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = static_cast< uint32_t >( pool_size.size () );
		pool_info.pPoolSizes = pool_size.data ();
		pool_info.maxSets = static_cast< uint32_t >( VK_HELPER::MAX_FRAMES_IN_FLIGHT );

		if ( vkCreateDescriptorPool ( devices.m_logical_device , &pool_info , nullptr , &m_descriptor_pool ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to create descriptor pool!" );
		}
	}

	void Descriptor::CreateDescriptorSets ( Devices const& devices )
	{
		std::vector<VkDescriptorSetLayout> layouts ( VK_HELPER::MAX_FRAMES_IN_FLIGHT , m_descriptor_set_layout );
		VkDescriptorSetAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptor_pool;
		allocInfo.descriptorSetCount = static_cast< uint32_t >( VK_HELPER::MAX_FRAMES_IN_FLIGHT );
		allocInfo.pSetLayouts = layouts.data ();

		m_descriptor_sets.resize ( VK_HELPER::MAX_FRAMES_IN_FLIGHT );
		if ( vkAllocateDescriptorSets ( devices.m_logical_device , &allocInfo , m_descriptor_sets.data () ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to allocate descriptor sets!" );
		}
	}
}