#include "pch.h"
#include "Engine.h"

#include "OpenGL/OpenGL.h"
#include "OpenGL/Internal/OGLRenderPass.h"
#include "OpenGL/Internal/OGLTexture.h"

#include "Window/GLFWWindow.h"
#include "Window/DeltaTimer.h"
#include "Window/SystemTimer.h"

#include "EnttXSol/EngineComponents/EC_All.h"
#include "EnttXSol/EngineSystems/ES_All.h"
#include "EnttXSol/EnttXSol.h"

#include "Physics/godPhysics.h"

#include "Audio/AudioAPI.h"

#include "Editor/Editor.h"
#include "Editor/EngineResources.h"
#include "Editor/OpenGLEditor.h"
#include "Editor/Window/EW_MainMenuBar.h"
#include "Editor/Window/EW_EditorStyles.h"
#include "Editor/Window/EW_AssetImporter.h"
#include "Editor/Window/EW_AssetManager.h"
#include "Editor/Window/EW_SceneTree.h"
#include "Editor/Window/EW_EntityEditor.h"
#include "Editor/Window/EW_SceneView.h"
#include "Editor/Window/EW_Performance.h"
#include "Editor/Window/EW_TilemapEditor.h"
#include "Editor/Editor.h"

#include <godCamera/Camera.h>
#include <godUtility/Utility.h>
#include <godUtility/Scene.h>
#include <godUtility/TemplateManipulation.h>
#include <godUtility/FileIO.h>
#include <godUtility/Math.h>

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
		GLFWWindow window ( 1920 , 1080 );
		DeltaTimer delta_timer;
		OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );
		OGLRenderPass first_renderpass ( window.GetWindowWidth () , window.GetWindowHeight () );

		// setup camera
		Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );

		// setup resources
		Asset3DManager assets_3d;
		InsertAllAsset3DsFromConfig ( AssetPath::File_ModelsConfig , AssetPath::Folder_BuildModels , assets_3d );
		OGLTextureManager ogl_textures;
		InsertEngineOGLTextures ( ogl_textures ); // temp solution to insert engine textures, might change
		InsertAllOGLTexturesFromConfig ( AssetPath::File_TexturesConfig , AssetPath::Folder_RawTextures , ogl_textures );

		opengl.BuildOGLModels ( assets_3d );

		AudioAPI audio_api;

		// setup ecs and scripting
		EnttXSol enttxsol;
		enttxsol.BindEngineComponents< EngineComponents > ();
		enttxsol.BindEngineSystemUpdate ( EngineSystems );
		enttxsol.SetupBindings ();

		// setup scene
		Scene scene;

		// glfw+opengl imgui setup
		ImGuiOpenGLEditor ogl_editor ( window );

		// engine resources used by imgui as defined in EditorResourcesDefinition.h
		EngineResources engine_resources (
			window ,
			opengl ,
			camera ,
			assets_3d ,
			ogl_textures
		);

		// imgui editor windows
		EditorWindows<EngineResources> editor_windows;
		editor_windows.AddWindow<god::EW_MainMenuBar> ( true );
		editor_windows.AddWindow<god::EW_EditorStyles> ( false );
		editor_windows.AddWindow<god::EW_AssetManager> ( true );
		editor_windows.AddWindow<god::EW_Asset3DImporter> ( true );
		editor_windows.AddWindow<god::EW_SceneTree> ( true , std::ref ( enttxsol ) );
		editor_windows.AddWindow<god::EW_EntityEditor> ( true , std::ref ( enttxsol ) );
		editor_windows.AddWindow<god::EW_SceneView> ( true , std::ref ( enttxsol ) );
		editor_windows.AddWindow<god::EW_Performance> ( true );
		editor_windows.AddWindow<god::EW_TilemapEditor> ( true , std::ref ( enttxsol ) );

		while ( !window.WindowShouldClose () )
		{
			SystemTimer::StartTimeSegment ( "Overall" );
			delta_timer.StartFrame ();

			window.PollEvents ();

			// window resize changes
			if ( window.Resized () )
			{
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
				first_renderpass.UpdateWidth ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			opengl.ClearColour ();

			// update scene
			// ...
			SystemTimer::StartTimeSegment ( "EnTT Update" );
			enttxsol.Update ();
			SystemTimer::EndTimeSegment ( "EnTT Update" );
			SystemTimer::StartTimeSegment ( "Populating Scene" );
			enttxsol.PopulateScene<Scene , Transform , Renderable3D> ( scene );
			SystemTimer::EndTimeSegment ( "Populating Scene" );

			// render scene
			SystemTimer::StartTimeSegment ( "Rendering" );
			first_renderpass.Bind ();
			opengl.RenderScene (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position ,
				ogl_textures
			);
			opengl.RenderLines (
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix ()
			);
			first_renderpass.UnBind ();
			SystemTimer::EndTimeSegment ( "Rendering" );

			// ... render imgui windows
			SystemTimer::StartTimeSegment ( "Editor" );
			ogl_editor.BeginFrame ();
			// pass scene view the renderpass texture
			editor_windows.GetWindow<EW_SceneView> ()->SetRenderpassTexture ( first_renderpass.GetTexture () );
			editor_windows.Update ( 0.02f , engine_resources );
			ogl_editor.Render ();
			ogl_editor.EndFrame ();
			SystemTimer::EndTimeSegment ( "Editor" );

			SystemTimer::StartTimeSegment ( "Window Buffer Swap" );
			window.SwapWindowBuffers ();
			SystemTimer::EndTimeSegment ( "Window Buffer Swap" );
			// free camera update
			camera.FreeCamera ( 0.02f ,
				true ,
				window.KeyDown ( GLFW_KEY_W ) ,
				window.KeyDown ( GLFW_KEY_S ) ,
				window.KeyDown ( GLFW_KEY_A ) ,
				window.KeyDown ( GLFW_KEY_D ) ,
				window.KeyDown ( GLFW_KEY_SPACE ) ,
				window.KeyDown ( GLFW_KEY_LEFT_SHIFT ) ,
				window.MouseRDown () ,
				static_cast< float >( window.ScreenMouseX () ) ,
				static_cast< float >( window.ScreenMouseY () ) ,
				window.MouseRDown () ,
				window.MouseScrollUp () ,
				window.MouseScrollDown () ,
				window.KeyDown ( GLFW_KEY_LEFT_CONTROL ) ,
				window.MouseScrollUp () ,
				window.MouseScrollDown () );

			/*glm::vec3 dir = ViewportToWorldRay (
				{ window.ViewportMouseX (), window.ViewportMouseY () } ,
				window.GetWindowWidth () ,
				window.GetWindowHeight () ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () );
			glm::vec3 a = camera.m_position , b = camera.m_position + dir * 1000.0f;
			glm::vec3 ints = IntersectLineSegmentPlane ( a , b , { 0,1,0 } , 0 );*/
			//std::cout << ints.x << "," << ints.y << "," << ints.z << std::endl;

			delta_timer.EndFrame ();
			SystemTimer::EndTimeSegment ( "Overall" );
		}
	}
}