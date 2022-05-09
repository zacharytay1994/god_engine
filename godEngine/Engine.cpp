#include "pch.h"
#include "Engine.h"

#include <godOpenGL/OpenGL.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#define GLFW_INCLUDE_VULKAN
#pragma comment(lib, "glfw3.lib")
#include <godVulkanEx/Vulkan.h>
#include <godWindow/GLFWWindow.h>

#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>

#include <memory>
#include <stdexcept>

#define USING_VULKAN

namespace god
{
	static void SetupVulkanWindow ( ImGui_ImplVulkanH_Window* wd , VulkanEx const& vulkan , int width , int height )
	{
		wd->Surface = vulkan.m_surface.m_surface;

		auto family = VK_HELPER::GET::QueueFamilies ( vulkan.m_devices.m_physical_device , vulkan.m_surface.m_surface );
		auto g_QueueFamily = family.graphics_family_.value ();
		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR ( vulkan.m_devices.m_physical_device , g_QueueFamily , wd->Surface , &res );
		if ( res != VK_TRUE )
		{
			fprintf ( stderr , "Error no WSI support on physical device 0\n" );
			exit ( -1 );
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat ( vulkan.m_devices.m_physical_device , wd->Surface , requestSurfaceImageFormat , ( size_t ) IM_ARRAYSIZE ( requestSurfaceImageFormat ) , requestSurfaceColorSpace );

		// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode ( vulkan.m_devices.m_physical_device , wd->Surface , &present_modes[ 0 ] , IM_ARRAYSIZE ( present_modes ) );
		//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		//IM_ASSERT ( g_MinImageCount >= 2 );
		ImGui_ImplVulkanH_CreateOrResizeWindow ( vulkan.m_instance.m_instance , vulkan.m_devices.m_physical_device , vulkan.m_devices.m_logical_device , wd , g_QueueFamily , nullptr , width , height , 2 );
	}

	//static void FrameRender ( VulkanEx const& vulkan , ImGui_ImplVulkanH_Window* wd , ImDrawData* draw_data )
	//{
	//	VkResult err;

	//	VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].ImageAcquiredSemaphore;
	//	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].RenderCompleteSemaphore;
	//	err = vkAcquireNextImageKHR ( vulkan.m_devices.m_logical_device , wd->Swapchain , UINT64_MAX , image_acquired_semaphore , VK_NULL_HANDLE , &wd->FrameIndex );
	//	if ( err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR )
	//	{
	//		g_SwapChainRebuild = true;
	//		return;
	//	}
	//	check_vk_result ( err );

	//	ImGui_ImplVulkanH_Frame* fd = &wd->Frames[ wd->FrameIndex ];
	//	{
	//		err = vkWaitForFences ( g_Device , 1 , &fd->Fence , VK_TRUE , UINT64_MAX );    // wait indefinitely instead of periodically checking
	//		check_vk_result ( err );

	//		err = vkResetFences ( g_Device , 1 , &fd->Fence );
	//		check_vk_result ( err );
	//	}
	//	{
	//		err = vkResetCommandPool ( g_Device , fd->CommandPool , 0 );
	//		check_vk_result ( err );
	//		VkCommandBufferBeginInfo info = {};
	//		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	//		err = vkBeginCommandBuffer ( fd->CommandBuffer , &info );
	//		check_vk_result ( err );
	//	}
	//	{
	//		VkRenderPassBeginInfo info = {};
	//		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	//		info.renderPass = wd->RenderPass;
	//		info.framebuffer = fd->Framebuffer;
	//		info.renderArea.extent.width = wd->Width;
	//		info.renderArea.extent.height = wd->Height;
	//		info.clearValueCount = 1;
	//		info.pClearValues = &wd->ClearValue;
	//		vkCmdBeginRenderPass ( fd->CommandBuffer , &info , VK_SUBPASS_CONTENTS_INLINE );
	//	}

	//	// Record dear imgui primitives into command buffer
	//	ImGui_ImplVulkan_RenderDrawData ( draw_data , fd->CommandBuffer );

	//	// Submit command buffer
	//	vkCmdEndRenderPass ( fd->CommandBuffer );
	//	{
	//		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	//		VkSubmitInfo info = {};
	//		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//		info.waitSemaphoreCount = 1;
	//		info.pWaitSemaphores = &image_acquired_semaphore;
	//		info.pWaitDstStageMask = &wait_stage;
	//		info.commandBufferCount = 1;
	//		info.pCommandBuffers = &fd->CommandBuffer;
	//		info.signalSemaphoreCount = 1;
	//		info.pSignalSemaphores = &render_complete_semaphore;

	//		err = vkEndCommandBuffer ( fd->CommandBuffer );
	//		check_vk_result ( err );
	//		err = vkQueueSubmit ( g_Queue , 1 , &info , fd->Fence );
	//		check_vk_result ( err );
	//	}
	//}

	//static void FramePresent ( ImGui_ImplVulkanH_Window* wd )
	//{
	//	if ( g_SwapChainRebuild )
	//		return;
	//	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].RenderCompleteSemaphore;
	//	VkPresentInfoKHR info = {};
	//	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	//	info.waitSemaphoreCount = 1;
	//	info.pWaitSemaphores = &render_complete_semaphore;
	//	info.swapchainCount = 1;
	//	info.pSwapchains = &wd->Swapchain;
	//	info.pImageIndices = &wd->FrameIndex;
	//	VkResult err = vkQueuePresentKHR ( g_Queue , &info );
	//	if ( err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR )
	//	{
	//		g_SwapChainRebuild = true;
	//		return;
	//	}
	//	check_vk_result ( err );
	//	wd->SemaphoreIndex = ( wd->SemaphoreIndex + 1 ) % wd->ImageCount; // Now we can use the next set of semaphores
	//}

	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	static ImGui_ImplVulkanH_Window main_window_data;

	void godEngine::Update ()
	{
		// create window
		god::GLFWWindow window ( 800 , 450 );

		// setup camera
		god::Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );

		// setup resources
		god::AssimpModelManager model_manager;
		//model_manager.Insert ( "Backpack" , god::LoadModel ( "Assets/GameAssets/Models/Backpack/backpack.obj" ) );
		model_manager.Insert ( "Skull" , god::LoadModel ( "Assets/GameAssets/Models/Skull/skull.fbx" ) );

#ifdef USING_VULKAN
		VK_OBJECT::ConfigRef vulkan_config = std::make_shared<VK_OBJECT::Config> ( true , true );

		god::VulkanEx vulkan ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () , vulkan_config );

		vulkan.CreateGraphicsPipeline<VK_OBJECT::Model::PushConstant> ( "default" ,
			"Assets/EngineAssets/VulkanShaders/vert.spv" , "Assets/EngineAssets/VulkanShaders/frag.spv" );
		vulkan.BindGraphicsPipeline ( "default" );

		vulkan.BuildVulkanModels ( model_manager );

		vulkan.AddImage ( "Diffuse" , "Assets/GameAssets/Models/Skull/TD_Checker_Base_Color.png" );
		vulkan.AddImage ( "Normal" , "Assets/GameAssets/Models/Skull/TD_Checker_Normal_OpenGL.png" );
#else
		god::OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );
		opengl.BuildOGLModels ( model_manager );
#endif

		// setup scene
		god::Scene scene;
		god::SceneObjectID skull = scene.AddSceneObject ( model_manager.GetID ( "Skull" ) , { 0.0f,0.0f,-0.2f } );

		// setup imgui
		if ( !glfwVulkanSupported () )
		{
			throw std::runtime_error ( "GLFW does not support vulkan." );
		}

		ImGui_ImplVulkanH_Window* wd = &main_window_data;
		SetupVulkanWindow ( wd , vulkan , window.GetWindowWidth () , window.GetWindowHeight () );

		IMGUI_CHECKVERSION ();
		ImGui::CreateContext ();
		ImGuiIO& io = ImGui::GetIO (); ( void ) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		auto family = VK_HELPER::GET::QueueFamilies ( vulkan.m_devices.m_physical_device , vulkan.m_surface.m_surface );
		auto g_QueueFamily = family.graphics_family_.value ();

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size ( pool_sizes );
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		vkCreateDescriptorPool ( vulkan.m_devices.m_logical_device , &pool_info , nullptr , &imguiPool );

		ImGui_ImplGlfw_InitForVulkan ( window.GetGLFWWindow () , true );
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = vulkan.m_instance.m_instance;
		init_info.PhysicalDevice = vulkan.m_devices.m_physical_device;
		init_info.Device = vulkan.m_devices.m_logical_device;
		init_info.QueueFamily = g_QueueFamily;
		init_info.Queue = vulkan.m_graphics_queue;
		//init_info.PipelineCache = g_PipelineCache;
		init_info.DescriptorPool = imguiPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = 3;
		init_info.ImageCount = wd->ImageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		//init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init ( &init_info , wd->RenderPass );

		{
			// Use any command queue
			VkCommandPool command_pool = wd->Frames[ wd->FrameIndex ].CommandPool;
			VkCommandBuffer command_buffer = wd->Frames[ wd->FrameIndex ].CommandBuffer;

			/*err = */vkResetCommandPool ( vulkan.m_devices.m_logical_device , command_pool , 0 );
			//check_vk_result ( err );
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			/*err = */vkBeginCommandBuffer ( command_buffer , &begin_info );
			//check_vk_result ( err );

			ImGui_ImplVulkan_CreateFontsTexture ( command_buffer );

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			/*err = */vkEndCommandBuffer ( command_buffer );
			//check_vk_result ( err );
			/*err = */vkQueueSubmit ( vulkan.m_graphics_queue , 1 , &end_info , VK_NULL_HANDLE );
			//check_vk_result ( err );

			/*err = */vkDeviceWaitIdle ( vulkan.m_devices.m_logical_device );
			//check_vk_result ( err );
			ImGui_ImplVulkan_DestroyFontUploadObjects ();
		}

		while ( !window.WindowShouldClose () )
		{
			window.PollEvents ();
#ifndef USING_VULKAN
			window.SwapWindowBuffers ();
#endif

			// window resize changes
			if ( window.Resized () )
			{
#ifdef USING_VULKAN
#else
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
#endif
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			ImGui_ImplVulkan_NewFrame ();
			ImGui_ImplGlfw_NewFrame ();
			ImGui::NewFrame ();

			ImGui::Render ();
			if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
			{
				ImGui::UpdatePlatformWindows ();
				ImGui::RenderPlatformWindowsDefault ();
			}

#ifdef USING_VULKAN
			vulkan.StartFrame ();
#else
			opengl.ClearColour ();
#endif

			// update scene
			// ...
			scene.GetSceneObject ( skull ).m_rotation.y += 0.02f;

			// render scene
#ifdef USING_VULKAN
			vulkan.RenderScene (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position
			);
			vulkan.EndFrame ();
#else
			opengl.RenderScene (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position
			);
#endif

			// free camera update
			camera.FreeCamera ( 0.0002f ,
				true ,
				window.KeyDown ( GLFW_KEY_W ) ,
				window.KeyDown ( GLFW_KEY_S ) ,
				window.KeyDown ( GLFW_KEY_A ) ,
				window.KeyDown ( GLFW_KEY_D ) ,
				window.KeyDown ( GLFW_KEY_SPACE ) ,
				window.KeyDown ( GLFW_KEY_LEFT_SHIFT ) ,
				window.MouseRDown () ,
				static_cast< float >( window.MouseX () ) ,
				static_cast< float >( window.MouseY () ) ,
				window.MouseRDown () ,
				window.MouseScrollUp () ,
				window.MouseScrollDown () ,
				window.KeyDown ( GLFW_KEY_LEFT_CONTROL ) ,
				window.MouseScrollUp () ,
				window.MouseScrollDown () );
		}
#ifdef USING_VULKAN
		vulkan.Wait ();
		ImGui_ImplVulkan_Shutdown ();
		ImGui_ImplGlfw_Shutdown ();
		ImGui::DestroyContext ();
#endif
	}
}