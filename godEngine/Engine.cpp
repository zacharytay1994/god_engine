#include "pch.h"
#include "Engine.h"

#include "OpenGL/OpenGL.h"
#include "Window/GLFWWindow.h"
#include "EnttXSol/EnttXSol.h"

#include "EnttXSol/EngineComponents/EC_All.h"
#include "EnttXSol/EngineSystems/ES_All.h"

#include "Editor/Editor.h"
#include "Editor/OpenGLEditor.h"
#include "Editor/Window/EW_MainMenuBar.h"
#include "Editor/Window/EW_EditorStyles.h"
#include "Editor/Window/EW_AssetImporter.h"
#include "Editor/Window/EW_AssetManager.h"
#include "Editor/Window/EW_SceneManager.h"

#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>
#include <godUtility/TemplateManipulation.h>
#include <godUtility/FileIO.h>

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

		// setup ecs and scripting
		god::EnttXSol enttxsol { {"Assets/GameAssets/Scripts/test.lua", "Assets/GameAssets/Scripts/test2.lua"} };
		god::EngineComponentType engine_components ( g_EngineComponents );
		enttxsol.BindEngineComponents ( engine_components );
		enttxsol.BindEngineSystemUpdate ( EngineSystems );
		enttxsol.RegisterLuaType<glm::vec3> ( "vec3" ,
			"x" , &glm::vec3::x ,
			"y" , &glm::vec3::y ,
			"z" , &glm::vec3::z );
		/*auto e1 = enttxsol.CreateEntity ();
		enttxsol.AttachScript ( e1 , "test" );
		enttxsol.AttachScript ( e1 , "test2" );*/

		// setup scene
		god::Scene scene;
		/*god::SceneObjectID skull = scene.AddSceneObject ( model_manager.GetID ( "Skull" ) , { 0.0f,0.0f,-2.0f } );
		god::SceneObjectID backpack = scene.AddSceneObject ( model_manager.GetID ( "Backpack" ) , { 5.0f, 0.0f, -2.0f } );*/

		// glfw+opengl imgui setup
		god::ImGuiOpenGLEditor ogl_editor ( window );

		// imgui editors
		EditorResources<
			god::GLFWWindow ,
			god::Asset3DManager ,
			god::EnttXSol ,
			EngineComponentType
		> editor_resources (
			window ,
			assets_3d ,
			enttxsol ,
			engine_components
		);
		EditorWindows<decltype( editor_resources )> editor_windows;
		editor_windows.AddWindow<god::EW_MainMenuBar> ( true );
		editor_windows.AddWindow<god::EW_EditorStyles> ();
		editor_windows.AddWindow<god::EW_Asset3DImporter> ();
		editor_windows.AddWindow<god::EW_AssetManager> ();
		editor_windows.AddWindow<god::EW_SceneManager> ();

		/*rapidjson::Document document;
		document.SetObject ();*/
		/*rapidjson::Value value ( 10 );
		document.AddMember ( "test" , value , document.GetAllocator () );
		rapidjson::Value value2 ( rapidjson::kObjectType );
		value2.AddMember ( "value2" , "i am string?" , document.GetAllocator () );
		document.AddMember ( "objectmember" , value2 , document.GetAllocator () );
		god::WriteJSON ( document , "test.json" );*/
		/*RapidJSON::JSONify ( document , "something" , 11 );
		RapidJSON::JSONify ( document , "something3" , 15.0f );
		RapidJSON::JSONifyValues ( document , "arraytest" , "re" , 2 , 3 , 4);
		god::WriteJSON ( document , "test2.json" );*/

		/*rapidjson::Document document;
		god::ReadJSON ( document , "test.json" );
		rapidjson::Value value ( 11 );
		document["objectmember"].AddMember ("newvalue" , value , document.GetAllocator ());
		god::WriteJSON ( document , "test.json" );*/

		while ( !window.WindowShouldClose () )
		{
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
			enttxsol.Update ();
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