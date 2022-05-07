#include "pch.h"
#include "Engine.h"

#include <godOpenGL/OpenGL.h>
#include <godVulkanEx/Vulkan.h>
#include <godWindow/GLFWWindow.h>
#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>

#include <memory>

#define USING_VULKAN

namespace god
{
	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

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
		god::SceneObjectID skull = scene.AddSceneObject ( model_manager.GetID ( "Skull" ) , { 0.0f,0.0f,-2.0f } );
		//god::SceneObjectID backpack = scene.AddSceneObject ( model_manager.GetID ( "Backpack" ) , { 5.0f, 0.0f, -2.0f } );

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

#ifdef USING_VULKAN
			vulkan.StartFrame ();
			vulkan.Draw ( "test" , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f );
#else
			opengl.ClearColour ();
#endif

			// update scene
			// ...
			//scene.GetSceneObject ( skull ).m_rotation.y += 0.0002f;

			// render scene
#ifdef USING_VULKAN
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
#endif
	}
}