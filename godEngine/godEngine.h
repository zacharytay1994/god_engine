// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GODENGINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GODENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GODENGINE_EXPORTS
#define GODENGINE_API __declspec(dllexport)
#else
#define GODENGINE_API __declspec(dllimport)
#endif

//// This class is exported from the dll
//class GODENGINE_API CgodEngine {
//public:
//	CgodEngine(void);
//	// TODO: add your methods here.
//};
//
//extern GODENGINE_API int ngodEngine;
//
//GODENGINE_API int fngodEngine(void);
#include "Engine.h"
