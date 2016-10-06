#include <windows.h>  
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <commdlg.h>
#include "resource.h"    
#include "steim.h"
#include "steimlib.h"
#include "miniseed.h"
#include "my_time.h"
#include "response.h"
#include "station.h"
char DIRNAME[100];
HWND hWnd;
HINSTANCE hInst;
char Data[4096];
void save_wgse(void){;}
void save_sac(FILE * Sac,daat dd,int *ddd,int sp ,char *stan,char *chan,char *ntype,double rate)
{
int i;
float sacf[70];
int saci  [35];
int sacl[5];
char sacc1[8];
char sacc2[16];
char saca[21][8];
for(i=0;i<70;i++)sacf[i]=(float)(-12345.);
for(i=0;i<35;i++)saci[i]=-12345;
for(i=0;i<5;i++)sacl[i]=0;
sprintf(sacc1,"-12345");
sprintf(sacc2,"-12345");
for(i=0;i<21;i++)sprintf(saca[i],"-12345");
saci[9]=sp;
sacf[0]=(float)(rate/1000.);
sacf[5]=(float)0.;
sacf[6]=(float)((sp-1)*rate/1000.);
saci[15]=1;
sacl[0]=1;
saci[16]=5;
saci[17]=9;
sprintf(sacc1,"%s",stan);
sprintf(saca[17],"%s",chan);
for(i=0;i<6;i++)saci[i]=cdatostrj(dd,i);
saci[6]=6;

for(i=0;i<70;i++)
{
	if(i!=0&&i%5==0)fprintf(Sac,"\n");
	fprintf(Sac,"%15.7f",sacf[i]);
}

for(i=0;i<35;i++)
{
	if(i%5==0)fprintf(Sac,"\n");
	fprintf(Sac,"%10d",saci[i]);
}

for(i=0;i<5;i++)
{
	if(i%5==0)fprintf(Sac,"\n");
	fprintf(Sac,"%10d",sacl[i]);
}

fprintf(Sac,"\n%8s%16s",sacc1,sacc2);
for(i=0;i<21;i++)
{if(i%3==0)fprintf(Sac,"\n");
fprintf(Sac,"%8s",saca[i]);
}
for(i=0;i<sp;i++)
{
if(i%5==0)fprintf(Sac,"\n");
fprintf(Sac,"%15.7g",(double)ddd[i]);
}
fprintf(Sac,"\n");
}
/*void save_wgsn(FILE * Wgsn,daat dd,int *ddd,int sp ,char *stan,char *chan,char *ntype,double rate)
{
int i;
double sensetiv,period;
char lookup[80];



if(getrespdate("response.ini",cdatostrj(dd,0),cdatostrj(dd,1),chan,stan,ntype,lookup)!=0)
{sensetiv=period=1.;}
else 
if(getoldsens("response.ini",lookup,&sensetiv,&period)!=0)
{sensetiv=period=1.;}

	fprintf(Wgsn,"%s %s %d %d %d %d %d %d %g %d %g %g %s\n\n",
		stan,chan,
		cdatostrj(dd,0)-1900,
		cdatostrj(dd,1),
		cdatostrj(dd,2),
		cdatostrj(dd,3),
		cdatostrj(dd,4),
		cdatostrj(dd,5),
		1000./rate,
		sp,
		sensetiv,
		period,
		"ASL");
for(i=0;i<sp;i++)
fprintf(Wgsn,"%d\n",ddd[i]);
}*/

int save_wgsn_new(FILE * WgsnN,daat dd,int *ddd,int sp ,char *stan,char *chan,char *ntype,double rate)
{
char tmpstr[80];
char lookup[80];
char type[80];
double lat,lon,depth,azim,dip,elev;
double nfactor,nfreq,re,im,sens,fsens;
int i,np,nz;
fprintf(WgsnN,"ASL_00\n");
fprintf(WgsnN,"%s %s %s\n",stan,ntype,chan);
fprintf(WgsnN,"%s\n",cdatostr(dd,tmpstr,0));
fprintf(WgsnN,"%d\n",sp);
fprintf(WgsnN,"%g\n",rate);
if(getrespdate("response.ini",cdatostrj(dd,0),cdatostrj(dd,1),chan,stan,ntype,lookup)!=0)
goto Err;
if(get_type(lookup,type,"response.ini")!=0)
goto Err;
fprintf(WgsnN,"%s\n",type);
if(get_nfactor(lookup,&nfactor,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%g\n",nfactor);
if(get_nfreq(lookup,&nfreq,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%g\n",nfreq);
if(get_sens(lookup,&sens,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%g\n",sens);
if(get_fsens(lookup,&fsens,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%g\n",fsens);
if(get_nzeros(lookup,&nz,"response.ini")!=0)goto Err;
if(get_npoles(lookup,&np,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%d %d\n",nz,np);
for(i=0;i<nz;i++)
{
if(get_zeroi(lookup,i,&re,&im,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%g %g\n",re,im);
}

for(i=0;i<np;i++)
{
if(get_polei(lookup,i,&re,&im,"response.ini")!=0)goto Err;
fprintf(WgsnN,"%g %g\n",re,im);
}
if(getstandate("station.ini",cdatostrj(dd,0),cdatostrj(dd,1),chan,stan,ntype,lookup)!=0)
goto Err;
if(get_lat(lookup,&lat,"station.ini")!=0)goto Err;
if(get_lon(lookup,&lon,"station.ini")!=0)goto Err;
if(get_elev(lookup,&elev,"station.ini")!=0)goto Err;
if(get_depth(lookup,&depth,"station.ini")!=0)goto Err;
if(get_azim(lookup,&azim,"station.ini")!=0)goto Err;
if(get_dip(lookup,&dip,"station.ini")!=0)goto Err;
fprintf(WgsnN,"%g %g %g %g %g %g\n",lat,lon,elev,depth,azim,dip);
for(i=0;i<sp;i++)
fprintf(WgsnN,"%d\n",ddd[i]);
return 0;
Err:
fseek(WgsnN,0,SEEK_SET);
fprintf(WgsnN,"FERROR\n");
return -1;
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









daat confromstr(char *string)
{	char tmpstr[80];
	daat d;
	int yy,mm,dd,hh,mi,ss,ts;
	sscanf(string,"%4d.%2d.%2d %2d:%2d:%2d.%d",&yy,&mm,&dd,&hh,&mi,&ss,&ts);
	sprintf(tmpstr,"%04d%02d%02d%02d%02d%02d%04d",yy,mm,dd,hh,mi,ss,ts*10);
	d=cstrtoda(tmpstr);
	return d;
}
long dec_h(SEED_data_record *sdr,char *ntype,char *stname,char *chname,daat *d1,daat *d2,char *chfilter,char *stfilter,int filter,double *rate)
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

sprintf(ntype,"%c%c",
		sdr->Head.seednet[0],
		sdr->Head.seednet[1]);
if(ntype[0]==' ' &&ntype[1]==' '){ntype[0]='I';ntype[1]='U';}


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
ntype[2]=0;
return 0;
}

void Getlist(char *list,char *stname,char *chname,daat *d1,daat *d2,int *filepos)
{
int yy,mm,dd,hh,mi,ss,ts,i;
char tmpstr[80];
sscanf(list,"%s %s",stname,chname);
for(i=0;i<5;i++)stname[i]=list[i];stname[5]=0;
for(i=0;i<3;i++)chname[i]=list[i+5];chname[3]=0;
sscanf(list+9,"%4d.%2d.%2d %2d:%2d:%2d.%d",&yy,&mm,&dd,&hh,&mi,&ss,&ts);
sprintf(tmpstr,"%04d%02d%02d%02d%02d%02d%04d",yy,mm,dd,hh,mi,ss,ts*10);
*d1=cstrtoda(tmpstr);
sscanf(list+9+23,"%4d.%2d.%2d %2d:%2d:%2d.%d",&yy,&mm,&dd,&hh,&mi,&ss,&ts);
sprintf(tmpstr,"%04d%02d%02d%02d%02d%02d%04d",yy,mm,dd,hh,mi,ss,ts*10);
*d2=cstrtoda(tmpstr);
sscanf(list+23+9+24,"%d",filepos);
}

int event=0;
int onlin=0;
daat DC1,DC2;
//FILE *outfile;
typedef struct {char info[3];long inin[3];} inform; 
inform aaa;
LRESULT APIENTRY GetStatus(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
	char ST[80];
	char CH[80];
	char NT[80];
	char STLIST[80];
	char CHLIST[80];
	char ssdata[80];
	char sedata[80];
	daat StartList,EndList;
	static int iii;
	int filelist;
	int CurrPoz=0;
	int Udata[MAXSEEDSAMPLES];
	long flength;
	int persent,persent1;

	daat DT1,DT2;
	
	double RTT;
	static int BSIZE=4096;
	FILE *in,*out,*inlog,*outlog;
	int strcount=0;
	int filecount=0;
	int numb;
	long *DATAARRAY;
	int nsamp;
	daat DATASTART;
	
	int NSAMP;
	int i;
	daat currd;
	double raz;
	static SHORT Level=1;
	static SHORT Dframes=63;
char helpdir[200];
	daat d1,d2;
	char tmpstr1[80];
	char tmpstr2[80];
	char tmpstr10[80];
	char tmpstr20[80];
	OPENFILENAME ofn,ofn1;
	char Time1[80],Time2[80];
	static double Rate;
	int SeekPoz;
	int rrr=0;
	static char STATION[80];
	static char CHANNEL[80];
	static char NTYPE[80];
	static char szFile[128];
	static char szFile1[128];
	//static int event=0;
	char szFileTitle[128],szFilter[128];
	char szFileTitle1[128],szFilter1[128];
	char tmpstr[80];		

    switch (message) 
{
       case WM_INITDIALOG:
		  // outfile=fopen("C:\\header.txt","w");
		  SendDlgItemMessage(hDlg,IDC_SGR,BM_SETCHECK,(WPARAM)1,(LPARAM)0) ;
		   SetDlgItemText(hDlg,IDC_BSIZE,"4096");	
		   SetDlgItemText(hDlg,IDC_STAN,"?????");
		   SetDlgItemText(hDlg,IDC_CHAN,"???");	
			EnableWindow(GetDlgItem(hDlg,IDC_START),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_END),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_RESCAN),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_APPLY),FALSE);


		   return TRUE;
			case WM_CLOSE:          
			EndDialog(hDlg,TRUE);  			
			//fclose(outfile);

		return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
		  
          switch(LOWORD(wParam)) 
			{
			  	case IDC_MYHELP:
				sprintf(helpdir,"%s\\readseed.hlp",DIRNAME);
				WinHelp(hDlg,helpdir,HELP_FINDER,0);
				break;
           
		  case IDC_LDATA:
			EnableWindow(GetDlgItem(hDlg,IDC_START),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_END),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE),FALSE);
			  break;
          case IDC_RESCAN:
			EnableWindow(GetDlgItem(hDlg,IDC_START),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_END),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_APPLY),TRUE);

		SetCursor(LoadCursor(hInst,IDC_WAIT));
		strcount=filecount=0;
		GetDlgItemText(hDlg,IDC_BSIZE,tmpstr,80);
		BSIZE=atoi(tmpstr);
		GetDlgItemText(hDlg,IDC_STAN,tmpstr10,80);
		GetDlgItemText(hDlg,IDC_CHAN,tmpstr20,80);
		event=SendDlgItemMessage(hDlg,IDC_CHECK1,BM_GETCHECK,(WPARAM)0,(LPARAM)0);
		onlin=SendDlgItemMessage(hDlg,IDC_ONLINEF,BM_GETCHECK,(WPARAM)0,(LPARAM)0);

		while(SendDlgItemMessage(hDlg,IDC_LDATA,CB_DELETESTRING,(WPARAM)0,(LPARAM)0)!=CB_ERR);
		if((in=fopen(szFile,"rb"))==NULL){
			MessageBox(hDlg,"Cann't Open File","File Error",MB_OK);break;}
		flength=_filelength(_fileno(in))/100;
		


if(onlin)
{fread(&aaa.info[0],15,1,in);
if(aaa.info[0]!='A'||aaa.info[1]!='S'||aaa.info[2]!='L')
{MessageBox(hDlg,"It is Not Online Buffer","File Error",MB_OK);
			fclose(in);break;}

BSIZE=aaa.inin[2];
filecount=aaa.inin[0]+5;
filecount%=aaa.inin[1];
if(fseek(in,filecount*BSIZE+15,SEEK_SET)!=0){
fseek(in,15,SEEK_SET);filecount=0;}
}
			while(1)
			{
				rrr=fread(&Data[0],BSIZE,1,in);
				if(!rrr&&!onlin)break;
		    if(onlin){	
				if(!rrr)filecount=0;
			filecount%=aaa.inin[1];
			if(filecount==0)fseek(in,15,SEEK_SET);
			if(filecount==aaa.inin[0])break;}
			//	if(strcount>20)break;
		if(dec_h((SEED_data_record*)(Data),NT,ST,CH,&d1,&d2,tmpstr20,tmpstr10,event,&Rate)<0){filecount++;continue;}
		persent=ftell(in)/flength;
		if(persent!=persent1){sprintf(tmpstr,"Creating Data Base %ld%c",persent,0x25);
		SetWindowText(hDlg,tmpstr);}
		persent1=persent;
		sprintf(tmpstr,"%s%s %s %s %06d",ST,CH,cdatostr(d1,ssdata,1),cdatostr(d2,sedata,1),filecount);
		//SetWindowText(hDlg,tmpstr);

		if(strcount==0){SendDlgItemMessage(hDlg,IDC_LDATA,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	//	fprintf(outfile,"%s\n",tmpstr);
		strcount++;filecount++;continue;}
			for(i=0;i<strcount;i++)
			{
			SendDlgItemMessage(hDlg,IDC_LDATA,CB_GETLBTEXT,(WPARAM)i,(LPARAM)tmpstr);
			Getlist(tmpstr,STLIST,CHLIST,&StartList,&EndList,&filelist);
			if(strcmp(ST,STLIST)==0&&strcmp(CH,CHLIST)==0)
			{cdatostr(d1,tmpstr1,1);
			 cdatostr(EndList,tmpstr2,1);
			 raz=fabs(raznd(d1,EndList));
			if((raz>=0. &&raz<=Rate)||(raz<0.&&raz>-Rate))break;
			}
			}
	if(i!=strcount)
	{sprintf(tmpstr,"%s%s %s %s %06d",ST,CH,cdatostr(StartList,ssdata,1),cdatostr(d2,sedata,1),filelist);
	SendDlgItemMessage(hDlg,IDC_LDATA,CB_DELETESTRING,(WPARAM)i,(LPARAM)0);
	SendDlgItemMessage(hDlg,IDC_LDATA,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	}
	else {
	sprintf(tmpstr,"%s%s %s %s %06d",ST,CH,cdatostr(d1,ssdata,1),cdatostr(d2,sedata,1),filecount);
	SendDlgItemMessage(hDlg,IDC_LDATA,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	strcount ++;
	}
	
	filecount++;	

				}

			fclose(in);
			SendDlgItemMessage(hDlg,IDC_LDATA,CB_SETCURSEL,(WPARAM)0,(LPARAM)0);	  

			  break;
		  case IDC_APPLY:
			EnableWindow(GetDlgItem(hDlg,IDC_START),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_END),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE),TRUE);
		  i=SendDlgItemMessage(hDlg,IDC_LDATA,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);	  
		  SendDlgItemMessage(hDlg,IDC_LDATA,CB_GETLBTEXT,(WPARAM)i,(LPARAM)tmpstr);
		 //sscanf(tmpstr,"%s %s %s %s %s %d",STATION,CHANNEL,Time1,Time2,&SeekPoz);
		  for(i=0;i<5;i++)STATION[i]=tmpstr[i];STATION[5]=0;
		  for(i=0;i<3;i++)CHANNEL[i]=tmpstr[i+5];CHANNEL[3]=0;
		  sscanf(tmpstr+32+24,"%d",&SeekPoz);
		  strcpy(Time1,tmpstr+8);
		  strcpy(Time2,tmpstr+32);
			Time1[24]=0;
			Time2[24]=0;
			DC1=confromstr(Time1);
			DC2=confromstr(Time2);
		  SetDlgItemText(hDlg,IDC_START,Time1);
		  SetDlgItemText(hDlg,IDC_END,Time2);

			break;
		  case IDC_BUTTON1:
			strcpy(szFilter1,"(*.*)|*.*|");
			for (i=0; szFilter1[i]; i++)
			if (szFilter1[i] =='|') szFilter1[i] = '\0';
			memset(&ofn1, 0, sizeof(OPENFILENAME));
			ofn1.lStructSize = sizeof(OPENFILENAME);
			ofn1.hwndOwner = hWnd;
			ofn1.lpstrFilter = szFilter1;
			ofn1.nFilterIndex = 1;
			ofn1.lpstrFile = szFile1;
			ofn1.nMaxFile =128;
			ofn1.lpstrFileTitle = szFileTitle1;
			ofn1.nMaxFileTitle = 128;
			ofn1.lpstrInitialDir =NULL;
			ofn1.Flags=(long)NULL ;
			if(!GetSaveFileName(&ofn1))break;
			if((outlog=fopen(szFile1,"w"))==NULL)break;
			i=0;
			fprintf(outlog,"%s\n",szFile);
			while(SendDlgItemMessage(hDlg,IDC_LDATA,CB_GETLBTEXT,(WPARAM)i,(LPARAM)tmpstr)!=CB_ERR)
			{fprintf(outlog,"%s\n",tmpstr);i++;}
			 fclose(outlog);
			  
			  break;

			case IDC_BUTTON2:
			strcpy(szFilter1,"(*.*)|*.*|");
			for (i=0; szFilter1[i]; i++)
			if (szFilter1[i] =='|') szFilter1[i] = '\0';
			memset(&ofn1, 0, sizeof(OPENFILENAME));
			ofn1.lStructSize = sizeof(OPENFILENAME);
			ofn1.hwndOwner = hWnd;
			ofn1.lpstrFilter = szFilter1;
			ofn1.nFilterIndex = 1;
			ofn1.lpstrFile = szFile1;
			ofn1.nMaxFile =128;
			ofn1.lpstrFileTitle = szFileTitle1;
			ofn1.nMaxFileTitle = 128;
			ofn1.lpstrInitialDir =NULL;
			ofn1.Flags=(long)NULL ;
			if(!GetOpenFileName(&ofn1))break;
			if((inlog=fopen(szFile1,"r"))==NULL)break;
			while(SendDlgItemMessage(hDlg,IDC_LDATA,CB_DELETESTRING,(WPARAM)0,(LPARAM)0)!=CB_ERR);
			fscanf(inlog,"%s\n",szFile);
			while(fgets(tmpstr,80,inlog))
			{
			tmpstr[strlen(tmpstr)-1]=0;
			SendDlgItemMessage(hDlg,IDC_LDATA,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
			}
			EnableWindow(GetDlgItem(hDlg,IDC_APPLY),TRUE);
			fclose(inlog);
			break;

		  
		  case IDC_OPEN:
			EnableWindow(GetDlgItem(hDlg,IDC_START),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_END),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_APPLY),FALSE);
			
			strcpy(szFilter,"Online Buffers(*.*)|*.*|");
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
			if(!GetOpenFileName(&ofn))break;
		    EnableWindow(GetDlgItem(hDlg,IDC_RESCAN),TRUE);
			
			break;
		  case IDC_SAVE:
			strcpy(szFilter1,"(*.*)|*.*|");
			for (i=0; szFilter1[i]; i++)
			if (szFilter1[i] =='|') szFilter1[i] = '\0';
			memset(&ofn1, 0, sizeof(OPENFILENAME));
			ofn1.lStructSize = sizeof(OPENFILENAME);
			ofn1.hwndOwner = hWnd;
			ofn1.lpstrFilter = szFilter1;
			ofn1.nFilterIndex = 1;
			ofn1.lpstrFile = szFile1;
			ofn1.nMaxFile =128;
			ofn1.lpstrFileTitle = szFileTitle1;
			ofn1.nMaxFileTitle = 128;
			ofn1.lpstrInitialDir =NULL;
			ofn1.Flags=(long)NULL ;
			if(!GetSaveFileName(&ofn1))break;
			if((in=fopen(szFile,"rb"))==NULL)
			{MessageBox(hDlg,"Cann't Open input File","File Open Error",MB_OK);break;}
			if((out=fopen(szFile1,"wb"))==NULL)
			{MessageBox(hDlg,"Cann't Open input File","File Open Error",MB_OK);break;}
			fseek(in,SeekPoz*BSIZE,SEEK_SET);
			if(onlin)fseek(in,15,SEEK_CUR);
			CurrPoz=SeekPoz;
			GetDlgItemText(hDlg,IDC_START,tmpstr,80);
			DT1=confromstr(tmpstr);
			GetDlgItemText(hDlg,IDC_END,tmpstr,80);
			DT2=confromstr(tmpstr);
			Dframes=(BSIZE-64)/64;
			NSAMP=0;
			if(raznd(DC1,DT1)>0||raznd(DC2,DT2)<0)
			{
MessageBox(hDlg,"Selected Time Interval Must Be Inside Status Time","Time Error",MB_OK);
fclose(in);fclose(out);break;
			}


			while(1)
			{
				if(onlin){
					if(CurrPoz==0)fseek(in,15,SEEK_SET);
					CurrPoz++;
					CurrPoz%=aaa.inin[1];
				}

				rrr=fread(&Data[0],BSIZE,1,in);
				if(!rrr)break;
				
			
			if(dec_h((SEED_data_record*)(Data),NT,ST,CH,&d1,&d2,CHANNEL,STATION,event,&Rate)<0)continue;
			if(NSAMP==0)
			{   RTT=Rate;
				strcpy(NTYPE,NT);
				nsamp=(int)(raznd(DT2,DT1)/Rate)+10;
				DATAARRAY=(long *)malloc(nsamp*4);
				if(!DATAARRAY){
MessageBox(hDlg,"Not Enought Memory To allocate Data","Memory Error",MB_OK);
fclose(in);fclose(out);return(TRUE);}
			}
			//if(raznd(DT1,d1)<0)continue;
			if(raznd(DT1,d2)>0)continue;
			if(raznd(DT2,d1)<0)break;
			numb=sreadseed(Level,Dframes,(SEED_data_record *)Data,Udata);
			if(numb<=0)continue;
			for(i=0;i<numb;i++)
			{currd=daatadsec(d1,Rate*i);
			

			if(raznd(currd,DT1)>=0.)
			{if(NSAMP==0){DATASTART.day=currd.day;DATASTART.msec=currd.msec;}
				DATAARRAY[NSAMP]=Udata[i];NSAMP++;}
			if(raznd(currd,DT2)>0.)break;
			}

			}
			if(SendDlgItemMessage(hDlg,IDC_WGSE,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)
			save_wgse();
			else if(SendDlgItemMessage(hDlg,IDC_SGR,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)
			save_sac(out,DATASTART,DATAARRAY,NSAMP,STATION,CHANNEL,NTYPE,RTT);
				else if(save_wgsn_new(out,DATASTART,DATAARRAY,NSAMP,STATION,CHANNEL,NTYPE,RTT)!=0)
				{MessageBox(hDlg,"Cann't fill Information from response.ini and station.ini","Error Message",MB_OK|MB_ICONSTOP);}
			free(DATAARRAY);
			fclose(in);fclose(out);
			break;

		  case IDC_SAVE_ALL:
			  EnableWindow(GetDlgItem(hDlg,IDC_APPLY),TRUE);
			strcpy(szFilter1,"(*.*)|*.*|");
			for (i=0; szFilter1[i]; i++)
			if (szFilter1[i] =='|') szFilter1[i] = '\0';
			memset(&ofn1, 0, sizeof(OPENFILENAME));
			ofn1.lStructSize = sizeof(OPENFILENAME);
			ofn1.hwndOwner = hWnd;
			ofn1.lpstrFilter = szFilter1;
			ofn1.nFilterIndex = 1;
			ofn1.lpstrFile = szFile1;
			ofn1.nMaxFile =128;
			ofn1.lpstrFileTitle = szFileTitle1;
			ofn1.nMaxFileTitle = 128;
			ofn1.lpstrInitialDir =NULL;
			ofn1.Flags=(long)NULL ;
			if(!GetSaveFileName(&ofn1))break;
			if((in=fopen(szFile,"rb"))==NULL)
			{MessageBox(hDlg,"Cann't Open input File","File Open Error",MB_OK);break;}
			if((out=fopen(szFile1,"wb"))==NULL)
			{MessageBox(hDlg,"Cann't Open input File","File Open Error",MB_OK);break;}
			iii=0;
			while(SendDlgItemMessage(hDlg,IDC_LDATA,CB_GETLBTEXT,(WPARAM)iii,(LPARAM)tmpstr)!=CB_ERR)
			{
			//SendMessage(hDlg,WM_COMMAND,(WPARAM)IDC_APPLY,(LPARAM)0);
		  for(i=0;i<5;i++)STATION[i]=tmpstr[i];STATION[5]=0;
		  for(i=0;i<3;i++)CHANNEL[i]=tmpstr[i+5];CHANNEL[3]=0;
		  sscanf(tmpstr+32+24,"%d",&SeekPoz);
		  strcpy(Time1,tmpstr+8);
		  strcpy(Time2,tmpstr+32);
			Time1[24]=0;
			Time2[24]=0;
			DC1=confromstr(Time1);
			DC2=confromstr(Time2);
			DT1=confromstr(Time1);
			DT2=confromstr(Time2);
			fseek(in,SeekPoz*BSIZE,SEEK_SET);
			if(onlin)fseek(in,15,SEEK_CUR);
			CurrPoz=SeekPoz;
			Dframes=(BSIZE-64)/64;
			NSAMP=0;
			if(raznd(DC1,DT1)>0||raznd(DC2,DT2)<0)
			{
MessageBox(hDlg,"Selected Time Interval Must Be Inside Status Time","Time Error",MB_OK);
fclose(in);fclose(out);break;
			}


			while(1)
			{
				if(onlin){
					if(CurrPoz==0)fseek(in,15,SEEK_SET);
					CurrPoz++;
					CurrPoz%=aaa.inin[1];
				}

				rrr=fread(&Data[0],BSIZE,1,in);
				if(!rrr)break;
				
			
			if(dec_h((SEED_data_record*)(Data),NT,ST,CH,&d1,&d2,CHANNEL,STATION,event,&Rate)<0)continue;
			if(NSAMP==0)
			{   RTT=Rate;
				strcpy(NTYPE,NT);
				nsamp=(int)(raznd(DT2,DT1)/Rate)+10;
				DATAARRAY=(long *)malloc(nsamp*4);
				if(!DATAARRAY){
MessageBox(hDlg,"Not Enought Memory To allocate Data","Memory Error",MB_OK);
fclose(in);fclose(out);return(TRUE);}
			}
			//if(raznd(DT1,d1)<0)continue;
			if(raznd(DT1,d2)>0)continue;
			if(raznd(DT2,d1)<0)break;
			numb=sreadseed(Level,Dframes,(SEED_data_record *)Data,Udata);
			if(numb<=0)continue;
			for(i=0;i<numb;i++)
			{currd=daatadsec(d1,Rate*i);
			

			if(raznd(currd,DT1)>=0.)
			{if(NSAMP==0){DATASTART.day=currd.day;DATASTART.msec=currd.msec;}
				DATAARRAY[NSAMP]=Udata[i];NSAMP++;}
			if(raznd(currd,DT2)>0.)break;
			}

			}
			if(SendDlgItemMessage(hDlg,IDC_WGSE,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)
save_wgse();
				//save_wgse(out,DATASTART,DATAARRAY,NSAMP,STATION,CHANNEL,NTYPE,RTT);
			else if(SendDlgItemMessage(hDlg,IDC_SGR,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)
			save_sac(out,DATASTART,DATAARRAY,NSAMP,STATION,CHANNEL,NTYPE,RTT);
				else if(save_wgsn_new(out,DATASTART,DATAARRAY,NSAMP,STATION,CHANNEL,NTYPE,RTT)!=0)
				{MessageBox(hDlg,"Cann't fill Information from response.ini and station.ini","Error Message",MB_OK|MB_ICONSTOP);}
			free(DATAARRAY);
			iii++;			
			}
	fclose(in);fclose(out);
			break;


			default:
            return FALSE;
           }
        return (TRUE);
			}
    } 

    return (FALSE);                           /* Didn't process a message    */
        
}




LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);



int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
{
	
	MSG         msg ;
	WNDCLASS    wndclass ; 
	
	char *szAppName="MiniSeed";

	
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
		
		 DialogBox(hInst,"IDD_GETSTAT",hWnd,GetStatus);
		 PostQuitMessage(0);
			break;
		
		default:

	return DefWindowProc (hWnd, message, wParam, lParam) ;
		}

}
	



