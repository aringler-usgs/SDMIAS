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
typedef struct {
    char HeadInfo[3];
    int CurrPoz,
        OnlinSize,
        BlockSize;
} 
information;
int rec_read(SOCKET cs,char *buf,int nbytes)
{
    int count;
    int total;

    for(total=0;total<nbytes;total+=count)
    {
        count=recv(cs,&buf[total],nbytes-total,NO_FLAGS_SET);
        if(count<0)return -1;
        if(count==0)return total;
    }
    return total;
}

int FillAddr(PSOCKADDR_IN,char *,char *);

typedef struct 
{
    int   nThread;  
    HWND  hwnd;
} 
THREADPACK, *PTHREADPACK;

void AcceptThreadProc(PTHREADPACK);
LRESULT APIENTRY GetDPSocket(
        HWND,
        UINT,
        UINT,
        LONG );


int stop=1;
SOCKET sock;
SOCKADDR_IN dest_sin;
WSADATA WSAData;
HANDLE hInst;
int outdata;
char databuf[4096];
int event=1;
char hostname[80];
char portserv[80];
information info;

int reconnect(void)
{
    int status;
    Sleep(100);
    if ((status = WSAStartup(MAKEWORD(1,1), &WSAData)) != 0) 
    {WSACleanup();return -1;}
    sock = socket( AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) 
    {	closesocket(sock);
        WSACleanup();return -1;}
        if (!FillAddr(&dest_sin,hostname,portserv)) 
        {
            closesocket( sock );
            WSACleanup();return -1;}
            if (connect( sock, (PSOCKADDR) &dest_sin, sizeof( dest_sin)) != 0) 
            {closesocket( sock );
                WSACleanup();return -1;}
                return 0;
}


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

HWND hWnd;

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
        LPSTR lpszCmdParam, int nCmdShow)
{

    MSG         msg ;
    WNDCLASS    wndclass ; 

    char *szAppName="GetDpSocket";


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
            "SCANSCSI",     		// window caption
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






typedef struct {char asl[3];int Head[3];} header;
header aaa;

LRESULT APIENTRY GetDPSocket(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	


    char tmpstr1[80];
    char tmpstr2[80];
    char helpdir[200];
    int i;
    int status;
    OPENFILENAME ofn;
    static char szFile[255];
    char szFileTitle[255],szFilter[255];
    static THREADPACK tp;	
    FILE *out;


    switch (message) 
    {
        case WM_INITDIALOG:
            info.HeadInfo[0]='A';
            info.HeadInfo[1]='S';
            info.HeadInfo[2]='L';
            SetDlgItemText(hDlg,IDC_HOSTNAME,"default");
            SetDlgItemText(hDlg,IDC_SOCKETNAME,"realtime");
            SetDlgItemText( hDlg, IDC_OSIZE,"100000");
            SetDlgItemText( hDlg, IDC_BSIZE,"512");
            SendDlgItemMessage(hDlg,IDC_ALL,BM_SETCHECK ,(WPARAM)1,(LPARAM)0);
            return (TRUE);
        case  WM_CLOSE:
            if(stop==0)
                return (TRUE);
            EndDialog(hDlg, FALSE);   
            return (TRUE);

        case WM_COMMAND: 
            {     
                /* message: received a command */
                switch(LOWORD(wParam)) 
                {
                    case IDC_MYHELP:
                        sprintf(helpdir,"%s\\realtime.hlp",DIRNAME);
                        WinHelp(hDlg,helpdir,HELP_FINDER,0);
                        break;

                    case IDC_EXIT:
                        EndDialog(hDlg, FALSE);   
                        break;
                    case IDC_STOP:
                        for(i=1001;i<=1013;i++)
                            EnableWindow(GetDlgItem(hDlg,i),TRUE);
                        stop=1;
                        break;
                    case IDC_OUTNAME:	
                        strcpy(szFilter,"Online Buffers(*.asl)|*.asl|");
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

                        GetDlgItemText(hDlg,IDC_HOSTNAME,hostname,80);
                        GetDlgItemText(hDlg,IDC_SOCKETNAME,portserv,80);
                        GetDlgItemText( hDlg, IDC_OSIZE,tmpstr1,80);
                        GetDlgItemText( hDlg, IDC_BSIZE,tmpstr2,80);
                        event=SendDlgItemMessage(hDlg,IDC_EVENT,BM_GETCHECK ,(WPARAM)0,(LPARAM)0);
                        if(strlen(hostname)==0||strlen(portserv)==0||
                                (info.BlockSize=atoi(tmpstr2))==0||
                                (info.OnlinSize=atoi(tmpstr1))==0||
                                strlen(szFile)==0)
                        {MessageBox(hDlg,"Check If All Input Params Are Valid","Message",MB_OK);
                            break;}
                            tp.nThread = 0;
                            tp.hwnd = hDlg;

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
                                switch(WSAGetLastError())
                                {
                                    case WSANOTINITIALISED:MessageBox(hDlg,"	A successful WSAStartup must occur before using this function.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAENETDOWN	:MessageBox(hDlg,"The Windows Sockets implementation has detected that the network subsystem has failed.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEADDRINUSE:	MessageBox(hDlg,"The specified address is already in use.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEINTR:	MessageBox(hDlg,"The (blocking) call was canceled using WSACancelBlockingCall.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEINPROGRESS:	MessageBox(hDlg,"A blocking Windows Sockets call is in progress.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEADDRNOTAVAIL:	MessageBox(hDlg,"The specified address is not available from the local computer.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEAFNOSUPPORT:	MessageBox(hDlg,"Addresses in the specified family cannot be used with this socket.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAECONNREFUSED:	MessageBox(hDlg,"The attempt to connect was forcefully rejected.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEDESTADDRREQ:	MessageBox(hDlg,"A destination address is required.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEFAULT:	MessageBox(hDlg,"The namelen argument is incorrect.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEINVAL:	MessageBox(hDlg,"The socket is not already bound to an address.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEISCONN:	MessageBox(hDlg,"The socket is already connected.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEMFILE:MessageBox(hDlg,"	No more file descriptors are available.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAENETUNREACH:	MessageBox(hDlg,"The network can't be reached from this host at this time.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAENOBUFS:	MessageBox(hDlg,"No buffer space is available. The socket cannot be connected.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAENOTSOCK:	MessageBox(hDlg,"The descriptor is not a socket.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAETIMEDOUT:	MessageBox(hDlg,"Attempt to connect timed out without establishing a connection.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    case WSAEWOULDBLOCK: 	MessageBox(hDlg,"The socket is marked as nonblocking and the connection cannot be completed immediately. It is possible to select the socket while it is connecting by selecting it for writing.","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                    default :MessageBox(hDlg,"Unnown","FATAL ERROR",MB_OK|MB_ICONSTOP);break;
                                }

                                closesocket( sock );
                                WSACleanup();
                                MessageBox(hDlg,"Connection Failed","FATAL ERROR",MB_OK|MB_ICONSTOP);
                                break;
                            }
METKA:
                            if((outdata=_sopen(szFile,_O_RDWR|_O_BINARY,_SH_DENYWR))==-1)
                            {
                                if(MessageBox(hDlg,"Cann't open Online Buffer. Create New ?","Message",MB_OKCANCEL|MB_ICONINFORMATION)==IDOK)
                                {if((out=fopen(szFile,"w"))==NULL)
                                    {MessageBox(hDlg,"Cann't Open Onlin Buffer","File Open Error",MB_OK|MB_ICONSTOP);
                                        break;
                                    }
                                    else {fclose(out);goto METKA;}
                                }
                                else break;}


                                if(_read(outdata,&aaa.asl[0],15)==15)
                                {
                                    if(aaa.asl[0]!='A'||aaa.asl[1]!='S'||aaa.asl[2]!='L')
                                    {MessageBox(hDlg,"You trying to open File which is not Online buffer","FATAL ERROR",MB_OK|MB_ICONSTOP);_close(outdata);
                                        break;
                                    }

                                    if(aaa.Head[1]!=info.OnlinSize||aaa.Head[2]!=info.BlockSize)
                                    {MessageBox(hDlg,"Buffer Size or Block Size are different from previous","FATAL ERROR",MB_OK|MB_ICONSTOP);_close(outdata);
                                        break;
                                    }
                                    info.CurrPoz=aaa.Head[0];
                                    if(_lseek(outdata,info.CurrPoz*info.BlockSize+15,SEEK_SET)!=info.CurrPoz*info.BlockSize+15)
                                    {MessageBox(hDlg,"You are trying to Open Corrupted Online Buffer","FATAL ERROR",MB_OK|MB_ICONSTOP);_close(outdata);
                                        break;
                                    }

                                }
                                else info.CurrPoz=0;







                                stop=0;
                                tp.hwnd=hDlg;
                                tp.nThread = 0;
                                _beginthread(AcceptThreadProc, 0, &tp);

                                for(i=1001;i<=1013;i++)
                                    EnableWindow(GetDlgItem(hDlg,i),FALSE);
                                break;
                    default:
                                return FALSE;}




                                return (TRUE);
            }
    } 

    return (FALSE);                           /* Didn't process a message    */
    UNREFERENCED_PARAMETER(lParam);        
}


void AcceptThreadProc(PTHREADPACK ptp)
{
    char message[80];
    fd_set f1;
    int status;
    struct timeval t1={10000,0};
    int recount=0;

    while(!stop)
    {	

        f1.fd_count=1;
        f1.fd_array[0]=sock;


        if(select((int)NULL,&f1,NULL,NULL,&t1)!=1)
        {
            closesocket(sock);
            WSACleanup();
            while(reconnect()<0&&stop==0)
            {
                sprintf(message,"Reconnecting %d",recount++);
                SetDlgItemText(ptp->hwnd,IDC_STATUS,message);	
            }
            recount=0;
            continue;
        }

        status=rec_read(sock,databuf,info.BlockSize);
        //status = recv(sock,databuf,info.BlockSize,NO_FLAGS_SET );

        if(status!=info.BlockSize)
        {
            closesocket(sock);
            WSACleanup();
            while(reconnect()<0&&stop==0){
                sprintf(message,"Reconnecting %d",recount++);
                SetDlgItemText(ptp->hwnd,IDC_STATUS,message);	
            }
            recount=0;
            continue;
        }

        else 
        {
            memcpy(message,databuf,20);
            message[20]=0;
            SetDlgItemText(ptp->hwnd,IDC_STATUS,message);
            if(databuf[6]!='D')continue;
            if((event==1)&&((databuf[36]&76)==0))continue;
            _lseek(outdata,0,SEEK_SET);
            _write(outdata,&info.HeadInfo[0],15);
            _lseek(outdata,info.CurrPoz*info.BlockSize+15,SEEK_SET);
            //if((event==1)&&((databuf[36]&76)==0))continue;
            if(_write(outdata,&databuf[0],info.BlockSize)!=info.BlockSize)
            {
                MessageBox(ptp->hwnd,"Cann't Write Block To Online buffer ","FATAL ERROR",MB_OK|MB_ICONSTOP);
                closesocket( sock );
                WSACleanup();
                stop=1;
                _close(outdata);

                _endthread();
            }
            _close(_dup(outdata));
            info.CurrPoz+=1;
            info.CurrPoz%=info.OnlinSize;
        }

    }
    WSACleanup();
    stop=1;
    _close(outdata);
    _endthread();
}


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


