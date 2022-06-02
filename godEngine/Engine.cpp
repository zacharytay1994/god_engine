#include "pch.h"
#include "Engine.h"

#include "OpenGL/OpenGL.h"
#include "Window/GLFWWindow.h"
#include "Window/Time.h"

#include "Editor/Editor.h"
#include "Editor/OpenGLEditor.h"
#include "Editor/Window/EW_MainMenuBar.h"
#include "Editor/Window/EW_EditorStyles.h"
#include "Editor/Window/EW_AssetImporter.h"
#include "Editor/Window/EW_AssetManager.h"
#include "Editor/Window/EW_SceneManager.h"

#include "ECS/ECSManager.h"
//#include "ECS/Test.h"

#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>
#include <godUtility/TemplateManipulation.h>
#include <godUtility/FileIO.h>

namespace god
{
	struct Position
	{
		float x , y;
	};

	struct Velocity
	{
		float dx , dy;
	};

	struct TestStruct2
	{
		template<typename T>
		void operator()( int i );
	};

	template<typename T>
	void TestStruct2::operator()( int i )
	{
		std::cout << typeid( T ).name () << std::endl;
	}

	template<typename T>
	void TestFunc ( int i , float f ) { std::cout << f << typeid( T ).name () << std::endl; }

	void TestSystem ( Position& pos , Velocity& vel )
	{
		std::cout << pos.x << "," << pos.y << "v:" << vel.dx << std::endl;
	}

	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	void godEngine::Update ()
	{
		// create window
		god::GLFWWindow window ( 1200 , 775 );
		god::OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );

		// setup camera
		god::Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );

		// setup resources
		god::Asset3DManager assets_3d;
		/*model_manager.Insert ( "Backpack" , god::LoadModel ( "Assets/GameAssets/Models/Backpack/backpack.obj" ) );
		model_manager.Insert ( "Skull" , god::LoadModel ( "Assets/GameAssets/Models/Skull/skull.fbx" ) );*/

		opengl.BuildOGLModels ( assets_3d );

		// setup scene
		god::Scene scene;
		/*god::SceneObjectID skull = scene.AddSceneObject ( model_manager.GetID ( "Skull" ) , { 0.0f,0.0f,-2.0f } );
		god::SceneObjectID backpack = scene.AddSceneObject ( model_manager.GetID ( "Backpack" ) , { 5.0f, 0.0f, -2.0f } );*/

		// glfw+opengl imgui setup
		god::ImGuiOpenGLEditor ogl_editor ( window );

		// ecs manager
		ECSManager ecs_manager;

		// imgui editors
		EditorResources<
			god::GLFWWindow ,
			god::Asset3DManager ,
			god::ECSManager
		> editor_resources (
			window ,
			assets_3d ,
			ecs_manager
		);
		EditorWindows<decltype( editor_resources )> editor_windows;
		editor_windows.AddWindow<god::EW_MainMenuBar> ( true );
		editor_windows.AddWindow<god::EW_EditorStyles> ();
		editor_windows.AddWindow<god::EW_Asset3DImporter> ();
		editor_windows.AddWindow<god::EW_AssetManager> ();
		editor_windows.AddWindow<god::EW_SceneManager> ();

		god::T_Manip::TYPE_PACK<int , float> pack;
		//god::T_Manip::GetType ( pack , 1 , TestFunc<int> , 0 , 0.2f );

		for ( int i = 0; i < 2; ++i )
		{
			god::T_Manip::GetType ( pack , i , TestStruct2(), 1 );
		}
		//god::T_Manip::Testlambdafunction ( TestFunc );

		Time time;

		while ( !window.WindowShouldClose () )
		{
			time.StartFrame ();
			window.PollEvents ();

			// window resize changes
			if ( window.Resized () )
			{
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			opengl.ClearColour ();

			// ... render imgui windows
			ogl_editor.BeginFrame ();
			editor_windows.Update ( 0.02f , editor_resources );
			ogl_editor.Render ();
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

			time.EndFrame ();
		}
	}
}