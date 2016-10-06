#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <share.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include "resource.h"
#define NO_FLAGS_SET         0
char DIRNAME[100];
int rec_read(SOCKET cs,char *buf,int nbytes)
{


    int count;
    int total;

    for(total=0;total<nbytes;total+=count)
    {
        count=recv(cs,&buf[total],nbytes-total,NO_FLAGS_SET);
        if(count<0){return -1;}
        if(count==0){return total;}
    }

    return total;
}
char databuf[4096];

int FillAddr(PSOCKADDR_IN psin,char *hostname,char *portserv)

{

    PHOSTENT phe;
    PSERVENT pse;
    psin->sin_family = AF_INET;
    phe = gethostbyname(hostname);
    if(phe == NULL) return 0;

    memcpy((char FAR *)&(psin->sin_addr), phe->h_addr,phe->h_length);


    pse = getservbyname(portserv, "tcp");
    if (pse == NULL) return 0;

    psin->sin_port = pse->s_port;
    return 1;
}

LRESULT APIENTRY GetDPSocket(
        HWND,
        UINT,
        UINT,
        LONG );


SOCKET sock;
SOCKADDR_IN dest_sin;
WSADATA WSAData;
HANDLE hInst;
int Bsize;
FILE *outdata;
char databuf[4096];
char hostname[80];
char portserv[80];


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

HWND hWnd;

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
        LPSTR lpszCmdParam, int nCmdShow)
{

    MSG         msg ;
    WNDCLASS    wndclass ; 
    char *szAppName="NetDPRequest";


    if (!hPrevInstance)
    {
        wndclass.style			= CS_HREDRAW | CS_VREDRAW ;
        wndclass.lpfnWndProc	= WndProc ;
        wndclass.cbClsExtra    	= 0 ;
        wndclass.cbWndExtra    	= 0 ;
        wndclass.hInstance     	= hInstance ;
        wndclass.hIcon         	= NULL ;
        wndclass.hCursor       	= LoadCursor (NULL, IDC_ARROW) ;
        wndclass.hbrBackground 	= NULL;
        wndclass.lpszMenuName  	= NULL;
        wndclass.lpszClassName 	= szAppName ;

        RegisterClass (&wndclass) ;
    }
    hInst=hInstance;
    hWnd = CreateWindow (
            szAppName,       		// window class name
            "NetRquest",     		// window caption
            WS_OVERLAPPEDWINDOW,    // window style
            CW_USEDEFAULT,          // initial x position
            CW_USEDEFAULT,          // initial y position
            CW_USEDEFAULT,          // initial x size
            CW_USEDEFAULT,          // initial y size
            NULL,                   // parent window handle
            NULL,                   // window menu handle
            hInstance,              // program instance handle
            NULL) ;		     		// creation parameters


    GetCurrentDirectory(100,DIRNAME);


    while (GetMessage( &msg, NULL, 0, 0 ))
    {
        TranslateMessage( &msg ) ;
        DispatchMessage( &msg ) ;
    }
    return (msg.wParam);           /* Returns the value from PostQuitMessage */

}




LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)

{

    switch (message)

    {	case WM_CREATE:

        DialogBox(hInst,"IDD_GETDPSOCKET",hWnd,GetDPSocket);
        PostQuitMessage(0);
        break;

        default:

        return DefWindowProc (hWnd, message, wParam, lParam) ;
    }

}
char dpcommand[80];
char tmpstr2[80];
char starttime[20];
char startdate[20];
char duration[10];
char channel[10];



LRESULT APIENTRY GetDPSocket(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
    int Block=0;
    HDC hdc;
    char helpdir[200];
    char stmessage[80];
    int i;
    int recsz;
    int status;
    OPENFILENAME ofn;
    static char szFile[255];
    char szFileTitle[255],szFilter[255];
    FILE *out;


    switch (message) 
    {
        case WM_INITDIALOG:
            SetDlgItemText(hDlg,IDC_HOSTNAME,"default");
            SetDlgItemText(hDlg,IDC_SOCKETNAME,"reqdtime");
            SetDlgItemText( hDlg, IDC_BSIZE,"4096");
            SetDlgItemText( hDlg,IDC_EDIT3,"BHZ");
            SetDlgItemText( hDlg,IDC_EDIT4,"97/07/18");
            SetDlgItemText( hDlg,IDC_EDIT5,"00:00:00");
            SetDlgItemText( hDlg,IDC_EDIT6,"1000");

            return (TRUE);
        case WM_CLOSE:
            EndDialog(hDlg,FALSE);
            return (TRUE);

        case WM_COMMAND: 
            {
                SetWindowText(hDlg,"Request Data From DP Socket");
                /* message: received a command */
                switch(LOWORD(wParam)) 
                {
                    case IDC_MYHELP:
                        SetWindowText(hDlg,"Request Data From DP Socket");
                        sprintf(helpdir,"%s\\netrd.hlp",DIRNAME);
                        WinHelp(hDlg,helpdir,HELP_FINDER,0);
                        break;
                    case IDC_EXIT:

                        EndDialog(hDlg, FALSE);   
                        break;
                    case IDC_OUTNAME:	

                        strcpy(szFilter,"Online Buffers(*.*)|*.*|");
                        for (i=0; szFilter[i]; i++)
                            if (szFilter[i] =='|') szFilter[i] = '\0';
                        memset(&ofn, 0, sizeof(OPENFILENAME));
                        ofn.lStructSize = sizeof(OPENFILENAME);
                        ofn.hwndOwner = hWnd;
                        ofn.lpstrFilter = szFilter;
                        ofn.nFilterIndex = 1;
                        ofn.lpstrFile = szFile;
                        ofn.nMaxFile =255;
                        ofn.lpstrFileTitle = szFileTitle;
                        ofn.nMaxFileTitle = 255;
                        ofn.lpstrInitialDir =NULL;
                        ofn.Flags=(long)NULL ;
                        GetSaveFileName(&ofn);

                        break;
                    case IDC_START:
                        SetWindowText(hDlg,"Connecting to sockect...");
                        //SetWindowText(hDlg,"Connecting to sockect...");		
                        //hdc=GetDC(GetDlgItem(hDlg,IDC_STATUS));
                        //TextOut(hdc,0,0,"Connecting to sockect...",24);

                        GetDlgItemText(hDlg,IDC_HOSTNAME,hostname,80);
                        GetDlgItemText(hDlg,IDC_SOCKETNAME,portserv,80);
                        GetDlgItemText( hDlg, IDC_BSIZE,tmpstr2,80);
                        Bsize=atoi(tmpstr2);
                        GetDlgItemText(hDlg,IDC_EDIT3,channel,80);
                        GetDlgItemText(hDlg,IDC_EDIT4,startdate,80);
                        GetDlgItemText(hDlg,IDC_EDIT5,starttime,80);
                        GetDlgItemText(hDlg,IDC_EDIT6,duration,80);
                        sprintf(dpcommand,"DATREQ %s %s %s %s",channel,startdate,starttime,duration);
                        //SetDlgItemText(hDlg,IDC_STATUS,dpcommand);
                        //if(Bsize%128!=0)
                        //{MessageBox(hDlg,"Not Valid Block Size","FATAL ERROR",MB_OK|MB_ICONSTOP);break;}


                        if ((status = WSAStartup(MAKEWORD(1,1), &WSAData)) != 0) 
                        {
                            MessageBox(hDlg,"Cann't Load Windows Socket Support","FATAL ERROR",MB_OK|MB_ICONSTOP);
                            WSACleanup();break;
                        }
                        sock = socket( AF_INET, SOCK_STREAM, 0);
                        if (sock == INVALID_SOCKET) 
                        {
                            MessageBox(hDlg,"Socket Support Failed","FATAL ERROR",MB_OK|MB_ICONSTOP);
                            closesocket(sock);WSACleanup();break;
                        }
                        if (!FillAddr(&dest_sin,hostname,portserv)) {
                            closesocket( sock );
                            WSACleanup();
                            MessageBox(hDlg,"Address Failed","FATAL ERROR",MB_OK|MB_ICONSTOP);
                            break;
                        }
                        if (connect( sock, (PSOCKADDR) &dest_sin, sizeof( dest_sin)) < 0) 
                        {
                            closesocket( sock );
                            WSACleanup();
                            MessageBox(hDlg,"Connection Failed","FATAL ERROR",MB_OK|MB_ICONSTOP);
                            break;
                        }



                        SetWindowText(hDlg,dpcommand);
                        status=send(sock,dpcommand,strlen(dpcommand),NO_FLAGS_SET );
                        if(status==SOCKET_ERROR )
                        {
                            closesocket( sock );
                            WSACleanup();
                            MessageBox(hDlg,"Writing To Socket Failed","FATAL ERROR",MB_OK|MB_ICONSTOP);
                            break;
                        }

                        if((out=fopen(szFile,"ab"))==NULL)
                        {
                            closesocket( sock );
                            WSACleanup();
                            MessageBox(hDlg,"Cann't Create Output File","FATAL ERROR",MB_OK|MB_ICONSTOP);
                            break;
                        }


                        Block==0;

                        while(1)
                        {
                            recsz=rec_read(sock,databuf,Bsize);
                            sprintf(stmessage,"Getting Block %d %d",Block,recsz);
                            SetWindowText(hDlg,stmessage);
                            if(recsz!=Bsize)break;
                            fwrite(&databuf[0],Bsize,1,out);
                            Block+=1;
                        }
                        if(recsz==0&&databuf[15]=='L'&&databuf[16]=='O'&&databuf[17]=='G')
                        {if(Block<=1)sprintf(stmessage,"LOG Data Not available");
                            else sprintf(stmessage,"%d Blocks Copied ;OK LOG",Block);
                            SetWindowText(hDlg,stmessage);
                        }
                        else 
                        {sprintf(stmessage,"%d Blocks Copied ;ERR",Block);
                            SetWindowText(hDlg,stmessage);}


                            closesocket( sock );
                            WSACleanup();
                            fclose(out);
                            //ReleaseDC(GetDlgItem(hDlg,IDC_STATUS),hdc);
                            break;
                    default : return FALSE;}


                              return (TRUE);
            }




    } 

    return (FALSE);                           /* Didn't process a message    */
    UNREFERENCED_PARAMETER(lParam);        
}






