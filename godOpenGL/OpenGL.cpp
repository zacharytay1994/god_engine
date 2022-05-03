#include "pch.h"
#include "OpenGL.h"

#pragma comment (lib, "opengl32.lib")

#include <iostream>
#include <assert.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

namespace god
{
	OGLRenderData::OGLRenderData ( glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale )
		:
		m_position { position } ,
		m_rotation { rotation } ,
		m_scale { scale } ,
		m_active { true }
	{
	}

	OpenGL::OpenGL ( HWND windowHandle , int width , int height )
		:
		m_window_device_context { GetDC ( windowHandle ) }
	{
		// customize pixel format
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof ( PIXELFORMATDESCRIPTOR ),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	//Flags
			PFD_TYPE_RGBA,												// The kind of framebuffer. RGBA or palette.
			32,															// Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,															// Number of bits for the depthbuffer
			8,															// Number of bits for the stencilbuffer
			0,															// Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		// select pixel format
		m_pixel_format = ChoosePixelFormat ( m_window_device_context , &pfd );
		SetPixelFormat ( m_window_device_context , m_pixel_format , &pfd );

		// create opengl context
		m_opengl_rendering_context = wglCreateContext ( m_window_device_context );
		wglMakeCurrent ( m_window_device_context , m_opengl_rendering_context );

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

		// initialize cube mesh
		m_cube.m_vertices = OGLMesh::OGLVertices {
			// front face
			{{0.5f, 0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}},
			{{-0.5f, -0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.5f},	{0.0f, 0.0f, 1.0f}},
			// back face
			{{0.5f, 0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}},
			{{0.5f, -0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}},
			{{-0.5f, -0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}},
			{{-0.5f, 0.5f, -0.5f},	{0.0f, 0.0f, -1.0f}},
			// right face
			{{0.5f, 0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}}, // 8
			{{0.5f, -0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}}, // 9
			{{0.5f, 0.5f, 0.5f},	{1.0f, 0.0f, 0.0f}}, // 10
			{{0.5f, -0.5f, 0.5f},	{1.0f, 0.0f, 0.0f}}, // 11
			// left face
			{{-0.5f, 0.5f, -0.5f},	{-1.0f, 0.0f, 0.0f}}, // 12
			{{-0.5f, -0.5f, -0.5f},	{-1.0f, 0.0f, 0.0f}}, // 13
			{{-0.5f, 0.5f, 0.5f},	{-1.0f, 0.0f, 0.0f}}, // 14
			{{-0.5f, -0.5f, 0.5f},	{-1.0f, 0.0f, 0.0f}}, // 15
			// bottom face
			{{0.5f, -0.5f, 0.5f},	{0.0f, -1.0f, 0.0f}}, // 16
			{{0.5f, -0.5f, -0.5f},	{0.0f, -1.0f, 0.0f}}, // 17
			{{-0.5f, -0.5f, 0.5f},	{0.0f, -1.0f, 0.0f}}, // 18
			{{-0.5f, -0.5f, -0.5f},	{0.0f, -1.0f, 0.0f}}, // 19
			// top face
			{{0.5f, 0.5f, 0.5f},	{0.0f, 1.0f, 0.0f}}, // 20
			{{0.5f, 0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}}, // 21
			{{-0.5f, 0.5f, 0.5f},	{0.0f, 1.0f, 0.0f}}, // 22
			{{-0.5f, 0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}}, // 23
		};

		m_cube.m_indices = OGLMesh::OGLIndices {
			3, 1, 0,
			3, 2, 1,

			4, 5, 7,
			5, 6, 7,

			10, 11, 8,
			11, 9, 8,

			12, 15, 14,
			12, 13, 15,

			18, 19, 16,
			19, 17, 16,

			20, 23, 22,
			20, 21, 23
		};

		m_cube.Initialize ();

		// opengl settings
		glEnable ( GL_CULL_FACE );
		glEnable ( GL_DEPTH_TEST );

		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL ()
	{
		wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour () const
	{
		glClearColor ( 0.2f , 0.3f , 0.3f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGL::FrameRender ( glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position )
	{
		m_flat_shader.Use ();

		// projection matrix
		OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uProjection" , projection );

		// view matrix
		OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uView" , view );

		for ( auto const& data : m_render_data )
		{
			if ( data.m_active )
			{
				// set uniforms for vertex shader
				// model matrix
				glm::mat4 model_matrix = glm::mat4 ( 1.0f );
				model_matrix = glm::translate ( model_matrix , data.m_position );
				model_matrix = glm::rotate ( model_matrix , data.m_rotation.x , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
				model_matrix = glm::rotate ( model_matrix , data.m_rotation.y , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
				model_matrix = glm::rotate ( model_matrix , data.m_rotation.z , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
				model_matrix = glm::scale ( model_matrix , data.m_scale );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uModel" , model_matrix );

				// set uniforms for fragment shader
				// set view position
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uViewPosition" , camera_position );

				// set material
				OGLMaterial material;
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.ambient" , material.m_ambient );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.diffuse" , material.m_diffuse );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.specular" , material.m_specular );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.shininess" , material.m_shininess );

				// set light
				OGLLight light;
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.position" , light.m_position );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.colour" , light.m_colour );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.ambient" , light.m_ambient );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.diffuse" , light.m_diffuse );
				OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.specular" , light.m_specular );

				// draw cube
				m_cube.Draw ( GL_TRIANGLES );
			}
		}

		// unuse the bound shader
		OGLShader::UnUse ();
	}

	OGLEntityID OpenGL::AddCube ( glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale )
	{
		if ( m_free_render_data.empty () )
		{
			m_render_data.emplace_back ( position , rotation , scale );
			return static_cast< OGLEntityID >( m_render_data.size () - 1 );
		}
		OGLEntityID free_id = m_free_render_data.top ();
		m_free_render_data.pop ();

		m_render_data[ free_id ].m_position = position;
		m_render_data[ free_id ].m_rotation = rotation;
		m_render_data[ free_id ].m_scale = scale;
		m_render_data[ free_id ].m_active = true;

		return free_id;
	}

	void OpenGL::RemoveCube ( OGLEntityID id )
	{
		assert ( id < m_render_data.size () );
		m_free_render_data.push ( id );
		m_render_data[ id ].m_active = false;
	}

	OGLRenderData& OpenGL::GetCube ( OGLEntityID id )
	{
		// check if valid id and entity active
		assert ( id < m_render_data.size () && m_render_data[ id ].m_active );
		return m_render_data[ id ];
	}

	void OpenGL::ResizeViewport ( int width , int height ) const
	{
		glViewport ( 0 , 0 , width , height );
		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
	}
}