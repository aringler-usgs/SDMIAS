
#include <windows.h>
#include <commdlg.h>
BOOL NEAR InitApplication( HANDLE ) ;
HWND NEAR InitInstance( HANDLE, int ) ;
LONG APIENTRY WndProc( HWND, UINT, UINT, LONG ) ;
LRESULT APIENTRY GetSettings(HWND, UINT, UINT, LONG);


//menu items  
#define ID_OPEN 4001
#define ID_QUERY 4002
#define ID_EXIT 4003
#define ID_START 4004
#define ID_STOP 4005
#define ID_ABOUT 4006
#define ID_FONT  4007
//dialog items   
#define IDC_BUTTON1 5001
#define IDC_STATIC -1
#define IDC_MAGN  5002
#define IDC_COMPONENT 5005
#define IDC_SFRAME 5006
#define IDC_NFRAME 5007
