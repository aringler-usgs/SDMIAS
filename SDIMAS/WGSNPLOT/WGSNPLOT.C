#include <windows.h>
#include <sys/stat.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "resource.h"
#include "wgsnplot.h"
#include "complex.h"
#include "response.h"
#include "map.h"
#define DT 40
#define DB 40
#define DL 70
#define DR 30
HACCEL hAccel;
int REV1=1,REV2=1,REV3=1;
int MOTX,MOTY,MOTZ;
int FORDER=4;
int FIBAND=3;
char renwtype[80];
double FFLOW=0.0;
double FFHIGH=0.0;
char *Ftype[]={"LP","HP","BP","BS"};
LRESULT APIENTRY GetAxes(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetRename(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetSpectrAxes(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetComponents(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetDeleteComponents(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetUserFilter(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY FillTimeArrival(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY FillAmplitude(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY LookAmplitude(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY LookArrival(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetMotionComponents(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetPredictTimes(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetVadat(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetRotation(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetSourceAz(HWND ,UINT,UINT,LONG);
LRESULT APIENTRY GetStatistic(HWND ,UINT,UINT,LONG);
double GMIN,GMAX;
double samin,samax,sfmin,sfmax;
CHOOSEFONT cf;
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

sort_sta_enz( const void *a ,const void *b)
{
Seismogr *a1,*b1;
int k;
a1=(Seismogr *)a;
b1=(Seismogr *)b;
k=strcmp(a1->Sta,b1->Sta);
if(k<0)return -1;
else if(k>0)return 1;
k=strcmp(a1->Nt,b1->Nt);
if(k<0)return -1;
else if(k>0)return 1;
k=strcmp(a1->Comment,b1->Comment);
if(k<0)return -1;
else if(k>0)return 1;
else return strcmp(a1->Cha,b1->Cha);
}

sort_sta_zen( const void *a ,const void *b)
{
Seismogr *a1,*b1;
int k;
char t1[5],t2[5];
a1=(Seismogr *)a;
b1=(Seismogr *)b;
strcpy(t1,a1->Cha);
strcpy(t2,b1->Cha);
if(t1[2]=='Z')t1[2]='A';
if(t1[2]=='E')t1[2]='B';
if(t1[2]=='N')t1[2]='C';

if(t2[2]=='Z')t2[2]='A';
if(t2[2]=='E')t2[2]='B';
if(t2[2]=='N')t2[2]='C';
k=strcmp(a1->Sta,b1->Sta);
if(k<0)return -1;
else if(k>0)return 1;
k=strcmp(a1->Nt,b1->Nt);
if(k<0)return -1;
else if(k>0)return 1;
k=strcmp(a1->Comment,b1->Comment);
if(k<0)return -1;
else if(k>0)return 1;
return strcmp(t1,t2);
}



sort_sta_zne( const void *a ,const void *b)
{
Seismogr *a1,*b1;
int k;
a1=(Seismogr *)a;
b1=(Seismogr *)b;
k=strcmp(a1->Sta,b1->Sta);
if(k<0)return -1;
else if(k>0)return 1;
k=strcmp(a1->Nt,b1->Nt);
if(k<0)return -1;
else if(k>0)return 1;
k=strcmp(a1->Comment,b1->Comment);
if(k<0)return -1;
else if(k>0)return 1;
else return -strcmp(a1->Cha,b1->Cha);
}


sort_dist( const void *a ,const void *b)
{
Seismogr *a1,*b1;
double d1,d2;
a1=(Seismogr *)a;
b1=(Seismogr *)b;

d1=get_dist(a1->Sta,a1->Nt,DEP);
d2=get_dist(b1->Sta,b1->Nt,DEP);
if(d1<d2)return -1;
else if(d1>d2)return 1;
else return 0;
}




int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
{
	
	MSG         msg ;
	WNDCLASS    wndclass ; 
	int xScreen, yScreen;
	
	char *szAppName="WgsnplotNew";

	xScreen = GetSystemMetrics(SM_CXSCREEN);
	 yScreen = GetSystemMetrics(SM_CYSCREEN);
	if (!hPrevInstance)
	{
		wndclass.style			= CS_HREDRAW | CS_VREDRAW ;
		wndclass.lpfnWndProc	= WndProc ;
		wndclass.cbClsExtra    	= 0 ;
		wndclass.cbWndExtra    	= 0 ;
		wndclass.hInstance     	= hInstance ;
		wndclass.hIcon         	= NULL ;
		wndclass.hCursor       	= LoadCursor (NULL,IDC_CROSS) ;
		
		wndclass.hbrBackground 	= (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName  	= "IDR_MENU1";
		wndclass.lpszClassName 	= szAppName ;

   	if(!RegisterClass (&wndclass) )return 0;
	}

	hInst=hInstance;
	if(!hInst)return 0;

	hWnd = CreateWindow (
				szAppName,       		// window class name
				"WgsnPlotWin95",	// window caption
				WS_OVERLAPPEDWINDOW,    // window style
				CW_USEDEFAULT,          // initial x position
				CW_USEDEFAULT,          // initial y position
				CW_USEDEFAULT,          // initial x size
				CW_USEDEFAULT,          // initial y size
				NULL,                   // parent window handle
				NULL,                   // window menu handle
				hInstance,              // program instance handle
				NULL) ;		     		// creation parameters
	
	
	
	if(!hWnd )return 0;
	


hAccel=LoadAccelerators(hInst,"IDR_MENU1");

	ShowWindow(hWnd,SW_MAXIMIZE);
    UpdateWindow(hWnd);


	while (GetMessage( &msg, NULL, 0, 0 ))
   {
        if(!TranslateAccelerator(hWnd,hAccel,&msg) )
		
		{TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;}
   }
   return (msg.wParam);           /* Returns the value from PostQuitMessage */
	
}


int GetFileNameNumber(char *buffer)
{
int i=0;
int n=0;
while(!(buffer[i]==0&&buffer[i+1]==0))
{
if(buffer[i+1]==0)n++;
i++;
}
if(n>1)n-=1;
return n;
}

int GetFileNameName(char *buffer,int An,char *name,int Fn)
{
int i=0;
int n=0;
if(An==1){strcpy(name,buffer);
return 0;}
if(An<Fn)return -1;
while(!(buffer[i]==0&&buffer[i+1]==0))
{
if(buffer[i+1]==0)n++;
if(n==Fn)break;
i++;
}
strcpy(name,&buffer[i+2]);
return 0;
}



double getglobal_min(int n,int counts,daat tmin,daat tmax)
{
int i;
int j=0;
double Min,min;

for(i=0;i<n;i++)
{
if(S[i].A)
{min=get_min(S[i],tmin,tmax);
 if(!counts)min/=S[i].Sens;
 if(j==0)Min=min;else if(Min>min)Min=min; j++;}
}
return Min;
}

double getglobal_max(int n,int counts,daat tmin,daat tmax)
{
int i;
int j=0;
double Max,max;

for(i=0;i<n;i++)
{
if(S[i].A)
{max=get_max(S[i],tmin,tmax);
 if(!counts)max/=S[i].Sens;
 if(j==0)Max=max;else if(Max<max)Max=max; j++;}
}
return Max;
}

void ClearMemDc(HDC memdc)
{
	RECT r;
	r.left=r.top=0;
	r.bottom=maxY;
	r.right=maxX;
	FillRect(memdc,&r,GetStockObject(WHITE_BRUSH));
}

void draw_msg(HWND hwd,char *coma)
{
HDC hdc;
RECT r;
hdc=GetDC(hwd);
GetClientRect(hwd,&r);
ClearMemDc(hdc);
SetTextAlign(hdc,TA_CENTER|TA_TOP);
TextOut(hdc,(r.left+r.right)/2,(r.top+r.bottom)/2,coma,strlen(coma));
ReleaseDC(hwd,hdc);
}


	HPEN n_p,o_p;
	PRINTDLG pd;
	DOCINFO DocInfo;
	RECT R;
	daat Current,prevt;
	int cindex,previndex;
int comp_comp( void )
{
int i,j=0,k=-1;
int A[3];
int B[3];
char tmpbuf[200];
char tmpbuf1[200];
if(Nactiv<3)return 1;
A[0]=A[1]=A[2]=0;
B[0]=B[1]=B[2]=0;

for(i=0;i<Ntrass;i++)
{
if(S[i].A)
{if(j==0)
	{sprintf(tmpbuf,"%5s %2s %3s %s",S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment);
	tmpbuf[11]='*';
	if(S[i].Cha[2]=='Z')A[0]=B[0]=1;
	if(S[i].Cha[2]=='N')A[1]=1;
	if(S[i].Cha[2]=='E')A[2]=1;
	if(S[i].Cha[2]=='1')B[1]=1;
	if(S[i].Cha[2]=='2')B[2]=1;
	
	//SetWindowText(hWnd,tmpbuf);
	}
else 
	{sprintf(tmpbuf1,"%5s %2s %3s %s",S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment);
	tmpbuf1[11]='*';
	if(S[i].Cha[2]=='Z')A[0]=B[0]=1;
	if(S[i].Cha[2]=='N')A[1]=1;
	if(S[i].Cha[2]=='E')A[2]=1;
	if(S[i].Cha[2]=='1')B[1]=1;
	if(S[i].Cha[2]=='2')B[2]=1;
	if(strcmp(tmpbuf1,tmpbuf)!=0)return -1;
	}
j++;}
k=j;


}

if(k!=3)return k;

if(A[0]+A[1]+A[2]==3)
return k;
if(B[0]+B[1]+B[2]==3)return -k;
return -5;
}

void fill_amp(char *sta,char *nt,char *cha,int ind,daat time,daat time1,char type,char *comment,char *wtyp)
{
int i,k;
int kind1,kind2;
int nam;
double amin;
double amax;

//chat strbuf5[80];
for(i=0;i<Nampli;i++)
{
if((strcmp(sta,SA[i].Sta)==0)
   &&(strcmp(nt,SA[i].Nt)==0)
   &&(strcmp(cha,SA[i].Cha)==0)
   &&(strcmp(wtyp,SA[i].Wtype)==0))break;
}
nam=i;
strcpy(SA[nam].Sta,sta);
strcpy(SA[nam].Nt,nt);
strcpy(SA[nam].Cha,cha);
strcpy(SA[nam].Wtype,wtyp);
//cdatostr(time,strbuf5,0);
//SA[i].Tampli=cstrtoda(time);
//SA[i].Tampli.day=time.day;
//SA[i].Tampli.msec=time.msec;
kind1=getnum_sei(S[ind],time1);
kind2=getnum_sei(S[ind],time);
amin=amax=S[ind].Data[kind1];
//i=j=kind1;

for(k=kind1;k<=kind2;k++)
{
if(k<0||k>=S[ind].Samp)continue;

if(S[ind].Data[k]>amax){amax=S[ind].Data[k];}
if(S[ind].Data[k]<amin){amin=S[ind].Data[k];}
}
amax=(amax-amin)/2;

SA[nam].period=raznd(time,time1)/1000.;
SA[nam].amplit=fabs(amax/S[ind].Sens);
SA[nam].Tampli.day=time1.day;
SA[nam].Tampli.msec=time1.msec;
SA[nam].Type=type;
strcpy(SA[nam].Comment,comment);
if(nam==Nampli)Nampli+=1;
}


void fill_time(char *sta,char *nt,char *cha,daat time,char *wtyp,char *wqual,char *wsign,char *comment)
{
int i;
for(i=0;i<Narriv;i++)
{
if((strcmp(sta,ST[i].Sta)==0)
   &&(strcmp(nt,ST[i].Nt)==0)
   &&(strcmp(wtyp,ST[i].Wtype)==0))break;
}
strcpy(ST[i].Sta,sta);
strcpy(ST[i].Nt,nt);
strcpy(ST[i].Cha,cha);
strcpy(ST[i].Wtype,wtyp);
strcpy(ST[i].Wqual,wqual);
strcpy(ST[i].Wsign,wsign);
ST[i].Tarrive.day=time.day;
ST[i].Tarrive.msec=time.msec;
strcpy(ST[i].Comment,comment);
if(i==Narriv)Narriv+=1;

}


int get_index_sei(int x,int y)
{
int i;
int j=0;
for(i=0;i<Ntrass;i++)
{
	if(S[i].A)
	{if(x>W[j].left&&x<W[j].right&&y>W[j].top&&y<W[j].bottom)return i;
	j++;}
}
return -1;
}

	LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)

{
	
	//daat tp1,tp2;
	FILE *savexy;
	static daat PrevDDD;
	static zoom=0;
	double rzdt;
	static int mix=0;
	static int prdoc=0;
	static int prscr;
	static int polzer;
	int index;
	int startd,endd;
	int stll;
	double *a,*b,*px,*py;
	int filtertype;
	static int typgra=0;
	double dist,aza;
	int error;
	//double normf;
	POINT P;
	SIZE textm;
	int mmx,mmy;
	int color;
	double fi,lam;	
	static int sourpar=0;	
	static int counts=0;
	static scale=0;
	static smix=0;
	static sscale=0;
	int k,m;
	static int curx=-2,cury=-2;
	int NewN=0;
	int CurN=0;
	static int picktime=0,pickamp=0;
	static int flag=0;
	int wid;
//	double maxg,ming;
	HDC hDC;
	HDC hdc;
	static RECT r;
	int i,j;
	

	PAINTSTRUCT ps;
	static char szFile[1000];
	OPENFILENAME ofn;
	char szFileTitle[1000],szFilter[1000];
	char tmpstr[80];
	char tmpstr1[100];
	
	
		switch (message)
	          
		{
		case WM_CREATE:
			
			GetCurrentDirectory(100,DIRNAME);
			maxX=GetSystemMetrics(SM_CXSCREEN);
			maxY=GetSystemMetrics(SM_CYSCREEN);
			hDC=GetDC(hWnd);
			memdc=CreateCompatibleDC(hDC);
			hbit=CreateCompatibleBitmap(hDC,maxX,maxY);
			SelectObject(memdc,hbit);
			hbrush=GetStockObject(WHITE_BRUSH);
			SelectObject(memdc,hbrush);
			hbrush=GetStockObject(BLACK_PEN);
			SelectObject(memdc,hpen);
			PatBlt(memdc,0,0,maxX,maxY,PATCOPY);
					lf.lfHeight=-10;
					lf.lfItalic=0;
					lf.lfOutPrecision=OUT_TT_ONLY_PRECIS;
					strcpy(&lf.lfFaceName[0],"Arial");
			hFont = CreateFontIndirect(&lf);
			hOldFont=SelectObject(memdc,hFont);
			SetBkMode(memdc,TRANSPARENT);
			ReleaseDC(hWnd,hDC);
			break;

		case WM_RBUTTONDOWN:
	if(sourpar==1)
				{
		GetClientRect(hWnd,&r);
		r.top+=DT;
		r.bottom-=DT;
		r.left+=DT;
		r.right-=DT;
		mmx=LOWORD(lParam);
		mmy=HIWORD(lParam);
		dist=(double)(mmx-(r.right+r.left)/2)*(mmx-(r.right+r.left)/2)+(double)(mmy-(r.top+r.bottom)/2)*(mmy-(r.top+r.bottom)/2);
		dist=sqrt(dist)*MMM/(r.bottom-r.top);
		dist/=6371.;
		if(dist<=1.){
		dist=asin(dist)*6371.;
		if(dist==0.)aza=0.;
		else aza=atan2(mmx-(r.right+r.left)/2,-mmy+(r.top+r.bottom)/2)*180./M_PI;
		filam(FIC,LAMC,dist,aza,&fi,&lam);
		//sprintf(tmpstr,"%g %g",fi,lam);
		//SetWindowText(hWnd,tmpstr);
		}
	}break;

			case WM_LBUTTONDOWN:
		//GetClientRect(hWnd,&r);
		//r.top+=DT;
		//r.bottom-=DT;
		//r.left+=DT;
		//r.right-=DT;

				if(zoom>0||pickamp>0||picktime>0)
{hdc=GetDC(hWnd);
			MoveToEx(hdc,LOWORD(lParam)-1,0,&P);
			LineTo(hdc,LOWORD(lParam)-1,1000);
			ReleaseDC(hWnd,hdc);
}
				if(sourpar==1||typgra==5)
				{
		GetClientRect(hWnd,&r);
		r.top+=DT;
		r.bottom-=DT;
		r.left+=DT;
		r.right-=DT;

		
		mmx=LOWORD(lParam);
		mmy=HIWORD(lParam);
		dist=(double)(mmx-(r.right+r.left)/2)*(mmx-(r.right+r.left)/2)+(double)(mmy-(r.top+r.bottom)/2)*(mmy-(r.top+r.bottom)/2);
		dist=sqrt(dist)*MMM/(r.bottom-r.top);
		dist/=6371.;
		if(dist<=1.){
		dist=asin(dist)*6371.;
		if(dist==0.)aza=0.;
		else aza=atan2(mmx-(r.right+r.left)/2,-mmy+(r.top+r.bottom)/2)*180./M_PI;
		filam(FIC,LAMC,dist,aza,&fi,&lam);
		sprintf(tmpstr,"Lat=%g Lon=%g",fi,lam);
		SetWindowText(hWnd,tmpstr);
		if(sourpar==1){
		sourpar=0;
			FIC=fi;
		LAMC=lam;SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);}

		}
		
		break;}
			Current=daatadsec(Tmin,raznd(Tmax,Tmin)*(LOWORD(lParam)-r.left)/(r.right-r.left));
			cindex=get_index_sei(LOWORD(lParam),HIWORD(lParam));
			if(cindex>=0&&typgra==0)
			{
			
			index=getnum_sei(S[cindex],Current);
			if(index>=0&&index<S[cindex].Samp)
			{sprintf(tmpstr,"%s %s %s A=%g T=%g",S[cindex].Sta,S[cindex].Cha,cdatostr(Current,tmpstr1,1),S[cindex].Data[index]/S[cindex].Sens,raznd(Current,PrevDDD)/1000.);
			SetWindowText(hWnd,tmpstr);}
			}
			
			if(picktime==1)
			{picktime=0;
			if(cindex>=0)
			DialogBox(hInst,"IDD_DIALOG6",hWnd,FillTimeArrival);
			}
			
			if(pickamp==2)
			{
			pickamp=0;
			if(cindex>=0&&previndex==cindex)
			DialogBox(hInst,"IDD_DIALOG7",hWnd,FillAmplitude);
			}

			if(pickamp==1)
			{pickamp=2;
			prevt.day=Current.day;prevt.msec=Current.msec;
			previndex=cindex;}
			
			
			
			if(zoom==1){
						Previous.msec=Current.msec;
						Previous.day=Current.day;
						zoom=2;
						}
			else if(zoom==2){
						    Tmin.msec=Previous.msec;
						    Tmin.day= Previous.day;
							Tmax.msec=Current.msec;
							Tmax.day=Current.day;
							zoom=0;
							SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
							//sprintf(tmpstr,"%d %d",curx,cury);
							//SetWindowText(hWnd,tmpstr);
							}
if(typgra==0){PrevDDD.msec=Current.msec;
PrevDDD.day=Current.day;}

			break;
			case WM_SIZE:
							
			if(typgra==0)
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			if(typgra==1)
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			if(typgra==2)
			SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);
			if(typgra==3)
			SendMessage(hWnd,WM_PLOTVAD,(WPARAM)0,(LPARAM)0);
			if(typgra==5)
			SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);
			break;
			
		
			case WM_PLOTVAD:
			typgra=3;
			GetClientRect(hWnd,&r);
			ClearMemDc(memdc);
			r.top+=DT;
			r.bottom-=DB;
			r.left+=DL;
			r.right-=DR;
			draw_vadat(memdc,r);
			flag=1;
			InvalidateRect(hWnd,NULL,0);
			break;
			case WM_PLOTMAP:
				draw_msg(hWnd,"Filling Map Image , Please Wait!");
			typgra=5;
			GetClientRect(hWnd,&r);
			ClearMemDc(memdc);
			//if(r.bottom>r.right)r.bottom=r.right;
		//	else r.right=r.bottom;
			r.top+=DT;
			r.bottom-=DT;
			r.left+=DT;
			r.right-=DT;
			Rectangle(memdc,r.left,r.top,r.right,r.bottom);
			Draw_map(memdc,MMM/(r.bottom-r.top),FIC,LAMC,r);
			flag=1;
			InvalidateRect(hWnd,NULL,0);
			break;

			case WM_PLOTSEI:
			if(comp_comp()==3)
			EnableMenuItem(GetMenu(hWnd),ID_2DROTATION,MF_ENABLED);
			else 
			EnableMenuItem(GetMenu(hWnd),ID_2DROTATION,MF_GRAYED);
			
			
			if(comp_comp()==-5||comp_comp()==3||comp_comp()==-5)
				EnableMenuItem(GetMenu(hWnd),ID_MOTION,MF_ENABLED);
else EnableMenuItem(GetMenu(hWnd),ID_MOTION,MF_GRAYED);
			if(comp_comp()==-3)
			EnableMenuItem(GetMenu(hWnd),ID_ORIENT,MF_ENABLED);
			else EnableMenuItem(GetMenu(hWnd),ID_ORIENT,MF_GRAYED);
						
			typgra=0;
			

			GetClientRect(hWnd,&r);
			ClearMemDc(memdc);
			r.top+=DT;
			r.bottom-=DB;
			r.left+=DL;
			r.right-=DR;
			if(!Nactiv){InvalidateRect(hWnd,NULL,0);break;}
			wid=(r.bottom-r.top)/Nactiv;

			for(i=0;i<Nactiv;i++)
			{
			W[i].left=r.left;
			W[i].right=r.right;
			if(mix==0)
			{
			W[i].top=r.top+i*wid;
			W[i].bottom=W[i].top+wid;
			}
			else {W[i].top=r.top;W[i].bottom=r.bottom;}
			}
			
			if(scale==0){GMAX=getglobal_max(Ntrass,counts,Tmin,Tmax);
			GMIN=getglobal_min(Ntrass,counts,Tmin,Tmax);}
			j=0;
			color=1;
			for(i=0;i<Ntrass;i++)
			{
			if(S[i].A)
			{
			
			draw_sei(memdc,S[i],Tmin,Tmax,W[j],counts,scale,GMIN,GMAX,color);
			color+=mix;
			if(color>10)color=1;
			j++;
			}
			
			}
			flag=1;
			InvalidateRect(hWnd,NULL,0);
			break;
			case WM_MOUSEMOVE:
		//GetClientRect(hWnd,&r);
		//if(!flag)break;
		hDC=GetDC(hWnd);
		SetROP2(hDC,R2_NOT);
		//sprintf(tmpstr,"%d %d",curx,cury);
	    //SetWindowText(hWnd,tmpstr);
		//Line(hDC,curx,r.top,curx,r.bottom);
		//Line(hDC,r.left,cury,r.right,cury);
		//Line(hDC,LOWORD(lParam),r.top,LOWORD(lParam),r.bottom);
		//Line(hDC,r.left,HIWORD(lParam),r.right,HIWORD(lParam));
			Line(hDC,curx,r.top,curx,cury-10);
			Line(hDC,curx,r.bottom,curx,cury+10);
			Line(hDC,LOWORD(lParam),r.top,LOWORD(lParam),HIWORD(lParam)-10);
			Line(hDC,LOWORD(lParam),r.bottom,LOWORD(lParam),HIWORD(lParam)+10);
		
		curx=LOWORD(lParam);
		cury=HIWORD(lParam);
		SetROP2(hDC,R2_COPYPEN);
		ReleaseDC(hWnd,hDC);
			break;
			case WM_PLOTMOT:


if(comp_comp()==3)
			{EnableMenuItem(GetMenu(hWnd),ID_AZASOUR,MF_ENABLED);
			//EnableMenuItem(GetMenu(hWnd),ID_2DROTATION,MF_ENABLED);
			}
			else {EnableMenuItem(GetMenu(hWnd),ID_AZASOUR,MF_GRAYED);
			//EnableMenuItem(GetMenu(hWnd),ID_2DROTATION,MF_GRAYED);
			}
			typgra=2;
			GetClientRect(hWnd,&r);
			ClearMemDc(memdc);
			flag=1;
			W[0].left=W[2].left=r.left+DL;
			W[0].right=W[2].right=(r.right+r.left)/2-DR;
			W[1].left=W[3].left=(r.right+r.left)/2+DL;
			W[1].right=W[3].right=r.right-DR;
			W[0].top=W[1].top=r.top+DT;
			W[0].bottom=W[1].bottom=(r.bottom+r.top)/2-DB;
			W[2].top=W[3].top=(r.bottom+r.top)/2+DT;
			W[2].bottom=W[3].bottom=r.bottom-DB;
			draw_motion(memdc,S[MOTX],S[MOTY],Tmin,Tmax,W[0],SStrike,DDip,1,REV1,REV2);
			draw_motion(memdc,S[MOTX],S[MOTZ],Tmin,Tmax,W[1],SStrike,DDip,2,REV1,REV3);
			draw_motion(memdc,S[MOTY],S[MOTZ],Tmin,Tmax,W[2],SStrike,DDip,3,REV2,REV3);
			draw_m_axes(memdc,W[3]);
            InvalidateRect(hWnd,NULL,0);
			break;
			
		case WM_PLOTSPE:
			typgra=1;
			GetClientRect(hWnd,&r);
			ClearMemDc(memdc);
			
			if(!Nactiv){InvalidateRect(hWnd,NULL,0);break;}
			//wid=(r.bottom-r.top)/Nactiv;
			GetTextExtentPoint32(memdc,"E+008",5,&textm);
			r.top+=DT-textm.cy-10;
			r.bottom-=DB-textm.cy-10;
			r.left+=DL-textm.cx-10;
			r.right-=DR-textm.cx-10;
			k=1;
			m=1;
			while(k*m<Nactiv)
			{
			k+=1;
			if(k*m>=Nactiv)break;
			m+=1;
			}

			for(i=0;i<Nactiv;i++)
			{
			if(smix==0)
			{
			W[i].top=r.top+i/k*(r.bottom-r.top)/m;
			W[i].bottom=W[i].top+(r.bottom-r.top)/m;
			W[i].left=r.left+i%k*(r.right-r.left)/k;
			W[i].right=W[i].left+(r.right-r.left)/k;
			W[i].left+=textm.cx+10;
			W[i].right-=textm.cx+10;
			W[i].top+=textm.cy+10;
			W[i].bottom-=textm.cy+10;
			}
			else {
				  W[i].top=r.top+textm.cy+10;
				  W[i].bottom=r.bottom-textm.cy-10;
				  W[i].left=r.left+textm.cx+10;
				  W[i].right=r.right-textm.cx-10;
				  }
			}
			
			j=0;
			color=1;
			for(i=0;i<Ntrass;i++)
			{
			if(S[i].A)
			{if(sscale==0)
			{if(j==0){samin=SP[i].min;
					  samax=SP[i].max;
					  sfmin=SP[i].f1;
					  sfmax=SP[i].f2;}
			if(samin>SP[i].min&&SP[i].min>0.)samin=SP[i].min;
			if(samax<SP[i].max&&SP[i].max>0.)samax=SP[i].max;
			if(sfmin>SP[i].f1&&SP[i].f1>0.)sfmin=SP[i].f1;
			if(sfmax<SP[i].f2&&SP[i].f2>0.)sfmax=SP[i].f2;
			}
			j++;}
			}
			j=0;
			for(i=0;i<Ntrass;i++)
			{

			if(S[i].A)
			{
			draw_spectr(memdc,SP[i],W[j],samin,samax,sfmin,sfmax,color,sscale,polzer);
			color+=smix;
			if(color>10)color=1;
			j++;
			}
			}
			flag=1;
			
			InvalidateRect(hWnd,NULL,0);
			break;
		
		case WM_COMMAND:
			
			switch(LOWORD(wParam))
			{
			case ID_CONTEXT:
				sprintf(tmpstr,"%s\\wgsnplot.hlp",DIRNAME);
				//WinExec(tmpstr,SW_SHOW);
				WinHelp(hWnd,tmpstr,HELP_FINDER,0);
				break;
			case ID_SOURCE:
			get_source(&FIC,&LAMC);
			SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);
			hAccel=LoadAccelerators(hInst,"IDR_MENU4");
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU4"));
			break;
			case ID_STAT:
			DialogBox(hInst,"IDD_DIALOG15",hWnd,GetStatistic);
			break;

			case ID_SOURCE_RETURN:
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU1"));
			hAccel=LoadAccelerators(hInst,"IDR_MENU1");
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;

			case ID_VADATI:
			SendMessage(hWnd,WM_PLOTVAD,(WPARAM)0,(LPARAM)0);
			DialogBox(hInst,"IDD_DIALOG12",hWnd,GetVadat);
			break;
			case ID_SAVE_SPECTR:
			strcpy(szFilter,"Data Files(*.*)|*.*|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =1000;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 1000;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=//(long)NULL
			OFN_ALLOWMULTISELECT |OFN_EXPLORER ;
			if(!GetSaveFileName(&ofn))break;
			if((savexy=fopen(szFile,"w"))==NULL)break;
			if(typgra==1)
			{for(i=0;i<Ntrass;i++)
			{
				if(S[i].A)
				{
				fprintf(savexy,"** %s %s %s %s **\n",S[i].Sta,S[i].Nt,S[i].Cha,cdatostr(Tmin,tmpstr,1));
				fprintf(savexy,"Frequency(Hz) Period(Sec) Spectr\n"); 
				for(j=1;j<SP[i].n;j++)
				fprintf(savexy,"%lf %lf %lf\n",j/SP[i].rate/2./SP[i].n,1./(j/SP[i].rate/2./SP[i].n),SP[i].spdata[j]/16./100000.);
				}
				
			}}
			if(typgra==0)
				{for(i=0;i<Ntrass;i++)
			{
				if(S[i].A)
				{
				fprintf(savexy,"** %s %s %s %s **\n",S[i].Sta,S[i].Nt,S[i].Cha,cdatostr(Tmin,tmpstr,1));
				fprintf(savexy,"Time(Sec) Data\n"); 
				startd=getnum_sei(S[i],Tmin);
				endd=getnum_sei(S[i],Tmax);
				for(j=startd;j<endd;j++)
				if(j>=0&&j<S[i].Samp)
				fprintf(savexy,"%lf %lf\n",(j-startd)*S[i].Rate/1000.,S[i].Data[j]/16./100000.);
				}
				
			}}

			fclose(savexy);
			break;
			case ID_FILE_NEWEARTHQUAKE :
				for(i=0;i<86;i++)PREDAC[i]=0;
			strcpy(szFilter,"Data Files(*.*)|*.*|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =1000;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 1000;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=//(long)NULL
				OFN_ALLOWMULTISELECT |OFN_EXPLORER ;
			if(!GetOpenFileName(&ofn))break;
			for(i=0;i<Ntrass;i++)
			{
			free((void *)S[i].Data);
			free((void *)S[i].R.im);
			free((void *)S[i].R.re);}
			Ntrass=0;
			Nactiv=0;
			Narriv=0;
			Nampli=0;

		    NewN=GetFileNameNumber(szFile);
			for(i=0;i<NewN;i++)
			{
			GetFileNameName(szFile,NewN,tmpstr,i+1);
			if(read_sei(&S[i],tmpstr,1)==1){
			sprintf(tmpstr1,"Read File %s into Memory",tmpstr);
			draw_msg(hWnd,tmpstr1);
				Ntrass++;}

            }
			if(Ntrass==0)break;
			
			for(i=0;i<Ntrass;i++)
			{
			if(i==0)
			{Tmin.day=S[0].Stime.day;
			Tmin.msec=S[0].Stime.msec;
			Tmax=daatadsec(S[0].Stime,S[0].Rate*S[0].Samp);
			}
			else 
			{if(raznd(Tmin,S[i].Stime)>0)
				{
				Tmin.day=S[i].Stime.day;
				Tmin.msec=S[i].Stime.msec;
				}
			if(raznd(Tmax,S[i].Stime)<(S[i].Rate*S[i].Samp))
			Tmax=daatadsec(S[i].Stime,S[i].Rate*S[i].Samp);

			}
			}
			Tming.day=Tminp.day=Tmin.day;Tming.msec=Tminp.msec=Tmin.msec;
			Tmaxg.day=Tmaxp.day=Tmax.day;Tmaxg.msec=Tmaxp.msec=Tmax.msec;
			Nactiv=Ntrass;
			for(i=0;i<Ntrass;i++)S[i].A=1;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			
			case ID_AZASOUR:
			DialogBox(hInst,"IDD_DIALOG14",hWnd,GetSourceAz);
			//SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);
			break;
			case ID_ORIENT:

			for(i=0;i<Ntrass;i++)
			{
				if(S[i].A&&S[i].Cha[2]=='1')k=i;
				if(S[i].A&&S[i].Cha[2]=='2')m=i;
			}
				
			cor_sei(Tming,Tmaxg,&S[k],&S[m]);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_MOTION_PRINT:
            memset(&pd, 0, sizeof(PRINTDLG));
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hWnd;
			pd.Flags = PD_RETURNDC;
			

			if (PrintDlg(&pd) != 0)
			{   
			
				DocInfo.cbSize = sizeof(DOCINFO);
				DocInfo.lpszDocName="WinGsnDoc";
				DocInfo.lpszOutput = (LPSTR) NULL;
				if(StartDoc(pd.hDC, &DocInfo)<=0)
				goto PPP;
				if(StartPage(pd.hDC)<=0)goto PPP;
				R.top=R.left=0;
				R.right=GetDeviceCaps(pd.hDC,HORZRES);
				R.bottom=GetDeviceCaps(pd.hDC,VERTRES);
				memcpy(&lfp,&lf,sizeof(LOGFONT));
				prscr=GetDeviceCaps(pd.hDC,LOGPIXELSX)/GetDeviceCaps(memdc,LOGPIXELSX);
				PRINTPARAM=1;
				lfp.lfHeight=lf.lfHeight*prscr;
				hFontP = CreateFontIndirect(&lfp);
				hOldFontP=SelectObject(pd.hDC,hFontP);
				n_p=CreatePen(PS_SOLID,1,RGB(0,0,0));
				o_p=SelectObject(pd.hDC,n_p);
				prscr+=1;
				
			W[0].left=W[2].left=R.left+DL*prscr;
			W[0].right=W[2].right=(R.right+R.left)/2-DR*prscr;
			W[1].left=W[3].left=(R.right+R.left)/2+DL*prscr;
			W[1].right=W[3].right=R.right-DR*prscr;
			W[0].top=W[1].top=R.top+DT*prscr;
			W[0].bottom=W[1].bottom=(R.bottom+R.top)/2-DB*prscr;
			W[2].top=W[3].top=(R.bottom+R.top)/2+DT*prscr;
			W[2].bottom=W[3].bottom=R.bottom-DB*prscr;
			draw_motion(pd.hDC,S[MOTX],S[MOTY],Tmin,Tmax,W[0],SStrike,DDip,1,REV1,REV2);
			draw_motion(pd.hDC,S[MOTX],S[MOTZ],Tmin,Tmax,W[1],SStrike,DDip,2,REV1,REV3);
			draw_motion(pd.hDC,S[MOTY],S[MOTZ],Tmin,Tmax,W[2],SStrike,DDip,3,REV2,REV3);
			draw_m_axes(pd.hDC,W[3]);

			
				PRINTPARAM=0;
				SelectObject(pd.hDC,hOldFontP);
				DeleteObject(hFontP);
				SelectObject(pd.hDC,o_p);
				DeleteObject(n_p);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
				if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				}   
			else {PPP:MessageBox(hWnd, "Print attempt failed", "", IDOK);}
			
			SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);	
			break;

case ID_SOURCE_PRINT:
            memset(&pd, 0, sizeof(PRINTDLG));
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hWnd;
			pd.Flags = PD_RETURNDC;
			

			if (PrintDlg(&pd) != 0)
			{   
			
				DocInfo.cbSize = sizeof(DOCINFO);
				DocInfo.lpszDocName="WinGsnDoc";
				DocInfo.lpszOutput = (LPSTR) NULL;
				if(StartDoc(pd.hDC, &DocInfo)<=0)
				goto ZZZ;
				if(StartPage(pd.hDC)<=0)goto ZZZ;
				R.top=R.left=0;
				R.right=GetDeviceCaps(pd.hDC,HORZRES);
				R.bottom=GetDeviceCaps(pd.hDC,VERTRES);
				memcpy(&lfp,&lf,sizeof(LOGFONT));
				prscr=GetDeviceCaps(pd.hDC,LOGPIXELSX)/GetDeviceCaps(memdc,LOGPIXELSX);
				PRINTPARAM=1;
				lfp.lfHeight=lf.lfHeight*prscr;
				hFontP = CreateFontIndirect(&lfp);
				hOldFontP=SelectObject(pd.hDC,hFontP);
				n_p=CreatePen(PS_SOLID,1,RGB(0,0,0));
				o_p=SelectObject(pd.hDC,n_p);
				prscr+=1;
				R.top+=DT*prscr;
			R.bottom-=DT*prscr;
			R.left+=DT*prscr;
			R.right-=DT*prscr;
			Rectangle(pd.hDC,R.left,R.top,R.right,R.bottom);
			Draw_map(pd.hDC,MMM/(R.bottom-R.top),FIC,LAMC,R);
			
			
			
				PRINTPARAM=0;
				SelectObject(pd.hDC,hOldFontP);
				DeleteObject(hFontP);
				SelectObject(pd.hDC,o_p);
				DeleteObject(n_p);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
				if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				}   
			else {ZZZ:MessageBox(hWnd, "Print attempt failed", "", IDOK);}
			
			flag=1;
			InvalidateRect(hWnd,NULL,0);
			//SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);	
			break;

			case ID_SPECTR_PRINT:
			memset(&pd, 0, sizeof(PRINTDLG));
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hWnd;
			pd.Flags = PD_RETURNDC;
			//pd.Flags=PD_RETURNDEFAULT;
			//pd.hDevMode=NULL;
		//	pd.hDevNames=NULL;
		//	pd.hInstance=NULL;


			if (PrintDlg(&pd) != 0)
			{   
				//GlobalLock(pd.hDevMode);
				//GlobalLock(pd.hDevNames);
				//if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				//if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				
				DocInfo.cbSize = sizeof(DOCINFO);
				DocInfo.lpszDocName="WinGsnDoc";
				DocInfo.lpszOutput = (LPSTR) NULL;
				if(StartDoc(pd.hDC, &DocInfo)<=0)
				goto OOO;
				if(StartPage(pd.hDC)<=0)goto OOO;
				R.top=R.left=0;
				R.right=GetDeviceCaps(pd.hDC,HORZRES);
				R.bottom=GetDeviceCaps(pd.hDC,VERTRES);
				memcpy(&lfp,&lf,sizeof(LOGFONT));
				prscr=GetDeviceCaps(pd.hDC,LOGPIXELSX)/GetDeviceCaps(memdc,LOGPIXELSX);
				PRINTPARAM=1;
				lfp.lfHeight=lf.lfHeight*prscr;
				hFontP = CreateFontIndirect(&lfp);
				hOldFontP=SelectObject(pd.hDC,hFontP);
				n_p=CreatePen(PS_SOLID,1,RGB(0,0,0));
				o_p=SelectObject(pd.hDC,n_p);
				prscr+=1;
				GetTextExtentPoint32(pd.hDC,"E+008",5,&textm);
				R.top+=DT*prscr-textm.cy-10;
				R.bottom-=DB*prscr-textm.cy-10;
				R.left+=DL*prscr-textm.cx-10;
				R.right-=DR*prscr-textm.cx-10;
			k=1;
			m=1;
			while(k*m<Nactiv)
			{
			k+=1;
			if(k*m>=Nactiv)break;
			m+=1;
			}

			for(i=0;i<Nactiv;i++)
			{
			if(smix==0)
			{
			W[i].top=R.top+i/k*(R.bottom-R.top)/m;
			W[i].bottom=W[i].top+(R.bottom-R.top)/m;
			W[i].left=r.left+i%k*(R.right-R.left)/k;
			W[i].right=W[i].left+(R.right-R.left)/k;
			W[i].left+=textm.cx+10;
			W[i].right-=textm.cx+10;
			W[i].top+=textm.cy+10;
			W[i].bottom-=textm.cy+10;
			}
			else {
				  W[i].top=R.top+textm.cy+10;
				  W[i].bottom=R.bottom-textm.cy-10;
				  W[i].left=R.left+textm.cx+10;
				  W[i].right=R.right-textm.cx-10;
			     }
			}
			
			j=0;
			color=0;
			for(i=0;i<Ntrass;i++)
			{
			if(S[i].A)
			{if(sscale==0)
			{if(j==0){samin=SP[i].min;
					  samax=SP[i].max;
					  sfmin=SP[i].f1;
					  sfmax=SP[i].f2;}
			if(samin>SP[i].min&&SP[i].min>0.)samin=SP[i].min;
			if(samax<SP[i].max&&SP[i].max>0.)samax=SP[i].max;
			if(sfmin>SP[i].f1&&SP[i].f1>0.)sfmin=SP[i].f1;
			if(sfmax<SP[i].f2&&SP[i].f2>0.)sfmax=SP[i].f2;
			}
			j++;}
			}
			j=0;
			for(i=0;i<Ntrass;i++)
			{

			if(S[i].A)
			{
			draw_spectr(pd.hDC,SP[i],W[j],samin,samax,sfmin,sfmax,color,sscale,polzer);
			j++;
			}
			}


				PRINTPARAM=0;
				SelectObject(pd.hDC,hOldFontP);
				DeleteObject(hFontP);
				SelectObject(pd.hDC,o_p);
				DeleteObject(n_p);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
				//GlobalUnlock(pd.hDevMode);
				//GlobalUnlock(pd.hDevNames);

				if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				}   
			else {OOO:MessageBox(hWnd, "Print attempt failed", "", IDOK);}
			
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);	
			break;
			case ID_PICKTIME:
			picktime=1;
			break;
			case ID_PICKAMP:
			pickamp=1;
			break;
			case ID_LISTAMP:
			DialogBox(hInst,"IDD_DIALOG8",hWnd,LookAmplitude);
						SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_PREDICT:
			DialogBox(hInst,"IDD_DIALOG11",hWnd,GetPredictTimes);
			break;
			case ID_2DROTATION:
			DialogBox(hInst,"IDD_DIALOG13",hWnd,GetRotation);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_MOTION:
            if(!DialogBox(hInst,"IDD_DIALOG10",hWnd,GetMotionComponents))break;
			polariz(S[MOTX],S[MOTY],S[MOTZ],Tmin,Tmax,REV1,REV2,REV3);
			SStrike=POLAR[0].STRIKE;
			DDip=POLAR[0].DIP;
			hAccel=LoadAccelerators(hInst,"IDR_MENU3");
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU3"));
			SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);
			
			break;
			//polar=fopen("polar","w");
			//for(i=0;i<Narriv;i+=2)
			//{
			//fprintf(polar,"%lf ",raznd(ST[i+1].Tarrive,ST[i].Tarrive));
			//tp1.day=ST[i+1].Tarrive.day;
			//tp1.msec=ST[i+1].Tarrive.msec;
			//tp2=daatadsec(tp1,400);
			//polariz(S[0],S[1],S[2],tp1,tp2,polar,raznd(ST[i+1].Tarrive,ST[i].Tarrive));
			//}
			//fclose(polar);
			//break;

			case ID_LISTTIME:
			DialogBox(hInst,"IDD_DIALOG9",hWnd,LookArrival);
						SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_FILE_PRINT :
			memset(&pd, 0, sizeof(PRINTDLG));
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hWnd;
			pd.Flags = PD_RETURNDC;
			//pd.Flags=PD_RETURNDEFAULT;
			//pd.hDevMode=NULL;
		//	pd.hDevNames=NULL;
		//	pd.hInstance=NULL;


			if (PrintDlg(&pd) != 0)
			{   
				//GlobalLock(pd.hDevMode);
				//GlobalLock(pd.hDevNames);
				//if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				//if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				
				DocInfo.cbSize = sizeof(DOCINFO);
				DocInfo.lpszDocName="WinGsnDoc";
				DocInfo.lpszOutput = (LPSTR) NULL;
				if(StartDoc(pd.hDC, &DocInfo)<=0)
				goto OOO;
				if(StartPage(pd.hDC)<=0)goto AAA;
				R.top=R.left=0;
				R.right=GetDeviceCaps(pd.hDC,HORZRES);
				R.bottom=GetDeviceCaps(pd.hDC,VERTRES);
				memcpy(&lfp,&lf,sizeof(LOGFONT));
				prscr=GetDeviceCaps(pd.hDC,LOGPIXELSX)/GetDeviceCaps(memdc,LOGPIXELSX);
				PRINTPARAM=1;
				lfp.lfHeight=lf.lfHeight*prscr;
				hFontP = CreateFontIndirect(&lfp);
				hOldFontP=SelectObject(pd.hDC,hFontP);
				n_p=CreatePen(PS_SOLID,1,RGB(0,0,0));
				o_p=SelectObject(pd.hDC,n_p);
				prscr+=1;
				R.top+=DT*prscr;
				R.bottom-=DB*prscr;
				R.left+=DL*prscr;
				R.right-=DR*prscr;
				if(!Nactiv)break;
				wid=(R.bottom-R.top)/Nactiv;
				for(i=0;i<Nactiv;i++)
			{
			W[i].left=R.left;
			W[i].right=R.right;
			if(mix==0)
			{
			W[i].top=R.top+i*wid;
			W[i].bottom=W[i].top+wid;
			}
			else 
			{
			W[i].top=R.top;
			W[i].bottom=R.bottom;
			}
			}
			
			if(scale==0)
			{GMAX=getglobal_max(Ntrass,counts,Tmin,Tmax);
			GMIN=getglobal_min(Ntrass,counts,Tmin,Tmax);}
			j=0;
			color=0;
			for(i=0;i<Ntrass;i++)
			{
			if(S[i].A)
			{
			draw_sei(pd.hDC,S[i],Tmin,Tmax,W[j],counts,scale,GMIN,GMAX,color);
			j++;
			}
			}
				PRINTPARAM=0;
				SelectObject(pd.hDC,hOldFontP);
				DeleteObject(hFontP);
				SelectObject(pd.hDC,o_p);
				DeleteObject(n_p);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
				//GlobalUnlock(pd.hDevMode);
				//GlobalUnlock(pd.hDevNames);

				if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				}   
			else {AAA: MessageBox(hWnd, "Print attempt failed", "", IDOK);}
			
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);	
			break;
			case ID_VIEW_SORT_ZNE:
			qsort((void *)S,Ntrass,sizeof(Seismogr),sort_sta_zne);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);	
			break;
			case ID_VIEW_SORT_ZEN:
			qsort((void *)S,Ntrass,sizeof(Seismogr),sort_sta_zne);
			qsort((void *)S,Ntrass,sizeof(Seismogr),sort_sta_zen);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);	
			break;

			case ID_VIEW_SORT_ENZ:
			qsort((void *)S,Ntrass,sizeof(Seismogr),sort_sta_enz);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);	
			break;
			case ID_VIEW_SORT_DIST:
			qsort((void *)S,Ntrass,sizeof(Seismogr),sort_dist);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);	
			break;
			case ID_FILE_DELETETRACE:
			DialogBox(hInst,"IDD_DIALOG3",hWnd,GetDeleteComponents);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_VIEW_SELECTCOMPONENTS:
			DialogBox(hInst,"IDD_DIALOG2",hWnd,GetComponents);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_FILTERS_LPSRO:
			filtertype=1;
			goto FILTER;
			case ID_FILTERS_KIRNOS:
			filtertype=2;
			goto FILTER;
			case ID_FILTERS_WWSSLP:
			filtertype=3;
			goto FILTER;
			case ID_FILTERS_WWSSSP:
			filtertype=4;
			goto FILTER;
			case ID_FILTERS_WA:
			filtertype=5;
			FILTER:
			draw_msg(hWnd,"Emulating Standart Seismometer, Please Wait!");
			for(i=0;i<Ntrass;i++)
			{
			
			if(S[i].A)
			{
            dup_sei(&S[Ntrass],S[i],i);
			S[Ntrass].A=0;
			Ntrass+=1;
		//	normf=normalf(S[i],10.);
			//taper(S[i].Data,S[i].Samp,10);
			filter_smgr(&S[i],filtertype);
			}
			}
			scale=0;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;

			case ID_FILTERS_USERDEFINED:
			
			if(DialogBox(hInst,"IDD_DIALOG4",hWnd,GetUserFilter))
			{
			 for(i=0;i<Ntrass;i++)
			 if(S[i].A){
			 a=(double *)malloc((FORDER+1)*5*sizeof(double));
			 if(!a){goto MMM;}
			 b=(double *)malloc((FORDER+1)*5*sizeof(double));
			 if(!b){free((void*)a);goto MMM;}
			 px=(double *)malloc((FORDER+1)*5*sizeof(double));
			 if(!px){free((void*)a);free((void*)b);goto MMM;}
			 py=(double *)malloc((FORDER+1)*5*sizeof(double));
			 if(!py){free((void*)a);free((void*)b);free((void*)px);goto MMM;}
			 error=filter_disign(FIBAND,S[i].Rate,FFLOW,FFHIGH,FORDER,a,b);
			 if(error!=0){free((void*)a);free((void*)b);free((void*)px);free((void*)py);goto MMM;}
			 draw_msg(hWnd,"Calculating User Defined Filter, Please Wait!");
			 dup_sei(&S[Ntrass],S[i],i);
			 S[Ntrass].A=0;
			 Ntrass+=1;
			 stll=strlen(S[i].Comment);	
			 if(FIBAND<3)
			 sprintf(S[i].Comment+stll," [%s=%ghz^%d]",Ftype[FIBAND-1],FFLOW,FORDER);
			 else sprintf(S[i].Comment+stll," [%s=%g:%ghz^%d]",Ftype[FIBAND-1],FFLOW,FFHIGH,FORDER);
             if(FIBAND>2)
			 spfilt(b,&a[1],FORDER*2,FORDER*2,&S[i],px,py);
			 else spfilt(b,&a[1],FORDER,FORDER,&S[i],px,py);
			 free((void*)a);free((void*)b);free((void*)px);free((void*)py);
			 }
			
			}

MMM:	SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_VIEW_MIX:
			if(mix==0){CheckMenuItem(GetMenu(hWnd),ID_VIEW_MIX,MF_CHECKED);mix=1;}
			else {CheckMenuItem(GetMenu(hWnd),ID_VIEW_MIX,MF_UNCHECKED);mix=0;}
			if(scale==1)scale=0;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
				break;
			
			case ID_VIEW_SELECTTYPEOFVIEW_SCLALEMAXLOCAL:
			scale=1;
			SCFACTOR=1.;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			
			break;

			
			case ID_FILE_ADDTRACE:
				strcpy(szFilter,"Data Files(*.*)|*.*|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =1000;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 1000;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=0;
			if(!GetOpenFileName(&ofn))break;
			if(read_sei(&S[Ntrass],szFile,1)!=1)break;
			if(raznd(Tming,S[Ntrass].Stime)>0)
				{
				Tming.day=S[Ntrass].Stime.day;
				Tming.msec=S[Ntrass].Stime.msec;
				}
			if(raznd(Tmaxg,S[Ntrass].Stime)<(S[Ntrass].Rate*S[Ntrass].Samp))
			Tmaxg=daatadsec(S[Ntrass].Stime,S[Ntrass].Rate*S[Ntrass].Samp);
			S[Ntrass].A=1;
			Ntrass+=1;
			Nactiv+=1;
			Tmin.day=Tming.day;
			Tmin.msec=Tming.msec;
			Tmax.day=Tmaxg.day;
			Tmax.msec=Tmaxg.msec;

			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);	
			break;
			
			case ID_VIEW_SELECTTYPEOFVIEW_ZOOMIN:
				Tminp.day=Tmin.day;
				Tminp.msec=Tmin.msec;
				Tmaxp.day=Tmax.day;
				Tmaxp.msec=Tmax.msec;
				zoom=1;
				break;
			case ID_SCROL_L:
				rzdt=raznd(Tmax,Tmin);
				Tminp.day=Tmin.day;
				Tminp.msec=Tmin.msec;
				Tmaxp.day=Tmax.day;
				Tmaxp.msec=Tmax.msec;
				Tmin=daatadsec(Tmin,-rzdt);
				Tmax=daatadsec(Tmax,-rzdt);
				SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
				break;
			case ID_SCROL_R:
				rzdt=raznd(Tmax,Tmin);
				Tminp.day=Tmin.day;
				Tminp.msec=Tmin.msec;
				Tmaxp.day=Tmax.day;
				Tmaxp.msec=Tmax.msec;
				Tmin=daatadsec(Tmin,rzdt);
				Tmax=daatadsec(Tmax,rzdt);
				
				SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
				break;


			case ID_VIEW_SELECTTYPEOFVIEW_ZOOMOUT:
			
			Tmin.day=Tminp.day;
			Tmin.msec=Tminp.msec;
			Tmax.day=Tmaxp.day;
			Tmax.msec=Tmaxp.msec;

			Tminp.day=Tming.day;
			Tminp.msec=Tming.msec;
			Tmaxp.day=Tmaxg.day;
			Tmaxp.msec=Tmaxg.msec;
			
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_VIEW_SELECTFONT:
			SelectObject(memdc,hOldFont);
			DeleteObject(hFont);
			memset(&cf, 0, sizeof(CHOOSEFONT));
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = hWnd;
			cf.lpLogFont = &lf;
			cf.Flags = CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT|CF_EFFECTS;
			cf.nFontType = SCREEN_FONTTYPE;
			ChooseFont(&cf);
			SetTextColor(memdc,cf.rgbColors);

			hFont = CreateFontIndirect(&lf);
			hOldFont=SelectObject(memdc,hFont);
			if(typgra==0)
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			if(typgra==1)
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			if(typgra==2)
			SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);
			if(typgra==3)
			SendMessage(hWnd,WM_PLOTVAD,(WPARAM)0,(LPARAM)0);
			if(typgra==5)
			SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);
			break;
			case ID_VIEW_SELECTTYPEOFVIEW_SCALEMAXGLOBAL:
			scale=0;
			SCFACTOR=1.;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SCALE1_2:
			SCFACTOR*=2.;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SCALE2_1:
			SCFACTOR/=2.;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_VIEW_SELECTTYPEOFVIEW_SCALEUSER:
			SCFACTOR=1.;
			scale=2;

			DialogBox(hInst,"IDD_DIALOG1",hWnd,GetAxes);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SVIEW_SCALE_USERDEFINED:
			sscale=2;
			DialogBox(hInst,"IDD_DIALOG5",hWnd,GetSpectrAxes);
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			break;

			case ID_SPECTR:
				polzer=0;
				draw_msg(hWnd,"Computing FFT,Please Wait!");
			for(i=0;i<Ntrass;i++)
			{
			if(S[i].A)
			Spectr_smgr(S[i],&SP[i],Tmin,Tmax,0);
			}
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU2"));
			hAccel=LoadAccelerators(hInst,"IDR_MENU2");
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			break;
			case ID_POLZERTEST:
			draw_msg(hWnd,"Computing Response Function,Please Wait!");
				polzer=1;

			for(i=0;i<Ntrass;i++)
			{
			if(S[i].A)
			Spectr_smgr(S[i],&SP[i],Tmin,Tmax,1);
			}
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU2"));
			hAccel=LoadAccelerators(hInst,"IDR_MENU2");
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			break;

			case ID_SVIEW_PLOTONONE:
			if(smix==0){CheckMenuItem(GetMenu(hWnd),ID_SVIEW_PLOTONONE,MF_CHECKED);smix=1;}
			else {CheckMenuItem(GetMenu(hWnd),ID_SVIEW_PLOTONONE,MF_UNCHECKED);smix=0;}
			if(scale==1)scale=0;
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SVIEW_SCALE_MAXLOCAL:
				sscale=1;
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SVIEW_SCALE_MAXGLOBAL:
				sscale=0;
			SendMessage(hWnd,WM_PLOTSPE,(WPARAM)0,(LPARAM)0);
			break;
			case ID_MOTION_RETURN:
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU1"));
			hAccel=LoadAccelerators(hInst,"IDR_MENU1");
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SZIN:
				MMM/=2.;
			SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SZOUT:
				MMM*=2.;
			SendMessage(hWnd,WM_PLOTMAP,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SPOZ:
				sourpar=1;
				break;
			case ID_SPECTR_RETURN:
			for(i=0;i<Ntrass;i++)if(S[i].A)free((void*)SP[i].spdata);
			SetMenu(hWnd,LoadMenu(hInst,"IDR_MENU1"));
			hAccel=LoadAccelerators(hInst,"IDR_MENU1");
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case ID_SPECTR_EXIT:
				for(i=0;i<Ntrass;i++)free((void*)SP[i].spdata);
				case ID_FILE_EXIT :
				case ID_SOURCE_EXIT:
				case ID_MOTION_EXIT:
			    for(i=0;i<Ntrass;i++)free((void*)S[i].Data);
			PostQuitMessage(0);
			break;
			default:
			break;
			}
			break;
			
			
			case WM_PAINT:
				if(flag==0){InvalidateRect(hWnd,NULL,0);flag=1;}					
				 hDC=BeginPaint(hWnd,&ps);
				 BitBlt(hDC,0,0,maxX,maxY,memdc,0,0,SRCCOPY);
				 ReleaseDC(hWnd,hDC);
				 GetCurrentPositionEx(hDC,&P);
				 SetROP2(hDC,R2_NOT);
				 curx=P.x;
				 cury=P.y;
				 //Line(hDC,curx,r.top,curx,r.bottom);
				 //Line(hDC,r.left,cury,r.right,cury);
				 Line(hDC,curx,r.top,curx,cury-10);
				 Line(hDC,curx,r.bottom,curx,cury+10);
				 SetROP2(hDC,R2_COPYPEN);
				 EndPaint(hWnd,&ps);
				 flag=0;
				 break;

			case WM_DESTROY:
			SelectObject(memdc,hOldFont);
			DeleteObject(hFont);
			DeleteDC(memdc);
			PostQuitMessage(0);
			break;
		default:
    	return DefWindowProc (hWnd, message, wParam, lParam) ;
		}

}
	





/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
LRESULT APIENTRY GetAxes(
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
		sprintf(tmpstr,"%g",GMIN);
		SetDlgItemText( hDlg, IDC_MIN,tmpstr);
		sprintf(tmpstr,"%g",GMAX);
		SetDlgItemText( hDlg, IDC_MAX,tmpstr);

       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
			GetDlgItemText(hDlg,IDC_MIN,tmpstr,80);
			if(sscanf(tmpstr,"%lf",&GMIN)!=1)break;
			GetDlgItemText(hDlg,IDC_MAX,tmpstr,80);
			if(sscanf(tmpstr,"%lf",&GMAX)!=1)break;
			if(GMAX<=GMIN)break;
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


/************************************************************************/
LRESULT APIENTRY GetComponents(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[80];
int i;


	
    switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		for(i=0;i<Ntrass;i++)
	{
	sprintf(tmpstr,"%s %s %s %s %c",S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,S[i].Type);
	SendDlgItemMessage(hDlg,IDC_LIST1,LB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	}
		
		for(i=0;i<Ntrass;i++)
		{if(S[i].A)
			SendDlgItemMessage(hDlg,IDC_LIST1,LB_SETSEL,1,i);
else SendDlgItemMessage(hDlg,IDC_LIST1,LB_SETSEL,0,i);
		}
		

       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
				Nactiv=0;
	for(i=0;i<Ntrass;i++)
	{if(SendDlgItemMessage(hDlg,IDC_LIST1,LB_GETSEL,i,(LPARAM)0))
	{S[i].A=1;Nactiv+=1;}
	 else S[i].A=0;
	}
			
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



LRESULT APIENTRY GetDeleteComponents(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[80];
int i;int n;


	
    switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		for(i=0;i<Ntrass;i++)
	{
	sprintf(tmpstr,"%s %s %s %s %c",S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,S[i].Type);
	SendDlgItemMessage(hDlg,IDC_LIST1,LB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	}
		
		

       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case ID_DELETE:          
		n=SendDlgItemMessage(hDlg,IDC_LIST1,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		if(n!=LB_ERR)
		{free((void*)S[n].Data);free((void*)S[n].R.im);free((void*)S[n].R.re);
		if(S[n].A==1)Nactiv-=1;
		for(i=n;i<Ntrass-1;i++)
		{
		S[i]=S[i+1];
		}
		Ntrass-=1;
		SendDlgItemMessage(hDlg,IDC_LIST1,LB_DELETESTRING,(WPARAM)n,(LPARAM)0);
		if(Ntrass<0)Ntrass=0;
		if(Nactiv<0)Ntrass=0;
		}
		break;
			case IDOK:
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



LRESULT APIENTRY GetUserFilter(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

   char tmpstr[80];



	
    switch (message) 
{
case WM_INITDIALOG:
//FIBAND=3;
sprintf(tmpstr,"%d",FORDER);
SetDlgItemText(hDlg,IDC_EDIT1,tmpstr);
sprintf(tmpstr,"%g",FFLOW);
SetDlgItemText(hDlg,IDC_EDIT4,tmpstr);				
sprintf(tmpstr,"%g",FFHIGH);
SetDlgItemText(hDlg,IDC_EDIT3,tmpstr);				
if(FIBAND==4)SendDlgItemMessage(hDlg,IDC_BANDSTOP,BM_SETCHECK,(WPARAM)1,(LPARAM)0);
if(FIBAND==3)SendDlgItemMessage(hDlg,IDC_BANDPASS,BM_SETCHECK,(WPARAM)1,(LPARAM)0);
if(FIBAND==2)SendDlgItemMessage(hDlg,IDC_HIPASS,BM_SETCHECK,(WPARAM)1,(LPARAM)0);
if(FIBAND==1)SendDlgItemMessage(hDlg,IDC_LOWPASS,BM_SETCHECK,(WPARAM)1,(LPARAM)0);
if(FIBAND==3||FIBAND==4)
EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),TRUE);
else
EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),FALSE);
return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
		    case IDC_BANDPASS:
			EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),TRUE);
			FIBAND=3;
			break;
		    case IDC_BANDSTOP:
			EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),TRUE);
			FIBAND=4;
			break;

			case IDC_HIPASS:
            EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),FALSE);
			FIBAND=2;
			break;

			case IDC_LOWPASS:
EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),FALSE);
			FIBAND=1;
			break;


            case IDOK:          
GetDlgItemText(hDlg,IDC_EDIT1,tmpstr,80);
if(sscanf(tmpstr,"%d",&FORDER)!=1)break;
GetDlgItemText(hDlg,IDC_EDIT4,tmpstr,80);				
if(sscanf(tmpstr,"%lf",&FFLOW)!=1)break;
GetDlgItemText(hDlg,IDC_EDIT3,tmpstr,80);				
if(sscanf(tmpstr,"%lf",&FFHIGH)!=1)break;

//if(SendDlgItemMessage(hDlg,IDC_BANDPASS,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)FIBAND=3;
//else if(SendDlgItemMessage(hDlg,IDC_LOWPASS,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)FIBAND=1;
//else if(SendDlgItemMessage(hDlg,IDC_HIPASS,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==1)FIBAND=2;
//else FIBAND=4;

		
		
		EndDialog(hDlg, TRUE);
			break;
			case IDCANCEL:
        EndDialog(hDlg, FALSE);
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


LRESULT APIENTRY GetSpectrAxes(
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
		sprintf(tmpstr,"%g",samin);
		SetDlgItemText( hDlg, IDC_EDIT1,tmpstr);
		sprintf(tmpstr,"%g",samax);
		SetDlgItemText( hDlg, IDC_EDIT2,tmpstr);
		sprintf(tmpstr,"%g",sfmin);
		SetDlgItemText( hDlg, IDC_EDIT3,tmpstr);
		sprintf(tmpstr,"%g",sfmax);
		SetDlgItemText( hDlg, IDC_EDIT4,tmpstr);

       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
			GetDlgItemText(hDlg,IDC_EDIT1,tmpstr,80);
			if(sscanf(tmpstr,"%lf",&samin)!=1)break;
			GetDlgItemText(hDlg,IDC_EDIT2,tmpstr,80);
			if(sscanf(tmpstr,"%lf",&samax)!=1)break;
			if(samin>=samax)break;
			GetDlgItemText(hDlg,IDC_EDIT3,tmpstr,80);
			if(sscanf(tmpstr,"%lf",&sfmin)!=1)break;
			GetDlgItemText(hDlg,IDC_EDIT4,tmpstr,80);
			if(sscanf(tmpstr,"%lf",&sfmax)!=1)break;
			if(sfmin>=sfmax)break;
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


LRESULT APIENTRY FillTimeArrival(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

int nwt=86;
int i;
char strbuf1[80];
char strbuf2[80];
char strbuf3[80];
char strbuf4[80];
		static char *WaveType[]=
				 {
"P"
,"S"
,"Pg"
,"PgPg"
,"Sg"
,"SgSg"
,"PcP"
,"PcS"
,"ScP"
,"ScS"
,"PKiKP"
,"SKiKP"
,"PKKPdf"
,"PKKSdf"
,"SKKPdf"
,"SKKSdf"
,"P'P'df"
,"S'S'df"
,"Pn"
,"Pb"
,"PbPb"
,"PnPn"
,"Sn"
,"Sb"
,"SbSb"
,"SnSn"
,"P'P'ab"
,"SPg"
,"PgS"
,"PP"
,"SS"
,"P'P'bc"
,"SPn"
,"PnS"
,"SKSac"
,"SKKSac"
,"S'S'ac"
,"PKKPbc"
,"SKKPbc"
,"PKKSbc"
,"PS"
,"SP"
,"Pdiff"
,"Sdiff"
,"SKSdf"
,"PKKPab"
,"PKSdf"
,"SKPdf"
,"PKPdf"
,"SKPbc"
,"PKSbc"
,"SKPab"
,"PKSab"
,"PKKSab"
,"SKKPab"
,"PKPbc"
,"PKPab"
,"pPKiKP"
,"sPKiKP"
,"pPn"
,"sPn"
,"pPb"
,"sPb"
,"sPg"
,"sSn"
,"sSb"
,"pPg"
,"sSg"
,"pP"
,"sP"
,"sS"
,"pS"
,"pSKSac"
,"sSKSac"
,"pPdiff"
,"sPdiff"
,"pSdiff"
,"sSdiff"
,"pSKSdf"
,"sSKSdf"
,"pPKPdf"
,"sPKPdf"
,"pPKPbc"
,"pPKPab"
,"sPKPbc"
,"sPKPab"};


	char *WaveSign[]={"(+)","(-)","()"};
		char *WaveQuality[]={"(I)","(E)","()"};
	
	
    switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		   SendDlgItemMessage(hDlg,IDC_WAVETYPE,CB_LIMITTEXT,(WPARAM)7,(LPARAM)0);
		   SendDlgItemMessage(hDlg,IDC_WAVEQ,CB_LIMITTEXT,(WPARAM)4,(LPARAM)0);
		   SendDlgItemMessage(hDlg,IDC_WAVESIGN,CB_LIMITTEXT,(WPARAM)8,(LPARAM)0);
		   SendDlgItemMessage(hDlg,IDC_COMMENT,EM_LIMITTEXT,(WPARAM)80,(LPARAM)0);
		   SetDlgItemText( hDlg,IDC_COMMENT,S[cindex].Comment);
		   for(i=0;i<nwt;i++)
		   SendDlgItemMessage(hDlg,IDC_WAVETYPE,CB_ADDSTRING,(WPARAM)0,(LPARAM)WaveType[i]);
		   for(i=0;i<3;i++)
		   {
			SendDlgItemMessage(hDlg,IDC_WAVESIGN,CB_ADDSTRING,(WPARAM)0,(LPARAM)WaveSign[i]);
			SendDlgItemMessage(hDlg,IDC_WAVEQ,CB_ADDSTRING,(WPARAM)0,(LPARAM)WaveQuality[i]);
		   }
			sprintf(strbuf1,"%s %s %s %s",cdatostr(Current,strbuf2,1),S[cindex].Sta,
			S[cindex].Nt,S[cindex].Cha);
			SetDlgItemText( hDlg, IDC_CTIME,strbuf1);
			SendDlgItemMessage(hDlg,IDC_WAVETYPE,CB_SETCURSEL,(WPARAM)0,(LPARAM)0);		  	
			SendDlgItemMessage(hDlg,IDC_WAVEQ,CB_SETCURSEL,(WPARAM)1,(LPARAM)0);		  	
			SendDlgItemMessage(hDlg,IDC_WAVESIGN,CB_SETCURSEL,(WPARAM)2,(LPARAM)0);		  	
       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDC_MARK:      
			if(!GetDlgItemText( hDlg,IDC_WAVETYPE,strbuf1,7))
			{
			MessageBox(hDlg,"No Wave Type Selected","Error Message",MB_ICONINFORMATION|MB_OK);
			break;
			}
			if(!GetDlgItemText( hDlg,IDC_WAVEQ,strbuf2,5))
			{
			MessageBox(hDlg,"No Wave Qality Selected","Error Message",MB_ICONINFORMATION|MB_OK);
			break;
			}
			if(!GetDlgItemText( hDlg,IDC_WAVESIGN,strbuf3,9))
			{
			MessageBox(hDlg,"No Wave Sign Selected","Error Message",MB_ICONINFORMATION|MB_OK);
			break;
			}
			GetDlgItemText( hDlg,IDC_COMMENT,strbuf4,80);
			fill_time(S[cindex].Sta,S[cindex].Nt,S[cindex].Cha,Current,strbuf1,strbuf2,strbuf3,strbuf4);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			EndDialog(hDlg,TRUE);	
			case IDC_SKIP:
			EndDialog(hDlg,FALSE);
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

LRESULT APIENTRY FillAmplitude(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

int nwt=8;
int i,k;
char *units[]={"M","M/S","M/S*S","UKNOWN"};
char strbuf1[80];
char strbuf2[80];


		static char *WaveType[]=
{
"MPV","MPH","MSH","MSV","MLRV","MLRH","MLQ","MWA"
};
		
	
	
    switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
SendDlgItemMessage(hDlg,IDC_WAVETYPE,CB_LIMITTEXT,(WPARAM)7,(LPARAM)0);
SendDlgItemMessage(hDlg,IDC_COMMENT,EM_LIMITTEXT,(WPARAM)80,(LPARAM)0);
SetDlgItemText( hDlg,IDC_COMMENT,S[cindex].Comment);

		   if(S[cindex].Type=='D')k=0;
			else if(S[cindex].Type=='V')k=1;
				else if(S[cindex].Type=='A')k=2;
					else k=3;
		   
		   for(i=0;i<nwt;i++)
		   SendDlgItemMessage(hDlg,IDC_WAVETYPE,CB_ADDSTRING,(WPARAM)0,(LPARAM)WaveType[i]);
		   sprintf(strbuf1,"%s %s %s %s %g %s",cdatostr(Current,strbuf2,1),S[cindex].Sta,
		   S[cindex].Nt,S[cindex].Cha,S[cindex].Data[getnum_sei(S[cindex],Current)],units[k]);
		   SetDlgItemText( hDlg, IDC_CAMPL,strbuf1);
SendDlgItemMessage(hDlg,IDC_WAVETYPE,CB_SETCURSEL,(WPARAM)0,(LPARAM)0);		  

       return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDC_MARK:
			if(!GetDlgItemText( hDlg,IDC_WAVETYPE,strbuf1,7))
			{
			MessageBox(hDlg,"No Wave Type Selected","Error Message",MB_ICONINFORMATION|MB_OK);
			break;
			}
			GetDlgItemText( hDlg,IDC_COMMENT,strbuf2,80);
			fill_amp(S[cindex].Sta,S[cindex].Nt,S[cindex].Cha,cindex,Current,prevt,S[cindex].Type,strbuf2,strbuf1);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			EndDialog(hDlg,TRUE);	

			break;
			case IDC_SKIP:
			EndDialog(hDlg,FALSE);
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


LRESULT APIENTRY LookAmplitude(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
char strbuf1[200];
char strbuf2[80];
char strbuf3[80];
char strbuf4[80];
int i,lbindex;
static char szFile[256];
OPENFILENAME ofn;
char szFileTitle[256],szFilter[256];
FILE *amp;

switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		   for(i=0;i<Nampli;i++)
		   {sprintf(strbuf3,"%g",SA[i].amplit);
			sprintf(strbuf4,"%g",SA[i].period);
			sprintf(strbuf1,"%5s %2s %3s %6s %18s %20s %20s %c %s",
			SA[i].Sta,SA[i].Nt,SA[i].Cha,SA[i].Wtype,
			cdatostr(SA[i].Tampli,strbuf2,0),strbuf3,strbuf4,
			SA[i].Type,SA[i].Comment);
		   SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_ADDSTRING,(WPARAM)0,(LPARAM)strbuf1);
		   }

		      return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDC_DELETE:
			lbindex=SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);	
			if(lbindex!=LB_ERR)
			SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_DELETESTRING,(WPARAM)lbindex,(LPARAM)0);	
			
			break;
			case IDC_LOADLIST:
			strcpy(szFilter,"Arrivals(*.amp)|*.amp|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =256;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 256;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=0;
			if(!GetSaveFileName(&ofn))break;
			if((amp=fopen(szFile,"r"))==NULL)break;
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_DELETESTRING,(WPARAM)0,(LPARAM)0)!=LB_ERR);	
			while(fgets(strbuf1,200,amp))
			{
			strbuf1[strlen(strbuf1)-1]=0;
			SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_ADDSTRING,(WPARAM)0,(LPARAM)strbuf1);
			i++;
			}

			Nampli=i;
			fclose(amp);
			break;

			break;
			case IDC_SAVELIST:
			strcpy(szFilter,"Arival Files(*.amp)|*.amp|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =256;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 256;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=0;
			if(!GetSaveFileName(&ofn))break;
			if((amp=fopen(szFile,"w"))==NULL)break;
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_GETTEXT,(short)i,(LPARAM)strbuf1)>0)
			{
			fprintf(amp,"%s\n",strbuf1);			
			i++;
			}
			fclose(amp);

			break;
			case IDOK:
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LISTAMP,LB_GETTEXT,(WPARAM)i,(LPARAM)strbuf1)>0)
			{
			sscanf(strbuf1,"%s %s %s %s %s %lf %lf %s",
			SA[i].Sta,SA[i].Nt,SA[i].Cha,SA[i].Wtype,
			strbuf2,&SA[i].amplit,&SA[i].period,strbuf3);
			SA[i].Type=strbuf3[0];
		    strcpy(SA[i].Comment,&strbuf1[83]);
			SA[i].Tampli=cstrtoda(strbuf2);
		    i+=1;
			}
			Nampli=i;
			EndDialog(hDlg,TRUE);
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


LRESULT APIENTRY LookArrival(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
char strbuf1[200];
char strbuf2[80];
int i,lbindex;
static char szFile[256];
OPENFILENAME ofn;
char szFileTitle[256],szFilter[256];
FILE *arv;

switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		   for(i=0;i<Narriv;i++)
		{
	    sprintf(strbuf1,"%5s %2s %3s %6s %18s %8s %4s %s",
		ST[i].Sta,ST[i].Nt,ST[i].Cha,ST[i].Wtype,
		cdatostr(ST[i].Tarrive,strbuf2,0),ST[i].Wsign,ST[i].Wqual,ST[i].Comment);
		SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_ADDSTRING,(WPARAM)0,(LPARAM)strbuf1);
		}
	   return (TRUE);
       case WM_COMMAND: 
		   {     
		   /* message: received a command */
          switch(LOWORD(wParam)) 
			{
            case IDC_DELETE:
			lbindex=SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);	
			if(lbindex!=LB_ERR)
			SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_DELETESTRING,(WPARAM)lbindex,(LPARAM)0);	

			break;
			case IDC_RENAME:
				lbindex=SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);	
			if(lbindex!=LB_ERR){i=lbindex;
				if(DialogBox(hInst,"IDD_DIALOG16",hWnd,GetRename))
				strcpy(ST[i].Wtype,renwtype);
				sprintf(strbuf1,"%5s %2s %3s %6s %18s %8s %4s %s",
		ST[i].Sta,ST[i].Nt,ST[i].Cha,ST[i].Wtype,
		cdatostr(ST[i].Tarrive,strbuf2,0),ST[i].Wsign,ST[i].Wqual,ST[i].Comment);
				SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_DELETESTRING,(WPARAM)lbindex,(LPARAM)0);	
				SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_ADDSTRING,(WPARAM)0,(LPARAM)strbuf1);
			}
			break;
			case IDC_LOADLIST:
			strcpy(szFilter,"Arrivals(*.arv)|*.arv|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =256;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 256;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=0;
			if(!GetSaveFileName(&ofn))break;
			if((arv=fopen(szFile,"r"))==NULL)break;
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_DELETESTRING,(WPARAM)0,(LPARAM)0)!=LB_ERR);	
			while(fgets(strbuf1,200,arv))
			{
			strbuf1[strlen(strbuf1)-1]=0;
			SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_ADDSTRING,(WPARAM)0,(LPARAM)strbuf1);
			i++;
			}

			Narriv=i;
			fclose(arv);
			break;
			case IDC_SAVELIST:
	        strcpy(szFilter,"Arival Files(*.arv)|*.arv|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =256;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 256;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=0;
			if(!GetSaveFileName(&ofn))break;
			if((arv=fopen(szFile,"w"))==NULL)break;
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_GETTEXT,(short)i,(LPARAM)strbuf1)>0)
			{
			fprintf(arv,"%s\n",strbuf1);			
			i++;
			}
			fclose(arv);
			break;
			case IDOK:
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LISTTIME,LB_GETTEXT,(WPARAM)i,(LPARAM)strbuf1)>0)
			{
			sscanf(strbuf1,"%s %s %s %s %s %s %s",
		    ST[i].Sta,ST[i].Nt,ST[i].Cha,ST[i].Wtype,
		    strbuf2,ST[i].Wsign,ST[i].Wqual);
			strcpy(ST[i].Comment,&strbuf1[53]);
			ST[i].Tarrive=cstrtoda(strbuf2);
		    i+=1;
			}
			Narriv=i;
			EndDialog(hDlg,TRUE);
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




LRESULT APIENTRY GetMotionComponents(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[200];
int i;int n,m,k,j;
switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
	j=0;	
		for(i=0;i<Ntrass;i++)
	{
	if(S[i].A){
	sprintf(tmpstr,"%s %s %s %s %c",S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,S[i].Type);
	SendDlgItemMessage(hDlg,IDC_AX,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	SendDlgItemMessage(hDlg,IDC_AY,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
	SendDlgItemMessage(hDlg,IDC_AZ,CB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);
    if(S[i].Cha[2]=='E'||S[i].Cha[2]=='T'||S[i].Cha[2]=='2'){    SendDlgItemMessage(hDlg,IDC_AX,CB_SETCURSEL,(WPARAM)j,(LPARAM)tmpstr);
	if(S[i].angl==270.) SendDlgItemMessage(hDlg,IDC_REV1,BM_SETCHECK,(WPARAM)1,(LPARAM)0);}
    if(S[i].Cha[2]=='N'||S[i].Cha[2]=='R'||S[i].Cha[2]=='1'){    SendDlgItemMessage(hDlg,IDC_AY,CB_SETCURSEL,(WPARAM)j,(LPARAM)tmpstr);
	if(S[i].angl==180.) SendDlgItemMessage(hDlg,IDC_REV2,BM_SETCHECK,(WPARAM)1,(LPARAM)0);}
    if(S[i].Cha[2]=='Z'){    SendDlgItemMessage(hDlg,IDC_AZ,CB_SETCURSEL,(WPARAM)j,(LPARAM)tmpstr);
	if(S[i].Dip!=-90.) SendDlgItemMessage(hDlg,IDC_REV3,BM_SETCHECK,(WPARAM)1,(LPARAM)0);}
    j++;
	}

	}
		return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{
            case IDOK:          
		if(SendDlgItemMessage(hDlg,IDC_REV1,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==0)REV1=1;
		else REV1=-1;
		if(SendDlgItemMessage(hDlg,IDC_REV2,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==0)REV2=1;
		else REV2=-1;
		if(SendDlgItemMessage(hDlg,IDC_REV3,BM_GETCHECK,(WPARAM)0,(LPARAM)0)==0)REV3=1;
		else REV3=-1;
		n=SendDlgItemMessage(hDlg,IDC_AX,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		m=SendDlgItemMessage(hDlg,IDC_AY,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		k=SendDlgItemMessage(hDlg,IDC_AZ,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
		j=0;
		for(i=0;i<Ntrass;i++)
		if(S[i].A){if(j==n)MOTX=i;if(j==m)MOTY=i;if(j==k)MOTZ=i;j++;}
     	EndDialog(hDlg, TRUE);
		break;
		case IDCANCEL:
			EndDialog(hDlg,FALSE);
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




LRESULT APIENTRY GetPredictTimes(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
FILE *in;
//daat tor;
static int fg=0;
RECT r;
char ph[10];
char tmpstr[100];
int i;
static int nph;
static daat tor;
static double dist,dph;
int teleflag[]=
				 {

1,//"Pg"
1,//"PgPg"
1,//"Sg"
1,//"SgSg"
3,//"PcP"
3,//"PcS"
3,//"ScP"
3,//"ScS"
3,//"PKiKP"
3,//"SKiKP"
3,//"PKKPdf"
3,//"PKKSdf"
3,//"SKKPdf"
3,//"SKKSdf"
3,//"P'P'df"
3,//"S'S'df"
1,//"Pn"
1,//"Pb"
1,//"PbPb"
1,//"PnPn"
1,//"Sn"
1,//"Sb"
1,//"SbSb"
1,//"SnSn"
3,//"P'P'ab"
2,//"P"
2,//"S"
1,//"SPg"
1,//"PgS"
3,//"PP"
3,//"SS"
3,//"P'P'bc"
1,//"SPn"
1,//"PnS"
3,//"SKSac"
3,//"SKKSac"
3,//"S'S'ac"
3,//"PKKPbc"
3,//"SKKPbc"
3,//"PKKSbc"
3,//"PS"
3,//"SP"
3,//"Pdiff"
3,//"Sdiff"
3,//"SKSdf"
3,//"PKKPab"
3,//"PKSdf"
3,//"SKPdf"
3,//"PKPdf"
3,//"SKPbc"
3,//"PKSbc"
3,//"SKPab"
3,//"PKSab"
3,//"PKKSab"
3,//"SKKPab"
3,//"PKPbc"
3,//"PKPab"
3,//"pPKiKP"
3,//"sPKiKP"
1,//"pPn"
1,//"sPn"
1,//"pPb"
1,//"sPb"
1,//"sPg"
1,//"sSn"
1,//"sSb"
1,//"pPg"
1,//"sSg"
3,//"pP"
3,//"sP"
3,//"sS"
3,//"pS"
3,//"pSKSac"
3,//"sSKSac"
3,//"pPdiff"
3,//"sPdiff"
3,//"pSdiff"
3,//"sSdiff"
3,//"pSKSdf"
3,//"sSKSdf"
3,//"pPKPdf"
3,//"sPKPdf"
3,//"pPKPbc"
3,//"pPKPab"
3,//"sPKPbc"
3//"sPKPab"
};


static int k=0;
switch (message) 
{
case WM_INITDIALOG:
GetClientRect(hDlg,&r);
for(i=0;i<Ntrass;i++)if(S[i].A)break;
k=i;
draw_vadat(NULL,r);
tor.day=TOR.day;tor.msec=TOR.msec;
dph=DEP;
dist=S[i].dist=get_dist(S[i].Sta,S[i].Nt,DEP);
cdatostr(TOR,tmpstr,1);
SetDlgItemText(hDlg,IDC_EDIT1,tmpstr);
sprintf(tmpstr,"Delta(degrees)=%g",S[i].dist);
SetDlgItemText(hDlg,IDC_EDIT2,tmpstr);
sprintf(tmpstr,"Depth(km)=%g",DEP);
SetDlgItemText(hDlg,IDC_EDIT3,tmpstr);
sprintf(tmpstr,"%s\\aspi91.bin",DIRNAME);
in=fopen(tmpstr,"rb");
fread(&nph,4,1,in);
fseek(in,20,SEEK_SET);
for(i=0;i<nph;i++)
{
fread(&ph[0],10,1,in);
strcpy(&PREDPH[i*10],&ph[0]);
if(!fg)PREDAC[i]=0;
SendDlgItemMessage(hDlg,IDC_ARA,LB_ADDSTRING,(WPARAM)0,(LPARAM)ph);
}


fg=1;
for(i=0;i<nph;i++)
{
SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,(WPARAM)(PREDAC[i]),(LPARAM)i);
}



fclose(in);
		   
	   return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{
		  case IDC_ALL:
			  for(i=0;i<86;i++)
				  SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,1,i);
			  break;

			case IDC_CLEAR:
			  for(i=0;i<86;i++)
				  SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,0,i);
			  break;
			case IDC_TELE:
for(i=0;i<86;i++)
if(teleflag[i]>=2)SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,1,i);
else SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,0,i);
				break;


				case IDC_LOCAL:
for(i=0;i<86;i++
	)
if(teleflag[i]<=2)SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,1,i);
else SendDlgItemMessage(hDlg,IDC_ARA,LB_SETSEL,0,i);
break;
		  case IDC_SLIDER1L:
		TOR=daatadsec(TOR,-500.);
		cdatostr(TOR,tmpstr,1);
		SetDlgItemText(hDlg,IDC_EDIT1,tmpstr);
//		SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
			
		  break;
		  case IDC_SLIDER1R:
		TOR=daatadsec(TOR,500.);
		cdatostr(TOR,tmpstr,1);

		SetDlgItemText(hDlg,IDC_EDIT1,tmpstr);
//		SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
		  break;
		  case IDC_SLIDER2L:
		  S[k].dist-=0.1;
		  if(S[k].dist<0.)S[k].dist=0;
		  sprintf(tmpstr,"Delta(degrees)=%g",S[k].dist);
		  SetDlgItemText(hDlg,IDC_EDIT2,tmpstr);
//		  SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
		  break;
		  case IDC_SLIDER2R:
		  S[k].dist+=0.1;
		  if(S[k].dist>180.)S[k].dist=180.;
		  sprintf(tmpstr,"Delta(degrees)=%g",S[k].dist);
		  SetDlgItemText(hDlg,IDC_EDIT2,tmpstr);
//		  SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
		  break;
		  case IDC_SLIDER3L:
		  DEP-=10.;
		  if(DEP<0.)DEP=0;
		  sprintf(tmpstr,"Depth(km)=%g",DEP);
		  SetDlgItemText(hDlg,IDC_EDIT3,tmpstr);
//		  SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
		  break;
		  case IDC_SLIDER3R:
		  DEP+=10.;
		  if(DEP>670.)DEP=670.;
		  sprintf(tmpstr,"Depth(km)=%g",DEP);
		  SetDlgItemText(hDlg,IDC_EDIT3,tmpstr);
//		  SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
		  break;
            case IDOK:          
			for(i=0;i<nph;i++)
			PREDAC[i]=SendDlgItemMessage(hDlg,IDC_ARA,LB_GETSEL,i,(LPARAM)0);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
		   	EndDialog(hDlg, TRUE);
			break;
			case IDC_REFRESH:
			for(i=0;i<nph;i++)
			PREDAC[i]=SendDlgItemMessage(hDlg,IDC_ARA,LB_GETSEL,i,(LPARAM)0);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
			break;
			case IDCANCEL:
				for(i=0;i<nph;i++)
					PREDAC[i]=0;

			TOR.day=tor.day;
			TOR.msec=tor.msec;
			DEP=dph;
			S[k].dist=dist;
			EndDialog(hDlg,FALSE);
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



LRESULT APIENTRY GetVadat(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	

char tmpstr[200];
RECT r;
int prscr;
static double vpvs=1.8;
switch (message) 
{
       case WM_INITDIALOG:
		   /* message: initialize dialog box */
		   sprintf(tmpstr,"Average Vp:Vs=%g",VPVS);
		SetDlgItemText(hDlg,IDC_EVPVS,tmpstr);
		vpvs=VPVS;

		return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{

          case IDC_PRIN:

			memset(&pd, 0, sizeof(PRINTDLG));
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hWnd;
			pd.Flags = PD_RETURNDC;
			//pd.Flags=PD_RETURNDEFAULT;
			//pd.hDevMode=NULL;
		//	pd.hDevNames=NULL;
		//	pd.hInstance=NULL;


			if (PrintDlg(&pd) != 0)
			{   
				//GlobalLock(pd.hDevMode);
				//GlobalLock(pd.hDevNames);
				//if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				//if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				
				DocInfo.cbSize = sizeof(DOCINFO);
				DocInfo.lpszDocName="VadGsnDoc";
				DocInfo.lpszOutput = (LPSTR) NULL;
				if(StartDoc(pd.hDC, &DocInfo)<=0)
				goto OOOD;
				if(StartPage(pd.hDC)<=0)goto OOOD;
				r.top=r.left=0;
				r.right=GetDeviceCaps(pd.hDC,HORZRES);
				r.bottom=GetDeviceCaps(pd.hDC,VERTRES);
				memcpy(&lfp,&lf,sizeof(LOGFONT));
				prscr=GetDeviceCaps(pd.hDC,LOGPIXELSX)/GetDeviceCaps(memdc,LOGPIXELSX);
				PRINTPARAM=1;
				lfp.lfHeight=lf.lfHeight*prscr;
				hFontP = CreateFontIndirect(&lfp);
				hOldFontP=SelectObject(pd.hDC,hFontP);
				n_p=CreatePen(PS_SOLID,1,RGB(0,0,0));
				o_p=SelectObject(pd.hDC,n_p);
				prscr+=1;
				r.top+=DT*prscr;
			r.bottom-=DB*prscr;
			r.left+=DL*prscr;
			r.right-=DR*prscr;
			draw_vadat(pd.hDC,r);
				PRINTPARAM=0;
				SelectObject(pd.hDC,hOldFontP);
				DeleteObject(hFontP);
				SelectObject(pd.hDC,o_p);
				DeleteObject(n_p);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
				

				if (pd.hDevMode != NULL) GlobalFree(pd.hDevMode);
				if (pd.hDevNames != NULL) GlobalFree(pd.hDevNames);
				}   
			else {OOOD:MessageBox(hWnd, "Print attempt failed", "", IDOK);}

			  SendMessage(hWnd,WM_PLOTVAD,(WPARAM)0,(LPARAM)0);		  
			  break;

		  case IDC_SVPSL:
			  VPVS-=0.01;
			  if(VPVS<1.)VPVS=1.;
			  sprintf(tmpstr,"Average Vp:Vs=%g",VPVS);
		SetDlgItemText(hDlg,IDC_EVPVS,tmpstr);
			  SendMessage(hWnd,WM_PLOTVAD,(WPARAM)0,(LPARAM)0);		
			  break;
			case IDC_SVPSR:
			  VPVS+=0.01;
			  if(VPVS>3.)VPVS=3.;
			  sprintf(tmpstr,"Average Vp:Vs=%g",VPVS);
	        	SetDlgItemText(hDlg,IDC_EVPVS,tmpstr);
			  SendMessage(hWnd,WM_PLOTVAD,(WPARAM)0,(LPARAM)0);		
			  break;
            case IDOK:          
		SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
     	EndDialog(hDlg, TRUE);
		break;
		case IDCANCEL:
			VPVS=vpvs;
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);		
			EndDialog(hDlg,FALSE);

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



LRESULT APIENTRY GetRotation(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
char tmpstr[200];
static double angle=0;
int i,j;
static int ik[3];
static int ki[3];
switch (message) 
{
       case WM_INITDIALOG:
		sprintf(tmpstr,"Azimuth=%g ",angle);
		SetDlgItemText(hDlg,IDC_ANGLE,tmpstr);
		j=0;
        for(i=0;i<Ntrass;i++)
		{
		if(S[i].A)ik[j++]=i;
		}

		if(S[ik[0]].Cha[2]=='E')ki[1]=0;
		else if(S[ik[0]].Cha[2]=='N')ki[0]=0;
		else if(S[ik[0]].Cha[2]=='Z')ki[2]=0;
		if(S[ik[1]].Cha[2]=='E')ki[1]=1;
		else if(S[ik[1]].Cha[2]=='N')ki[0]=1;
		else if(S[ik[1]].Cha[2]=='Z')ki[2]=1;
		if(S[ik[2]].Cha[2]=='E')ki[1]=2;
		else if(S[ik[2]].Cha[2]=='N')ki[0]=2;
		else if(S[ik[2]].Cha[2]=='N')ki[2]=2;


        for(i=0;i<Ntrass;i++)
        if(S[i].A)
        {
        dup_sei(&S[Ntrass],S[i],i);
        S[Ntrass].A=0;
		Ntrass+=1;
        }
		Ntrass-=Nactiv;
		 /* message: initialize dialog box */
		return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{


		  case IDC_S1L:
			  angle-=0.5;
			  if(angle<0.)angle+=360.;
			  sprintf(tmpstr,"Azimuth=%g ",angle);
		      SetDlgItemText(hDlg,IDC_ANGLE,tmpstr);
		      
			  
			break;
			case IDC_S1R:
			angle+=0.5;
			if(angle>360.)angle-=360.;
			sprintf(tmpstr,"Azimuth=%g ",angle);
		    SetDlgItemText(hDlg,IDC_ANGLE,tmpstr);	
			break;
			case IDC_SR1L:
			  angle-=10.;
			  if(angle<0.)angle+=360.;
			  sprintf(tmpstr,"Azimuth=%g ",angle);
		      SetDlgItemText(hDlg,IDC_ANGLE,tmpstr);
		      
			  
			break;
			case IDC_SR1R:
			angle+=10.;
			if(angle>360.)angle-=360.;
			sprintf(tmpstr,"Azimuth=%g ",angle);
		    SetDlgItemText(hDlg,IDC_ANGLE,tmpstr);	
			break;

			case IDC_REFRESH:
			rot_sei(Tmin,Tmax,&S[ik[ki[0]]],&S[ik[ki[1]]],&S[Ntrass+ki[0]],&S[Ntrass+ki[1]],angle);
			sprintf(S[ik[ki[2]]].Comment,"%s [R=%g]",S[Ntrass+ki[2]].Comment,angle);
			SendMessage(hWnd,WM_PLOTSEI,(WPARAM)0,(LPARAM)0);
			break;
			case IDOK:          
			rot_sei(Tming,Tmaxg,&S[ik[ki[0]]],&S[ik[ki[1]]],&S[Ntrass+ki[0]],&S[Ntrass+ki[1]],angle);
			sprintf(S[ik[ki[2]]].Comment,"%s [R=%g]",S[Ntrass+ki[2]].Comment,angle);
			Ntrass+=Nactiv;
		    EndDialog(hDlg, TRUE);
		    break;
		    case IDCANCEL:
			for(i=0;i<3;i++)
			{
			free((void*)S[ik[i]].Data);free((void*)S[ik[i]].R.im);free((void*)S[ik[i]].R.re);
			dup_sei(&S[ik[i]],S[i+Ntrass],i);
			free((void*)S[Ntrass+i].Data);free((void*)S[Ntrass+i].R.im);free((void*)S[Ntrass+i].R.re);
			}
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




LRESULT APIENTRY GetSourceAz(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
char tmpstr[200];
char staname[20],ntname[20];
static double angle;
static double iangle;
int i;

switch (message) 
{
       case WM_INITDIALOG:
		sprintf(tmpstr,"Azimuth=%g ",SStrike);
		SetDlgItemText(hDlg,IDC_STRIKE,tmpstr);
		sprintf(tmpstr,"Angle of Incedence=%g ",DDip);
		SetDlgItemText(hDlg,IDC_DIP,tmpstr);
		angle=SStrike;
		iangle=DDip;
		return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{


		     case IDC_S2L:
			  SStrike-=0.5;
			  if(SStrike<0.)SStrike+=360.;
			  sprintf(tmpstr,"Azimuth=%g ",SStrike);
		      SetDlgItemText(hDlg,IDC_STRIKE,tmpstr);
		      
			break;
			case IDC_SR2R:
			SStrike+=10.;
			if(SStrike>360.)SStrike-=360.;
			sprintf(tmpstr,"Azimuth=%g ",SStrike);
		    SetDlgItemText(hDlg,IDC_STRIKE,tmpstr);	
			break;

			case IDC_SR2L:
			  SStrike-=10.;
			  if(SStrike<0.)SStrike+=360.;
			  sprintf(tmpstr,"Azimuth=%g ",SStrike);
		      SetDlgItemText(hDlg,IDC_STRIKE,tmpstr);
		      
			break;
			case IDC_S2R:
			SStrike+=0.5;
			if(SStrike>360.)SStrike-=360.;
			sprintf(tmpstr,"Azimuth=%g ",SStrike);
		    SetDlgItemText(hDlg,IDC_STRIKE,tmpstr);	
			break;
			case IDC_S3L:
			  DDip-=0.5;
			  if(DDip<0.)DDip=0.;
			  sprintf(tmpstr,"Angle of Incedence=%g ",DDip);
		      SetDlgItemText(hDlg,IDC_DIP,tmpstr);
		      
			break;
			case IDC_SR3R:
			DDip+=10.;
			if(DDip>90.)DDip=90.;
			sprintf(tmpstr,"Angle of Incedence=%g ",DDip);
		    SetDlgItemText(hDlg,IDC_DIP,tmpstr);	
			break;

			case IDC_SR3L:
			  DDip-=10.;
			  if(DDip<0.)DDip=0.;
			  sprintf(tmpstr,"Angle of Incedence=%g ",DDip);
		      SetDlgItemText(hDlg,IDC_DIP,tmpstr);
		      
			break;
			case IDC_S3R:
			DDip+=0.5;
			if(SStrike>90.)DDip=90.;
			sprintf(tmpstr,"Angle of Incedence=%g ",DDip);
		    SetDlgItemText(hDlg,IDC_DIP,tmpstr);	
			break;
			case IDC_REFRESH:
			SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);
			break;
			case IDOK:          
			for(i=0;i<Ntrass;i++)if(S[i].A)break;
			strcpy(staname,S[i].Sta);
			strcpy(ntname,S[i].Nt);
			for(i=0;i<Ntrass;i++)
				if(strcmp(staname,S[i].Sta)==0&&strcmp(S[i].Nt,ntname)==0)
			{S[i].Azim=SStrike;}
			EndDialog(hDlg, TRUE);
			SendMessage(hWnd,WM_PLOTMOT,(WPARAM)0,(LPARAM)0);
		    break;
		    case IDCANCEL:
			SStrike=angle;
			DDip=iangle;
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



LRESULT APIENTRY GetStatistic(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
char tmpstr[200];

static char szFile[256];
OPENFILENAME ofn;
char szFileTitle[256],szFilter[256];
FILE *stata;


int i;

switch (message) 
{
       case WM_INITDIALOG:
	for(i=0;i<Ntrass;i++)
	{

	sprintf(tmpstr,
		"%s%s%s dlt=%6.2lf[%6.2lf] az=%6.1lf[%6.1lf] MPV=%g MPH=%g MSH=%g MSV=%g MLRV=%g MLRH=%g MLQ=%g MWA=%g '%s'",
		S[i].Sta,S[i].Nt,S[i].Cha,get_dist(S[i].Sta,S[i].Nt,DEP),cal_dist(FIC,LAMC,S[i].Lat,S[i].Lon),S[i].Azim,cal_azim(S[i].Lat,S[i].Lon,FIC,LAMC),
		calc_mb_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MPV"),
		calc_mb_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MPH"),
		calc_mb_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MSH"),
		calc_mb_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MSV"),
		calc_ms_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MLRV"),
		calc_ms_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MLRH"),
		calc_ms_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MLQ"),
		calc_ml_ch(S[i].Sta,S[i].Nt,S[i].Cha,S[i].Comment,"MWA"),
		S[i].Comment);  
	SendDlgItemMessage(hDlg,IDC_LIST1,LB_ADDSTRING,(WPARAM)0,(LPARAM)tmpstr);

//calc_mb("MPV"),calc_mb("MPH"),calc_mb("MSH"),calc_mb("MSV"),calc_ms("MLRV"),calc_ms("MLRH"),calc_ms("MLQ"),calc_ml("MWA"));
	}
		
	    return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{

		  case ID_SAVE_STAT:
			  strcpy(szFilter,"Arival Files(*.*)|*.*|");
			for (i=0; szFilter[i]; i++)
			if (szFilter[i] =='|') szFilter[i] = '\0';
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile =256;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = 256;
			ofn.lpstrInitialDir =NULL;
			ofn.Flags=0;
			if(!GetSaveFileName(&ofn))break;
			if((stata=fopen(szFile,"w"))==NULL)break;
			i=0;
			while(SendDlgItemMessage(hDlg,IDC_LIST1,LB_GETTEXT,(short)i,(LPARAM)tmpstr)>0)
			{
			fprintf(stata,"%s\n",tmpstr);			
			i++;
			}
			fclose(stata);
			break;


			  break;
		   			case IDOK:          
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


LRESULT APIENTRY GetRename(
        HWND hDlg,                /* window handle of the dialog box */
        UINT message,             /* type of message                 */
        UINT wParam,              /* message-specific information    */
        LONG lParam)
{	
//char tmpstr[200];
static char *WaveType[]=
				 {
"P"
,"S"
,"Pg"
,"PgPg"
,"Sg"
,"SgSg"
,"PcP"
,"PcS"
,"ScP"
,"ScS"
,"PKiKP"
,"SKiKP"
,"PKKPdf"
,"PKKSdf"
,"SKKPdf"
,"SKKSdf"
,"P'P'df"
,"S'S'df"
,"Pn"
,"Pb"
,"PbPb"
,"PnPn"
,"Sn"
,"Sb"
,"SbSb"
,"SnSn"
,"P'P'ab"
,"SPg"
,"PgS"
,"PP"
,"SS"
,"P'P'bc"
,"SPn"
,"PnS"
,"SKSac"
,"SKKSac"
,"S'S'ac"
,"PKKPbc"
,"SKKPbc"
,"PKKSbc"
,"PS"
,"SP"
,"Pdiff"
,"Sdiff"
,"SKSdf"
,"PKKPab"
,"PKSdf"
,"SKPdf"
,"PKPdf"
,"SKPbc"
,"PKSbc"
,"SKPab"
,"PKSab"
,"PKKSab"
,"SKKPab"
,"PKPbc"
,"PKPab"
,"pPKiKP"
,"sPKiKP"
,"pPn"
,"sPn"
,"pPb"
,"sPb"
,"sPg"
,"sSn"
,"sSb"
,"pPg"
,"sSg"
,"pP"
,"sP"
,"sS"
,"pS"
,"pSKSac"
,"sSKSac"
,"pPdiff"
,"sPdiff"
,"pSdiff"
,"sSdiff"
,"pSKSdf"
,"sSKSdf"
,"pPKPdf"
,"sPKPdf"
,"pPKPbc"
,"pPKPab"
,"sPKPbc"
,"sPKPab"};


int i;

switch (message) 
{
       case WM_INITDIALOG:
	for(i=0;i<86;i++)
	{

		   SendDlgItemMessage(hDlg,IDC_COMBARR,CB_ADDSTRING,(WPARAM)0,(LPARAM)WaveType[i]);	
	}
		
	    return TRUE;
	
       case WM_COMMAND: 
		   {     
		   /* message: recei+ved a command */
          switch(LOWORD(wParam)) 
			{


		   			case IDOK:          
					if(!GetDlgItemText( hDlg,IDC_COMBARR,renwtype,7))
					EndDialog(hDlg,FALSE);
					else EndDialog(hDlg,TRUE);
					
		
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

