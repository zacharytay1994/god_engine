#include "../pch.h"
#include "OpenGL.h"
#include "../godUtility/Scene.h"


#include "Internal/OGLDebug.h"

#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <assert.h>

#pragma warning(disable : 4201)
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default : 4201)

namespace god
{
	OpenGL::Lines OpenGL::m_lines{};

	OpenGL::OpenGL( HWND windowHandle, int width, int height )
		:
		m_screen_width{ width },
		m_screen_height{ height },
		m_window_device_context{ GetDC( windowHandle ) }
	{
		// Load opengl functions with glad
		if ( !gladLoadGL() )
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport( width, height );

		// OpenGl debug features
		int flags;
		// Features of contexts can be detected via context flags
		glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
		// Check if the context is a debug context
		if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
		{
			OGLDebug::EnableOpenGLDebugging();
			std::cout << "Debug Mode" << std::endl;
		}

		// Create flat shader
		m_flat_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/flatcolour.vs",
			"Assets/EngineAssets/OpenGLShaders/flatcolour.fs" );

		// Create textured shader
		m_textured_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs",
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs" );

		m_single_colour_outline_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.vs",
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.fs" );

		// Create cubemap shader
		m_cubemap_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/skybox.vs",
			"Assets/EngineAssets/OpenGLShaders/skybox.fs" );

		// Create depthmap shader
		m_depthmap_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/depthmap.vs",
			"Assets/EngineAssets/OpenGLShaders/depthmap.fs"
		);

		// Calculation for shadow map
		m_light_space_matrix =
			glm::ortho( -10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f ) *
			glm::lookAt(
			glm::vec3( 10.0f, 10.0f, 10.0f ),
			glm::vec3( 0.0f, 0.0f, 0.0f ),
			glm::vec3( 0.0f, 1.0f, 0.0f ) );

		// opengl settings
		// glEnable( GL_CULL_FACE );
		// Enables the Depth Buffer
		glEnable( GL_DEPTH_TEST );
		// Enables the Stencil Buffer
		glEnable( GL_STENCIL_TEST );
		// Sets rules for outcomes of stecil tests
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

		// glEnable ( GL_DEPTH_TEST );
		SetLineWidth( m_default_line_size );

		// Temp line rendering code 
		constexpr int MAX_POINTS{ 1000 };
		char const line_vs[] = {
			"#version 430 core\n"
			"layout ( location = 0 ) in vec3 aPos;\n"
			"layout ( location = 4 ) in vec4 aColour;\n"
			"out vec4 vColour;\n"
			"uniform mat4 uProjection;\n"
			"uniform mat4 uView;\n"
			"void main ()\n"
			"{\n"
			"gl_Position = uProjection * uView * vec4(aPos, 1.0);\n"
			"vColour = aColour;\n"
			"}\n" };

		char const line_fs[] = {
			"#version 430 core\n"
			"in vec4 vColour;\n"
			"out vec4 fFragColor;\n"
			"void main ()\n"
			"{\n"
			"fFragColor = vColour;\n"
			"}\n" };

		m_line_shader.InitializeFromCode( line_vs, line_fs );
		m_lines_mesh.m_vertices.resize( MAX_POINTS );
		m_lines_mesh.m_indices.resize( MAX_POINTS );
		m_lines_mesh.Initialize();


		// Creating cube map
		m_cubemap.Initialize();
		std::string skybox_model_path = "Assets/GameAssets/2DAssets/Raw/Skybox/";
		// All the faces of the cubemap (make sure they are in this exact order)
		std::string facesCubemap[6] =
		{
			skybox_model_path + "right.jpg",
			skybox_model_path + "left.jpg",
			skybox_model_path + "top.jpg",
			skybox_model_path + "bottom.jpg",
			skybox_model_path + "front.jpg",
			skybox_model_path + "back.jpg"
		};
		m_cubemap.CubeTexture( facesCubemap );

		// Creating shadow map
		m_shadowmap.Initialize( 2048, 2048 );

		glCheckError();
		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL()
	{
		// wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour() const
	{
		glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	}

	void OpenGL::BuildOGLModels( Asset3DManager const& asset3DManager )
	{
		// Clear current mesh list
		m_models.clear();

		// Copy meshes
		for ( auto const& asset : asset3DManager.GetResources() )
		{
			m_models.emplace_back();
			BuildOGLMeshesFromAssimpMeshes( m_models.back(), std::get<1>( asset ).m_model.m_meshes );
		}

		// Copy mesh ids
		m_model_ids = asset3DManager.GetIDs();
	}

	void OpenGL::UpdateOGLModel( ResourceID id, Asset3DManager const& asset3DManager )
	{
		m_models[id].clear();
		BuildOGLMeshesFromAssimpMeshes( m_models[id], std::get<1>( asset3DManager.Get( id ) ).m_model.m_meshes );
		// Copy mesh ids
		m_model_ids = asset3DManager.GetIDs();
	}

	void OpenGL::RenderScene( Scene const& scene,
							  glm::mat4 const& projection, glm::mat4 const& view, glm::vec3 const& camera_position, OGLTextureManager& textures )
	{
		glViewport( 0, 0, m_screen_width, m_screen_height );
		ClearColour();

		for ( auto const& data : scene.m_instanced_render_data )
		{
			// Make it so the stencil test always passes
			glStencilFunc ( GL_ALWAYS , 1 , 0xFF );
			// Enable modifying of the stencil buffer
			glStencilMask ( 0xFF );

			// Draw the normal model
			m_textured_shader.Use ();

			// projection matrix
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uProjection" , projection );

			// view matrix
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uView" , view );

			// set uniforms for fragment shader
			// set view position
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uViewPosition" , camera_position );

			// Set uniform for shadowmap lighting
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLightSpaceMatrix" , m_light_space_matrix );

			// set material
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( data.first.m_diffuse_id ) ).Bind ( 0 );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
			std::get<1> ( textures.Get ( data.first.m_specular_id ) ).Bind ( 1 );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.shininess" , data.first.m_shininess );

			// Set reflection
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uSkybox" , 2 );
			m_cubemap.Bind ( 2 );

			// Set shadowmap
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uShadowMap" , 3 );
			m_shadowmap.Bind ( 3 );

			// set light
			OGLLight light;
			light.m_position = { 0.0f, 100.0f, 0.0f };
			light.m_ambient = { 0.8f, 0.8f, 0.8f };
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.position" , light.m_position );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.colour" , light.m_colour );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.ambient" , light.m_ambient );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.diffuse" , light.m_diffuse );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.specular" , light.m_specular );

			// draw model
			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}

			// Make it so only the pixels without the value 1 pass the test
			glStencilFunc ( GL_NOTEQUAL , 1 , 0xFF );
			// Disable modifying of the stencil buffer
			glStencilMask ( 0x00 );
			// Disable the depth buffer
			glDisable ( GL_DEPTH_TEST );

			m_single_colour_outline_shader.Use ();
			OGLShader::SetUniform ( m_single_colour_outline_shader.GetShaderID () , "uProjection" , projection );
			OGLShader::SetUniform ( m_single_colour_outline_shader.GetShaderID () , "uView" , view );
			OGLShader::SetUniform ( m_single_colour_outline_shader.GetShaderID () , "uOutlining" , 0.01f );

			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}

			// Enable modifying of the stencil buffer
			glStencilMask ( 0xFF );
			// Clear stencil buffer
			glStencilFunc ( GL_ALWAYS , 0 , 0xFF );
			// Enable the depth buffer
			glEnable ( GL_DEPTH_TEST );
		}

		// Draw skybox as last
		m_cubemap.CubeMapEnableDepth();
		m_cubemap_shader.Use();
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "view", view );
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "projection", projection );
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "camera", camera_position );
		m_cubemap.Bind();
		m_cubemap.Draw();
		m_cubemap.UnBind();
		m_cubemap.CubeMapDisableDepth();

		// Unuse the bound shader
		OGLShader::UnUse();
	}

	void OpenGL::ResizeViewport( int width, int height )
	{
		m_screen_width = width;
		m_screen_height = height;
		glViewport( 0, 0, width, height );
		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
	}

	void OpenGL::DrawLine( glm::vec3 const& a, glm::vec3 const& b, glm::vec4 const& c, float size )
	{
		m_lines[size].push_back( { a, b, c, size } );
	}

	void OpenGL::RenderLines( glm::mat4 const& projection, glm::mat4 const& view )
	{
		m_line_shader.Use();

		m_line_shader.SetUniform( m_line_shader.GetShaderID(), "uProjection", projection );
		m_line_shader.SetUniform( m_line_shader.GetShaderID(), "uView", view );

		for ( auto const& line_group : m_lines )
		{
			m_lines_mesh.m_indices.clear();
			m_lines_mesh.m_vertices.clear();

			for ( auto const& line : line_group.second )
			{
				OGLMesh::OGLVertex start, end;
				start.m_position = std::get<0>( line );
				end.m_position = std::get<1>( line );
				start.m_colour = std::get<2>( line );
				end.m_colour = std::get<2>( line );

				m_lines_mesh.m_indices.push_back( static_cast< Index3D >( m_lines_mesh.m_vertices.size() ) );
				m_lines_mesh.m_vertices.push_back( start );
				m_lines_mesh.m_indices.push_back( static_cast< Index3D >( m_lines_mesh.m_vertices.size() ) );
				m_lines_mesh.m_vertices.push_back( end );
			}

			m_lines_mesh.SetData();
			SetLineWidth( line_group.first );
			m_lines_mesh.Draw( GL_LINES );
		}

		OGLShader::UnUse();
		m_lines.clear();
	}

	void OpenGL::SetLineWidth( float size )
	{
		glLineWidth( size );
	}

	OGLMesh OpenGL::BuildOGLMeshFromAssimpMesh( Mesh3D const& mesh3d ) const
	{
		OGLMesh mesh;

		// copy vertices
		mesh.m_vertices.resize( mesh3d.m_vertices.size() );
		for ( auto i = 0; i < mesh3d.m_vertices.size(); ++i )
		{
			mesh.m_vertices[i].m_position = mesh3d.m_vertices[i].m_position;
			mesh.m_vertices[i].m_uv = mesh3d.m_vertices[i].m_uv;
			mesh.m_vertices[i].m_tangent = mesh3d.m_vertices[i].m_tangent;
			mesh.m_vertices[i].m_normal = mesh3d.m_vertices[i].m_normal;
			mesh.m_vertices[i].m_colour = mesh3d.m_vertices[i].m_colour;
		}

		// copy indices
		mesh.m_indices = mesh3d.m_indices;

		mesh.Initialize();

		return mesh;
	}

	void OpenGL::BuildOGLMeshesFromAssimpMeshes( std::vector<OGLMesh>& oglMeshes, std::vector<Mesh3D> const& meshes3D ) const
	{
		for ( auto& mesh : meshes3D )
		{
			oglMeshes.emplace_back( BuildOGLMeshFromAssimpMesh( mesh ) );
		}
	}

	void OpenGL::FirstPassRenderToDepthmap( Scene const& scene, glm::mat4 const& projection, glm::mat4 const& view, glm::vec3 const& camera_position, OGLTextureManager& textures )
	{
		m_shadowmap.EnableDepthMap();
		for ( auto const& data : scene.m_instanced_render_data )
		{
			// Draw the normal model
			m_depthmap_shader.Use ();

			OGLShader::SetUniform ( m_depthmap_shader.GetShaderID () ,
				"uLightSpaceMatrix" ,
				m_light_space_matrix );

			// Draw model
			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}
		m_shadowmap.DisableDepthMap();
		glCheckError();
	}
}