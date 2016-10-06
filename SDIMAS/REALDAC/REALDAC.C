#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <share.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include "s_time.h"
#include "steim.h"
#include "steimlib.h"
#include "miniseed.h"
#include "resource.h"
#include "resrc1.h"
#define NO_FLAGS_SET         0
int threadstart=0;
char DIRNAME[100];
FILE *indata;
int Bsize;
int Nblock;
char Data[4096];
int Currblock;


typedef struct 
	{
   	int   nThread;  
   	HWND  hwnd;
   	} 
	THREADPACK, *PTHREADPACK;

void AcceptThreadProc(PTHREADPACK);

LRESULT APIENTRY GETDACSETTINGS(
        HWND,
        UINT,
        UINT,
        LONG );

int stop=1;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HANDLE hInst;
HWND hWnd;
typedef struct {
		char HeadInfo[3];
		int CurrPoz,
		OnlinSize,
		BlockSize;
		} 
		information;

information info;
void convert (int b, unsigned char *a1,unsigned char *a2)
{

short a=0;
short c=0;
if(b>2047)b=2047;
if(b<-2047)b=-2047;
a=(a|((unsigned int)b&63));
c=(c|(((unsigned int)b>>6)&63));
a|=(c<<8);
a+=(short)0x40c0;
*a1=(unsigned char)((a>>8)&255);
*a2=(unsigned char)((a)&255);
}


LONG decode_SEED_micro_header (SEED_data_record *sdr, SHORT *firstframe, SHORT *level, SHORT *flip, SHORT *dframes)
   {
   
      SHORT             hfirstdata, hfirstblockette, h1000 ;
      LONG              headertotal ;

	  
	  *flip=1;
	  
      /*
       *
       * decide whether this computer needs to "flip" the data for internal processing.
       * note that the kind of flipping will not work for hermaphroditic computers like a PDP-11
       * that can't make up its mind. it's either intel/vax or sensible order.
       */


      
            headertotal = swapb (sdr->Head.samples_in_record) ;
            hfirstdata = swapb (sdr->Head.first_data_byte) ;
            hfirstblockette = swapb (sdr->Head.first_blockette_byte) ;
            h1000 = swapb (sdr->Head.DOB.blockette_type) ;
		
          
        
		
      if ((hfirstdata % sizeof(compressed_frame)) != 0)
        {
            //printf ("first data does not begin on frame boundary!\n") ;
            return -2;
        }
      *firstframe = hfirstdata/sizeof(compressed_frame) - 1 ;
      /*
       * if a data-only blockette is present, read the encoding format and
       * record length from it. otherwise, the caller's default level will not be changed.
       * similarly, do not overwrite the caller's default number of data frames if the
       * blockette 1000 is not present.
       */
      if ((hfirstblockette == 48) && (h1000 == 1000))
            {
                switch (sdr->Head.DOB.encoding_format)
                {
                    case 10 :
                        *level = 1 ;
                        break ;
                    case 11 :
                        *level = 2 ;
                        break ;
                    /*
                     * this format code must be defined by the FDSN SEED working group
                     * for level 3 compression.
                     */
                    case 20 :
                        *level = 3 ;
                        break ;
                    default :
                       // printf ("unknown encoding format %d\n",(int)sdr->Head.DOB.encoding_format) ;
                        return -3 ;
                        break ;
                }
                switch (sdr->Head.DOB.rec_length)
                {
                    case 12 :
                        *dframes = 63 ;
                        break ;
                    case 11 :
                        *dframes = 31 ;
                        break ;
                    case 10 :
                        *dframes = 15 ;
                        break ;
                   case  9 :
                        *dframes = 7 ;
                        break ;
                    case 8 :
                        *dframes = 3 ;
                        break ;
                    case 7 :
                        *dframes = 1 ;
                        break ;
                    default :
                        //printf ("unknown record length code %d\n",(int)sdr->Head.DOB.rec_length) ;
                        return -4 ;
                        break ;
                }
            }
      return (headertotal) ;
    }




    LONG validate_frames (SHORT firstframe, SHORT level, SHORT dframes) 
   {
      if ((dframes < 1) || (dframes >= MAXSEEDFRAMES))
        {
          //printf("illegal number of data frames specified!\n");
          return -11;
        }
      if ((firstframe < 0) || (firstframe > dframes))
        {
          //printf("illegal first data frame!\n");
          return -12;
        }
      if ((level < 1) || (level > 3))
        {
          //printf("illegal compression level!\n");
          return -13;
        }
    }


  LONG sreadseed(SHORT Level,SHORT Dframes,SEED_data_record *sdr,LONG *udata)
  {
     
      
      LONG                      rectotal, headertotal ;
      SHORT                     dstat ;
      SHORT                     firstframe, flip, level ;
      dcptype                   dcp[1] ;
      SHORT                     dframes ;
      int chindex;





	  for (chindex = 0; chindex < 1; chindex++)
        if ((dcp[chindex] = init_generic_decompression ()) == NULL )
            return -1;


      
  
     level = Level ;
     dframes = Dframes ;
      
     

	 headertotal = decode_SEED_micro_header (sdr, &firstframe, &level, &flip, &dframes) ;

			  if(headertotal<=0)return headertotal;
			    
            
              rectotal = decompress_generic_record ((generic_data_record *)sdr, udata, &dstat, dcp[0],
                                                    firstframe, headertotal, level, flip, dframes) ;
              
              if (dferrorfatal(dstat,NULL))
                      {
              
                        return -103 ;
                      } ;

   return rectotal;           
                }


long dec_h(SEED_data_record *sdr,char *stname,
		   char *chname,daat *d1,double *rate)
{
int yy,dd,hh,mi,ss,ts;
SHORT samples;
SHORT rfactor,rmultip;
char startdat[80]; 

if(sdr->Head.seed_record_type!='D'){
return -1;}
sprintf(stname,"%c%c%c%c%c",
sdr->Head.station_ID_call_letters [0],
sdr->Head.station_ID_call_letters [1],
sdr->Head.station_ID_call_letters [2],
sdr->Head.station_ID_call_letters [3],
sdr->Head.station_ID_call_letters [4]
);
sprintf(chname,"%c%c%c",
sdr->Head.channel_ID [0],
sdr->Head.channel_ID [1],
sdr->Head.channel_ID [2]);
if(strcmp(chname,"LOG")==0)
{return -1;}
hh=sdr->Head.starting_time.hr;
mi=sdr->Head.starting_time.minute;
ss=sdr->Head.starting_time.seconds;
dd=swapb(sdr->Head.starting_time.jday);
ts=swapb(sdr->Head.starting_time.tenth_millisec );
yy=swapb(sdr->Head.starting_time.yr);	
samples  = swapb (sdr->Head.samples_in_record) ;
rfactor=swapb(sdr->Head.sample_rate_factor);
rmultip	=swapb(sdr->Head.sample_rate_multiplier );
if(samples==0)return -1;
if(rfactor==0||rmultip==0)return -1;
if(rfactor>0&&rmultip>0)*rate=(double)rfactor*rmultip;
if(rfactor>0&&rmultip<0)*rate=-(double)rfactor/rmultip;
if(rfactor<0&&rmultip>0)*rate=-(double)rmultip/rfactor;
if(rfactor<0&&rmultip<0)*rate=(double)rmultip/rfactor;
if(*rate<=0.)return -1;
else *rate=1000./(*rate);
sprintf(startdat,"%04d%03d%02d%02d%02d%04d",yy,dd,hh,mi,ss,ts);
*d1=cjstrtoda(startdat);
chname[3]=0;
stname[5]=0;
return 0;
}

char *dacchan="LHZ";
double dacdiv=1.;
int midvalue=0;
int chfilter=0;
char *chcom="COM2";
int BlockSize;
int chan=0;
int outrate=20;
unsigned char rrr;

int write_command(char *portname,unsigned char *samp,int len,int delay)
{
HANDLE sem;
int wrlen;
char semaphorecom[80];
DCB    comdcb;
HANDLE Hcom;
int i;
int writen=0;

sprintf(semaphorecom,"semaphorecom%s",portname);

sem=CreateSemaphore(NULL,1,1,semaphorecom);
if(!sem)
{
printf("Create semaphore error\n");
return -1;
}

WaitForSingleObject(sem,INFINITE);

Hcom=CreateFile( 
portname, GENERIC_READ | GENERIC_WRITE,
0,                    // exclusive access
NULL,                 // no security attrs
OPEN_EXISTING,0,
//FILE_ATTRIBUTE_NORMAL | 
//FILE_FLAG_OVERLAPPED, // overlapped I/O
NULL );

if(Hcom==INVALID_HANDLE_VALUE)
{
printf("Cann't open Com port =%s\n",portname);
ReleaseSemaphore(sem,1,NULL);
CloseHandle(sem);
return -1;}
GetCommState(Hcom,&comdcb);
comdcb.BaudRate=CBR_9600;
comdcb.StopBits=ONESTOPBIT;
comdcb.fParity=FALSE;
comdcb.fBinary=1;
comdcb.ByteSize=8;
SetCommState(Hcom,&comdcb);
SetupComm(Hcom, 4096, 4096);
for(i=writen;i<len;i++)
{
WriteFile(Hcom,&samp[i],1,&wrlen,NULL);
}
CloseHandle(Hcom);
ReleaseSemaphore(sem,1,NULL);
CloseHandle(sem);
Sleep(delay);
return 1;
}
int divpara=0;
unsigned char SAMPLE[3000];
int dacoutput(HWND window, SEED_data_record *sdr)
{
static int Y[3]={0,0,0};
static int Z[3]={0,0,0};
static int X[3]={0,0,0};
double A[3],B[2],C[3],D[2];
char stname[10];
char chname[10];
char tempstr[80];

daat starttime;
int i;
int k;
int par=0;

//unsigned char ooo;
unsigned char datapoint=48;
int nsamp;
double rate;
//int len;
int sum=0;
int Udata[MAXSEEDSAMPLES];
int delay;

if(dec_h(sdr,stname,chname,&starttime,&rate)<0)return -1;
if(strcmp(chname,dacchan )!=0)return -1;

sprintf(tempstr,"Sending %s Packet To Dac",chname);
SetDlgItemText(window,IDC_STATUS,tempstr);	
if(chfilter==1) //Wood Anderson 20
{
A[0]=1.;A[1]=-1.;A[2]=0.;B[0]=0.;B[1]=0.;
C[0]=0.2995458220700E-01;
C[1]=0.5990916441401E-01;
C[2]=0.2995458220700E-01;
D[0]=-0.1454243586288E+01;
D[1]=0.5749619151156E+00;
}
else if(chfilter==2)//spwwss 20
{
C[0]=0.80914241E+00;
C[1]=-0.16182848E+01;
C[2]=0.80914241E+00;
D[0]=-0.15815203E+01;
D[1]=0.65504935E+00;
A[0]=0.33571795E-01;
A[1]=0.67143589E-01;
A[2]=0.33571795E-01;
B[0]=-0.14189828E+01;
B[1]=0.55326997E+00;
}
else if(chfilter==3)//lpwwss20
{
C[0]=0.9977810241032E+00;
C[1]=-0.1995562048206E+01;
C[2]=0.9977810241032E+00;
D[0]=-0.1995557124346E+01;
D[1]=0.9955669720664E+00;
A[0]=0.1081653735993E-03;
A[1]=0.2163307471986E-03;
A[2]=0.1081653735993E-03;
B[0]=-0.1970368091922E+01;
B[1]=0.9708007534167E+00;
}
else if(chfilter==4)//lpwwss01
{
C[0]=0.9565432255608E+00;
C[1]=-0.1913086451122E+01;
C[2]=0.9565432255608E+00;
D[0]=-0.191197067434E+01;
D[1]=0.9149758348092E+00;
A[0]=0.3357181232166E-01;
A[1]=0.6714362464331E-01;
A[2]=0.3357181232166E-01;
B[0]=-0.1418982624114E+01;
B[1]=0.5532698734007E+00;
}

else  //(none)
{
A[0]=1.;A[1]=A[2]=B[0]=B[1]=0.;
C[0]=1;C[1]=C[2]=D[0]=D[1]=0.;
}



nsamp=sreadseed(1,(SHORT)((BlockSize-64)/64),sdr,Udata);
if(nsamp<=0)return -1;
for(i=0;i<nsamp;i++)
{
sum+=Udata[i];
}
sum/=nsamp;
if(midvalue!=0)sum=(sum+midvalue)/2;
midvalue=sum;
rrr=(unsigned char)(1000./rate+0.5);
if(divpara==0)
{
divpara=1;
//clear cts
SAMPLE[0]=44;
SAMPLE[1]=192;
//writeng delay
SAMPLE[2]=29;
convert(1000,&SAMPLE[3],&SAMPLE[4]);
SAMPLE[5]=12;
SAMPLE[6]=192+1;
SAMPLE[7]=14;
SAMPLE[8]=64+20;
if(outrate<=50)SAMPLE[8]=64+60;
SAMPLE[9]=128;
SAMPLE[10]=192+16;
SAMPLE[11]=28;
SAMPLE[12]=(((outrate>>4)&15)+64);
SAMPLE[13]=((outrate)&15)+192;
SAMPLE[14]=20+chan;
SAMPLE[15]=((rrr>>4)&15)+64;
SAMPLE[16]=((rrr)&15)+192;
if(write_command(chcom,&SAMPLE[0],17,0)<0)return -2;
}
par=2560/outrate*rrr*4/5;
k=0;
for(i=0;i<nsamp;i++)
{
Udata[i]=(int)((Udata[i]-midvalue )*dacdiv );
//Udata[i]=(int)(2047*cos(2*3.14159265/1*i/20.));
SAMPLE[k]=48+chan;
if(fmod(starttime.msec,60000)<rate)
SAMPLE[k]=48+chan+8;
X[2] =Udata[i];
Y[2] =(int)(A[0]*X[2] +A[1]*X[1] +A[2]*X[0] -B[0]*Y[1] -B[1]*Y[0] );
Z[2] =(int)(C[0]*Y[2] +C[1]*Y[1] +C[2]*Y[0] -D[0]*Z[1] -D[1]*Z[0] );
Y[0] =Y[1] ;
Y[1] =Y[2] ;
Z[0] =Z[1] ;
Z[1] =Z[2] ;
X[1] =X[2] ;
X[0] =X[1] ;
convert(Z[2] ,&SAMPLE[k+1],&SAMPLE[k+2]);
k+=3;
if(k>=par*3||i==nsamp-1){
if(i==nsamp-1)delay=0;
else delay=500/rrr*par;

//printf("%d %d %d\n",par,delay,nsamp);
if(write_command(chcom,&SAMPLE[0],k,delay)<0)return -2;
par/=2;
if(par<1)par=1;
k=0;
}
starttime=daatadsec(starttime,rate);
}
sprintf(tempstr,"%s Packet Sent to Dac",chname);
SetDlgItemText(window,IDC_STATUS,tempstr);	
//SetDlgItemText(window,IDC_STATUS,"Stop writing to Dac");	
return nsamp;
}


int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
{
	
	MSG         msg ;
	WNDCLASS    wndclass ; 
	
	char *szAppName="RealOutputToDac";

	
	if (!hPrevInstance)
	{
		wndclass.style		= CS_HREDRAW | CS_VREDRAW ;
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
				"DACOUTPUT",     		// window caption
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
		
		 DialogBox(hInst,"IDD_DIALOG1",hWnd,GETDACSETTINGS);
		 PostQuitMessage(0);
			break;
		
		default:

	return DefWindowProc (hWnd, message, wParam, lParam) ;
		}

}
	






LRESULT APIENTRY GETDACSETTINGS(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
	char helpdir[200];
	char *cportn[]={"COM1","COM2","COM3","COM4"};
	char *filtern[]={"(none)","WA_20","Spwwss_20","Lpwwss_20","Lpwwss_01"};
	char *frequency[]={"16","20","25","32","40","50","80","100","125","160","200","250"};
	char tmpstr[80];
	int i,k;
	OPENFILENAME ofn;
	static char szFile[255];
	char szFileTitle[255],szFilter[255];
	static THREADPACK tp;	
	

	
    switch (message) 
{
       case WM_INITDIALOG:
  EnableWindow(GetDlgItem(hDlg,IDC_STOP),FALSE);
  EnableWindow(GetDlgItem(hDlg,IDC_REFRESH),FALSE);
  for(i=0;i<4;i++)
  SendDlgItemMessage(hDlg,IDC_PORT,CB_ADDSTRING,(WPARAM)0,(LPARAM)cportn[i]);
  for(i=0;i<5;i++)
  SendDlgItemMessage(hDlg,IDC_FILTER,CB_ADDSTRING,(WPARAM)0,(LPARAM)filtern[i]);
  for(i=0;i<12;i++)
  SendDlgItemMessage(hDlg,IDC_FREQ,CB_ADDSTRING,(WPARAM)0,(LPARAM)frequency[i]);
  SendDlgItemMessage(hDlg,IDC_PORT,CB_SETCURSEL,(WPARAM)1,(LPARAM)0);		  
  SendDlgItemMessage(hDlg,IDC_FILTER,CB_SETCURSEL,(WPARAM)0,(LPARAM)0);		  
  SendDlgItemMessage(hDlg,IDC_FREQ,CB_SETCURSEL,(WPARAM)1,(LPARAM)0);		  
  SetDlgItemText(hDlg,IDC_SEED,dacchan);
  SendDlgItemMessage(hDlg,IDC_CH1,BM_SETCHECK ,(WPARAM)1,(LPARAM)0);
  sprintf(tmpstr,"%g",dacdiv);
  SetDlgItemText(hDlg,IDC_SCALE,tmpstr);
  SendDlgItemMessage(hDlg,IDC_SEED,EM_LIMITTEXT,(WPARAM)3,(LPARAM)0);
		return (TRUE);
		case  WM_CLOSE:
		if(threadstart==0)
		return (TRUE);
		EndDialog(hDlg, FALSE);   
		return (TRUE);

       case WM_COMMAND: 
		   {     
		   /* message: received a command */
			switch(LOWORD(wParam)) 
			{

				case IDC_MYHELP:
				sprintf(helpdir,"%s\\realdac.hlp",DIRNAME);
				WinHelp(hDlg,helpdir,HELP_FINDER,0);
				break;
             
            case IDC_REFRESH:
		//k=SendDlgItemMessage(hDlg,IDC_PORT,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		//SendDlgItemMessage(hDlg,IDC_PORT,CB_GETLBTEXT,(WPARAM)k,(LPARAM)chcom);
		chfilter=SendDlgItemMessage(hDlg,IDC_FILTER,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		//if(SendDlgItemMessage(hDlg,IDC_CH1,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=0;
		//else if(SendDlgItemMessage(hDlg,IDC_CH2,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=1;
		//else if(SendDlgItemMessage(hDlg,IDC_CH3,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=2;
		//else if(SendDlgItemMessage(hDlg,IDC_CH4,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=3;
		//if(strlen(szFile)==0)
		//{MessageBox(hDlg,"Select Online Buffer","Error",MB_ICONEXCLAMATION|MB_OK);break;}
		//GetDlgItemText(hDlg,IDC_SEED,dacchan,80);
		GetDlgItemText(hDlg,IDC_SCALE,tmpstr,80);
		if(sscanf(tmpstr,"%lf",&dacdiv)!=1){MessageBox(hDlg,"Select Correct Scale","Error",MB_ICONEXCLAMATION|MB_OK);break;}
		//k=SendDlgItemMessage(hDlg,IDC_FREQ,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		//SendDlgItemMessage(hDlg,IDC_FREQ,CB_GETLBTEXT,(WPARAM)k,(LPARAM)tmpstr);
		//sscanf(tmpstr,"%d",&outrate);
		sprintf(tmpstr,"%g",dacdiv);
        SetDlgItemText(hDlg,IDC_SCALE,tmpstr);
		SendDlgItemMessage(hDlg,IDC_FILTER,CB_SETCURSEL,(WPARAM)chfilter,(LPARAM)0);
		break;
			case IDC_EXIT:
			EndDialog(hDlg, FALSE);   
			break;
			case IDC_STOP:
			stop=1;
			SetDlgItemText(hDlg,IDC_STATUS,"Waiting for Correct Termination");	
			break;
			case IDC_OPEN:	
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
			GetOpenFileName(&ofn);
			break;
			case IDC_START:
        
        k=SendDlgItemMessage(hDlg,IDC_PORT,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		SendDlgItemMessage(hDlg,IDC_PORT,CB_GETLBTEXT,(WPARAM)k,(LPARAM)chcom);
		chfilter=SendDlgItemMessage(hDlg,IDC_FILTER,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		if(SendDlgItemMessage(hDlg,IDC_CH1,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=0;
		else if(SendDlgItemMessage(hDlg,IDC_CH2,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=1;
		else if(SendDlgItemMessage(hDlg,IDC_CH3,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=2;
		else if(SendDlgItemMessage(hDlg,IDC_CH4,BM_GETCHECK ,(WPARAM)0,(LPARAM)0))chan=3;
		if(strlen(szFile)==0)
		{MessageBox(hDlg,"Select Online Buffer","Error",MB_ICONEXCLAMATION|MB_OK);break;}
		GetDlgItemText(hDlg,IDC_SEED,dacchan,80);
		GetDlgItemText(hDlg,IDC_SCALE,tmpstr,80);
		if(sscanf(tmpstr,"%lf",&dacdiv)!=1){MessageBox(hDlg,"Select Correct Scale","Error",MB_ICONEXCLAMATION|MB_OK);break;}
		k=SendDlgItemMessage(hDlg,IDC_FREQ,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		SendDlgItemMessage(hDlg,IDC_FREQ,CB_GETLBTEXT,(WPARAM)k,(LPARAM)tmpstr);
		sscanf(tmpstr,"%d",&outrate);
				divpara=0;
if((indata=fopen(szFile,"rb"))==NULL)
{MessageBox(hDlg,"Cann't Open Online Buffer","Error",MB_ICONEXCLAMATION|MB_OK);break;}
if(fread(&info.HeadInfo[0],15,1,indata)!=1){MessageBox(hDlg,"Cann't Read Online Buffer","Error",MB_ICONEXCLAMATION|MB_OK);fclose(indata);break;}
if(info.HeadInfo[0]!='A'||
info.HeadInfo[1]!='S'||
info.HeadInfo[2]!='L'){MessageBox(hDlg,"Wrong online buffer","Error",MB_ICONEXCLAMATION|MB_OK);fclose(indata);break;}
Bsize=BlockSize=info.BlockSize;
Nblock=info.OnlinSize;
Currblock=info.CurrPoz; 
			stop=0;		
			tp.hwnd=hDlg;
			tp.nThread = 0;
			_beginthread(AcceptThreadProc, 0, &tp);
			EnableWindow(GetDlgItem(hDlg,IDC_REFRESH),TRUE);
            EnableWindow(GetDlgItem(hDlg,IDC_PORT),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_FREQ),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_SEED),FALSE);  
            EnableWindow(GetDlgItem(hDlg,IDC_CH1),FALSE);  
			EnableWindow(GetDlgItem(hDlg,IDC_CH2),FALSE);  
			EnableWindow(GetDlgItem(hDlg,IDC_CH3),FALSE);  
			EnableWindow(GetDlgItem(hDlg,IDC_CH4),FALSE);  
			EnableWindow(GetDlgItem(hDlg,IDC_START),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_STOP),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_OPEN),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EXIT),FALSE);
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
char buf[80];
threadstart=0;
SetDlgItemText(ptp->hwnd,IDC_STATUS,"Thread Started by User");	
while(!stop)
{
Sleep(100);
fseek(indata,0,SEEK_SET);
if(fread(&info.HeadInfo,15,1,indata)!=1)
{
SetDlgItemText(ptp->hwnd,IDC_STATUS,"Fatal Reading Error");	
stop=1;threadstart=1;break;
}
if(Currblock==info.CurrPoz)continue;
sprintf(buf,"Waiting For Parcket %d %d",Currblock,info.CurrPoz);
SetDlgItemText(ptp->hwnd,IDC_STATUS,buf);	
fseek(indata,Currblock*info.BlockSize+15,SEEK_SET);
fread(&Data[0],info.BlockSize,1,indata);
dacoutput(ptp->hwnd,(SEED_data_record *)Data);
Currblock++;
}
SetDlgItemText(ptp->hwnd,IDC_STATUS,"Thread Terminated by User");	
fclose(indata);
threadstart=1;
EnableWindow(GetDlgItem(ptp->hwnd,IDC_EXIT),TRUE);
EnableWindow(GetDlgItem(ptp->hwnd,IDC_REFRESH),FALSE);
            EnableWindow(GetDlgItem(ptp->hwnd,IDC_PORT),TRUE);
            EnableWindow(GetDlgItem(ptp->hwnd,IDC_FREQ),TRUE);
            EnableWindow(GetDlgItem(ptp->hwnd,IDC_SEED),TRUE);  
            EnableWindow(GetDlgItem(ptp->hwnd,IDC_CH1),TRUE);  
			EnableWindow(GetDlgItem(ptp->hwnd,IDC_CH2),TRUE);  
			EnableWindow(GetDlgItem(ptp->hwnd,IDC_CH3),TRUE);  
			EnableWindow(GetDlgItem(ptp->hwnd,IDC_CH4),TRUE);  
			EnableWindow(GetDlgItem(ptp->hwnd,IDC_START),TRUE);
			EnableWindow(GetDlgItem(ptp->hwnd,IDC_STOP),FALSE);
            EnableWindow(GetDlgItem(ptp->hwnd,IDC_OPEN),TRUE);

_endthread();
}


