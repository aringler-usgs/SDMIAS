#include <windows.h>  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commdlg.h>
#include "resource.h"    
#include "scsidefs.h"
#include "wnaspi32.h"
#include "steim.h"
#include "steimlib.h"
#include "miniseed.h"
#include "my_time.h"
#include "my_draw.h"
char DIRNAME[100];
int UData[MAXSEEDSAMPLES];
int filter=1;
char chfilter[10];
char Data[4096*16];
char Text[4097];
char Hex [256*54+1];
SHORT Level=1;
SHORT Dframes=63;
int SRR;
//SEED_data_record sdr;
SEED_fixed_data_record_header IbmH;
HWND hwnd;
long dec_h(SEED_data_record *sdr,char *status)
{
daat d1;
int yy,dd,hh,mi,ss,ts;
char staname[80];
char chname[80];
char stardat[80];
char voluem[80];
sprintf(voluem,"%c%c%c%c%c%c%c",
		sdr->Head.sequence[0],
		sdr->Head.sequence[1],
		sdr->Head.sequence[2],
		sdr->Head.sequence[3],
		sdr->Head.sequence[4],
		sdr->Head.sequence[5],
		sdr->Head.seed_record_type);
if(filter==3)return 1;
if(voluem[6]!='D'){
strcpy(status,voluem);
if(filter==0)
return 0;
else return 0;}
sprintf(staname,"%c%c%c%c%c",
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
{sprintf(status,"%s %s %s",voluem,staname,chname);
if(filter==0)return 1;
else return 0;}
hh=sdr->Head.starting_time.hr;
mi=sdr->Head.starting_time.minute;
ss=sdr->Head.starting_time.seconds;
 dd=swapb(sdr->Head.starting_time.jday);
 ts=swapb(sdr->Head.starting_time.tenth_millisec );
yy=swapb(sdr->Head.starting_time.yr);	
sprintf(stardat,"%04d%03d%02d%02d%02d%04d",yy,dd,hh,mi,ss,ts);
d1=cjstrtoda(stardat);
cdatostr(d1,stardat,1);
sprintf(status,"%s %s %s",staname,chname,stardat);
if(filter==1)
{	if((chname[0]==chfilter[0]||chfilter[0]=='?')&&
	   (chname[1]==chfilter[1]||chfilter[1]=='?')&&
	   (chname[2]==chfilter[2]||chfilter[2]=='?'))
	return 1;
	else return 0;
}
else 
if(filter==2)
{  

	if((chname[0]==chfilter[0]||chfilter[0]=='?')&&
	   (chname[1]==chfilter[1]||chfilter[1]=='?')&&
	   (chname[2]==chfilter[2]||chfilter[2]=='?')&&
	   (((sdr->Head.activity_flags)&(76))!=0))return 1;
	else return 0;
}

return 0;
}

  LONG decode_IBM_header 
(SEED_data_record *sdr, SHORT *firstframe, SHORT *level, 
 SHORT *flip, SHORT *dframes,
 SEED_fixed_data_record_header *IbmH)
   {
      int i;
	  COMPWORD          ftest;
//      SHORT             hfirstdata, hfirstblockette, h1000 ;
  //    LONG              headertotal ;

      ftest.b[0] = 0x12 ;
      ftest.b[1] = 0x34 ;
      ftest.b[2] = 0x56 ;
      ftest.b[3] = 0x78 ;
      if (ftest.l == 0x78563412)
          *flip = 1 ;
        else
          if (ftest.l == 0x12345678)
              *flip = 0 ;
            else
              {
                    return -1; 
              }

	  if(sdr->Head.seed_record_type!='D')return -1000;                  

	  for(i=0;i<6;i++)
		  IbmH->sequence [i]=sdr->Head.sequence[i];
		  IbmH->seed_record_type=sdr->Head.seed_record_type;	
		  IbmH->continuation_record =sdr->Head.continuation_record;
	  for(i=0;i<5;i++)
		  IbmH->station_ID_call_letters[i]=sdr->Head.station_ID_call_letters [i];
	  
	  for(i=0;i<2;i++)
      IbmH->location_id [i]=sdr->Head.location_id [i];
      for(i=0;i<3;i++)
	  IbmH->channel_ID [i]=sdr->Head.channel_ID [i];  
	  if(IbmH->channel_ID [0]=='L'&&
		IbmH->channel_ID [1]=='O'&&
		IbmH->channel_ID [2]=='G')return -1000;

      for(i=0;i<2;i++)
	  IbmH->seednet [i]=sdr->Head.seednet [i];
      IbmH->starting_time.hr=sdr->Head.starting_time.hr;
	  IbmH->starting_time.minute=sdr->Head.starting_time.minute;
	  IbmH->starting_time.seconds=sdr->Head.starting_time.seconds;
	  IbmH->starting_time.unused=sdr->Head.starting_time.unused;		
	  IbmH->activity_flags =sdr->Head.activity_flags ;
      IbmH->IO_flags=sdr->Head.IO_flags;
      IbmH->data_quality_flags =sdr->Head.data_quality_flags ;
      IbmH->number_of_following_blockettes=sdr->Head.number_of_following_blockettes ; 
      if (*flip)
          {
		  IbmH->tenth_msec_correction=swapl(sdr->Head.tenth_msec_correction);  
          IbmH->samples_in_record  = swapb (sdr->Head.samples_in_record) ;
          IbmH->first_data_byte  = swapb (sdr->Head.first_data_byte) ;
          IbmH->first_blockette_byte  = swapb (sdr->Head.first_blockette_byte) ;
          IbmH->DOB.blockette_type  = swapb (sdr->Head.DOB.blockette_type) ;
		  IbmH->starting_time.yr	=swapb(sdr->Head.starting_time.yr);
		  IbmH->starting_time.jday	=swapb(sdr->Head.starting_time.jday);
		  IbmH->sample_rate_factor	=swapb(sdr->Head.sample_rate_factor);
		  IbmH->sample_rate_multiplier	=swapb(sdr->Head.sample_rate_multiplier );
		  IbmH->starting_time.tenth_millisec=swapb(sdr->Head.starting_time.tenth_millisec );
		   }
        else
          {
            IbmH->tenth_msec_correction=sdr->Head.tenth_msec_correction;  
			IbmH->samples_in_record= sdr->Head.samples_in_record ;
            IbmH->first_data_byte = sdr->Head.first_data_byte ;
            IbmH->first_blockette_byte= sdr->Head.first_blockette_byte ;
            IbmH->DOB.blockette_type = sdr->Head.DOB.blockette_type ;
			IbmH->starting_time.yr=sdr->Head.starting_time.yr;
			IbmH->starting_time.jday=sdr->Head.starting_time.jday;
			IbmH->starting_time.tenth_millisec=sdr->Head.starting_time.tenth_millisec;
			IbmH->sample_rate_factor=sdr->Head.sample_rate_factor;
			IbmH->sample_rate_multiplier=sdr->Head.sample_rate_multiplier;

          }
		if(IbmH->samples_in_record==0) return 0;
		//if(Rfactor>=0&&Rmultip>=0)*Rate=(float)Rfactor*Rmultip;
		//if(Rfactor>=0&&Rmultip<=0)*Rate=-1.*(float)Rfactor/Rmultip;
		//if(Rfactor<=0&&Rmultip>=0)*Rate=-1.*(float)Rmultip/Rfactor;
		//if(Rfactor<=0&&Rmultip<=0)*Rate=(float)Rmultip/Rfactor;
		//*Rate=1./(*Rate);

		//sprintf(timestart,"%04d%03d%02d%02d%02d%04d",YY,DD,HH,MI,SS,TS);
		//*sdata=cjstrtoda(timestart);
		
      /*
       * find the first data frame. note that compressed data must begin on a frame boundary.
       * abort if an illegal start-of-data is found.
       */
      if ((IbmH->first_data_byte  % sizeof(compressed_frame)) != 0)
        {
            //printf ("first data does not begin on frame boundary!\n") ;
            return -2;
        }
      *firstframe = IbmH->first_data_byte /sizeof(compressed_frame) - 1 ;
      /*
       * if a data-only blockette is present, read the encoding format and
       * record length from it. otherwise, the caller's default level will not be changed.
       * similarly, do not overwrite the caller's default number of data frames if the
       * blockette 1000 is not present.
       */
      if ((IbmH->first_blockette_byte== 48) && (IbmH->DOB.blockette_type == 1000))
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
      return (IbmH->samples_in_record ) ;
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


LONG sreadseed(SHORT Level,SHORT Dframes,SEED_data_record *sdr,LONG *udata,char *CHNAME,char *STANAME,SEED_fixed_data_record_header *IbmH)
  {
     
      
      LONG                      rectotal, headertotal ;
      SHORT                     dstat ;
      SHORT                     firstframe, flip, level ;
      dcptype                   dcp[1] ;
      SHORT                     dframes ;
      //int                       numread ;
	  int chindex;
	unsigned	int i;




	  for (chindex = 0; chindex < 1; chindex++)
        if ((dcp[chindex] = init_generic_decompression ()) == NULL )
            return -1;


      
      //if ((udata = (LONG *)malloc(sizeof(LONG)*MAXSEEDSAMPLES)) == NULL )
	//	return -101;

      //      if ((sdr = (SEED_data_record *)malloc(sizeof(SEED_data_record))) == NULL )
       // return -102;

     level = Level;
     dframes = Dframes;
     	 headertotal = decode_IBM_header (sdr, &firstframe, &level, &flip, &dframes,IbmH) ;

			  if(headertotal<=0)return headertotal;
			  //if(headertotal==0)return -1002;
              validate_frames (firstframe, level, dframes) ;
			  if(STANAME)
			  for(i=0;i<strlen(STANAME);i++)
			if(STANAME[i]!=sdr->Head.station_ID_call_letters[i])return -1000;
			if(CHNAME)
			  for(i=0;i<strlen(CHNAME);i++)
			{if(CHNAME[i]!=sdr->Head.channel_ID [i])
			//sdr->Head.channel_ID [3]=0;
			//SetWindowText(hWndg,sdr->Head.sequence);
			return -1001;
			}

            
              rectotal = decompress_generic_record ((generic_data_record *)sdr, udata, &dstat, dcp[0],
                                                    firstframe, headertotal, level, flip, dframes) ;
              
              if (dferrorfatal(dstat,NULL))
                      {
                        //printf ("decompress: decompress_SEED_record reports errors!\n") ;
                        return -5000 ;
                      } ;

   return rectotal;           
                }








WORD 		NumAdapters;
char 		szAppName[] = "SCANSCSI" ;
HWND        hwndedit;
HINSTANCE hRTFLib;
int BUSY=0;
int Bnumber=1;
HINSTANCE hInst;
BYTE Dev[2]={(BYTE)200,(BYTE)200};
BYTE Sour[2]={(BYTE)200,(BYTE)200};
BYTE Dest[2]={(BYTE)200,(BYTE)200};
long CurrTarget(HWND,BYTE,BYTE);
long ScanTarget(HWND,BYTE,BYTE,char *);
long RewTarget(HWND ,BYTE ,BYTE );
long ForwardTarget(HWND hwnd,BYTE,BYTE ,long);
long ReadTarget(HWND hwnd,BYTE,BYTE ,char *,long);
long WriteTarget(HWND hwnd,BYTE,BYTE ,char *,long);
long VerifyTarget(HWND hwnd,BYTE,BYTE ,long);
int Stop=0;
LRESULT APIENTRY GetNumber(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[80];


	
    switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		sprintf(tmpstr,"%d",Bnumber);
		SetDlgItemText( hDlg, IDC_EDIT1,tmpstr);
		
       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
			GetDlgItemText(hDlg,IDC_EDIT1,tmpstr,80);
			if((Bnumber=atoi(tmpstr))==0)break;
				EndDialog(hDlg, TRUE);
			   	break;

   
             default:
              return FALSE;
           }
        return (TRUE);
      }
    } 

    return (FALSE);                           /* Didn't process a message    */
        UNREFERENCED_PARAMETER(lParam);        
}


LRESULT APIENTRY GetFilter(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[80];


	
    switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		sprintf(tmpstr,"%d",Bnumber);
		SetDlgItemText( hDlg, IDC_CHN,"???");
		SendDlgItemMessage(hDlg,IDC_ODATA,BM_SETCHECK,1,0);
		filter=1;
		
       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
			GetDlgItemText(hDlg,IDC_CHN,chfilter,10);
			if(SendDlgItemMessage(hDlg,IDC_STATUS,BM_GETCHECK,0,0)==1)filter=0;
			if(SendDlgItemMessage(hDlg,IDC_ODATA,BM_GETCHECK,0,0)==1)filter=1;
			if(SendDlgItemMessage(hDlg,IDC_OEVENT,BM_GETCHECK,0,0)==1)filter=2;
			else filter=3;
			EndDialog(hDlg, TRUE);
			break;

   
             default:
              return FALSE;
           }
        return (TRUE);
      }
    } 

    return (FALSE);                           /* Didn't process a message    */
        UNREFERENCED_PARAMETER(lParam);        
}



LRESULT APIENTRY GetDevice(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
int k;
char tmpstr[80];
char Smessage[80];
BYTE j,i;
    switch (message) 
{
       case WM_INITDIALOG:
	for(j=0;j<NumAdapters;j++)
	for(i=0;i<8;i++)
	{
	ScanTarget(hDlg,i,j,Smessage);
	Smessage[23]=0;
	if(strlen(Smessage)>0){
	sprintf(tmpstr,"#IDA=%d #IDT=%d %s",j,i,Smessage);
	SendDlgItemMessage(hDlg,IDC_DEVICE, CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
			      }
	}
	SendDlgItemMessage(hDlg,IDC_DEVICE, CB_SETCURSEL,(WPARAM)0,(LPARAM)0);		
		
       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:     
		k=SendDlgItemMessage(hDlg,IDC_DEVICE, CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		SendDlgItemMessage(hDlg,IDC_DEVICE, CB_GETLBTEXT,(WPARAM)k,(LPARAM)tmpstr);		
		if(sscanf(tmpstr,"#IDA=%d #IDT=%d",&Dev[1],&Dev[0])!=2)
				EndDialog(hDlg, FALSE);
		else	EndDialog(hDlg, TRUE);
			   

   
             default:
              return FALSE;
           }
        return (TRUE);
      }
    } 

    return (FALSE);                           /* Didn't process a message    */
        UNREFERENCED_PARAMETER(lParam);        
}
LRESULT APIENTRY GetTapeToTape(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[80];
char Smessage[80];
BYTE j,i;
int k;
    switch (message) 
{
       case WM_INITDIALOG:
	for(j=0;j<NumAdapters;j++)
	for(i=0;i<8;i++)
	{
	ScanTarget(hDlg,i,j,Smessage);
	Smessage[23]=0;
	if(strlen(Smessage)>0){
	sprintf(tmpstr,"#IDA=%d #IDT=%d %s",j,i,Smessage);
	SendDlgItemMessage(hDlg,IDC_SOUR, CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	SendDlgItemMessage(hDlg,IDC_DEST, CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
			      }
	}
	
	SendDlgItemMessage(hDlg,IDC_SOUR, CB_SETCURSEL,(WPARAM)0,(LPARAM)0);					
    SendDlgItemMessage(hDlg,IDC_DEST, CB_SETCURSEL,(WPARAM)1,(LPARAM)0);		
		
       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
				k=SendDlgItemMessage(hDlg,IDC_SOUR, CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		SendDlgItemMessage(hDlg,IDC_SOUR, CB_GETLBTEXT,(WPARAM)k,(LPARAM)tmpstr);		
		if(sscanf(tmpstr,"#IDA=%d #IDT=%d",&Sour[1],&Sour[0])!=2)
				EndDialog(hDlg, FALSE);
		k=SendDlgItemMessage(hDlg,IDC_DEST, CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		SendDlgItemMessage(hDlg,IDC_DEST, CB_GETLBTEXT,(WPARAM)k,(LPARAM)tmpstr);		
		if(sscanf(tmpstr,"#IDA=%d #IDT=%d",&Dest[1],&Dest[0])!=2)
				EndDialog(hDlg, FALSE);
		if(Dest[0]==Sour[0]&&Dest[1]==Sour[1])
				EndDialog(hDlg,FALSE);
				EndDialog(hDlg, TRUE);
			   	break;
             default:
              return FALSE;
           }
        return (TRUE);
      }
    } 

    return (FALSE);                           /* Didn't process a message    */
        UNREFERENCED_PARAMETER(lParam);        
}


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void Invert(void *In,short L_in,void *Out,short L_out)
  { 

	
   int I, L;
   unsigned char C='\x00';
   if( *(char*)In &'\x80' ) C='\xff';
   L=( L_in > L_out ) ? L_out : L_in;
   for( I=0; I < L_out ; I++ )
      ((char *)Out)[I]=( I < L ) ? ((char *)In)[L-1-I] : C;
   
   }

TBBUTTON  tbButtonn[8];
HWND tbwnd;
void InitToolBar()
{
	tbButtonn[0].iBitmap=0;
	tbButtonn[0].idCommand=ID_NEXT;
	tbButtonn[0].fsState=TBSTATE_ENABLED;	
    	tbButtonn[0].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[0].dwData=0l;
	tbButtonn[0].iString=0;
	
	tbButtonn[1].iBitmap=1;
	tbButtonn[1].idCommand=ID_PREV;
	tbButtonn[1].fsState=TBSTATE_ENABLED;	
    	tbButtonn[1].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[1].dwData=0l;
	tbButtonn[1].iString=0;

	tbButtonn[2].iBitmap=2;
	tbButtonn[2].idCommand=ID_FORWARD;
	tbButtonn[2].fsState=TBSTATE_ENABLED;	
    	tbButtonn[2].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[2].dwData=0l;
	tbButtonn[2].iString=0;

	tbButtonn[3].iBitmap=3;
	tbButtonn[3].idCommand=ID_BACK;
	tbButtonn[3].fsState=TBSTATE_ENABLED;	
    	tbButtonn[3].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[3].dwData=0l;
	tbButtonn[3].iString=0;

	tbButtonn[4].iBitmap=4;
	tbButtonn[4].idCommand=ID_COPY;
	tbButtonn[4].fsState=TBSTATE_ENABLED;	
    	tbButtonn[4].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[4].dwData=0l;
	tbButtonn[4].iString=0;
	
	tbButtonn[5].iBitmap=5;
	tbButtonn[5].idCommand=ID_STOP;
	tbButtonn[5].fsState=TBSTATE_ENABLED;	
    	tbButtonn[5].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[5].dwData=0l;
	tbButtonn[5].iString=0;
	
	tbButtonn[6].iBitmap=6;
	tbButtonn[6].idCommand=ID_REW;
	tbButtonn[6].fsState=TBSTATE_ENABLED;	
    	tbButtonn[6].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[6].dwData=0l;
	tbButtonn[6].iString=0;

	tbButtonn[7].iBitmap=7;
	tbButtonn[7].idCommand=ID_PAUSE;
	tbButtonn[7].fsState=TBSTATE_ENABLED;	
    	tbButtonn[7].fsStyle=TBSTYLE_BUTTON;
	tbButtonn[7].dwData=0l;
	tbButtonn[7].iString=0;

}




int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
{
	//HWND        hwnd;
	MSG         msg ;
	WNDCLASS    wndclass ; 
	RECT        rect;
	DWORD 	ASPIStatus;
	


	if (!hPrevInstance)
	{
		wndclass.style			= CS_HREDRAW | CS_VREDRAW ;
		wndclass.lpfnWndProc	= WndProc ;
		wndclass.cbClsExtra    	= 0 ;
		wndclass.cbWndExtra    	= 0 ;
		wndclass.hInstance     	= hInstance ;
		wndclass.hIcon         	= LoadIcon (hInstance,"SCANSCSI") ;
		wndclass.hCursor       	= LoadCursor (NULL, IDC_ARROW) ;
		wndclass.hbrBackground 	= GetStockObject (WHITE_BRUSH);
		wndclass.lpszMenuName  	= "Menu1";
		wndclass.lpszClassName 	= szAppName ;

   	RegisterClass (&wndclass) ;
	}
	hInst=hInstance;
	hwnd = CreateWindow (
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
	
hRTFLib=LoadLibrary("RICHED32.DLL");
if(!hRTFLib)
{			MessageBox(hwnd,"RICHED32.DLL not available!",szAppName,MB_ICONSTOP);	 
			return 0;
}

GetClientRect(hwnd,&rect);
hwndedit=CreateWindowEx(0l,"RICHEDIT","",
WS_BORDER|ES_NOHIDESEL|ES_AUTOVSCROLL|ES_MULTILINE|
WS_VISIBLE|WS_CHILD|WS_HSCROLL|WS_VSCROLL,
0,32,rect.right-rect.left,(rect.bottom-rect.top)*3,
hwnd,NULL/*(HMENU)1236*/,hInstance,NULL);
if(hwndedit==0)
{			MessageBox(hwnd,"RICHED WINDOW not available!",szAppName,MB_ICONSTOP);	 
			return 0;
}

InitToolBar();
InitCommonControls();
tbwnd=CreateToolbarEx(
					  hwnd,
					  WS_VISIBLE|WS_CHILD|WS_BORDER,
					  IDR_TOOLBAR1,
					  7,
					  hInstance,
					  IDR_TOOLBAR2,
					  tbButtonn,7
					  ,0,0,16,16,
					  sizeof(TBBUTTON));


if(tbwnd==0)
{			MessageBox(hwnd,"TOOLBAR WINDOW not available!",szAppName,MB_ICONSTOP);	 
			return 0;
}
	ShowWindow (hwnd, nCmdShow) ;
	UpdateWindow(hwnd);

	 ASPIStatus = GetASPI32SupportInfo();
	 switch (HIBYTE(LOWORD(ASPIStatus)))

	 {
		case SS_COMP:			
		

			NumAdapters = (LOWORD(LOBYTE(ASPIStatus)));
			
			break; 

		default:
			MessageBox(hwnd,"ASPI for Windows not available!",szAppName,MB_ICONSTOP);	 
	
			return 0;
	 }

	if(!DialogBox(hInst,"IDD_SOURCE",hwnd,GetDevice))
	{	MessageBox(hwnd,"No Tape Devices Selected!",szAppName,MB_ICONSTOP);	 
		return 0;	
	}

GetCurrentDirectory(100,DIRNAME);
		while (TRUE)
		{
			if (PeekMessage (&msg,NULL,0,0,PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
    
   return msg.wParam ;
}


Dis_all(HWND hwnd)
{

EnableMenuItem(GetMenu(hwnd),ID_REW,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_PREV,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_NEXT,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_FORWARD,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_BACK,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_COPY,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_TAPE,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_SELECT,MF_GRAYED);
EnableMenuItem(GetMenu(hwnd),ID_EXIT,MF_GRAYED);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_REW,0);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_PREV,0);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_NEXT,0);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_FORWARD,0);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_BACK,0);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_COPY,0);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_STOP,0);

}

Enb_all(HWND hwnd)
{

EnableMenuItem(GetMenu(hwnd),ID_REW,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_PREV,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_NEXT,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_FORWARD,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_BACK,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_COPY,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_TAPE,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_SELECT,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_EXIT,MF_ENABLED);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_REW,1);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_PREV,1);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_NEXT,1);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_FORWARD,1);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_BACK,1);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_COPY,1);
SendMessage(tbwnd,TB_ENABLEBUTTON,ID_STOP,1);

}



// **************************************************************************
// *																								 
// *	Procedure:	WINPROC()																 
// *																								 
// **************************************************************************

	LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)

{
static int a=0;
char helpdir[200];
FILE *outseed;
		HDC hdc;
		int adj;
	RECT rect;
	MSG msg;
	OPENFILENAME ofn;
static char szFile[128];
static char szFileTitle[128],szFilter[128];
	//char Smessage[80];
	int i,j;
	int k;
	static char status[80];
	static valid=0;
	char tmpstr[80];
	static int NUMBLOCKS=0;
		switch (message)
	{          
			
		
		case WM_SIZE:
		GetClientRect(hwnd,&rect);
		MoveWindow(hwndedit,0,32,rect.right-rect.left,(rect.bottom-rect.top)-32,1);
		MoveWindow(tbwnd,0,0,rect.right-rect.left,32,1);
		if(a)SendMessage(hwnd,WM_COMMAND,ID_VIEW_HEX,0);
		else a=1;
		return 0;
		case WM_COMMAND:
			switch (wParam)
			{      
			        case ID_EXIT:
				if(hwndedit)DestroyWindow(hwndedit);
				if(hRTFLib)FreeLibrary(hRTFLib);
				SendMessage(hwnd,WM_CLOSE,0,0L);
				return 0;
				case ID_HELP:
				sprintf(helpdir,"%s\\readtape.hlp",DIRNAME);
				WinHelp(hwnd,helpdir,HELP_FINDER,0);
				return 0;
				case ID_STOP:
					Stop=1;
					return 0;
				case ID_TAPE:
					NUMBLOCKS=0;
					
					if(!DialogBox(hInst,"IDD_DESTINATION",hwnd,GetTapeToTape))return 0;
					Dis_all(hwnd);
					RewTarget(hwnd,Sour[0],Sour[1]);
					RewTarget(hwnd,Dest[0],Dest[1]);
					while(1)
					{
					sprintf(tmpstr,"Reading Block Number:%d[65536]",NUMBLOCKS);
					SetWindowText(hwnd,tmpstr);
					//for(i=0;i<65536;i++)Data[i]=0;
					ZeroMemory(&Data[0],65536);

					if(ReadTarget(hwnd,Sour[0],Sour[1],Data,16*8)!=1)break;
					sprintf(tmpstr,"Write Block Number:%d[65536]",NUMBLOCKS);
					SetWindowText(hwnd,tmpstr);
					if(WriteTarget(hwnd,Dest[0],Dest[1],Data,16*8)!=1){goto METKA1;}
					NUMBLOCKS++;
					}

					NUMBLOCKS++;
					if(WriteTarget(hwnd,Dest[0],Dest[1],Data,16*8)!=1)
					{METKA1: MessageBox(hwnd,"Write to Destination Failed","Error",MB_OK);}
					sprintf(tmpstr,"%d[65536] Blocks were writen to Destination",NUMBLOCKS);

					if(MessageBox(hwnd,"Verify Destination ?","Message",MB_OKCANCEL)==IDOK)
					{
					RewTarget(hwnd,Sour[0],Sour[1]);
					RewTarget(hwnd,Dest[0],Dest[1]);
					for(i=0;i<NUMBLOCKS;i++)
					{if(VerifyTarget(hwnd,Dest[0],Dest[1],16*8)!=1)break;
					sprintf(tmpstr,"Verified Block %d[65536] of %d OK",i,NUMBLOCKS);
					SetWindowText(hwnd,tmpstr);
					}
					}
					RewTarget(hwnd,Sour[0],Sour[1]);
					RewTarget(hwnd,Dest[0],Dest[1]);
					Enb_all(hwnd);
					return 0;					 
				case ID_SELECT:
					Dis_all(hwnd);
				DialogBox(hInst,"IDD_SOURCE",hwnd,GetDevice);	
				Enb_all(hwnd);	
				return 0;
					
				case ID_COPY:
					Dis_all(hwnd);
					SendMessage(tbwnd,TB_ENABLEBUTTON,ID_STOP,1);
					
			Stop=0;
			strcpy(szFilter,"(*.*)|*.*|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';

			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =128;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 128;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=(long)NULL ;
			if(!GetSaveFileName(&ofn))goto METKA;
			if((outseed=fopen(szFile,"wb"))==NULL)
			{MessageBox(hwnd,"Cann't Create File For Output","Eror",MB_OK);
								goto METKA;
			//return 0;
			}
			DialogBox(hInst,"IDD_FILTER",hwnd,GetFilter);
			while(!Stop)
			{	
				valid=dec_h((SEED_data_record *)Data,status);
				SetWindowText(hwnd,status);
				if(valid)
				{	if(filter==0)fprintf(outseed,"%s",&Data[56]);
					else if(fwrite(&Data[0],4096,1,outseed)!=1)break;
				}
			
				if (PeekMessage( &msg, NULL, 0, 0 ,PM_REMOVE))
				{
				TranslateMessage( &msg ) ;
				DispatchMessage( &msg ) ;
				}
				if(ReadTarget(hwnd,Dev[0],Dev[1],Data,8)!=1)break;
				


			}
			fclose(outseed);
				goto METKA;
				case ID_VIEW_ANSI:
		GetClientRect(hwnd,&rect);
		MoveWindow(hwndedit,0,32,rect.right-rect.left,(rect.bottom-rect.top)-32,1);
				memcpy(Text,Data,4096);
				Text[4096]=0;
				for(k=0;k<4096;k++)
				if(Text[k]==0)
				Text[k]='.';
				SetWindowText(hwndedit,Text);
				return 0;
				case ID_VIEW_HEX:

		GetClientRect(hwnd,&rect);
		MoveWindow(hwndedit,0,32,rect.right-rect.left,(rect.bottom-rect.top)-32,1);
				for(j=0;j<256;j++)
				{
sprintf(Hex+j*54,"%04d  %02X %02X %02X %02X\t%02X %02X %02X %02X\t%02X %02X %02X %02X\t%02X %02X %02X %02X\n",j*16,
(unsigned char)Data[j*16+0],(unsigned char)Data[j*16+1],(unsigned char)Data[j*16+2],(unsigned char)Data[j*16+3],
(unsigned char)Data[j*16+4],(unsigned char)Data[j*16+5],(unsigned char)Data[j*16+6],(unsigned char)Data[j*16+7],
(unsigned char)Data[j*16+8],(unsigned char)Data[j*16+9],(unsigned char)Data[j*16+10],(unsigned char)Data[j*16+11],
(unsigned char)Data[j*16+12],(unsigned char)Data[j*16+13],(unsigned char)Data[j*16+14],(unsigned char)Data[j*16+15]);}
				Hex[256*54]=0;
				SetWindowText(hwndedit,Hex);
				return 0;
				case ID_VIEW_HEADER:
		
		
		GetClientRect(hwnd,&rect);
		MoveWindow(hwndedit,0,32,rect.right-rect.left,(rect.bottom-rect.top)/3,1);
		sprintf(Hex,"Sequence Number:\t\t\t%c%c%c%c%c%c\n",
		IbmH.sequence [0],
		IbmH.sequence [1],
		IbmH.sequence [2],
		IbmH.sequence [3],
		IbmH.sequence [4],
		IbmH.sequence [5]);
		sprintf(Hex+strlen(Hex),"Data Header indicator:\t\t%c\n",IbmH.seed_record_type);
		sprintf(Hex+strlen(Hex),"Reserved Byte:\t\t\t%c\n",54);//IbmH.continuation_record);
		sprintf(Hex+strlen(Hex),"Station identifier code:\t\t%c%c%c%c%c\n",
		IbmH.station_ID_call_letters [0],
		IbmH.station_ID_call_letters [1],
		IbmH.station_ID_call_letters [2],
		IbmH.station_ID_call_letters [3],
		IbmH.station_ID_call_letters [4]);
		sprintf(Hex+strlen(Hex),"Location identifier:\t\t\t%c%c\n",
		IbmH.location_id[0],
		IbmH.location_id[1]);
		sprintf(Hex+strlen(Hex),"Channal identifier:\t\t\t%c%c%c\n",
		IbmH.channel_ID [0],
		IbmH.channel_ID [1],
		IbmH.channel_ID [2]);
		sprintf(Hex+strlen(Hex),"Network Code:\t\t\t\t%c%c\n",
	 	IbmH.seednet [0],
		IbmH.seednet [1]);
		sprintf(Hex+strlen(Hex),"Record Start Time:\t\t\t%04d.%03d %02d:%02d:%02d %d\n",
		IbmH.starting_time.yr,
		IbmH.starting_time.jday,
		IbmH.starting_time.hr,
		IbmH.starting_time.minute,
		IbmH.starting_time.seconds,
		IbmH.starting_time.tenth_millisec);
		sprintf(Hex+strlen(Hex),"Number of samples:\t\t\t%d\n",IbmH.samples_in_record);
		sprintf(Hex+strlen(Hex),"Sample rate factor:\t\t\t%d\n",IbmH.sample_rate_factor );
		sprintf(Hex+strlen(Hex),"Sample rate multiplier:\t\t%d\n",IbmH.sample_rate_multiplier);
		sprintf(Hex+strlen(Hex),"Activity flags:\t\t\t\t0x%02X\n",(unsigned char)IbmH.activity_flags);
		sprintf(Hex+strlen(Hex),"I/O and clock flags:\t\t\t0x%02X\n",(unsigned char)IbmH.IO_flags);
		sprintf(Hex+strlen(Hex),"Data quality flags:\t\t\t0x%02X\n",(unsigned char)IbmH.data_quality_flags);
		sprintf(Hex+strlen(Hex),"Number of Blockettes that follow:\t%d\n",IbmH.number_of_following_blockettes);
		sprintf(Hex+strlen(Hex),"Time correction:\t\t\t%d\n",IbmH.tenth_msec_correction );
		sprintf(Hex+strlen(Hex),"Bigginning of data:\t\t\t%d\n",IbmH.first_data_byte);
		sprintf(Hex+strlen(Hex),"First blocket:\t\t\t\t%d\n",IbmH.first_blockette_byte);
		SetWindowText(hwndedit,Hex);
		GetClientRect(hwnd,&rect);
		rect.top+=32+(rect.bottom-rect.top)/3;
		hdc=GetDC(hwnd);

		FillRect(hdc,&rect,GetStockObject(WHITE_BRUSH));
		rect.left+=(rect.right-rect.left)/8;
		SetBkMode(hdc,TRANSPARENT);
		SetTextColor(hdc,RGB(0,127,0));
		
		rect.top+=20;
		rect.right-=10;
		rect.bottom-=30;
		
		draw_block(hdc,UData,IbmH.samples_in_record,rect,status);
		ReleaseDC(hwnd,hdc);
return 0;
				case ID_REW:
				Dis_all(hwnd);
				RewTarget(hwnd,Dev[0],Dev[1]);
				goto METKA;								
				case ID_FORWARD:

					DialogBox(hInst,"IDD_FORWARD",hwnd,GetNumber);
					Dis_all(hwnd);
					ForwardTarget(hwnd,Dev[0],Dev[1],Bnumber*8);
					goto METKA;				
				

				case ID_BACK:
				DialogBox(hInst,"IDD_FORWARD",hwnd,GetNumber);
				Dis_all(hwnd);
				ForwardTarget(hwnd,Dev[0],Dev[1],(-Bnumber-1)*8);
				goto METKA;
				case ID_PREV:
				Dis_all(hwnd);
				ForwardTarget(hwnd,Dev[0],Dev[1],(-2)*8);
				goto METKA;
METKA:				case ID_NEXT:
				Dis_all(hwnd);
				adj=CurrTarget(hwnd,Dev[0],Dev[1]);
				if(adj-2>0&&(adj-2)%8!=0)
				ForwardTarget(hwnd,Dev[0],Dev[1],-(adj-2)%8);
				ReadTarget(hwnd,Dev[0],Dev[1],Data,8);
				//SetWindowText(hwndedit,Text);
				SRR=sreadseed(Level,Dframes,(SEED_data_record *)Data,UData,NULL,NULL,&IbmH);
				valid=dec_h((SEED_data_record *)Data,status);
				SetWindowText(hwnd,status);
				Enb_all(hwnd);
				EnableMenuItem(GetMenu(hwnd),ID_VIEW_HEADER,MF_GRAYED);	
				if(SRR<0)SendMessage(hwnd,WM_COMMAND,ID_VIEW_ANSI,0);
				else if(SRR==0)SendMessage(hwnd,WM_COMMAND,ID_VIEW_HEX,0);
				else 
				{
					EnableMenuItem(GetMenu(hwnd),ID_VIEW_HEADER,MF_ENABLED);
					SendMessage(hwnd,WM_COMMAND,ID_VIEW_HEADER,0);
				}
				return 0;
				default:
				return 0;
			}
		case WM_CLOSE:  
			Stop=1;
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0) ;
			return 0 ;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
	
	SRB_ExecSCSICmd ExecSRB;


long ScanTarget(HWND hwnd,BYTE target_id,BYTE adapter_id,char *devinfo)
{          
		char Buffer[100];
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_IN | SRB_DIR_SCSI;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	Buffer;
		ExecSRB.SRB_BufLen      =	32;
		ExecSRB.SRB_SenseLen    =	SENSE_LEN;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	SCSI_INQUIRY; 
		ExecSRB.CDBByte[1] 		=  	0;
		ExecSRB.CDBByte[2]      =  	0;
		ExecSRB.CDBByte[3]      =  	0;
		ExecSRB.CDBByte[4]		=  	32;
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		if(ExecSRB.SRB_Status==SS_COMP)
		strcpy(devinfo,&ExecSRB.SRB_BufPointer[8]);
		else strcpy(devinfo,"");
		BUSY=0;
		return 1;                                                 
}
long RewTarget(HWND hwnd,BYTE target_id,BYTE adapter_id)
{          
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_OUT ;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	0;
		ExecSRB.SRB_BufLen      =	0;
		ExecSRB.SRB_SenseLen    =	0;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	1; 
		ExecSRB.CDBByte[1] 		=  	0;
		ExecSRB.CDBByte[2]      =  	0;
		ExecSRB.CDBByte[3]      =  	0;
		ExecSRB.CDBByte[4]		=  	0;
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		BUSY=0;
		if(ExecSRB.SRB_Status==SS_COMP)
		return 1;
			else return 0;                                                 
}
  
long ForwardTarget(HWND hwnd,BYTE target_id,BYTE adapter_id,long Number)
{          
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_OUT ;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	0;
		ExecSRB.SRB_BufLen      =	0;
		ExecSRB.SRB_SenseLen    =	0;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	17; 
		ExecSRB.CDBByte[1] 		= 0;

		Invert((void *)&Number,4,(void *)&ExecSRB.CDBByte[2],3);
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		BUSY=0;
		if(ExecSRB.SRB_Status==SS_COMP)

		return 1;
			else return 0;                                                 
}

long ReadTarget(HWND hwnd,BYTE target_id,BYTE adapter_id,char *Data,long Number)
{          
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_OUT;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	Data;
		ExecSRB.SRB_BufLen      =	512*Number;
		ExecSRB.SRB_SenseLen    =	0;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	8; 
		ExecSRB.CDBByte[1] 		=   1 ;
		Invert((void *)&Number,4,(void *)&ExecSRB.CDBByte[2],3);
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		BUSY=0;
		if(ExecSRB.SRB_Status==SS_COMP)
		return 1;
		else return -1;
                                                 
}

	           
long CurrTarget(HWND hwnd,BYTE target_id,BYTE adapter_id)
{       char Curr[3];    
	    char tmpstr[80]; 
		int Number;
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_OUT ;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	Curr;
		ExecSRB.SRB_BufLen      =	3;
		ExecSRB.SRB_SenseLen    =	0;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	2; 
		ExecSRB.CDBByte[1] 		=   0;
		ExecSRB.CDBByte[2]      =   0;
		ExecSRB.CDBByte[3]      =   0;
		ExecSRB.CDBByte[4]	=   3;
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		BUSY=0;
		if(ExecSRB.SRB_Status==SS_COMP)
		{
		Invert((void *)Curr,3,(void *)&Number,4);
		//sprintf(tmpstr,"%d",Number);
		//SetWindowText(hwnd,tmpstr);
		return Number;}
			else return -1;                                                 
}


long WriteTarget(HWND hwnd,BYTE target_id,BYTE adapter_id,char *Data,long Number)
{          
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_OUT;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	Data;
		ExecSRB.SRB_BufLen      =	512*Number;
		ExecSRB.SRB_SenseLen    =	0;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	10; 
		ExecSRB.CDBByte[1] 		=   1 ;
		Invert((void *)&Number,4,(void *)&ExecSRB.CDBByte[2],3);
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		BUSY=0;
		if(ExecSRB.SRB_Status==SS_COMP)
		return 1;
		else return -1;
                                                 
}

long VerifyTarget(HWND hwnd,BYTE target_id,BYTE adapter_id,long Number)
{          
		if(BUSY)return -2;
		BUSY=1;
		ExecSRB.SRB_Cmd         = 	SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId        =	adapter_id;
		ExecSRB.SRB_Flags       =	SRB_DIR_OUT;
		ExecSRB.SRB_Target      =	target_id;
		ExecSRB.SRB_BufPointer  =	0;
		ExecSRB.SRB_BufLen      =	0;
		ExecSRB.SRB_SenseLen    =	0;
		ExecSRB.SRB_CDBLen		=	6;
		ExecSRB.CDBByte[0]		=	19; 
		ExecSRB.CDBByte[1] 		=   1 ;
		Invert((void *)&Number,4,(void *)&ExecSRB.CDBByte[2],3);
		ExecSRB.CDBByte[5]      =  	0;
		ExecSRB.SRB_Status == SS_PENDING;
		SendASPI32Command(&ExecSRB);
		while (ExecSRB.SRB_Status == SS_PENDING);
		BUSY=0;
		if(ExecSRB.SRB_Status==SS_COMP)
		return 1;
		else return -1;
                                                 
}
