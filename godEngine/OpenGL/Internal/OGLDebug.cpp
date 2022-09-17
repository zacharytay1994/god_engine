#include "../../pch.h"
#include "OGLDebug.h"

#include <assert.h> 

namespace OGLDebug
{
	GLenum glCheckError_( const char* function, int line )
	{
		GLenum error_code_;
		while ( ( ( error_code_ = glGetError() ) != GL_NO_ERROR ) )
		{
			std::string error_{};
			switch ( error_code_ )
			{
			case GL_INVALID_ENUM:
				error_ = "INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				error_ = "INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				error_ = "INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				error_ = "STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				error_ = "STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				error_ = "OUT_OF_MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error_ = "INVALID_FRAMEBUFFER_OPERATION";
				break;
			}

			std::cout
				<< "[OpenGL Error] " << error_ << std::endl
				<< " Function : " << function << std::endl
				<< " Line : " << line << std::endl;
		}
		return error_code_;
	}

	// APIENTRY is an alias for WINAPI.
	void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam )
	{
		UNREFERENCED_PARAMETER( userParam );
		UNREFERENCED_PARAMETER( length );
		// ignore these non-significant error codes
		if ( id == 131169 || id == 131185 || id == 131218 || id == 131204 ) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch ( source )
		{
		case GL_DEBUG_SOURCE_API:
			std::cout << "Source: API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			std::cout << "Source: Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			std::cout << "Source: Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			std::cout << "Source: Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			std::cout << "Source: Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			std::cout << "Source: Other";
			break;
		} std::cout << std::endl;

		switch ( type )
		{
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "Type: Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "Type: Deprecated Behaviour";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "Type: Undefined Behaviour";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "Type: Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "Type: Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			std::cout << "Type: Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			std::cout << "Type: Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			std::cout << "Type: Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "Type: Other";
			break;
		}
		std::cout << std::endl;

		switch ( severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "Severity: high";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "Severity: medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "Severity: low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cout << "Severity: notification";
			break;
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}

	// enable OpenGL debug context if context allows for debug context
	void EnableOpenGLDebugging()
	{
		glEnable( GL_DEBUG_OUTPUT );
		// makes sure errors are displayed synchronously
		// it tells OpenGL to directly call the callback function the moment an error occurred
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( glDebugOutput, nullptr );
		// filter the type(s) of errors
		glDebugMessageControl( GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE );
	}
}