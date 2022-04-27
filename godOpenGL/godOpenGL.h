// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GODOPENGL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GODOPENGL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GODOPENGL_EXPORTS
#define GODOPENGL_API __declspec(dllexport)
#else
#define GODOPENGL_API __declspec(dllimport)
#endif

// This class is exported from the dll
class GODOPENGL_API CgodOpenGL {
public:
	CgodOpenGL(void);
	// TODO: add your methods here.
};

extern GODOPENGL_API int ngodOpenGL;

GODOPENGL_API int fngodOpenGL(void);
