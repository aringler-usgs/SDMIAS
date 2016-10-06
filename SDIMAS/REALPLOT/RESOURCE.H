BOOL NEAR InitApplication( HANDLE ) ;
HWND NEAR InitInstance( HANDLE, int ) ;
LONG APIENTRY WndProc( HWND, UINT, UINT, LONG ) ;
LRESULT APIENTRY GetSettings(HWND, UINT, UINT, LONG);


#define IDC_OK                     1003
#define IDC_OPEN                     1004
#define IDC_MIN                       1005
#define IDC_LIN                       1006
#define IDC_CHAN                      1007
#define IDC_MAG                       1008
#define IDC_FONT                     1009
#define ID_PLOTINFO            40001
#define ID_OPENBUFFER          40002
#define ID_RESCALE             40003
#define ID_EXIT                40004
#define ID_START                    40005
#define ID_STOP                     40006
#define IDC_STATIC                      -1
#define IDC_RADIO0                      1010
#define IDC_RADIO1                      1011
#define IDC_RADIO2                      1012
#define IDC_RADIO3                      1013
#define IDC_RADIO4                      1014
#define ID_HELP               40010
