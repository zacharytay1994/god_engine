#pragma once
#pragma comment (lib, "vulkan-1.lib")
#include "../../godVulkanEx.h"

#include "vk_swapchain.h"

#include <vulkan/vulkan.h>
#include <glm/glm/glm.hpp>

#include <iostream>

namespace VK_OBJECT
{
	struct GraphicsPipeline
	{
		VkPipeline				m_pipeline { VK_NULL_HANDLE };
		VkPipelineLayout		m_pipeline_layout { VK_NULL_HANDLE };

		GraphicsPipeline () = default;
		GODVULKANEX_API GraphicsPipeline (
			VkPipelineLayout pipelineLayout ,
			VkDevice vkLogicalDevice ,
			VkExtent2D extent ,
			VkRenderPass renderpass ,
			char const* vertexSPV ,
			char const* fragmentSPV
		);

		void DestroyPipeline ();
		bool Create (
			VkDevice vkLogicalDevice ,
			VkExtent2D extent ,
			VkRenderPass renderpass );

		template <typename PushConstant>
		static VkPipelineLayout CreatePipelineLayout ( VkDevice vkLogicalDevice, VkDescriptorSetLayout descriptorSetLayout )
		{
			VkPipelineLayoutCreateInfo pipeline_layout_info {};
			pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipeline_layout_info.setLayoutCount = 1;
			pipeline_layout_info.pSetLayouts = &descriptorSetLayout;

			// push constants here - to be changed
			VkPushConstantRange push_constant;
			push_constant.offset = 0;
			push_constant.size = sizeof ( PushConstant );
			push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			pipeline_layout_info.pushConstantRangeCount = 1;
			pipeline_layout_info.pPushConstantRanges = &push_constant;

			VkPipelineLayout pipeline_layout;
			if ( vkCreatePipelineLayout ( vkLogicalDevice , &pipeline_layout_info , nullptr , &pipeline_layout ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] VK_OBJECT::GraphicsPipeline::CreatePipelineLayout - Failed to create pipeline layout." << std::endl;
				return VK_NULL_HANDLE;
			}

			std::cerr << "[SUCCESS] VK_OBJECT::GraphicsPipeline::CreatePipelineLayout - Created successfully." << std::endl;
			return pipeline_layout;
		}

	private:
		VkDevice m_logical_device { VK_NULL_HANDLE };
		std::string m_vertex_spv { "" };
		std::string m_fragment_spv { "" };
	};
}