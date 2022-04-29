// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GODEDITOR_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GODEDITOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GODEDITOR_EXPORTS
#define GODEDITOR_API __declspec(dllexport)
#else
#define GODEDITOR_API __declspec(dllimport)
#endif

// This class is exported from the dll
class GODEDITOR_API CgodEditor {
public:
	CgodEditor(void);
	// TODO: add your methods here.
};

extern GODEDITOR_API int ngodEditor;

GODEDITOR_API int fngodEditor(void);
