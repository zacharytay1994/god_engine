#include "../pch.h"
#include "OpenGL.h"
#include "../godUtility/Scene.h"

#pragma comment (lib, "opengl32.lib")

#include <iostream>
#include <assert.h>

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default: 4201)

namespace god
{
	OpenGL::OpenGL ( HWND windowHandle , int width , int height )
		:
		m_window_device_context { GetDC ( windowHandle ) }
	{
		// load opengl functions with glad
		if ( !gladLoadGL () )
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport ( width , height );

		// create flat shader
		m_flat_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/flatcolour.vs" ,
			"Assets/EngineAssets/OpenGLShaders/flatcolour.fs" );

		// create textured shader
		m_textured_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs"
		);

		m_single_colour_outline_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.vs",
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.fs"
		);


		// opengl settings
		glEnable ( GL_CULL_FACE );
		// Enables the Depth Buffer
		glEnable( GL_DEPTH_TEST );
		// Enables the Stencil Buffer
		glEnable( GL_STENCIL_TEST );
		// Sets rules for outcomes of stecil tests
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );


		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL ()
	{
		//wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour () const
	{
		glClearColor ( 0.2f , 0.2f , 0.2f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	}

	void OpenGL::BuildOGLModels ( Asset3DManager const& asset3DManager )
	{
		// clear current mesh list
		m_models.clear ();

		// copy meshes
		for ( auto const& asset : asset3DManager.GetResources () )
		{
			m_models.emplace_back ();
			BuildOGLMeshesFromAssimpMeshes ( m_models.back () , std::get<1> ( asset ).m_model.m_meshes );
		}

		// copy mesh ids
		m_model_ids = asset3DManager.GetIDs ();
	}

	void OpenGL::UpdateOGLModel ( ResourceID id , Asset3DManager const& asset3DManager )
	{
		m_models[ id ].clear ();
		BuildOGLMeshesFromAssimpMeshes ( m_models[ id ] , std::get<1> ( asset3DManager.Get ( id ) ).m_model.m_meshes );
		// copy mesh ids
		m_model_ids = asset3DManager.GetIDs ();
	}

	void OpenGL::RenderScene ( Scene const& scene ,
		glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures )
	{
		ClearColour();
		//use the shader
		//m_flat_shader.Use ();

		for ( auto const& data : scene.m_render_data )
		{
			if ( data.Active () )
			{
				// Make it so the stencil test always passes
				glStencilFunc( GL_ALWAYS, 1, 0xFF );
				// Enable modifying of the stencil buffer
				glStencilMask( 0xFF );

				// Draw the normal model
				m_textured_shader.Use ();

				// projection matrix
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uProjection" , projection );

				// view matrix
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uView" , view );

				// set uniforms for vertex shader
				// set model transform
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uModel" , data.m_model_transform );

				// set uniforms for fragment shader
				// set view position
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uViewPosition" , camera_position );

				// set material
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
				std::get<1> ( textures.Get ( data.m_diffuse_id ) ).Bind ( 0 );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
				std::get<1> ( textures.Get ( data.m_specular_id ) ).Bind ( 1 );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.shininess" , data.m_shininess );

				// set light
				OGLLight light;
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.position" , light.m_position );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.colour" , light.m_colour );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.ambient" , light.m_ambient );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.diffuse" , light.m_diffuse );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.specular" , light.m_specular );

				// draw model
				for ( auto const& mesh : m_models[ data.m_model_id ] )
				{
					mesh.Draw ( GL_TRIANGLES );
				}

				// Make it so only the pixels without the value 1 pass the test
				glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
				// Disable modifying of the stencil buffer
				glStencilMask( 0x00 );
				// Disable the depth buffer
				glDisable( GL_DEPTH_TEST );

				m_single_colour_outline_shader.Use();
				OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uProjection", projection );
				OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uView", view );
				OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uModel", data.m_model_transform );
				OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uOutlining", 0.01f );

				for ( auto const& mesh : m_models[data.m_model_id] )
				{
					mesh.Draw( GL_TRIANGLES );
				}

				// Enable modifying of the stencil buffer
				glStencilMask( 0xFF );
				// Clear stencil buffer
				glStencilFunc( GL_ALWAYS, 0, 0xFF );
				// Enable the depth buffer
				glEnable( GL_DEPTH_TEST );
			}
		}

		// unuse the bound shader
		OGLShader::UnUse ();
	}

	void OpenGL::ResizeViewport ( int width , int height ) const
	{
		glViewport ( 0 , 0 , width , height );
		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
	}

	OGLMesh OpenGL::BuildOGLMeshFromAssimpMesh ( Mesh3D const& mesh3d ) const
	{
		OGLMesh mesh;

		// copy vertices
		mesh.m_vertices.resize ( mesh3d.m_vertices.size () );
		for ( auto i = 0; i < mesh3d.m_vertices.size (); ++i )
		{
			mesh.m_vertices[ i ].m_position = mesh3d.m_vertices[ i ].m_position;
			mesh.m_vertices[ i ].m_uv = mesh3d.m_vertices[ i ].m_uv;
			mesh.m_vertices[ i ].m_tangent = mesh3d.m_vertices[ i ].m_tangent;
			mesh.m_vertices[ i ].m_normal = mesh3d.m_vertices[ i ].m_normal;
			mesh.m_vertices[ i ].m_colour = mesh3d.m_vertices[ i ].m_colour;
		}

		// copy indices
		mesh.m_indices = mesh3d.m_indices;

		mesh.Initialize ();

		return mesh;
	}

	void OpenGL::BuildOGLMeshesFromAssimpMeshes ( std::vector<OGLMesh>& oglMeshes , std::vector<Mesh3D> const& meshes3D ) const
	{
		for ( auto& mesh : meshes3D )
		{
			oglMeshes.emplace_back ( BuildOGLMeshFromAssimpMesh ( mesh ) );
		}
	}
}