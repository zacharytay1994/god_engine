#include "../../pch.h"
#include "Shader.h"
#include <godUtility/FileIO.h>

#include <glad/glad.h>
#pragma warning(disable: 4201)
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default: 4201)

namespace god
{
	OGLShader::OGLShader ()
	{
	}

	uint32_t OGLShader::InitializeFromFile ( char const* vsPath , char const* fsPath )
	{
		// read vertex and fragment shader file
		std::string vs_string = ReadFileToString ( vsPath );
		std::string fs_string = ReadFileToString ( fsPath );

		return InitializeFromCode ( vs_string.c_str () , fs_string.c_str () );
	}

	uint32_t OGLShader::InitializeFromCode ( char const* vsCode , char const* fsCode )
	{
		uint32_t vs , fs;
		vs = glCreateShader ( GL_VERTEX_SHADER );
		fs = glCreateShader ( GL_FRAGMENT_SHADER );

		int success;
		char infoLog[ 512 ];

		// compile vertex shader
		glShaderSource ( vs , 1 , &vsCode , NULL );
		glCompileShader ( vs );
		glGetShaderiv ( vs , GL_COMPILE_STATUS , &success );
		if ( !success )
		{
			glGetShaderInfoLog ( vs , 512 , NULL , infoLog );
			printf ( "[FAIL] ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n%s\n" , "FROM CODE" , infoLog );
		}

		// compile fragment shader
		glShaderSource ( fs , 1 , &fsCode , NULL );
		glCompileShader ( fs );
		glGetShaderiv ( fs , GL_COMPILE_STATUS , &success );
		if ( !success )
		{
			glGetShaderInfoLog ( fs , 512 , NULL , infoLog );
			printf ( "[FAIL] ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n%s\n" , "FROM CODE" , infoLog );
		}

		// compile shader program
		uint32_t shader_program = glCreateProgram ();
		glAttachShader ( shader_program , vs );
		glAttachShader ( shader_program , fs );
		glLinkProgram ( shader_program );
		glGetProgramiv ( shader_program , GL_LINK_STATUS , &success );
		if ( !success )
		{
			glGetProgramInfoLog ( shader_program , 512 , NULL , infoLog );
			printf ( "[FAIL] ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n%s\n%s\n" , "FROM CODE" , infoLog );
		}

		m_shader_program = shader_program;

		glDeleteShader ( vs );
		glDeleteShader ( fs );

		return m_shader_program;
	}

	void OGLShader::Use ()
	{
		glUseProgram ( m_shader_program );
	}

	void OGLShader::UnUse ()
	{
		glUseProgram ( 0 );
	}

	uint32_t OGLShader::GetShaderID ()
	{
		return m_shader_program;
	}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , bool val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform1i ( loc , val );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , int val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform1i ( loc , val );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , float val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform1f ( loc , val );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , float x , float y )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform2f ( loc , x , y );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , float x , float y , float z )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform3f ( loc , x , y , z );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , float x , float y , float z , float w )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform4f ( loc , x , y , z , w );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , glm::vec2 const& val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform2f ( loc , val.x , val.y );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , glm::vec3 const& val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform3f ( loc , val.x , val.y , val.z );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , glm::vec4 const& val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniform4f ( loc , val.x , val.y , val.z , val.w );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , glm::mat3 const& val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniformMatrix3fv ( loc , 1 , GL_FALSE , glm::value_ptr ( val ) );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}

	void OGLShader::SetUniform ( uint32_t ShaderProgram , char const* name , glm::mat4 const& val )
	{
		assert ( ShaderProgram != GL_UNINITIALIZED );
		GLint loc = glGetUniformLocation ( ShaderProgram , name );
		if ( loc >= 0 )
		{
			glUniformMatrix4fv ( loc , 1 , GL_FALSE , glm::value_ptr ( val ) );
		}
		else
		{
#ifdef SHADER_DEBUG
			printf ( "Uniform variable %s doesn't exist.\n" , name );
#endif
	}
		}
	}