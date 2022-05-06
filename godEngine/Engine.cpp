#include "pch.h"
#include "Engine.h"

#include <godOpenGL/OpenGL.h>
#include <godWindow/GLFWWindow.h>
#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>

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
		god::OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );

		// setup camera
		god::Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );

		// setup resources
		god::AssimpModelManager model_manager;
		model_manager.Insert ( "Backpack" , god::LoadModel ( "Assets/GameAssets/Models/Backpack/backpack.obj" ) );
		model_manager.Insert ( "Skull" , god::LoadModel ( "Assets/GameAssets/Models/Skull/skull.fbx" ) );

		opengl.BuildOGLModels ( model_manager );

		// setup scene
		god::Scene scene;
		god::SceneObjectID skull = scene.AddSceneObject ( model_manager.GetID ( "Skull" ) , { 0.0f,0.0f,-2.0f } );
		god::SceneObjectID backpack = scene.AddSceneObject ( model_manager.GetID ( "Backpack" ) , { 5.0f, 0.0f, -2.0f } );

		while ( !window.WindowShouldClose () )
		{
			window.PollEvents ();
			window.SwapWindowBuffers ();

			// window resize changes
			if ( window.Resized () )
			{
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			opengl.ClearColour ();

			// update scene
			// ...
			scene.GetSceneObject ( skull ).m_rotation.y += 0.0002f;

			// render scene
			opengl.RenderScene (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position
			);

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
	}
}