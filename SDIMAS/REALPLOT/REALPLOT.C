#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>
#include <process.h>
#include "resource.h"
#include "s_time.h"
#include "steim.h"
#include "steimlib.h"
#include "miniseed.h"
char DIRNAME[100];
typedef struct {
   int    nThread; 
   HWND  hWnd;
   } THREADPACK, *PTHREADPACK;
void AcceptThreadProc(PTHREADPACK);
typedef struct {char aslc[3];int asli[3];}inform;
int STOP=1; 
HWND hWnd;
int PAINTFLAG=0;
int FIRSTFLAG=0;
int fontfl=0; 
char *CHANNEL="BHZ";
int LASTSEEKPOZ;
int  MINUTES=6;
int LINES=10;
daat END;
char STARTDATA[80];
int FILTERTYPE=0;
char FNAME[255];
double MAG=0.01;
HANDLE hInst;

HDC memdc;
int maxX,maxY;
HBITMAP hbit;
HBRUSH hbrush;
HPEN hpen;
HFONT hFont, hOld;
LOGFONT lf;
CHOOSEFONT cf;

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


long dec_h(SEED_data_record *sdr,char *stname,char *chname,daat *d1,daat *d2,char *chfilter,char *stfilter,int filter,double *rate)
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
*d2=daatadsec(*d1,(*rate)*samples);
if(stname[0]!=stfilter[0]&&stfilter[0]!='?')return -1;
if(stname[1]!=stfilter[1]&&stfilter[1]!='?')return -1;
if(stname[2]!=stfilter[2]&&stfilter[2]!='?')return -1;
if(stname[3]!=stfilter[3]&&stfilter[3]!='?')return -1;
if(stname[4]!=stfilter[4]&&stfilter[4]!='?')return -1;
if(chname[0]!=chfilter[0]&&chfilter[0]!='?')return -1;
if(chname[1]!=chfilter[1]&&chfilter[1]!='?')return -1;
if(chname[2]!=chfilter[2]&&chfilter[2]!='?')return -1;
if(filter) 
if(((sdr->Head.activity_flags)&(76))==0)return -1;
chname[3]=0;
stname[5]=0;
return 0;
}

inform info;
inform info1;
inform info2;
long wait_for_packet(char *fname,char *channel,daat end,int min,int lines,char *startdata)
{
char stname[80];
char chname[80];
daat dd1,dd2;
double Rate;

FILE *aslbuf;
char dat[4096];
int currpoz=0;
if((aslbuf=fopen(fname,"rb"))==NULL)
{MessageBox(hWnd,"Cann't Open Online Buffer","File Open Error",MB_OK|MB_ICONSTOP);
fclose(aslbuf);return -1;}
if(fread(&info.aslc[0],15,1,aslbuf)!=1)
{fclose(aslbuf);MessageBox(hWnd,"Cann't Read Online Buffer","File Read Error",MB_OK|MB_ICONSTOP);
return -1;}
currpoz=info.asli[0];
for(;;)
{
if(STOP)return -1;
Sleep(100);
fseek(aslbuf,0,SEEK_SET);
fread(&info.aslc[0],15,1,aslbuf);
if(currpoz==info.asli[0]){/*SetWindowText(hWnd,"BREAK");*/continue;}
fseek(aslbuf,currpoz*info.asli[2]+15,SEEK_SET);
fread(&dat[0],info.asli[2],1,aslbuf);
currpoz=info.asli[0];
if(dec_h((SEED_data_record *)dat,stname,chname, &dd1,&dd2,channel,"?????",0,&Rate)<0)
{/*SetWindowText(hWnd,chname);*/continue;}
if(raznd(dd2,end)>min*60000.)break;
}
dd2=daatadsec(dd2,-min*lines*60000.);
cdatostr(dd2,startdata,0);

fclose(aslbuf);
return 1;
}


void DrawPage(HWND hwnd,
	      HDC hdc,
	      char *channel,
	      char * fname,
          int *seekpoz,
          char *datastart,
          int min,
          int lines,
          int filter,
          double mag)
{
SIZE textm;
char *tempor="99h:999";
daat d1,d2;
FILE *aslbuf;
char tmpstr[80];
int moveflag=0;
double Rate;
int aaa=0;
int spoz;
int yy,mm,dd,hh,mi;
static double Y[3]={0.,0.,0.};
static double Z[3]={0.,0.,0.};
static double X[3]={0.,0.,0.};
char *Data[4096];
double A[3],B[2],C[3],D[2];
char stname[80],chname[80];
daat currdat;
RECT rect;
int Udata[MAXSEEDSAMPLES];
int x,y,y1=0;
int hhh,mii;
int linenumber=-1;
POINT P;
int dest;
int nsamp;
int i;
daat dd1,dd2;
aslbuf=fopen(fname,"rb");
GetTextExtentPoint32(memdc,tempor,7,&textm);
fread(&info1.aslc[0],15,1,aslbuf);
if(filter==1) //Wood Anderson 20
{
A[0]=1.;A[1]=-1.;A[2]=0.;B[0]=0.;B[1]=0.;
C[0]=0.2995458220700E-01;
C[1]=0.5990916441401E-01;
C[2]=0.2995458220700E-01;
D[0]=-0.1454243586288E+01;
D[1]=0.5749619151156E+00;
}
if(filter==2)//spwwss 20
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
if(filter==3)//lpwwss20
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
if(filter==4)//lpwwss01
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
if(filter==0)
{
//A[0]=1.;A[1]=A[2]=B[0]=B[1]=0.;
A[0]=0.91497170E+00;
A[1]=-0.18299430E+01;
A[2]=0.91497170E+00;
B[0]=-0.18226960E+01;
B[1]=0.83718200E+00;
//A[0]=1.;A[1]=-2;A[2]=1.;B[0]=-2.;B[1]=1.;
C[0]=1;C[1]=C[2]=D[0]=D[1]=0.;
/*
C[0]=0.24972220E-05;
C[1]=0.14983330E-04;
C[2]=0.37458340E-04;
C[3]=0.49944450E-04;
C[4]=0.37458340E-04;
C[5]=0.14983330E-04;
C[6]=0.24972220E-05;
D[0]=-0.50294360E+01;
D[1]=0.10607040E+02;
D[2]=-0.11999310E+02;
D[3]=0.76754680E+01;
D[4]=-0.26310540E+01;
D[5]=0.37745280E+00;
*/
}

if(filter>4)filter=0;


sscanf(datastart,"%4d%2d%2d%2d%2d",&yy,&mm,&dd,&hh,&mi);
sprintf(tmpstr,"%04d%02d%02d%02d%02d000000",yy,mm,dd,hh,mi);
hhh=hh;
mii=mi;
d1=cstrtoda(tmpstr);
d2=daatadsec(d1,min*60.*lines*1000.);
GetClientRect(hwnd,&rect);
rect.left=textm.cx;
rect.right-=rect.left/4;
rect.top=(rect.bottom-rect.top)/50;
rect.bottom-=rect.top;
for(i=0;i<=min;i++)
{
x=rect.left+(int)((float)(rect.right-rect.left)*i/min+0.5);
MoveToEx(hdc,x,rect.top,&P);
LineTo(hdc,x,rect.bottom);
}
dest=(rect.bottom-rect.top)/(lines+1);

for(i=1;i<=lines;i++)
{
y=rect.top+i*dest;
MoveToEx(hdc,rect.left,y,&P);
LineTo(hdc,rect.left-5,y);
if(i%2)
{SetTextAlign(hdc,TA_RIGHT|TA_BASELINE);
sprintf(tmpstr,"%02d:%02d",hh,mi);
TextOut(hdc,rect.left-7,y+textm.cy/2,tmpstr,strlen(tmpstr));
}
mi+=min;
hh+=mi/60;
mi%=60;
}


//SetTextAlign(hdc,TA_CENTER|TA_RIGHT);

//TextOut(hdc,rect.right+5,(rect.bottom+rect.top)/2,tmpstr,strlen(tmpstr));
fseek(aslbuf,15+*seekpoz*info1.asli[2],SEEK_SET);
for(;;)
{
if(*seekpoz==0)fseek(aslbuf,15,SEEK_SET);
if(!fread(&Data,info1.asli[2],1,aslbuf)){*seekpoz=0;continue;}
if(!aaa){spoz=*seekpoz;}
*seekpoz+=1;
*seekpoz%=info1.asli[1];
if(dec_h((SEED_data_record *)Data,stname,chname, &dd1,&dd2,channel,"?????",0,&Rate)<0)continue;
if(raznd(d1,dd2)>0)continue;
if(raznd(d2,dd1)<0)break;
if(!aaa){aaa=1;}
nsamp=sreadseed(1,(SHORT)((info1.asli[2]-64)/64),(SEED_data_record *)Data,Udata);
if(nsamp<=0)continue;
for(i=0;i<nsamp;i++)
{
X[2]=Udata[i];
Y[2]=A[0]*X[2]+A[1]*X[1]+A[2]*X[0]-B[0]*Y[1]-B[1]*Y[0];
Z[2]=C[0]*Y[2]+C[1]*Y[1]+C[2]*Y[0]-D[0]*Z[1]-D[1]*Z[0];
Y[0]=Y[1];
Y[1]=Y[2];
Z[0]=Z[1];
Z[1]=Z[2];
X[1]=X[2];
X[0]=X[1];
currdat=daatadsec(dd1,i*Rate);
if(raznd(currdat,d1)<0)continue;
if(raznd(currdat,d2)>0)break;
x=(int)(fmod(raznd(currdat,d1)/60000.,min)*(rect.right-rect.left)/min);
y=(int)(raznd(currdat,d1)/60000./min)+1;
if(y1!=y)moveflag=0;
else moveflag=1;
y1=y;
y=rect.top+y*dest;y-=(int)(Z[2]*mag);
x+=rect.left;
if(moveflag==0)MoveToEx(hdc,x,y,&P);
else LineTo(hdc,x,y);
}

}
sprintf(tmpstr,"Start Time:%02d.%02d.%02d %02d:%02d %s %s V=%g",yy,mm,dd,hhh,mii,stname,chname,mag);
SetWindowText(hWnd,tmpstr);

END=daatadsec(d2,0);
*seekpoz=spoz;
fclose(aslbuf);
}


WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow )
{
   MSG   msg ;
   

      UNREFERENCED_PARAMETER( lpszCmdLine );

   if (!hPrevInstance)
      if (!InitApplication( hInstance ))
         return ( FALSE ) ;

   if (!InitInstance( hInstance, nCmdShow ))
      return ( FALSE ) ;

GetCurrentDirectory(100,DIRNAME);
   while (GetMessage( &msg, NULL, 0, 0 ))
   {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
   }
   return (msg.wParam);           /* Returns the value from PostQuitMessage */
   

}
// end of WinMain()

//---------------------------------------------------------------------------
//  BOOL NEAR InitApplication( HANDLE hInstance )
//
//  Description:
//     First time initialization stuff.  This registers information
//     such as window classes.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to this instance of the application.
//
//---------------------------------------------------------------------------

char *WgsnClass="RealTimePlot"; 
BOOL NEAR InitApplication( HANDLE hInstance )
{




WNDCLASS  wndclass ;
   wndclass.style =         0 ;
   wndclass.lpfnWndProc =   WndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    0 ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         0;
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
   wndclass.lpszMenuName =  "IDR_FILE" ;
   wndclass.lpszClassName = WgsnClass ;

   return( RegisterClass( &wndclass ) ) ;

} // end of InitApplication()


HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
{

	      hInst=hInstance;
	  



   hWnd = CreateWindow( WgsnClass,"PLOT REAL TIME",
                           WS_OVERLAPPEDWINDOW,
						   CW_USEDEFAULT,
						   CW_USEDEFAULT,
						   CW_USEDEFAULT,
						   CW_USEDEFAULT,
                           NULL, NULL, hInstance, NULL 
			) ;

   if (NULL == hWnd)
      return ( NULL ) ;

   ShowWindow( hWnd, nCmdShow ) ;
   UpdateWindow( hWnd ) ;

 
   return ( hWnd ) ;

} // end of InitInstance()


LONG APIENTRY WndProc( HWND hWnd, UINT Msg,
                               UINT wParam, LONG lParam )
{
	
FILE *in;
RECT r;
char helpdir[200];
PAINTSTRUCT paintstruct;
HDC hdc;
static THREADPACK tp;

if(STOP==0){
EnableMenuItem(GetMenu(hWnd),ID_START,MF_GRAYED);
EnableMenuItem(GetMenu(hWnd),ID_EXIT,MF_GRAYED);
}
else       
{
EnableMenuItem(GetMenu(hWnd),ID_START,MF_ENABLED);
EnableMenuItem(GetMenu(hWnd),ID_EXIT,MF_ENABLED);}



   switch (Msg)



   {
		
      case WM_CREATE:
			END.day=0;
			END.msec=0.;
		  maxX=GetSystemMetrics(SM_CXSCREEN);
		  maxY=GetSystemMetrics(SM_CYSCREEN);
		 
		  
			hdc=GetDC(hWnd);
			memdc=CreateCompatibleDC(hdc);
			hbit=CreateCompatibleBitmap(hdc,maxX,maxY);
			SelectObject(memdc,hbit);
			hbrush=GetStockObject(WHITE_BRUSH);
			SelectObject(memdc,hbrush);
			hbrush=GetStockObject(BLACK_PEN);
			SelectObject(memdc,hpen);
			PatBlt(memdc,0,0,maxX,maxY,PATCOPY);
			ReleaseDC(hWnd,hdc);
		 //   EnableMenuItem(GetMenu(hWnd),ID_START,MF_GRAYED);
		//	EnableMenuItem(GetMenu(hWnd),ID_STOP,MF_GRAYED);
      break;

      case WM_COMMAND:
      
         switch ( LOWORD( wParam ) )
         {
		 
		 case ID_HELP:
				sprintf(helpdir,"%s\\realplot.hlp",DIRNAME);
				WinHelp(hWnd,helpdir,HELP_FINDER,0);
				break;

         case ID_START:
			 LASTSEEKPOZ=-1;
			 END.day=0;
			 END.msec=0.;
			 STOP=0;
			 tp.nThread = 0;
			 tp.hWnd = hWnd;
			 _beginthread(AcceptThreadProc, 0, &tp);

		
			break;
			 STOP=0;

			 break;
		 case ID_STOP:
			 FIRSTFLAG=0;
			 LASTSEEKPOZ=-1;
			 END.day=0;
			 END.msec=0.;
			 STOP=1;
			 break;

		 case ID_EXIT:
               PostMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;
	       case ID_PLOTINFO:

	       DialogBox(hInst,"IDD_GETSETTINGS",hWnd,GetSettings);
		   InvalidateRect(hWnd,NULL,0);
			break;
					
		   
			 default:
         		 return( DefWindowProc( hWnd, Msg, wParam, lParam ) ) ;

                   break ;
		
		 }
			break;
case WM_SIZE:
			   			InvalidateRect(hWnd,NULL,0);
						break;

     case WM_DESTROY:
		  if(fontfl!=0)
			{
			SelectObject(memdc,hOld);
			DeleteObject(hFont);
			}
		 DeleteDC(memdc);
         PostQuitMessage( 0 ) ;
         break ;
	 

	 case WM_PAINT:
		 
		 if(PAINTFLAG==0)PAINTFLAG=1;
		 else break;
		r.left=r.top=0;
		r.bottom=maxY;
		r.right=maxX;
		
		FillRect(memdc,&r,GetStockObject(WHITE_BRUSH));
		if(FIRSTFLAG&&STOP==0)
		{
		in=fopen(FNAME,"rb");
		fread(&info2.aslc[0],15,1,in);
		if(LASTSEEKPOZ<0)
		LASTSEEKPOZ=(info2.asli[0]+1)%(info2.asli[1]);
		fclose(in);	
		  DrawPage(hWnd,
	      memdc,
	      CHANNEL,
	      FNAME,
          &LASTSEEKPOZ,
          STARTDATA,
          MINUTES,
          LINES,
          FILTERTYPE,
          MAG);}

		 hdc=BeginPaint(hWnd,&paintstruct);
		//hdc=GetDC(hWnd);
		 BitBlt(hdc,0,0,maxX,maxY,memdc,0,0,SRCCOPY);
		 EndPaint(hWnd,&paintstruct);
		 //ReleaseDC(hWnd,hdc);
		PAINTFLAG=0;
		 break;
	 case WM_CLOSE:
	if(STOP==0)break;

      default:


         return( DefWindowProc( hWnd, Msg, wParam, lParam ) ) ;
   }
   return 0L ;
}


	
LRESULT APIENTRY GetSettings(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

static char szFile[128];
OPENFILENAME ofn;
char szFileTitle[128],szFilter[128];
char tmpstr[80];

int i;
	
    switch (message) 
{
       case WM_INITDIALOG:                     /* message: initialize dialog box */
		SendDlgItemMessage(hDlg,1010+FILTERTYPE,BM_SETCHECK,(WPARAM)1,(LPARAM)0);
		SetDlgItemText(hDlg,IDC_CHAN,CHANNEL);
		sprintf(tmpstr,"%d",MINUTES);
		SetDlgItemText(hDlg,IDC_MIN,tmpstr);
		sprintf(tmpstr,"%d",LINES);
		SetDlgItemText(hDlg,IDC_LIN,tmpstr);
		sprintf(tmpstr,"%g",MAG);
		SetDlgItemText(hDlg,IDC_MAG,tmpstr);
		if(STOP==0)
		{
		EnableWindow(GetDlgItem(hDlg,IDC_CHAN),FALSE);
		for(i=0;i<5;i++)EnableWindow(GetDlgItem(hDlg,1010+i),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_OPEN),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_MIN),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_LIN),FALSE);
		}

		else
		{
		EnableWindow(GetDlgItem(hDlg,IDC_CHAN),TRUE);
		for(i=0;i<5;i++)EnableWindow(GetDlgItem(hDlg,1010+i),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_OPEN),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_MIN),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_LIN),TRUE);
		}
		
	   return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            
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
			ofn.nMaxFile =128;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 128;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=(long)NULL ;
			GetOpenFileName(&ofn);
			break;
		case IDC_FONT:
					if(fontfl!=0)
			{
				SelectObject(memdc,hOld);
				DeleteObject(hFont);

			}
			memset(&cf, 0, sizeof(CHOOSEFONT));
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = hWnd;
			cf.lpLogFont = &lf;
			cf.Flags = CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT;
			//cf.rgbColors = RGB(255, 0, 0);  // Red
			cf.nFontType = SCREEN_FONTTYPE;
			//cf.Flags=CF_TTONLY;
			ChooseFont(&cf);
			// Display the selected font:
			hFont = CreateFontIndirect(&lf);
			hOld=SelectObject(memdc,hFont);
			fontfl=1;

			break;
		  
		case IDC_OK:          /* "OK" box selected?        */
		for(i=0;i<5;i++)
		{
		if(SendDlgItemMessage(hDlg,1010+i,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)
		{FILTERTYPE=i;break;}
		}

		GetDlgItemText(hDlg,IDC_CHAN,CHANNEL,80);
		sprintf(tmpstr,"%d",MINUTES);
		GetDlgItemText(hDlg,IDC_MIN,tmpstr,80);
		if((MINUTES=atoi(tmpstr))==0)break;
		GetDlgItemText(hDlg,IDC_LIN,tmpstr,80);
		if((LINES=atoi(tmpstr))==0)break;
		GetDlgItemText(hDlg,IDC_MAG,tmpstr,80);
		if((MAG=atof(tmpstr))==0)break;
		strcpy(FNAME,szFile);
		if(strlen(FNAME)==0)break;
		

			EndDialog(hDlg, TRUE);          /* Exits the dialog box        */
           	break;
            case IDCANCEL:     /* System menu close command? */
             EndDialog(hDlg, FALSE);         /* Exits the dialog box        */
             break;
             default:
              return FALSE;
           }
        return (TRUE);
      }
    }   /* switch message */

    return (FALSE);                           /* Didn't process a message    */
        UNREFERENCED_PARAMETER(lParam);        
}



void AcceptThreadProc(PTHREADPACK ptp)
{
			

		
	while(!STOP)
			{
	
			

			if(wait_for_packet(FNAME,CHANNEL,END,MINUTES,LINES,STARTDATA)<0)STOP=1;
			SetWindowText(hWnd,STARTDATA);
			FIRSTFLAG=1;
			InvalidateRect(hWnd,NULL,0);
			//Sleep(MINUTES*60000);
			}
			_endthread();	
}

