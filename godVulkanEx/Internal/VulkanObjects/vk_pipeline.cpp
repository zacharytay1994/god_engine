#include "vk_pipeline.h"
#include "vk_vertex.h"
#include "vk_shader.h"

#include <array>
#include <assert.h>

namespace VK_OBJECT
{
	GraphicsPipeline::GraphicsPipeline (
		VkPipelineLayout pipelineLayout ,
		VkDevice vkLogicalDevice ,
		VkExtent2D extent ,
		VkRenderPass renderpass ,
		char const* vertexSPV ,
		char const* fragmentSPV )
		:
		m_logical_device ( vkLogicalDevice ) ,
		m_pipeline_layout ( pipelineLayout ) ,
		m_vertex_spv { vertexSPV } ,
		m_fragment_spv { fragmentSPV }
	{
		if ( !Create ( m_logical_device , extent , renderpass ) )
		{
			std::cerr << "[ERROR!] VK_OBJECT::GraphicsPipeline - Failed to create graphics pipeline!" << std::endl;
			return;
		}
	}
	void GraphicsPipeline::DestroyPipeline ()
	{
		assert ( m_logical_device != VK_NULL_HANDLE );
		vkDestroyPipeline ( m_logical_device , m_pipeline , nullptr );
	}

	bool GraphicsPipeline::Create (
		VkDevice vkLogicalDevice ,
		VkExtent2D extent ,
		VkRenderPass renderpass )
	{
		assert ( m_pipeline_layout != VK_NULL_HANDLE &&
			!m_vertex_spv.empty () &&
			!m_fragment_spv.empty () );

		// set shader
		Shader vertex_shader ( vkLogicalDevice , VK_OBJECT::SHADER_TYPE::VERTEX , m_vertex_spv.c_str () );
		Shader fragment_shader ( vkLogicalDevice , VK_OBJECT::SHADER_TYPE::FRAGMENT , m_fragment_spv.c_str () );
		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader.GetStageCreateInfo (), fragment_shader.GetStageCreateInfo () };

		// set vertex input info
		auto vertex_input_state_create_info_data = Vertex::GetInputStateCreateInfoData ();
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = vertex_input_state_create_info_data.Get ();

		// set fix function states
		VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// viewport
		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = ( float ) extent.width;
		viewport.height = ( float ) extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// scizzor rectangle
		VkRect2D scissor {};
		scissor.offset = { 0,0 };
		scissor.extent = extent;

		// combine viewport and scizzor rectangle into a viewport state
		VkPipelineViewportStateCreateInfo viewport_state {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		// rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		// multisampling
		VkPipelineMultisampleStateCreateInfo multisampling {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		// color blending
		VkPipelineColorBlendAttachmentState colour_blend_attachment {};
		colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		// non
		colour_blend_attachment.blendEnable = VK_FALSE;
		colour_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colour_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colour_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		colour_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colour_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colour_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// depth stenciling
		VkPipelineDepthStencilStateCreateInfo depth_stencil {};
		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_TRUE;
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

		// alpha blend
		/*colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/

		// color blend state
		VkPipelineColorBlendStateCreateInfo color_blending {};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &colour_blend_attachment;
		color_blending.blendConstants[ 0 ] = 0.0f;
		color_blending.blendConstants[ 1 ] = 0.0f;
		color_blending.blendConstants[ 2 ] = 0.0f;
		color_blending.blendConstants[ 3 ] = 0.0f;

		// setting dynamic states of the pipeline to modify it without recreating entire pipeline
		VkDynamicState dynamic_states[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamic_state {};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.dynamicStateCount = 2;
		dynamic_state.pDynamicStates = dynamic_states;

		VkGraphicsPipelineCreateInfo graphics_pipeline_create_info {};
		graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// shader info
		graphics_pipeline_create_info.stageCount = 2;
		graphics_pipeline_create_info.pStages = shader_stages;

		// vertex input info
		graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;

		// fixed function states
		graphics_pipeline_create_info.pInputAssemblyState = &inputAssembly;
		graphics_pipeline_create_info.pViewportState = &viewport_state;
		graphics_pipeline_create_info.pRasterizationState = &rasterizer;
		graphics_pipeline_create_info.pMultisampleState = &multisampling;
		graphics_pipeline_create_info.pDepthStencilState = &depth_stencil; // Optional
		graphics_pipeline_create_info.pColorBlendState = &color_blending;

		// create pipeline
		graphics_pipeline_create_info.layout = m_pipeline_layout;

		// bind render pass
		graphics_pipeline_create_info.renderPass = renderpass;
		graphics_pipeline_create_info.subpass = 0;

		// base pipeline to aid efficient creation of pipeline based on existing
		graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
		graphics_pipeline_create_info.basePipelineIndex = -1;

		if ( vkCreateGraphicsPipelines ( vkLogicalDevice , VK_NULL_HANDLE , 1 , &graphics_pipeline_create_info , nullptr , &m_pipeline ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] VK_OBJECT::GraphicsPipeline::Create - Failed to create graphics pipeline!" << std::endl;
			return false;
		}

		std::cout << "[SUCCESS] VK_OBJECT::GraphicsPipeline::Create - Created Successfully!" << std::endl;
		return true;
	}
}