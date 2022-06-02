// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GODECS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GODECS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GODECS_EXPORTS
#define GODECS_API __declspec(dllexport)
#else
#define GODECS_API __declspec(dllimport)
#endif

// This class is exported from the dll
class GODECS_API CgodECS {
public:
	CgodECS(void);
	// TODO: add your methods here.
};

extern GODECS_API int ngodECS;

GODECS_API int fngodECS(void);
