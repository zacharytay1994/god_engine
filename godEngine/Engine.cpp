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

#include <tuple>

namespace god
{
	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	void godEngine::Update ()
	{
		std::cout << "godEngine Update." << std::endl;
		// create window
		GLFWWindow window ( 1920 , 1080 );
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

		// setup camera
		Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
		camera.m_pitch = -25.0f;
		camera.m_yaw = 155.0f;
		//camera.m_look_at = { 0,0,0 };
		camera.m_position = { -2,7,-10 };
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
		ImGuiOpenGLEditor ogl_editor ( window );

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
			scene
		);

		std::string scene_to_change { "nil" };
		RegisterLuaCPP ( enttxsol , engine_resources , scene_to_change );

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

		godPhysicsSystem.Init ( &window , &camera );

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
			enttxsol.PopulateScene<Scene , Transform , Renderable3D , Fonts> ( scene , fonts );
			SystemTimer::EndTimeSegment ( "Populating Scene" );

			//Physics Simulate update
			SystemTimer::StartTimeSegment ( "Physics System" );
			godPhysicsSystem.Update ( delta_timer.m_dt , enttxsol.m_pause );
			SystemTimer::EndTimeSegment ( "Physics System" );

			// render scene
			SystemTimer::StartTimeSegment ( "Rendering" );
			// depth map pass
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
				camera.GetCameraViewFaceCamera () ,
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

			extra_renderpass.Bind ();
			opengl.RenderBlendTextures ( imgui_renderpass.GetTexture () , blur.GetTexture () );
			opengl.RenderGUI ( scene , camera.GetOrthographicProjectionMatrix ( static_cast< float >( window.GetWindowWidth () ) , static_cast< float >( window.GetWindowHeight () ) ) , ogl_textures );
			extra_renderpass.UnBind ();

			SystemTimer::EndTimeSegment ( "Rendering" );

			// ... render imgui windows
			SystemTimer::StartTimeSegment ( "Editor" );
			ogl_editor.BeginFrame ();
			// pass scene view the renderpass texture
			//editor_windows.GetWindow<EW_SceneView> ()->SetRenderpassTexture ( imgui_renderpass.GetTexture () );
			//editor_windows.GetWindow<EW_SceneView>()->SetRenderpassTexture( hdr_renderpass.GetTexture(1) );
			editor_windows.GetWindow<EW_SceneView> ()->SetRenderpassTexture ( extra_renderpass.GetTexture () );
			//editor_windows.GetWindow<EW_SceneView> ()->SetRenderpassTexture ( opengl.m_blur_pingpong_1.GetTexture() );
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
			/*camera.SceneCamera(
				window.KeyDown ( GLFW_KEY_LEFT_CONTROL ) ,
				0.5f ,
				0.7f , // value between 0-1
				scene_camera_position_offset ,
				window.MouseLDown () ,
				window.MouseRDown () ,
				static_cast< float >( window.ScreenMouseX () ) ,
				static_cast< float >( window.ScreenMouseY () ) ,
				scene_camera_zoom_distance ,
				window.MouseScrollUp () ,
				window.MouseScrollDown ()
			);*/

			EngineSystemsFrameEnd ( enttxsol , engine_resources );

			// update FMOD system
			AudioAPI::Update ();

			// change scene if any
			if ( scene_to_change != "nil" )
			{
				std::cout << "Attempting to change scene to " << scene_to_change << std::endl;

				editor_windows.GetWindow<EW_TilemapEditor> ()->ClearPreview ( engine_resources );
				EntityGrid& grid = engine_resources.Get<EntityGrid> ().get ();
				grid = EntityGrid ();
				auto scene_tree = editor_windows.GetWindow<EW_SceneTree> ();
				scene_tree->Reset ();
				enttxsol.ClearEntt ( engine_resources );
				enttxsol.DeserializeStateV2 ( engine_resources , scene_to_change , &grid );
				scene_tree->m_select_hierarchy_tab = true;

				SoundManager& sound_manager = engine_resources.Get<SoundManager> ().get ();
				auto& sounds = sound_manager.GetResources ();
				AudioAPI::StopAndResetAll ( sounds );

				scene_to_change = "nil";
				std::cout << "Change Scene attempt done." << std::endl;
			}

			delta_timer.EndFrame ();
			SystemTimer::EndTimeSegment ( "Overall" );
		}
	}
}