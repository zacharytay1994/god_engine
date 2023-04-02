#include "pch.h"
#include "Engine.h"

#include "OpenGL/OpenGL.h"
#include "OpenGL/Internal/OGLRenderPass.h"
#include "OpenGL/Internal/OGLTexture.h"
#include "OpenGL/Font/Font.h"

#include "Window/GLFWWindow.h"
#include "Window/DeltaTimer.h"
#include "Window/SystemTimer.h"

#include "EnttXSol/EngineComponents/EC_All.h"
#include "EnttXSol/EngineSystems/ES_All.h"
#include "EnttXSol/EnttXSol.h"

#include "Physics/godPhysics.h"
#include "Physics/PhysicsController.h"

#include "Audio/AudioAPI.h"
#include "Audio/Internal/SoundManager.h"

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
#include <godUtility/Grid3D.h>

// comment out this for no editor
#define ENABLE_EDITOR

namespace god
{
	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	void godEngine::Update ()
	{
		FreeConsole ();
		std::cout << "godEngine Update." << std::endl;
		// create window
		GLFWWindow window ( 1920 , 1080 );
		//GLFWWindow window ( 1920/2 , 1080/2 );
#ifndef ENABLE_EDITOR
		window.m_fullscreen = true;
#endif
		DeltaTimer delta_timer;
		OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );
		Fonts fonts;
		fonts.InsertNewFont ( "Arial" , "Assets/GameAssets/Fonts/arial.ttf" );
		OGLRenderPass imgui_renderpass ( window.GetWindowWidth () , window.GetWindowHeight () );
		OGLRenderPass<2> hdr_renderpass ( window.GetWindowWidth () , window.GetWindowHeight () , GL_RGBA16F , GL_RGBA , GL_FLOAT );
		OGLRenderPass extra_renderpass ( window.GetWindowWidth () , window.GetWindowHeight () );
		//OGLRenderPass tonemap_renderpass(window.GetWindowWidth(), window.GetWindowHeight());
		glfwWindowHint ( GLFW_OPENGL_DEBUG_CONTEXT , true );

		PhysicsSystem godPhysicsSystem {};
		PX::PhysicsController godPhysicsController;

		// setup camera
		Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
		//camera.m_pitch = -25.0f;
		//camera.m_yaw = 155.0f;
		camera.SetLookAt ( { 1,0,-2 } );
		camera.SetPosition ( { 8,10,2 } );
		//float scene_camera_zoom_distance { 20.0f };
		glm::vec3 scene_camera_position_offset { 0.0f };

		// setup FMOD system
		AudioAPI audio_api;

		// setup resources
		Asset3DManager assets_3d;
		InsertAllAsset3DsFromConfig ( AssetPath::File_ModelsConfig , AssetPath::Folder_BuildModels , assets_3d );
		OGLTextureManager ogl_textures;
		InsertEngineOGLTextures ( ogl_textures ); // temp solution to insert engine textures, might change
		InsertAllOGLTexturesFromConfig ( AssetPath::File_TexturesConfig , AssetPath::Folder_RawTextures , ogl_textures );
		SoundManager sound_assets;
		InsertAllSoundsFromConfig ( AssetPath::File_SoundsConfig , AssetPath::Folder_BuildSounds , sound_assets );

		opengl.BuildOGLModels ( assets_3d );


		// setup ecs and scripting
		EnttXSol enttxsol;
		enttxsol.BindEngineComponents< EngineComponents > ();
		enttxsol.BindEngineSystemUpdate ( EngineSystems , EngineSystemsInit , EngineSystemsCleanup );

		// setup scene
		Scene scene;

		// setup grid for tilemap
		EntityGrid grid;

		// glfw+opengl imgui setup
#ifdef ENABLE_EDITOR
		ImGuiOpenGLEditor ogl_editor ( window );
#endif

		// engine resources used by imgui as defined in EditorResourcesDefinition.h
		EngineResources engine_resources (
			window ,
			opengl ,
			camera ,
			assets_3d ,
			ogl_textures ,
			grid ,
			sound_assets ,
			godPhysicsSystem ,
			godPhysicsController ,
			scene ,
			fonts
		);

#ifdef ENABLE_EDITOR
		// starting scene for editor mode
		MainVariables main_variables = { "nil", false };
#else
		// starting scene for non editor mode
		MainVariables main_variables = { "DigipenScreen", true }; 
		AudioAPI::PlayBGM ();
#endif
		RegisterLuaCPP ( enttxsol , engine_resources , main_variables );

		// imgui editor windows
#ifdef ENABLE_EDITOR
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
#endif

		godPhysicsSystem.Init ( &window , &camera );
		godPhysicsController.CreateControllerManager ( godPhysicsSystem.GetPhysicsScene () );

		while ( !window.WindowShouldClose () )
		{
			SystemTimer::StartTimeSegment ( "Overall" );
			delta_timer.StartFrame ();

			window.PollEvents ();

			// window resize changes
			if ( window.Resized () && !window.WindowsMinimized () )
			{
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
				imgui_renderpass.UpdateWidth ( window.GetWindowWidth () , window.GetWindowHeight () );
				extra_renderpass.UpdateWidth ( window.GetWindowWidth () , window.GetWindowHeight () );
				hdr_renderpass.UpdateWidth ( window.GetWindowWidth () , window.GetWindowHeight () );

				opengl.m_blur_pingpong_1.UpdateWidth ( window.GetWindowWidth () , window.GetWindowHeight () );
				opengl.m_blur_pingpong_2.UpdateWidth ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			opengl.ClearColour ();

			EngineSystemsFrameStart ( enttxsol , engine_resources );

			// update scene
			// ...
			SystemTimer::StartTimeSegment ( "EnTT Update" );
			enttxsol.Update ( engine_resources );
			SystemTimer::EndTimeSegment ( "EnTT Update" );
			SystemTimer::StartTimeSegment ( "Populating Scene" );
			enttxsol.PopulateScene<Scene , Transform , Renderable3D , Fonts> ( scene , fonts , camera.m_position );
			SystemTimer::EndTimeSegment ( "Populating Scene" );

			//Physics Simulate update
			SystemTimer::StartTimeSegment ( "Physics System" );
			godPhysicsSystem.Update ( delta_timer.m_dt , enttxsol.m_pause );
			SystemTimer::EndTimeSegment ( "Physics System" );

			// render scene
#ifdef ENABLE_EDITOR
			SystemTimer::StartTimeSegment ( "Rendering" );
#endif

			// depth map pass
			opengl.UpdateAnimations ( DeltaTimer::m_dt );
			opengl.FirstPassRenderToDepthmap (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position ,
				ogl_textures );

			// hdr pass ================================================= start
			hdr_renderpass.Bind ();

			glm::vec3 camera_front = camera.m_look_at;
			camera_front.y = 0;
			camera_front = glm::normalize ( camera_front );

			opengl.RenderScene (
				scene ,
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position ,
				ogl_textures ,
				camera_front );

			opengl.RenderLines (
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () );

			hdr_renderpass.UnBind ();
			// hdr pass ================================================= end

			// imgui pass ================================================= start
			imgui_renderpass.Bind ();

			opengl.RenderTonemap ( hdr_renderpass );

			imgui_renderpass.UnBind ();

			auto& blur = opengl.BlurTexture ( hdr_renderpass.GetTexture ( 1 ) );

#ifdef ENABLE_EDITOR
			if ( !window.m_fullscreen )
			{
				extra_renderpass.Bind ();
				opengl.RenderBlendTextures ( imgui_renderpass.GetTexture () , blur.GetTexture () );
				opengl.RenderGUI ( scene , camera.GetOrthographicProjectionMatrix ( static_cast< float >( window.GetWindowWidth () ) , static_cast< float >( window.GetWindowHeight () ) ) , ogl_textures );
				extra_renderpass.UnBind ();
			}

			SystemTimer::EndTimeSegment ( "Rendering" );

			// ... render imgui windows
			SystemTimer::StartTimeSegment ( "Editor" );
			ogl_editor.BeginFrame ();

			if ( ImGui::BeginPopupModal ( "pausemodal" ) )
			{
				if ( !window.m_fullscreen )
				{
					ImGui::CloseCurrentPopup ();
				}
				ImGui::EndPopup ();
			}
			if ( window.m_fullscreen )
			{
				ImGui::OpenPopup ( "pausemodal" );
			}

			// pass scene view the renderpass texture
			if ( !window.m_fullscreen )
			{
				editor_windows.GetWindow<EW_SceneView> ()->SetRenderpassTexture ( extra_renderpass.GetTexture () );
				editor_windows.Update ( 0.02f , engine_resources );
			}
			ogl_editor.Render ();
			ogl_editor.EndFrame ();
			SystemTimer::EndTimeSegment ( "Editor" );
#endif

			// if fullscreen, render game over imgui
			if ( window.m_fullscreen )
			{
				//extra_renderpass.Bind ();
				opengl.RenderBlendTextures ( imgui_renderpass.GetTexture () , blur.GetTexture () );
				opengl.RenderGUI ( scene , camera.GetOrthographicProjectionMatrix ( static_cast< float >( window.GetWindowWidth () ) , static_cast< float >( window.GetWindowHeight () ) ) , ogl_textures );
				//extra_renderpass.UnBind ();

				//opengl.RenderTexture ( extra_renderpass.GetTexture () );
			}


			SystemTimer::StartTimeSegment ( "Window Buffer Swap" );
			window.SwapWindowBuffers ();
			SystemTimer::EndTimeSegment ( "Window Buffer Swap" );

			// free camera update
#ifdef ENABLE_EDITOR
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
#endif

			camera.InterpolateCameraState ( DeltaTimer::m_dt );

			EngineSystemsFrameEnd ( enttxsol , engine_resources );

			// update FMOD system
			AudioAPI::Update ();

			// update physics controllers state
			//godPhysicsController.FreeControllers ();

			// change scene if any
			auto& [scene_to_change , play_on_change] = main_variables;

			if ( scene_to_change != "nil" )
			{
				std::cout << "Attempting to change scene to " << scene_to_change << std::endl;

#ifdef ENABLE_EDITOR
				editor_windows.GetWindow<EW_TilemapEditor> ()->ClearPreview ( engine_resources );
				auto scene_tree = editor_windows.GetWindow<EW_SceneTree> ();
				scene_tree->Reset ();
				scene_tree->m_select_hierarchy_tab = true;
				scene_tree->SetSelectedScene ( scene_to_change );
#endif

				//EntityGrid& grid = engine_resources.Get<EntityGrid> ().get ();
				grid = EntityGrid ();

				enttxsol.ClearEntt ( engine_resources );
				enttxsol.DeserializeStateV2 ( engine_resources , scene_to_change , &grid );

				SoundManager& sound_manager = engine_resources.Get<SoundManager> ().get ();
				auto& sounds = sound_manager.GetResources ();
				AudioAPI::StopAndResetAll ( sounds );

				scene_to_change = "nil";

				if ( play_on_change )
				{
					enttxsol.m_pause = false;
					play_on_change = false;
				}
				std::cout << "Change Scene attempt done." << std::endl;
			}

			if ( window.KeyDown ( GLFW_KEY_LEFT_CONTROL ) && window.KeyPressed ( GLFW_KEY_F ) )
			{
				window.m_fullscreen = !window.m_fullscreen;
			}

			scene.ClearInstancedScene ();
			delta_timer.EndFrame ();
			SystemTimer::EndTimeSegment ( "Overall" );

			// set window title fps
			/*std::stringstream ss;
			ss << "God Engine - " << DeltaTimer::m_fps;
			window.SetWindowTitle ( ss.str () );*/
		}
	}
}
