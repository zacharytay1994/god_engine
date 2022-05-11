#include "pch.h"
#include "Engine.h"

#include "OpenGL/OpenGL.h"
#include "Window/GLFWWindow.h"

#include "Editor/Editor.h"
#include "Editor/OpenGLEditor.h"
#include "Editor/Window/TestWindow.h"
#include "Editor/Window/TestWindow2.h"

#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>
#include <godUtility/TemplateManipulation.h>

#include <tuple>

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
		/*model_manager.Insert ( "Backpack" , god::LoadModel ( "Assets/GameAssets/Models/Backpack/backpack.obj" ) );
		model_manager.Insert ( "Skull" , god::LoadModel ( "Assets/GameAssets/Models/Skull/skull.fbx" ) );*/

		opengl.BuildOGLModels ( model_manager );

		// setup scene
		god::Scene scene;
		/*god::SceneObjectID skull = scene.AddSceneObject ( model_manager.GetID ( "Skull" ) , { 0.0f,0.0f,-2.0f } );
		god::SceneObjectID backpack = scene.AddSceneObject ( model_manager.GetID ( "Backpack" ) , { 5.0f, 0.0f, -2.0f } );*/

		// test
		god::T_Manip::TYPE_PACK<int , float> type_pack;
		god::T_Manip::GetType ( type_pack , 1 );

		// glfw+opengl imgui setup
		god::ImGuiOpenGLEditor ogl_editor ( window );

		// imgui editors
		EditorResources<
			god::GLFWWindow ,
			god::AssimpModelManager
		> editor_resources (
			window ,
			model_manager
		);
		EditorWindows<decltype( editor_resources )> editor_windows;
		editor_windows.AddWindow<god::TestWindow> ();
		editor_windows.AddWindow<god::TestWindow2> ();

		while ( !window.WindowShouldClose () )
		{
			window.PollEvents ();

			// window resize changes
			if ( window.Resized () )
			{
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			ogl_editor.BeginFrame ();

			// ... render imgui windows
			editor_windows.Update ( 0.02f , editor_resources );

			ogl_editor.Render ();

			opengl.ClearColour ();

			ogl_editor.EndFrame ();

			// update scene
			// ...
			//scene.GetSceneObject ( skull ).m_rotation.y += 0.0002f;

			// render scene
			/*opengl.RenderScene (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position
			);*/

			window.SwapWindowBuffers ();
			// free camera update
			/*camera.FreeCamera ( 0.0002f ,
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
				window.MouseScrollDown () );*/
		}
	}
}