// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the MY2ENGINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// MY2ENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MY2ENGINE_EXPORTS
#define MY2ENGINE_API __declspec(dllexport)
#else
#define MY2ENGINE_API __declspec(dllimport)
#endif

// This class is exported from the dll
class MY2ENGINE_API CMy2Engine {
public:
	CMy2Engine(void);
	// TODO: add your methods here.
};

extern MY2ENGINE_API int nMy2Engine;

MY2ENGINE_API int fnMy2Engine(void);
