#include "VulkanRenderer.h"
#include "VulkanObjects/vk_helper.h"

#include <iostream>
#include <assert.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

namespace god
{
	VulkanEx::VulkanEx (
		HWND const& hwnd ,
		uint32_t windowWidth ,
		uint32_t windowHeight ,
		VK_OBJECT::ConfigRef const& config )
		:
		m_instance ( "Vulkan Renderer" , config ) ,
		m_surface ( hwnd , m_instance.m_instance ) ,
		m_devices ( m_instance.m_instance , m_surface.m_surface , config ) ,
		m_swapchain ( windowWidth , windowHeight , m_devices.m_physical_device , m_devices.m_logical_device , m_surface.m_surface ) ,
		m_renderpass ( m_devices.m_physical_device , m_devices.m_logical_device , m_swapchain.m_format ) ,
		m_framebuffers ( m_devices.m_physical_device , m_devices.m_logical_device , m_swapchain , m_renderpass.m_render_pass ) ,
		m_commands ( m_devices , m_swapchain , m_renderpass , m_framebuffers , m_surface.m_surface ) ,
		m_descriptor ( m_devices )
	{
		// create graphics and present queue
		m_graphics_queue = GetDeviceQueue ();
		m_present_queue = GetDeviceQueue ();

		// initialize vulkan sync objects
		if ( !InitializeSyncObjects () )
		{
			std::cerr << "[ERROR!] RENDERER::VulkanEx - Failed to initialize sync objects!" << std::endl;
		}
	}

	VulkanEx::~VulkanEx ()
	{
		for ( auto& image : m_images )
		{
			image.second.Free ();
		}
	}

	void VulkanEx::BindGraphicsPipeline ( std::string const& name )
	{
		assert ( m_graphics_pipelines.find ( name ) != m_graphics_pipelines.end () );
		m_current_graphics_pipeline = name;

		std::cout << "[SUCCESS] RENDERER::BindGraphicsPipeline - " << name << " pipeline bound!" << std::endl;
	}

	void VulkanEx::AddModel ( std::string const& name , VK_OBJECT::Vertices const& vertices , VK_OBJECT::Indices const& indices )
	{
		assert ( m_models.find ( name ) == m_models.end () );
		m_models.insert ( { name, { m_devices, m_graphics_queue, m_commands.m_command_pool, vertices, indices} } );

		if ( m_models[ name ].Success () )
		{
			std::cout << "[SUCCESS] RENDERER::VulkanEx::AddModel - " << name << " model added successfully." << std::endl;
		}
		else
		{
			std::cout << "[ERROR!] RENDERER::VulkanEx::AddModel - " << name << " model adding failed!" << std::endl;
		}
	}

	void VulkanEx::AddImage ( std::string const& name , std::string const& png )
	{
		assert ( m_images.find ( name ) == m_images.end () );

		m_images.insert ( { name, { png , m_devices, m_commands, m_graphics_queue} } );
	}

	void VulkanEx::BindImage ( std::string const& name )
	{
	}

	void VulkanEx::Draw ( std::string const& model , float x , float y , float z , float rx , float ry , float rz , float sx , float sy , float sz )
	{
		assert ( m_models.find ( model ) != m_models.end () );

		// bind model
		m_models[ model ].Bind ( m_commands.m_command_buffers[ m_current_image_index ] );

		// draw model
		glm::mat4 m_model = glm::mat4 ( 1.0 );

		// translate
		m_model *= glm::translate ( glm::mat4 ( 1.0f ) , { x, y, z } );

		// rotate
		m_model *= glm::rotate ( glm::mat4 { 1.0f } , glm::radians ( rx ) , glm::vec3 ( 1 , 0 , 0 ) );
		m_model *= glm::rotate ( glm::mat4 { 1.0f } , glm::radians ( ry ) , glm::vec3 ( 0 , 1 , 0 ) );
		m_model *= glm::rotate ( glm::mat4 { 1.0f } , glm::radians ( rz ) , glm::vec3 ( 0 , 0 , 1 ) );

		// scale
		m_model *= glm::scale ( glm::mat4 ( 1.0f ) , { sx, sy, sz } );


		// concate transformation matrix
		glm::mat4 mesh_matrix = m_transformation_matrix * m_model;

		// set as push constant
		VK_OBJECT::Model::PushConstant constants;
		constants.model_matrix_ = m_model;
		constants.render_matrix_ = mesh_matrix;

		// draw command
		vkCmdPushConstants ( m_commands.m_command_buffers[ m_current_image_index ] , m_graphics_pipelines[ m_current_graphics_pipeline ].m_pipeline_layout , VK_SHADER_STAGE_VERTEX_BIT , 0 , sizeof ( VK_OBJECT::Model::PushConstant ) , &constants );
		vkCmdBindDescriptorSets ( m_commands.m_command_buffers[ m_current_image_index ] , VK_PIPELINE_BIND_POINT_GRAPHICS ,
			m_graphics_pipelines[ m_current_graphics_pipeline ].m_pipeline_layout , 0 , 1 , &m_descriptor.m_descriptor_sets[ m_current_image_index ] , 0 , nullptr );
		vkCmdDrawIndexed ( m_commands.m_command_buffers[ m_current_image_index ] , m_models[ model ].m_index_buffer.GetSize () , 1 , 0 , 0 , 0 );
	}

	void VulkanEx::StartFrame ()
	{
		// check if graphics pipeline exists
		if ( m_current_graphics_pipeline == "" )
		{
			return;
		}

		// wait for frame to be finished before drawing next frame
		vkWaitForFences ( m_devices.m_logical_device , 1 , &m_in_flight_fences[ m_current_frame ] , VK_TRUE , UINT64_MAX );

		VkResult result = vkAcquireNextImageKHR ( m_devices.m_logical_device , m_swapchain.m_swapchain , UINT64_MAX , m_available_semaphores[ m_current_frame ] , VK_NULL_HANDLE , &m_current_image_index );
		if ( result == VK_ERROR_OUT_OF_DATE_KHR )
		{
			RecreateSwapchain ();
			return;
		}
		else if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
		{
			throw std::runtime_error ( "failed to acquire swap chain image!" );
		}

		// update command buffer
		// check if the previous frame is using this image
		if ( m_images_in_flight[ m_current_image_index ] != VK_NULL_HANDLE )
		{
			vkWaitForFences ( m_devices.m_logical_device , 1 , &m_images_in_flight[ m_current_image_index ] , VK_TRUE , UINT64_MAX );
		}

		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if ( vkBeginCommandBuffer ( m_commands.m_command_buffers[ m_current_image_index ] , &beginInfo ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] RENDERER::VulkanEx::Draw - Failed to begin command buffer." << std::endl;
		}

		// assign render pass to command buffer and begin render pass
		VkRenderPassBeginInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderpass.m_render_pass;
		renderPassInfo.framebuffer = m_framebuffers.m_framebuffers[ m_current_image_index ];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_swapchain.m_extent;

		std::array<VkClearValue , 2> clearValues {};
		clearValues[ 0 ].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast< uint32_t >( clearValues.size () );
		renderPassInfo.pClearValues = clearValues.data ();

		// begin render pass
		vkCmdBeginRenderPass ( m_commands.m_command_buffers[ m_current_image_index ] , &renderPassInfo , VK_SUBPASS_CONTENTS_INLINE );

		// bind graphics pipeline
		vkCmdBindPipeline ( m_commands.m_command_buffers[ m_current_image_index ] , VK_PIPELINE_BIND_POINT_GRAPHICS , m_graphics_pipelines[ m_current_graphics_pipeline ].m_pipeline );

		// calculate transformation matrix
		glm::vec3 camPos = { 0.f,0.f,-5.f };

		// camera view
		glm::mat4 view = glm::translate ( glm::mat4 ( 1.f ) , camPos );

		//camera projection
		glm::mat4 projection = glm::perspective (
			glm::radians ( 45.0f ) ,
			static_cast< float >( m_swapchain.m_extent.width ) / static_cast< float >( m_swapchain.m_extent.height ) ,
			0.1f ,
			200.0f );
		projection[ 1 ][ 1 ] *= -1;

		m_transformation_matrix = projection * view;

		m_descriptor.UpdateDescriptorSetWithSampler ( m_current_image_index ,
			m_images[ "Diffuse" ].m_texture_image_view , m_images[ "Diffuse" ].m_texture_sampler ,
			m_images[ "Normal" ].m_texture_image_view , m_images[ "Normal" ].m_texture_sampler );
	}

	void VulkanEx::EndFrame ()
	{
		// end render pass
		vkCmdEndRenderPass ( m_commands.m_command_buffers[ m_current_image_index ] );

		// end command buffer
		if ( vkEndCommandBuffer ( m_commands.m_command_buffers[ m_current_image_index ] ) != VK_SUCCESS )
		{
			std::cerr << "[ERROR!] RENDERER::VulkanEx::Draw - Failed to end command buffer." << std::endl;
		}

		// end frame sync objects
		// mark image as now being used by this frame
		m_images_in_flight[ m_current_image_index ] = m_in_flight_fences[ m_current_frame ];

		// queue submission and synchronization
		VkSubmitInfo submitInfo {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphore[] = { m_available_semaphores[ m_current_frame ] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commands.m_command_buffers[ m_current_image_index ];

		VkSemaphore signalSemaphores[] = { m_finished_semaphores[ m_current_frame ] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences ( m_devices.m_logical_device , 1 , &m_in_flight_fences[ m_current_frame ] );

		if ( vkQueueSubmit ( m_graphics_queue , 1 , &submitInfo , m_in_flight_fences[ m_current_frame ] ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to submit draw command buffer!" );
		}

		VkPresentInfoKHR presentInfo {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapchain.m_swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_current_image_index;

		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR ( m_present_queue , &presentInfo );

		if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
		{
			RecreateSwapchain ();
		}
		else if ( result != VK_SUCCESS )
		{
			throw std::runtime_error ( "failed to present swap chain image!" );
		}

		m_current_frame = ( m_current_frame + 1 ) % VK_HELPER::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanEx::Wait ()
	{
		vkDeviceWaitIdle ( m_devices.m_logical_device );
	}

	void VulkanEx::BuildVulkanModels ( god::AssimpModelManager const& modelManager )
	{
		for ( auto const& model : modelManager.GetResources () )
		{
			VK_OBJECT::Vertices vertices;
			for ( auto const& vertex : model.m_meshes[ 0 ].m_vertices )
			{
				VK_OBJECT::Vertex vk_vertex;
				vk_vertex.m_position = vertex.m_position;
				vk_vertex.m_uv = vertex.m_uv;
				vk_vertex.m_tangent = vertex.m_tangent;
				vk_vertex.m_normal = vertex.m_normal;
				vk_vertex.m_colour = vertex.m_colour;

				vertices.push_back ( vk_vertex );
			}
			AddModel ( "test" , vertices , model.m_meshes[ 0 ].m_indices );
		}
	}

	VkQueue VulkanEx::GetDeviceQueue ()
	{
		// ensure surface and devices successfully created
		assert ( m_surface.Success () && m_devices.Success () );

		VK_HELPER::GET::QueueFamilyIndices indices = VK_HELPER::GET::QueueFamilies ( m_devices.m_physical_device , m_surface.m_surface );
		VkQueue present_queue;
		vkGetDeviceQueue ( m_devices.m_logical_device , indices.present_family_.value () , 0 , &present_queue );
		return present_queue;
	}

	bool VulkanEx::InitializeSyncObjects ()
	{
		m_available_semaphores.resize ( VK_HELPER::MAX_FRAMES_IN_FLIGHT );
		m_finished_semaphores.resize ( VK_HELPER::MAX_FRAMES_IN_FLIGHT );
		m_in_flight_fences.resize ( VK_HELPER::MAX_FRAMES_IN_FLIGHT );
		m_images_in_flight.resize ( m_swapchain.m_images.size () , VK_NULL_HANDLE );

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for ( size_t i = 0; i < VK_HELPER::MAX_FRAMES_IN_FLIGHT; ++i )
		{
			if ( vkCreateSemaphore ( m_devices.m_logical_device , &semaphoreInfo , nullptr , &m_available_semaphores[ i ] ) != VK_SUCCESS ||
				vkCreateSemaphore ( m_devices.m_logical_device , &semaphoreInfo , nullptr , &m_finished_semaphores[ i ] ) != VK_SUCCESS ||
				vkCreateFence ( m_devices.m_logical_device , &fenceInfo , nullptr , &m_in_flight_fences[ i ] ) != VK_SUCCESS )
			{
				std::cerr << "[ERROR!] RENDERER::VulkanEx::InitializeSyncObjects - Failed to create semaphore for a frame." << std::endl;
				return false;
			}
		}
		std::cout << "[SUCCESS] RENDERER::VulkanEx::InitializeSyncObjects - Sync objects initialized successfully." << std::endl;
		return true;
	}

	void VulkanEx::RecreateSwapchain ()
	{
		vkDeviceWaitIdle ( m_devices.m_logical_device );

		// destroy all necessary parts
		m_framebuffers.Destroy ();
		m_commands.Destroy ();
		m_graphics_pipelines[ m_current_graphics_pipeline ].DestroyPipeline ();
		m_renderpass.Destroy ();
		m_swapchain.Destroy ();
		m_descriptor.Destroy ();

		// recreate all necessary parts
		m_swapchain.Create ( m_devices.m_physical_device , m_devices.m_logical_device , m_surface.m_surface );
		m_renderpass.Create ( m_devices.m_physical_device , m_devices.m_logical_device , m_swapchain.m_format );
		m_graphics_pipelines[ m_current_graphics_pipeline ].Create ( m_devices.m_logical_device , m_swapchain.m_extent , m_renderpass.m_render_pass );
		m_framebuffers.Create ( m_devices.m_physical_device , m_devices.m_logical_device , m_swapchain , m_renderpass.m_render_pass );
		m_commands.RecreateCommandBuffers ( m_swapchain , m_renderpass , m_framebuffers );
		m_descriptor.Create ( m_devices );
	}
}