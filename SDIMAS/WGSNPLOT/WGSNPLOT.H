#include <math.h>
#include <io.h> 
#include <stdio.h>
#include <windows.h>
#include <malloc.h>
#include "lnm.h"
double FIC=53;
double LAMC=160.;
double MMM=18000;

char DIRNAME[100];
char PREDPH[1000];
int  PREDAC[100];
double SStrike,DDip;
HDC memdc;
int maxX,maxY;
HBITMAP hbit;
HBRUSH hbrush;
HPEN hpen;
CHOOSEFONT cf;
HFONT hFont, hOldFont;
HFONT hFontP, hOldFontP;
HWND hWnd;
HANDLE hInst;

double VPVS=1.8;
double FI,LAM;
double DEP=0;
typedef struct {double LAMDA,STRIKE,DIP;}polarization;

polarization POLAR[4];

LOGFONT lf,lfp;
double SCFACTOR=1.;
int PRINTPARAM=0;
#define  M_PI        3.14159265358979323846


void fft(double  *fr,double  *fi,int n,int isign)

{
		int mr,nn,m,l,istep,i,j;
		double tr,ti,a,pisign,wr,wi,w;

		pisign = 3.1415926535*isign;
		mr=0;
		nn=n-1;
		for (m=1; m <= nn; m++)
		{
			l=n;
			while(mr+l > nn) l=l/2;
			mr = mr%l+l;
			if (mr <= m) continue;
			tr=fr[m];
			fr[m]=fr[mr];
			fr[mr]=tr;
			ti=fi[m];
			fi[m]=fi[mr];
			fi[mr]=ti;
		}

		l=1;
		while(l < n)
		{
			istep=2*l;
			for (m=0; m < l; m++)
			{
					a=pisign*m/l;
					wr=cos(a);
					wi=sin(a);
				for (i=m; i < n; i+=istep)
					{
				j=i+l;
				tr=wr*fr[j]-wi*fi[j];
				ti=wr*fi[j]+wi*fr[j];
				fr[j]=fr[i]-tr;
				fi[j]=fi[i]-ti;
				fr[i]=fr[i]+tr;
				fi[i]=fi[i]+ti;
				}
			}
			l=istep;
		}
	w=(double)n;
	if(isign>0)
	for(i=0;i<n;i++)
	{fr[i]/=w;
	fi[i]/=w;}
}





typedef struct {int day; double msec;}daat;
typedef struct {double left,top,right,bottom;}phisw;
typedef struct {double a0,f0;int nz,np;double *re,*im;} responce;
//typedef struct {char Sta[6];daat TP,TS;double A,T;double AZ;}statistic;
daat   TOR={0,0.};
daat Tming,Tmaxg,Tminp,Tmaxp,Previous;
daat Tmin,Tmax;
typedef struct {
	    char Sta[6];
		char Cha[4];
		char Nt[3];
		char Comment[100];
		char Type; /*D-Displecement V-Velocity A-acsel U-unknown*/
		int Samp;
		double Rate;
		daat Stime;
		double Sens;
		double Fsens;
		double Lat,Lon,Elev,Depth;
		double angl;
		double Azim,Dip;
		double Min,Max,Mid;
		responce R;
		double dist;
		double Mag;
		int A;
		double *Data;
		} Seismogr;


typedef struct {int n;double rate;double *spdata;double min,max,f1,f2;Seismogr s;} Spectr;
typedef struct {char Comment[100];char Wtype[6];daat Tarrive;char Wqual[5];char Wsign[9];
		char Sta[6];
		char Cha[4];
		char Nt[3];
        } Sarrival;
typedef struct {char Comment[100];
	char Wtype[6];daat Tampli;double amplit;double period;char Sta[6];
		char Cha[4];
		char Nt[3];
		char Type;
		} Samplitude;


Seismogr S[500];
RECT     W[500];
Spectr   SP[500];
Sarrival ST[500];
Samplitude SA[500];
int Ntrass=0;
int Nactiv=0;
int Narriv=0;
int Nampli=0;

double cal_dist(double fi1,double lam1,double fi2,double lam2)
{
double dist;
double x1,x2,y1,y2,z1,z2;
fi1*=M_PI/180;
lam1*=M_PI/180;
fi2*=M_PI/180;
lam2*=M_PI/180;

x1=cos(lam1)*cos(fi1);y1=cos(fi1)*sin(lam1);z1=sin(fi1);
x2=cos(lam2)*cos(fi2);y2=cos(fi2)*sin(lam2);z2=sin(fi2);
dist=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
dist=asin(dist/2)*180/M_PI*2;

return dist;

}

double  get_t_time(char *fname,char *phasa,double del,double dep)
{
FILE *in;
int i,j,k;
float t1,t2;
double t;
int nph;
int ndi;
int nde;
char *name;
int sdi;
int sde;
if((in=fopen(fname,"rb"))==NULL){fclose(in);return -6.;}
fread(&nph,4,1,in);
fread(&ndi,4,1,in);
fread(&nde,4,1,in);
fread(&sdi,4,1,in);
fread(&sde,4,1,in);
name=(char *)malloc(10*nph);
if(!name){fclose(in);return -5.;}
fread(&name[0],10*nph,1,in);
for(i=0;i<nph;i++)
{
if(strcmp(phasa,&name[i*10])==0)break;
}
if(i==nph){fclose(in);return -3.;}
if(del>(ndi-1)*sdi){fclose(in);return -1.;}
if(dep>(nde-1)*sde){fclose(in);return -1.;}
k=(int)(del/sdi);
j=(int)(dep/sde);
fseek(in,(i*nde*ndi+k*nde+j)*4+10*nph+20,SEEK_SET);
fread(&t1,4,1,in);
if(t1<0.){fclose(in);return -1.;}
if(del==k*sdi){t=(double)t1;fclose(in);return t;}
fseek(in,(i*nde*ndi+(k+1)*nde+j)*4+10*nph+20,SEEK_SET);
fread(&t2,4,1,in);
if(t2<0.){fclose(in);return -1.;}
t=(double)t1+(t2-t1)*(del-k*sdi)/sdi;
fclose(in);
free((void*)name);
return t;
}

int get_ps_time(char *fname,double del,double dep,double *tp,double *ts)
{
double t0;

char *pph[]=
{
"Pg",
"Pn",
"P",
"Pdiff",
"PKPdf"};
char *sph[]=
{
"Sg",
"Sn",
"S",
"Sdiff",
"SKSdf"
};

int i,k;
if(del>144)return -1;
t0=-1;
*tp=99999999.;
for(i=0;i<5;i++)
{
t0=get_t_time(fname,pph[i],del,dep);
if(t0>=0.&&t0<*tp){k=i;*tp=t0;}
}
*ts=get_t_time(fname,sph[k],del,dep);
return 1;
}

void Line(HDC hdc,int x1,int y1,int x2,int y2)
{
POINT P;
MoveToEx(hdc,x1,y1,&P);
LineTo(hdc,x2,y2);
}

double raznd(daat d1,daat d2)
{double d;
double ms;
d=d1.day-d2.day;
ms=d1.msec-d2.msec;
d*=86400000.;
d+=ms;
return d;
}

double razndd(daat d1,daat d2)
{double d;
double ms;
d=d1.day-d2.day;
ms=d1.msec-d2.msec;
d+=ms/86400000.;
return d;
}
//*********************************************************************
daat cjstrtoda(char *str)
{
daat d;
long yy,dd,hh,mi,ss,ts;
sscanf(str,"%4d%3d%2d%2d%2d%d",&yy,&dd,&hh,&mi,&ss,&ts);
d.msec=3600000.*hh + 60000.*mi +ss*1000.+ts/10.;
d.day=yy*365l + yy/4l+1l;
if(yy%4==0)d.day-=1l;
d.day=d.day+dd-1;
return d;
}
//********************************************************************
daat cstrtoda(char *str)
{
daat d;
long mon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
long yy,mm,dd,hh,mi,ss,ts,i;
sscanf(str,"%4d%2d%2d%2d%2d%2d%d",&yy,&mm,&dd,&hh,&mi,&ss,&ts);
d.msec=3600000.*hh + 60000.*mi +1000.*ss+ts/10.;
d.day=yy*365l + yy/4l+1l;
if(yy%4==0)d.day-=1l;
if(yy%4==0 && mm>2)d.day+=1l;
for(i=0;i<mm-1;i++)d.day+=mon[i];
d.day=d.day+dd-1;
return d;
}
//****************************************************************
char * cdatostr(daat d,char *str,int flag)
{
int yy,mm,dd,hh,mi,i;
int mon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
double msec;
hh=(int)floor(d.msec/3600000);
mi=(int)floor((d.msec-hh*3600000)/60000.);
msec=d.msec-hh*3600000 - mi*60000;
yy=(int)((d.day/1461)*4);
dd=(int)(d.day-((long)yy/4)*1461L);
for(i=3;i>0;i--)if(dd>365*i)break;
yy+=i;
dd-=(365*i);
if(yy%4==0){mon[1]+=1;dd+=1;}
for(mm=0;mm<12;mm++)
{
if(dd<=mon[mm])break;
dd-=mon[mm];}
mm+=1;
if(!flag)sprintf(str,"%04d%02d%02d%02d%02d%06ld",yy,mm,dd,hh,mi,(long)(msec*10+0.5));
if(flag==1) sprintf(str,"%04d.%02d.%02d %02d:%02d:%02d.%03d",yy,mm,dd,hh,mi,(int)msec/1000,(int)msec%1000);
if(flag==2) sprintf(str,"%02dh:%02d:%02d",hh,mi,(int)(msec/1000+0.5));
return str;
}
int cdatostrj(daat d,int flag)
{
int yy,dd,hh,mi,ss,ms,i;
double msec;
hh=(int)floor(d.msec/3600000);
mi=(int)floor((d.msec-hh*3600000)/60000.);
msec=d.msec-hh*3600000 - mi*60000;
yy=(int)((d.day/1461)*4);
dd=(int)(d.day-((long)yy/4)*1461L);
for(i=3;i>0;i--)if(dd>365*i)break;
yy+=i;
dd-=(365*i);
ss=(int)msec/1000;
ms=((int)msec)%1000;
if(yy%4==0){dd+=1;}
if(flag==0)return yy;
if(flag==1)return dd;
if(flag==2)return hh;
if(flag==3)return mi;
if(flag==4)return ss;
if(flag==5)return ms;
}
//******************************************************************
daat daatadsec(daat t,double msec)
{
long day;
double sec;
day=t.day;
sec=t.msec;
sec+=msec;
if(sec >=86400000.){day+=(int)(sec/86400000.);sec=fmod(sec,86400000.);}
if(sec<0.){
day+=(int)(sec/86400000.)-1;sec=86400000.+fmod(sec,86400000.);
}
t.day=day;
t.msec=sec;
return t;
}
void log_ax(HDC hdc,phisw f,RECT w,char *name,char typ)
{
int i,k;
HPEN p_s,p_d,p_o;
HFONT f_o,f_s;
char str[20];
double wdx=w.right-w.left;
double fdx=log10(f.right)-log10(f.left);
double hor=wdx/fdx;
double minf,maxf,sf,ef;
double fx,x;
if(PRINTPARAM==0)
{f_s=CreateFontIndirect(&lf);
f_o=SelectObject(hdc,f_s);
}
else {f_s=CreateFontIndirect(&lfp);
f_o=SelectObject(hdc,f_s);
}
p_s=CreatePen(PS_SOLID,1,RGB(0,0,0));
p_d=CreatePen(PS_DOT,1,RGB(0,0,0));
p_o=SelectObject(hdc,p_s);

Line(hdc,w.left,w.top,w.right,w.top);
Line(hdc,w.left,w.bottom,w.right,w.bottom);
minf=f.left;maxf=f.right;
if(minf>maxf){minf=f.right;maxf=f.left;}
sf=minf;
ef=maxf;
minf=log10(minf);
maxf=log10(maxf);
i=0;
while(sf>10.||sf<1.)
{	if(sf<1.)
	{sf*=10.;i--;}
		else {sf/=10.;i++;}
}
sf=(float)(i-10.);
i=0;
while(ef>10.||ef<1.)
{	if(ef<1.)
	{ef*=10.;i--;}
		else {ef/=10.;i++;}
}
ef=(float)(i+10.);
i=(int)(ef-sf-20);

SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);
TextOut(hdc,(w.right+w.left)/2,w.top-3,name,strlen(name));
SetTextAlign(hdc,TA_CENTER|TA_TOP);
minf=log10(f.left);
for(fx=sf;fx<ef;fx+=1.)
{
x=w.left+(fx-minf)*hor;
x=(int)(x+0.5);
if(x<w.left||x>w.right)goto MLOGX;
else	{sprintf(str,"%0.0E",pow(10,fx));
	if(typ!=0){
		SelectObject(hdc,p_d);
		Line(hdc,(int)x,w.top,(int)x,w.bottom);
			}
		SelectObject(hdc,p_s);

		Line(hdc,(int)x,w.bottom,(int)x,w.bottom+5);
		TextOut(hdc,(int)x,w.bottom+8,str+1,strlen(str+1));
	}
	MLOGX:
for(k=2;k<=9;k++)
{x=w.left+(log10(k*pow(10,fx))-minf)*hor;
x=(int)(x+0.5);
if(x<w.left||x>w.right)continue;
else { sprintf(str,"%d",k);
		 if(i<=1)sprintf(str,"%0.0E",k*pow(10,fx));
		 if(typ!=0&&i<5){
			 SelectObject(hdc,p_d);
			 Line(hdc,(int)x,w.top,(int)x,w.bottom);
			 }
			 if((i==2&&k%2==0&&k!=8)||(i==3&&k==4)||i<1) TextOut(hdc,(int)x,w.bottom+6,str,strlen(str));
		SelectObject(hdc,p_s);
		  Line(hdc,(int)x,w.bottom,(int)x,w.bottom+3);
		}


}
}
SelectObject(hdc,p_o);
SelectObject(hdc,f_o);
DeleteObject(p_s);
DeleteObject(p_d);
DeleteObject(f_s);
}

void log_ay(HDC hdc,phisw f,RECT w,char *name,char typ)
{
int i,k;
HPEN p_s,p_d,p_o;
HFONT f_s,f_o;
char str[20];
double wdy=w.bottom-w.top;
double fdy=log10(f.top)-log10(f.bottom);
double ver=wdy/fdy;
double minf,maxf,sf,ef;
double fy,y;
if(PRINTPARAM==0){
lf.lfEscapement=900;
f_s=CreateFontIndirect(&lf);}
else {
lfp.lfEscapement=900;
f_s=CreateFontIndirect(&lfp);
}



Line(hdc,w.left,w.top,w.left,w.bottom);
Line(hdc,w.right,w.top,w.right,w.bottom);
minf=f.bottom;maxf=f.top;
if(minf>maxf){minf=f.top;maxf=f.bottom;}
sf=minf;
ef=maxf;
minf=log10(minf);
maxf=log10(maxf);
i=0;
p_s=CreatePen(PS_SOLID,1,RGB(0,0,0));
p_d=CreatePen(PS_DOT,1,RGB(0,0,0));
f_o=SelectObject(hdc,f_s);
p_o=SelectObject(hdc,p_s);

while(sf>10.||sf<1.)
{	if(sf<1.)
	{sf*=10.;i--;}
		else {sf/=10.;i++;}
}
sf=(float)(i-10.);
i=0;
while(ef>10.||ef<1.)
{	if(ef<1.)
	{ef*=10.;i--;}
		else {ef/=10.;i++;}
}
ef=(float)(i+10.);
i=(int)(ef-sf-20.);
SetTextAlign(hdc,TA_CENTER|TA_TOP);
TextOut(hdc,w.right+5,(w.bottom+w.top)/2,name,strlen(name));


SelectObject(hdc,f_o);
DeleteObject(f_s);
if(PRINTPARAM==0){
lf.lfEscapement=0;
f_s=CreateFontIndirect(&lf);
f_o=SelectObject(hdc,f_s);
}
else 
{
lfp.lfEscapement=0;
f_s=CreateFontIndirect(&lfp);
f_o=SelectObject(hdc,f_s);
}

SetTextAlign(hdc,TA_RIGHT|TA_BASELINE);


minf=log10(f.bottom);
for(fy=sf-1.;fy<=ef;fy+=1.)
{
y=w.bottom-(fy-minf)*ver;
if(y<w.top||y>w.bottom)goto MLOGY;
else	{
	sprintf(str,"%0.0E",pow(10,fy));
	if(typ!=0){
		SelectObject(hdc,p_d);
		Line(hdc,w.right,(int)y,w.left,(int)y);}
		SelectObject(hdc,p_s);
		Line(hdc,w.left,(int)y,w.left-7,(int)y);
		TextOut(hdc,w.left-10,(int)y,str+1,strlen(str+1));
	}
	MLOGY:
for(k=2;k<=9;k++)
{y=w.bottom-(log10(k*pow(10,fy))-minf)*ver;
y=(int)(y+0.5);
if(y<w.top||y>w.bottom)continue;
else { sprintf(str,"%d",k);
		 if(i<=1)sprintf(str,"%0.0E",k*pow(10,fy));
		 if(typ!=0&&i<5){
			SelectObject(hdc,p_d);
			 Line(hdc,w.right,(int)y,w.left,(int)y);
			 }
				if((i==2&&k%2==0&&k!=8)||(i==3&&k==4)||i<1) TextOut(hdc,w.left-8,(int)y,str,strlen(str));
				SelectObject(hdc,p_s);
				Line(hdc,w.left,(int)y,w.left-5,(int)y);
		}


}
}
SelectObject(hdc,f_o);
DeleteObject(f_s);
SelectObject(hdc,p_o);
DeleteObject(p_s);
DeleteObject(p_d);
}


void lin_ax(HDC hdc,phisw f,RECT w,char *name,char typ,char *sn)
{
int i;
char str[20];
double fmd;
double wdx=w.right-w.left;
double fdx=f.right-f.left;
double hor=wdx/fdx;
double minf,maxf;
double dx,fx,x;
HPEN n_pen,o_pen;
double ddx;
Line(hdc,w.left,w.top,w.right,w.top);
Line(hdc,w.left,w.bottom,w.right,w.bottom);
n_pen=CreatePen(PS_DOT,1,RGB(0,0,0));
minf=f.left;maxf=f.right;

if(minf>maxf){minf=f.right;maxf=f.left;}
dx=(maxf-minf)/4.;
i=0;
while(dx>10.||dx<1.)
{	if(dx<1.)
	{dx*=10.;i--;}
		else {dx/=10.;i++;}
}
dx=(int)(dx);
if(dx==3.)dx=2.;
if(dx==4.)dx=5.;
if(dx>7.)dx=10.;
if(dx>5&&dx<=7)dx=5.;
dx*=pow(10,i);
ddx=pow(10,i);
maxf=dx*(int)(maxf/dx +1);
minf=dx*(int)(minf/dx-1);
SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);
//if(SIGG)
TextOut(hdc,(w.left+w.right)/2,w.top-5,name,strlen(name));
SetTextAlign(hdc,TA_CENTER|TA_TOP);
for(fx=minf;fx<maxf;fx+=ddx)
{
if(fx<0)fx=(int)(fx/ddx-0.5)*ddx;
else fx=(int)(fx/ddx+0.5)*ddx;
fmd=fmod(fabs(fx),dx);
x=w.left+(fx-f.left)*hor;
x=(int)(x+0.5);
if(x<=w.left||x>=w.right)continue;
sprintf(str,"%G%s",fx,sn);
	if(fmd<ddx/2.||fabs(fmd-dx)<ddx/2.){	Line(hdc,(int)x,w.bottom,(int)x,w.bottom+9);
		TextOut(hdc,(int)x,w.bottom+12,str,strlen(str));
		if(typ!=0){
		o_pen=SelectObject(hdc,n_pen);
		Line(hdc,(int)x,w.top,(int)x,w.bottom);
		n_pen=SelectObject(hdc,o_pen);
					 }

		}
		else Line(hdc,(int)x,w.bottom,(int)x,w.bottom+6);
	}
DeleteObject(n_pen);
}

//***************************************************************

void lin_ay(HDC hdc,phisw f,RECT w,char *name,char typ,char *sn)
{
int i;
HFONT n_font,o_font;
char str[80];
double fmd;
double wdy=w.bottom-w.top;
double fdy=f.top-f.bottom;
double ver=wdy/fdy;
double minf,maxf;
double dy,fy,y;
HPEN n_pen,o_pen;
double ddy;
if(PRINTPARAM==0)
{lf.lfEscapement=900;
n_font=CreateFontIndirect(&lf);}
else 
{lfp.lfEscapement=900;
n_font=CreateFontIndirect(&lfp);}
n_pen=CreatePen(PS_DOT,1,RGB(0,0,0));
Line(hdc,w.left,w.top,w.left,w.bottom);
Line(hdc,w.right,w.top,w.right,w.bottom);
minf=f.bottom;maxf=f.top;
if(minf>maxf){minf=f.top;maxf=f.bottom;}
dy=(maxf-minf)/4;
i=0;
while(dy>10.||dy<1.){if(dy<1.){dy*=10.;i--;}else {dy/=10.;i++;}}
dy=(int)(dy);
if(dy==3.)dy=2.;
if(dy==4.)dy=5.;
if(dy>7.)dy=10.;
if(dy>5&&dy<=7)dy=5.;
dy*=pow(10,i);
ddy=pow(10,i);
maxf=dy*(int)(maxf/dy +1);
minf=dy*(int)(minf/dy-1);

o_font=SelectObject(hdc,n_font);
SetTextAlign(hdc,TA_CENTER|TA_TOP);
TextOut(hdc,w.right+2,(w.bottom+w.top)/2,name,strlen(name));
if(PRINTPARAM==0)lf.lfEscapement=0;
else lfp.lfEscapement=0;
SelectObject(hdc,o_font);
DeleteObject(n_font);
SetTextAlign(hdc,TA_RIGHT|TA_BASELINE);
for(fy=minf;fy<maxf;fy+=ddy)
{
if(fy<0)fy=(int)(fy/ddy-0.5)*ddy;
else fy=(int)(fy/ddy+0.5)*ddy;
fmd=fmod(fabs(fy),dy);
y=w.bottom-(fy-f.bottom)*ver;
y=(int)(y+0.5);
if(y<=w.top||y>=w.bottom)continue;
sprintf(str,"%G%s",fy,sn);
	if(fmd<ddy/2.||fabs(fmd-dy)<ddy/2.){	Line(hdc,w.left,(int)y,w.left-7,(int)y);
	 TextOut(hdc,w.left-10,(int)y,str,strlen(str));
			if(typ!=0){
					o_pen=SelectObject(hdc,n_pen);
		Line(hdc,w.right,(int)y,w.left,(int)y);
				n_pen=SelectObject(hdc,o_pen);
		}

		}
	 else Line(hdc,w.left,(int)y,w.left-4,(int)y);
	}
	DeleteObject(n_pen);
}
void tim_ax(HDC hdc,char *dts,char *dte,RECT w,char typ)
{
char *mon[]=
{"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
int s[12]={31,28,31,30,31,30,31,31,30,31,30,31};
double wdx=w.right-w.left;
char str[80];
double wdd;
phisw f;
double dd;
double du;
HPEN n_pen,o_pen;
double hor;
int yys,mms,dds,hhs,mis;
long mss;
int mmm;
int yy,mm,dda,hh;
int i;
double x;
int yye,mme,dde,hhe,mie;
int dyy=0,dmm=0,ddd=0;
long mse;
daat ds;
daat de;
daat tt;
ds=cstrtoda(dts);
de=cstrtoda(dte);
wdd=razndd(de,ds);
hor=wdx/wdd;
n_pen=CreatePen(PS_DOT,1,RGB(0,0,0));
sscanf(dts,"%4d%2d%2d%2d%2d%6ld",&yys,&mms,&dds,&hhs,&mis,&mss);
sscanf(dte,"%4d%2d%2d%2d%2d%6ld",&yye,&mme,&dde,&hhe,&mie,&mse);
Line(hdc,w.left,w.top,w.right,w.top);
Line(hdc,w.left,w.bottom,w.right,w.bottom);
dd=wdd/4.;
SetTextAlign(hdc,TA_RIGHT|TA_BOTTOM);
sprintf(str,"%04d.%02d.%02d %02d:%02d:%02ld %ldms",yys,mms,dds,hhs,mis,mss/10000,(int)(mss%10000l)/10);
if(dd<(double)(1./24./60.)){
	 f.left=0.;f.right=wdd*24.*60.*60.;
	 lin_ax(hdc,f,w,str,typ,"S");
	 DeleteObject(n_pen);
	 return;
		  }
if(dd<(double)(1./24.)){
	 f.left=0.;f.right=wdd*24.*60.;
	 lin_ax(hdc,f,w,str,typ,"M");
	 DeleteObject(n_pen);
	 return;
		 }

if(dd<1.){
	 f.left=0.;f.right=wdd*24.;
	 lin_ax(hdc,f,w,str,typ,"H");
	 DeleteObject(n_pen);
	 return;
	 }
TextOut(hdc,w.right,w.top-3,str,strlen(str));
SetTextAlign(hdc,TA_CENTER|TA_TOP);

if(dd/365.>=1.){dyy=1;dd/=365.;}
  else if(dd/30>=1.){dmm=1;dd/=30.;}
		 if(dd>=1.)ddd=1;

i=0;
while(dd>10.||dd<1.){if(dd<1.){dd*=10.;i--;}else {dd/=10.;i++;}}
dd=(int)(dd);
if(dd>6)dd=6;
if(dyy==1&&(dd==6||dd==4))dd=5;
if(dmm==1&&dd==5)dd=4;
du=pow(10,i);
dd*=pow(10,i);
du=pow(10,i-1);
if(du<1.)du=1;
if(dyy)
{
	for(yy=(yys)-yys%(int)dd;yy<=yye;yy+=(int)du)
	{
	sprintf(str,"%04d01010000000000",yy);
	tt=cstrtoda(str);
	x=w.left+razndd(tt,ds)*hor;
	if(x<w.left||x>w.right)continue;
	if((int)(yy/dd)==yy/dd){
	if(typ==1){
			o_pen=SelectObject(hdc,n_pen);
			Line(hdc,(int)x,w.bottom,(int)x,w.top);
			n_pen=SelectObject(hdc,o_pen);
			}

	Line(hdc,(int)x,w.bottom,(int)x,w.bottom+8);
	sprintf(str,"%d",yy);
	TextOut(hdc,(int)x,w.bottom+10,str,strlen(str));
	}
	else Line(hdc,(int)x,w.bottom,(int)x,w.bottom+5);
	if(dd==1)for(mm=1;mm<=12;mm++){
					sprintf(str,"%04d%02d010000000000",yy,mm);
					tt=cstrtoda(str);
					x=w.left+razndd(tt,ds)*hor;
					if(x<w.left||x>w.right)continue;
					Line(hdc,(int)x,w.bottom,(int)x,w.bottom+3);
						}
	}
	DeleteObject(n_pen);
return;
}
if(dmm)
{
 mmm=0;
	for(yy=yys;yy<=yye;yy+=1)
	{
	if(yy%4==0)s[1]=29;
	else s[1]=28;
	for(mm=1;mm<=12;mm++,mmm++){	sprintf(str,"%04d%02d010000000000",yy,mm);
					tt=cstrtoda(str);
					x=w.left+razndd(tt,ds)*hor;
					if(x<w.left||x>w.right)continue;
					if((int)(mmm/dd)==mmm/dd)
					{	if(typ==1){
	o_pen=SelectObject(hdc,n_pen);
	Line(hdc,(int)x,w.bottom,(int)x,w.top);
	n_pen=SelectObject(hdc,o_pen);
	}

					Line(hdc,(int)x,w.bottom,(int)x,w.bottom+8);
					sprintf(str,"%s %d",mon[mm-1],yy);
					TextOut(hdc,(int)x,w.bottom+10,str,strlen(str));
					}
					else
					Line(hdc,(int)x,w.bottom,(int)x,w.bottom+5);
		if(dd==1)for(dda=1;dda<=s[mm-1];dda++)
		{
		sprintf(str,"%04d%02d%02d0000000000",yy,mm,dda);
		tt=cstrtoda(str);
		x=w.left+razndd(tt,ds)*hor;
		if(x<w.left||x>w.right)continue;
		 Line(hdc,(int)x,w.bottom,(int)x,w.bottom+3);
		  }
	}
}
DeleteObject(n_pen);
return;
}
if(ddd)
{
	for(yy=yys;yy<=yye;yy+=1)
	{
	if(yy%4==0)s[1]=29;
	else s[1]=28;
	for(mm=1;mm<=12;mm++)
	for(dda=1;dda<=s[mm-1];dda++){
	sprintf(str,"%04d%02d%02d0000000000",yy,mm,dda);
	tt=cstrtoda(str);
	x=w.left+razndd(tt,ds)*hor;
	if(x<w.left||x>w.right)continue;
	if((int)(dda/dd)==dda/dd){
		if(typ==1){
		o_pen=SelectObject(hdc,n_pen);
		Line(hdc,(int)x,w.bottom,(int)x,w.top);
		n_pen=SelectObject(hdc,o_pen);
		}

					Line(hdc,(int)x,w.bottom,(int)x,w.bottom+8);
					sprintf(str,"%d %s",dda,mon[mm-1]);
					TextOut(hdc,(int)x,w.bottom+10,str,strlen(str));
					}
					else
					Line(hdc,(int)x,w.bottom,(int)x,w.bottom+5);

	  if(dd==1)for(hh=0;hh<24;hh++)
		{
		sprintf(str,"%04d%02d%02d%02d00000000",yy,mm,dda,hh);
		tt=cstrtoda(str);
		x=w.left+razndd(tt,ds)*hor;
		if(x<w.left||x>w.right)continue;
		 Line(hdc,(int)x,w.bottom,(int)x,w.bottom+3);
		  }
		  }
 }
  DeleteObject(n_pen);
return;
}
DeleteObject(n_pen);
 return;
}

int dup_sei(Seismogr *sd,Seismogr ss,int trasse)
{
int i;

//for(i=0;i<Ntrass;i++)
//{
//if(i==trasse)continue;
//if((strcmp(ss.Sta,S[i].Sta)==0)&&
 //  (strcmp(ss.Nt,S[i].Nt)==0)&&
 //  (strcmp(ss.Cha,S[i].Cha)==0)&&
  // (strcmp(ss.Comment,S[i].Comment)==0))return 0;

//}

sd->R.re=(double *)malloc((ss.R.nz+ss.R.np)*sizeof(double));
if(!sd->R.re)return -1;
sd->R.im=(double *)malloc((ss.R.nz+ss.R.np)*sizeof(double));
if(!sd->R.im)return -1;
sd->Data=(double *)malloc(ss.Samp*sizeof(double));
if(!sd->Data)return -1;
strcpy(sd->Sta,ss.Sta);
strcpy(sd->Nt,ss.Nt);
strcpy(sd->Cha,ss.Cha);
strcpy(sd->Comment,ss.Comment);
sd->Stime.day=ss.Stime.day;
sd->Stime.msec=ss.Stime.msec;
sd->Samp=ss.Samp;
sd->Rate=ss.Rate;
sd->Type=ss.Type;
sd->R.a0=ss.R.a0;
sd->R.f0=ss.R.f0;
sd->R.nz=ss.R.nz;
sd->R.np=ss.R.np;
sd->Sens=ss.Sens;
sd->Fsens=ss.Fsens;
sd->Lat=ss.Lat;
sd->Lon=ss.Lon;
sd->Elev=ss.Elev;
sd->Depth=ss.Depth;
sd->Azim=ss.Azim;
sd->angl=ss.angl;
sd->Dip=ss.Dip;
sd->Mid=ss.Mid;
sd->Min=ss.Min;
sd->Max=ss.Max;
for(i=0;i<(ss.R.np+ss.R.nz);i++)
{sd->R.re[i]=ss.R.re[i];
sd->R.im[i]=ss.R.im[i];}
for(i=0;i<ss.Samp;i++)
sd->Data[i]=ss.Data[i];
return 1;

}


int read_sei(Seismogr *s,char *fname,int rmv)
{

FILE *in;
double zero=0.,prev=0.;
int i;
char tmpstr[80];
if((in=fopen(fname,"r"))==NULL)
return 0;
if(fscanf(in,"%s",tmpstr)!=1)return 0;
if(strcmp(tmpstr,"ASL_00")!=0)return 0;
if(fscanf(in,"%s %s %s",s->Sta,s->Nt,s->Cha)!=3)return 0;
strcpy(s->Comment,"ORG");
if(fscanf(in,"%s",tmpstr)!=1)return 0;
s->Stime=cstrtoda(tmpstr);
if(fscanf(in,"%d",&s->Samp)!=1)return 0;
if(fscanf(in,"%lf",&s->Rate)!=1)return 0;
if(fscanf(in,"%s",tmpstr)!=1)return 0;
if(strcmp(tmpstr,"M")==0)s->Type='D';
else if(strcmp(tmpstr,"M/S")==0)s->Type='V';
	else if(strcmp(tmpstr,"M/S**2")==0)s->Type='A';
	     else s->Type='U';
if(fscanf(in,"%lf",&s->R.a0)!=1)return 0;
if(fscanf(in,"%lf",&s->R.f0)!=1)return 0;
if(fscanf(in,"%lf",&s->Sens)!=1)return 0;
if(fscanf(in,"%lf",&s->Fsens)!=1)return 0;
if(fscanf(in,"%d %d",&s->R.nz,&s->R.np)!=2)return 0;
s->R.re=(double *)malloc((s->R.nz+s->R.np)*sizeof(double));
if(!s->R.re)return -1;
s->R.im=(double *)malloc((s->R.nz+s->R.np)*sizeof(double));
if(!s->R.im)return -1;
for(i=0;i<(s->R.np+s->R.nz);i++)
if(fscanf(	in,"%lf %lf",&s->R.re[i],&s->R.im[i])!=2)return 0;
if(fscanf(in,"%lf %lf %lf %lf %lf %lf",
   &s->Lat,&s->Lon,&s->Elev,&s->Depth,&s->angl,&s->Dip)!=6)return 0;

s->Data=(double *)malloc(s->Samp*sizeof(double));
if(!s->Data)return -1;
s->Mid=0;
for(i=0;i<s->Samp;i++)
{
if(fscanf(in,"%lf",&s->Data[i])!=1)break;
/********************
if(i==0)zero=s->Data[i];
else prev=s->Data[i];
s->Data[i]-=zero;
zero=prev;
********************/
s->Mid+=s->Data[i];
}
s->Samp=i;



if(s->Samp<=0)return 0;
s->Mid/=s->Samp;
s->Max=s->Min=s->Data[0];
for(i=1;i<s->Samp;i++)
{
if(s->Max<s->Data[i])s->Max=s->Data[i];
if(s->Min>s->Data[i])s->Min=s->Data[i];
}
if(s->Max==s->Min)return 0;

if(rmv)
{
for(i=0;i<s->Samp;i++)s->Data[i]-=s->Mid;
s->Min-=s->Mid;
s->Max-=s->Mid;
}

fclose(in);

return 1;
}

long getnum_sei(Seismogr s,daat t)
{
long num;
double msec;
msec=raznd(t,s.Stime);
num=(int)(msec/s.Rate+0.5);
return num;
}

double get_max(Seismogr s,daat tmin,daat tmax)
{
int i;
double max=0.;
int start;int end;
start=getnum_sei(s,tmin);
end=getnum_sei(s,tmax);
if(start>=s.Samp)start=s.Samp-1;
if(end>=s.Samp)end=s.Samp-1;

if(start<0)start=0;
if(end<0)end=0;

max=s.Data[start];
for(i=start;i<=end;i++)
{if(max<s.Data[i])max=s.Data[i];}
return max;
}
double get_min(Seismogr s,daat tmin,daat tmax)
{
int i;
double min=0.;
int start;int end;
start=getnum_sei(s,tmin);
end=getnum_sei(s,tmax);
if(start<0)start=0;
if(end<0)end=0;
if(start>=s.Samp)start=s.Samp-1;
if(end>=s.Samp)end=s.Samp-1;
min=s.Data[start];
for(i=start;i<=end;i++)
{if(min>s.Data[i])min=s.Data[i];}
return min;
}

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau);
void jacobi(double a[3][3],int n,double d[],double v[3][3],int *nrot)
{
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c;

	double b[3];
	double z[3];
	for (ip=0;ip<n;ip++) {
		for (iq=0;iq<n;iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	}
	for (ip=0;ip<n;ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0;
	}
	*nrot=0;
	for (i=0;i<50;i++) {
		sm=0.0;
		for (ip=0;ip<n-1;ip++) {
			for (iq=ip+1;iq<n;iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0) {
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=0;ip<n-1;ip++) {
			for (iq=ip+1;iq<n;iq++) {
				g=100.0*fabs(a[ip][iq]);
				if (i > 4 && fabs(d[ip])+g == fabs(d[ip])
					&& fabs(d[iq])+g == fabs(d[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if (fabs(h)+g == fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5*h/(a[ip][iq]);
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=0;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=0;j<n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++(*nrot);
				}
			}
		}
		for (ip=0;ip<n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0;
		}
	}
//	printf("Too many iterations in routine JACOBI");
}

#undef ROTATE

sort_pol( const void *a ,const void *b)
{
polarization *a1,*b1;
a1=(polarization *)a;
b1=(polarization *)b;
if(a1->LAMDA<b1->LAMDA)return 1;
else if(a1->LAMDA>b1->LAMDA)return -1;
else return 0;
}

void polariz(Seismogr s1,Seismogr s2,Seismogr s3, daat tmin,daat tmax,int rev1,int rev2,int rev3)
{

double angle,iangle;
int nrot;
long start1,end1,start2,start3;//,end2;
int i,j,k,n;
double jc[3][3];
double ev[3];
double evv[3][3];
double xc=0.,yc=0.,zc=0.;
double sx=0.,sy=0.,sz=0.;
start1=getnum_sei(s1,tmin);
end1=getnum_sei(s1,tmax);
start2=getnum_sei(s2,tmin);
start3=getnum_sei(s3,tmin);
n=0;
for(i=(int)start1,j=(int)start2,k=(int)start3;i<=(int)end1;i++,j++,k++)
{
if(i<0||j<0||k<0)continue;
if(i>=s1.Samp||j>=s2.Samp||k>=s3.Samp)break;
xc+=s1.Data[i]*rev1/s1.Sens;
yc+=s2.Data[j]*rev2/s2.Sens;
zc+=s3.Data[k]*rev3/s3.Sens;
n++;
}
xc/=n;
yc/=n;
zc/=n;
for(i=0;i<3;i++)
for(j=0;j<3;j++)jc[i][j]=0.;

for(i=(int)start1,j=(int)start2,k=(int)start3;i<=(int)end1;i++,j++,k++)
{
if(i<0||j<0||k<0)continue;
if(i>=s1.Samp||j>=s2.Samp||k>=s3.Samp)break;
sx=(s1.Data[i]*rev1/s1.Sens-xc);
sy=(s2.Data[j]*rev2/s2.Sens-yc);
sz=(s3.Data[k]*rev3/s3.Sens-zc);
jc[0][0]+=sx*sx/n;jc[1][0]+=sx*sy/n;jc[0][1]=jc[1][0];
jc[2][0]+=sx*sz/n;jc[0][2]=jc[2][0];
jc[1][1]+=sy*sy/n;jc[2][1]+=sy*sz/n;jc[1][2]=jc[2][1];
jc[2][2]+=sz*sz/n;
}
jacobi(jc,3,ev, evv,&nrot);

for(i=0;i<3;i++)
{if(evv[2][i]>0){evv[0][i]=-evv[0][i];evv[1][i]=-evv[1][i];evv[2][i]=-evv[2][i];}
angle=atan2(evv[1][i],evv[0][i])*180./M_PI;
angle=90-angle;
while(angle<0)angle+=360;
iangle=acos(-evv[2][i])*180./M_PI;
POLAR[i].LAMDA=ev[i];
POLAR[i].STRIKE=angle;
POLAR[i].DIP=iangle;
}
qsort((void *)POLAR,3,sizeof(polarization),sort_pol);

}

void draw_m_axes(HDC hdc,RECT r)
{
int i,j;
double perc;
POINT p;
char string[80];
RECT recc[12];
Rectangle(hdc,r.left,r.top,r.right,r.bottom);
sprintf(string,"Main Axes [current str=%g dip=%g]",SStrike,DDip);
SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);
TextOut(hdc,(r.left+r.right)/2,r.top-5,string,strlen(string));
for(i=0;i<3;i++)
for(j=0;j<4;j++)
{
recc[j*3+i].left=r.left+i*(r.right-r.left)/3;
recc[j*3+i].right=r.left+(i+1)*(r.right-r.left)/3;
recc[j*3+i].top=r.top+j*(r.bottom-r.top)/4;
recc[j*3+i].bottom=r.top+(j+1)*(r.bottom-r.top)/4;

}

MoveToEx(hdc,recc[0].left,recc[0].bottom,&p);
LineTo(hdc,recc[2].right,recc[2].bottom);
MoveToEx(hdc,recc[3].left,recc[3].bottom,&p);
LineTo(hdc,recc[5].right,recc[5].bottom);
MoveToEx(hdc,recc[6].left,recc[6].bottom,&p);
LineTo(hdc,recc[8].right,recc[8].bottom);
MoveToEx(hdc,recc[1].left,recc[1].top,&p);
LineTo(hdc,recc[9].right,recc[9].bottom);
MoveToEx(hdc,recc[2].left,recc[2].top,&p);
LineTo(hdc,recc[10].right,recc[10].bottom);
//perc=POLAR[0].LAMDA*POLAR[0].LAMDA+POLAR[1].LAMDA*POLAR[1].LAMDA+POLAR[2].LAMDA*POLAR[2].LAMDA;
perc=POLAR[0].LAMDA+POLAR[1].LAMDA+POLAR[2].LAMDA;
perc=100./perc;
for(i=0;i<3;i++)
{sprintf(string,"Eg.Vec.%d",i);
TextOut(hdc,(recc[i].left+recc[i].right)/2,(recc[i].top+recc[i].bottom)/2+5,string,strlen(string));
sprintf(string,"%4.1lf%c",POLAR[i].LAMDA*perc,0x25);
TextOut(hdc,(recc[i+3].left+recc[i+3].right)/2,(recc[i+3].top+recc[i+3].bottom)/2+5,string,strlen(string));
sprintf(string,"STR=%5.1lf",POLAR[i].STRIKE);
TextOut(hdc,(recc[i+6].left+recc[i+6].right)/2,(recc[i+6].top+recc[i+6].bottom)/2+5,string,strlen(string));
sprintf(string,"DIP=%5.1lf",POLAR[i].DIP);
TextOut(hdc,(recc[i+9].left+recc[i+9].right)/2,(recc[i+9].top+recc[i+9].bottom)/2+5,string,strlen(string));
}
}

/*daat get_tor(void)
{
daat tor;
double rts=0.,rts1=0.;
double rt0=0.;
int i,j;
TOR.day=Tming.day;
TOR.msec=Tming.msec;

for(i=0;i<Narriv;i++)
for(j=0;j<Narriv;j++)
{
if(strcmp(ST[i].Sta,ST[j].Sta)==0&&strcmp(ST[i].Nt,ST[j].Nt)==0&&
   strcmp(ST[i].Wtype,"P")==0&&strcmp(ST[j].Wtype,"S")==0)

{
rts=raznd(ST[i].Tarrive,Tming);
if(rts<=rts1||rts1==0.)
{rt0=(raznd(ST[i].Tarrive,Tming)*VPVS-raznd(ST[j].Tarrive,Tming))/(VPVS-1.);
	rts1=rts;}
}
}

tor=daatadsec(Tming,rt0);
return tor;
}*/


int draw_vadat(HDC hdc,RECT r)
{
char buf2[80];
int flag=0;
POINT p;
char buf1[80];
HBRUSH hbrn,hbro;
char tmpstr[80];
phisw f;
int i,j;
daat t0;
daat mint,maxt;
double x,y;
double xc=0.,yc=0.;
daat tc;
int kc=0;

double mints=0.,maxts=0.,rts;
mint.day=Tmaxg.day;maxt.day=Tming.day;
mint.msec=Tmaxg.msec;maxt.msec=Tming.msec;
for(i=0;i<Narriv;i++)
for(j=0;j<Narriv;j++)
{
if(strcmp(ST[i].Sta,ST[j].Sta)==0&&strcmp(ST[i].Nt,ST[j].Nt)==0&&
   strcmp(ST[i].Wtype,"P")==0&&strcmp(ST[j].Wtype,"S")==0)

{
flag=1;
rts=raznd(ST[j].Tarrive,ST[i].Tarrive);
if(mints==0.)mints=rts;
if(mints>rts)mints=rts;
if(maxts<rts)maxts=rts;
if(raznd(mint,ST[i].Tarrive)>0){mint.day=ST[i].Tarrive.day;
								mint.msec=ST[i].Tarrive.msec;}
if(raznd(maxt,ST[i].Tarrive)<0){maxt.day=ST[i].Tarrive.day;
								maxt.msec=ST[i].Tarrive.msec;}
}
}

if(!flag)return 0;
mint=daatadsec(mint,-10000.);
maxt=daatadsec(maxt,10000.);
if(raznd(mint,maxt)>=0.)return 0;
mints/=1000.;
maxts/=1000.;
mints-=10;
maxts+=10;
f.top=maxts;
f.bottom=mints;
cdatostr(mint,buf1,0);
cdatostr(maxt,buf2,0);
if(hdc!=NULL){tim_ax(hdc,buf1,buf2,r,0);
lin_ay(hdc,f,r,"Time[S-P]",0,"s");
hbrn=GetStockObject(DKGRAY_BRUSH);
hbro=SelectObject(hdc,hbrn);
SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);}
for(i=0;i<Narriv;i++)
for(j=0;j<Narriv;j++)
{
if(strcmp(ST[i].Sta,ST[j].Sta)==0&&strcmp(ST[i].Nt,ST[j].Nt)==0&&
   strcmp(ST[i].Wtype,"P")==0&&strcmp(ST[j].Wtype,"S")==0)

{
rts=raznd(ST[j].Tarrive,ST[i].Tarrive)/1000;
y=r.bottom-(rts-mints)/(maxts-mints)*(r.bottom-r.top);
yc+=rts;
kc+=1;
xc+=raznd(ST[i].Tarrive,Tming);
x=r.left+raznd(ST[i].Tarrive,mint)/raznd(maxt,mint)*(r.right-r.left);
if(hdc!=NULL){Ellipse(hdc,(int)x-5-PRINTPARAM*10,(int)y-5-PRINTPARAM*10,(int)x+5+PRINTPARAM*10,(int)y+5+PRINTPARAM*10);
sprintf(tmpstr,"%s %s",ST[i].Sta,ST[i].Nt);
TextOut(hdc,(int)x,(int)y-8-PRINTPARAM*10,tmpstr,strlen(tmpstr));}
}
}
yc/=kc;
xc/=kc;
tc=daatadsec(Tming,xc);
t0=daatadsec(tc,-(yc-mints)/(VPVS-1.)*1000.);
x=r.left+raznd(t0,mint)/raznd(maxt,mint)*(r.right-r.left);
y=r.bottom-(0.)/(maxts-mints)*(r.bottom-r.top);
if(hdc!=NULL)MoveToEx(hdc,(int)x,(int)y,&p);
t0=daatadsec(tc,-(yc-maxts)/(VPVS-1.)*1000.);
x=r.left+raznd(t0,mint)/raznd(maxt,mint)*(r.right-r.left);
y=r.bottom-(maxts-mints)/(maxts-mints)*(r.bottom-r.top);
if(hdc!=NULL)LineTo(hdc,(int)x,(int)y);
TOR=daatadsec(tc,-yc/(VPVS-1.)*1000.);
if(hdc!=NULL){
SetTextAlign(hdc,TA_LEFT|TA_TOP);
sprintf(tmpstr,"Origin Time Of Source=%s",cdatostr(TOR,buf2,1));
TextOut(hdc,r.left+5,r.top+5,tmpstr,strlen(tmpstr));}
if(hdc!=NULL)SelectObject(hdc,hbro);
return 1;
}



double get_dist(char *stn,char *nt,double depth)
{
double dist,del;
char tmpstr[100];
int i;
double rsp=0.;
double rspp,rspp1=0.;
sprintf(tmpstr,"%s\\aspi91.bin",DIRNAME);


for(i=0;i<Narriv;i++)
if(strcmp(stn,ST[i].Sta)==0&&strcmp(nt,ST[i].Nt)==0&&strcmp(ST[i].Wtype,"P")==0)
{
rsp=raznd(ST[i].Tarrive,TOR)/1000.;break;
}

if(rsp<=0.)return 0.;
del=0.;
for(dist=0.;dist<=180.;dist+=2)
{
rspp=get_t_time(tmpstr,"P",dist,depth);
if(rsp<rspp){del=(dist-2)+(2)/(rspp-rspp1)*(rsp-rspp1);break;}
rspp1=rspp;
}
if(del<0)return 0.;
return del;
}




int draw_sei(HDC hdc,Seismogr s, daat tmin,daat tmax,
RECT w,int counts,int scale,double min,double max,int color)
{
POINT P;
SIZE leng;
char units[6];
daat tpr;
HFONT n_font,o_font;
HRGN rgn;
RECT r;
double dist;
long start,end;
int i,kn,knn,l;
double x,y;
HPEN h_pen,o_pen;
char tmpstr[80];
char buf1[20];
char buf2[20];
char buf3[20];
phisw f;
GetClipBox(hdc,&r);
rgn=CreateRectRgn(w.left,w.top,w.right,w.bottom);
if(scale==0||scale==2){f.bottom=min*SCFACTOR;
			  f.top=max*SCFACTOR;}
if(scale==1){f.bottom=get_min(s,tmin,tmax)*SCFACTOR;
			f.top=get_max(s,tmin,tmax)*SCFACTOR;
			if(!counts){f.bottom/=s.Sens;
						f.top/=s.Sens;}}
if(w.left>=w.right)return -1;
if(w.top>=w.bottom)return -1;
//if(f.left>=f.right)return -1;
if(f.top<=f.bottom)return -1;
SetTextAlign(hdc,TA_LEFT|TA_TOP);
//if(SIGG)
TextOut(hdc,w.left+5,w.top+5,s.Comment,strlen(s.Comment));
cdatostr(tmin,buf1,0);
cdatostr(tmax,buf2,0);
sprintf(buf3,"%s %s %s",s.Sta,s.Nt,s.Cha);
Line(hdc,w.left,w.top,w.right,w.top);
Line(hdc,w.left,w.bottom,w.right,w.bottom);
if(s.Type=='D')sprintf(units,"M");
else if(s.Type=='V')sprintf(units,"M/S");
		else if(s.Type=='A')sprintf(units,"M/S^2");
				else sprintf(units,"");
if(counts)sprintf(units,"C");
tim_ax(hdc,buf1,buf2,w,0);
lin_ay(hdc,f,w,buf3,0,units);

if(color==0)h_pen=CreatePen(PS_SOLID,1,RGB(0,0,0));
else if(color==1)h_pen=CreatePen(PS_SOLID,1,RGB(255,0,0));
else if(color==2)h_pen=CreatePen(PS_SOLID,1,RGB(0,255,0));
else if(color==3)h_pen=CreatePen(PS_SOLID,1,RGB(0,0,255));
else if(color==4)h_pen=CreatePen(PS_SOLID,1,RGB(255,255,0));
else if(color==5)h_pen=CreatePen(PS_SOLID,1,RGB(255,0,255));
else if(color==6)h_pen=CreatePen(PS_SOLID,1,RGB(0,255,255));
else if(color==7)h_pen=CreatePen(PS_SOLID,1,RGB(127,0,0));
else if(color==8)h_pen=CreatePen(PS_SOLID,1,RGB(0,127,0));
else if(color==9)h_pen=CreatePen(PS_SOLID,1,RGB(0,0,127));
else if(color==10)h_pen=CreatePen(PS_SOLID,1,RGB(127,127,0));
else if(color==11)h_pen=CreatePen(PS_SOLID,1,RGB(127,0,127));
else if(color==12)h_pen=CreatePen(PS_SOLID,1,RGB(0,127,127));
else h_pen=CreatePen(PS_SOLID,1,RGB(127,127,127));


o_pen=SelectObject(hdc,h_pen);
start=getnum_sei(s,tmin);
end=getnum_sei(s,tmax);
if(start<0)start=0;
if(end<0)end=0;
if(start>=s.Samp)start=s.Samp-1;
if(end>=s.Samp)end=s.Samp-1;
SelectClipRgn(hdc,rgn);
for(i=start;i<=end;i++)
{
if(counts)
y=w.bottom-(s.Data[i]-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
else 
y=w.bottom-(s.Data[i]/s.Sens-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
x=w.left+(raznd(s.Stime,tmin)+i*s.Rate)/raznd(tmax,tmin)*(w.right-w.left);
if(i==start)MoveToEx(hdc,(int)x,(int)y,&P);
else LineTo(hdc,(int)x,(int)y);
}


SelectObject(hdc,o_pen);
GetTextExtentPoint32(hdc,"W",1,&leng);
for(i=0;i<Narriv;i++)
{
if((strcmp(ST[i].Sta,s.Sta)==0)&&(strcmp(ST[i].Nt,s.Nt)==0))
{
kn=getnum_sei(s,ST[i].Tarrive);
if(kn<0||kn>=s.Samp)continue;
y=w.bottom-(s.Data[kn]/s.Sens-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
x=w.left+(raznd(ST[i].Tarrive,tmin))/raznd(tmax,tmin)*(w.right-w.left);
if(x<w.left||x>w.right)continue;
MoveToEx(hdc,(int)x,(int)y,&P);
LineTo(hdc,(int)x,(int)y-leng.cx*2);
MoveToEx(hdc,(int)x,(int)y,&P);
LineTo(hdc,(int)x-2,(int)y-5);
MoveToEx(hdc,(int)x,(int)y,&P);
LineTo(hdc,(int)x+2,(int)y-5);
TextOut(hdc,(int)x,(int)y,ST[i].Wtype,strlen(ST[i].Wtype));
}
}
sprintf(tmpstr,"%s\\aspi91.bin",DIRNAME);
dist=get_dist(s.Sta,s.Nt,DEP);
if(PRINTPARAM==0)
{lf.lfEscapement=900;
n_font=CreateFontIndirect(&lf);}
else 
{lfp.lfEscapement=900;
n_font=CreateFontIndirect(&lfp);}
o_font=SelectObject(hdc,n_font);
SetTextAlign(hdc,TA_RIGHT|TA_TOP);

if(TOR.day>0&&dist>0)
for(i=0;i<86;i++)
{
if(PREDAC[i]<=0)continue;
tpr=daatadsec(TOR,get_t_time(tmpstr,&PREDPH[i*10],dist,DEP)*1000);
if(raznd(tpr,TOR)<0.)continue;
y=w.top;
x=w.left+(raznd(tpr,tmin))/raznd(tmax,tmin)*(w.right-w.left);
if(x<w.left||x>w.right)continue;
MoveToEx(hdc,(int)x,(int)y,&P);
LineTo(hdc,(int)x,(int)(y+(-w.top+w.bottom)/3));
TextOut(hdc,(int)x,w.top,&PREDPH[i*10],strlen(&PREDPH[i*10]));
}

if(PRINTPARAM==0)lf.lfEscapement=0;
else lfp.lfEscapement=0;
SelectObject(hdc,o_font);
DeleteObject(n_font);


for(i=0;i<Nampli;i++)
{
if((strcmp(SA[i].Sta,s.Sta)==0)&&(strcmp(SA[i].Nt,s.Nt)==0)
   &&(strcmp(SA[i].Cha,s.Cha)==0)&&(strcmp(SA[i].Comment,s.Comment)==0))
{
kn=getnum_sei(s,SA[i].Tampli);
if(SA[i].period<=0)continue;
if(kn<0||kn>=s.Samp)continue;
knn=(int)(SA[i].period*1000./s.Rate);
if(SA[i].period<=0)continue;
for(l=kn;l<=knn+kn;l++)
{
y=w.bottom-(s.Data[l]/s.Sens-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
x=w.left+(raznd(SA[i].Tampli,tmin)+(l-kn)*s.Rate)/raznd(tmax,tmin)*(w.right-w.left);
MoveToEx(hdc,(int)x,(int)y,&P);
LineTo(hdc,(int)x,(int)(w.bottom-(-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top)));}

}
}


DeleteObject(h_pen);
DeleteObject(rgn);
rgn=CreateRectRgn(r.left,r.top,r.right,r.bottom);
SelectClipRgn(hdc,rgn);
DeleteObject(rgn);
return 0;
}



#define M_PI    3.14159265358979323846
#define BIG	1e10
#define ABS(x) ((x) >= 0 ? (x) : -(x))


/* SPBWCF     11/13/85 */
/* GENERATES KTH SECTION COEFFICIENTS FOR LTH ORDER NORMALIZED */
/*       LOWPASS BUTTERWORTH FILTER */
/* SECOND ORDER SECTIONS: K<=(L+1)/2 */
/* ODD ORDER L:  FINAL SECTION WILL CONTAIN 1ST ORDER POLE */
/* LN DEFINES COEFFICIENT ARRAY SIZE */
/* ANALOG COEFFICIENTS ARE RETURNED IN D AND C */
/* IERROR=0      NO ERRORS DETECTED */
/*        1      INVALID FILTER ORDER L */
/*        2      INVALID SECTION NUMBER K */

double spbfct(long*, long*);
void spfilt(double *, double *, long, long,Seismogr *, double *, double *);
void spbiln(double *, double *, long *, double *, double *, double *, long *);
void spbwcf(long *, long *, long *, double *, double *, long *);
double pow_ri(double *, long *);
void spcflt(double *, double *, long *, long *, double *, long *, double *, double *, long *);
void spfblt(double *, double *, long *, long *, double *, double *, double *, double *, double *, long *);
void spbwcf(long *l, long *k, long *ln, double *d, double *c, long *error)

{
    /* Local variables */
    long i;
    double tmp;

    if (*l <= 0)
    {
	*error = 1;
	return;
    }

    if (*k <= 0 || *k > ((*l + 1) / 2))
    {
	*error = 2;
	return;
    }

    d[0] = 1.0;
    c[0] = 1.0;
    for (i = 1 ; i <= *ln ; ++i)
    {
	d[i] = 0.0;
	c[i] = 0.0;
    }

    tmp = (double) *k - (*l + 1.0) / 2.0;

    if (tmp == 0.0)
    {
	c[1] = 1.0;
    }
    else
    {
        c[1] = (double) (-2.0 * cos((double) ((*k * 2) + *l - 1)
				   * M_PI / (double) (*l * 2)));
	c[2] = 1.0;
    }

    *error = 0;
    return;
} /* spbwcf */




/* SPFBLT     10/26/90 */
/* CONVERTS NORMALIZED LP ANALOG H(S) TO DIGITAL H(Z) */
/*       ANALOG TRANSFER FUNCTION           DIGITAL TRANSFER FUNCTION */
/*        D(M)*S**M+.....+D(0)               B(0)+.....+B(L)*Z**-L */
/*   H(S)=--------------------          H(Z)=-------------------- */
/*        C(M)*S**M+.....+C(0)                 1+......+A(L)*Z**-L */
/* FILTER ORDER L IS COMPUTED INTERNALLY */
/* IBAND=1    LOWPASS            FLN=NORMALIZED CUTOFF IN HZ-SEC */
/*       2    HIGHPASS           FLN=NORMALIZED CUTOFF IN HZ-SEC */
/*       3    BANDPASS           FLN=LOW CUTOFF; FHN=HIGH CUTOFF */
/*       4    BANDSTOP           FLN=LOW CUTOFF; FHN=HIGH CUTOFF */
/* LN SPECIFIES COEFFICIENT ARRAY SIZE */
/* WORK(0:LN,0:LN) IS A WORK ARRAY USED INTERNALLY */
/* RETURN IERROR=0    NO ERRORS DETECTED */
/*               1    ALL ZERO TRANSFER FUNCTION */
/*               2    SPBILN: INVALID TRANSFER FUNCTION */
/*               3    FILTER ORDER EXCEEDS ARRAY SIZE */
/*               4    INVALID FILTER TYPE PARAMETER (IBAND) */
/*               5    INVALID CUTOFF FREQUENCY */


void spfblt(double *d, double *c, long *ln, long *iband, double *fln, double *fhn, double *b, double *a, double *work, long *error)
{
      
    long work_dim1, tmp_int, i, k, l, m, zero_func, ll, mm, ls;
    double tmpc, tmpd, w, w1, w2, w02, tmp;

    if (*iband < 1 || *iband > 4)
    {
	*error = 4;
	return;
    }
    if ((*fln <= 0.0 || *fln > 0.5) ||
	(*iband >= 3 && ( *fln >= *fhn || *fhn > 0.5 )))
    {
	*error = 5;
	return;
    }

    zero_func = TRUE;
    for (i = *ln ; i >= 0 && zero_func ; --i)
    {
	if (c[i] != 0.0 || d[i] != 0.0)
	{
	    zero_func = FALSE;
	}
    }

    if ( zero_func ) 
    {
        *error = 1;
        return;
    }

    work_dim1 = *ln + 1;

    m = i + 1;
    w1 = (double) tan(M_PI * *fln);
    l = m;
    if (*iband > 2)
    {
        l = m * 2;
        w2 = (double) tan(M_PI * *fhn);
        w = w2 - w1;
        w02 = w1 * w2;
    }

    if (l > *ln)
    {
	*error = 3;
	return;
    }

    switch (*iband)
    {
	case 1:

/* SCALING S/W1 FOR LOWPASS,HIGHPASS */

	    for (mm = 0 ; mm <= m ; ++mm)
	    {
		d[mm] /= pow_ri(&w1, &mm);
		c[mm] /= pow_ri(&w1, &mm);
	    }

	    break;

	case 2:

/* SUBSTITUTION OF 1/S TO GENERATE HIGHPASS (HP,BS) */

	    for (mm = 0 ; mm <= (m / 2) ; ++mm)
	    {
		tmp = d[mm];
		d[mm] = d[m - mm];
		d[m - mm] = tmp;
		tmp = c[mm];
		c[mm] = c[m - mm];
		c[m - mm] = tmp;
	    }

/* SCALING S/W1 FOR LOWPASS,HIGHPASS */

	    for (mm = 0 ; mm <= m ; ++mm)
	    {
		d[mm] /= pow_ri(&w1, &mm);
		c[mm] /= pow_ri(&w1, &mm);
	    }

	    break;

	case 3:

/* SUBSTITUTION OF (S**2+W0**2)/(W*S)  BANDPASS,BANDSTOP */

	    for (ll = 0 ; ll <= l ; ++ll)
	    {
		work[ll] = 0.0;
		work[ll + work_dim1] = 0.0;
	    }

	    for (mm = 0 ; mm <= m ; ++mm)
	    {
		tmp_int = m - mm;
		tmpd = d[mm] * pow_ri(&w, &tmp_int);
		tmpc = c[mm] * pow_ri(&w, &tmp_int);

		for (k = 0 ; k <= mm ; ++k)
		{
		    ls = m + mm - (k * 2);
		    tmp_int = mm - k;
		    tmp = spbfct(&mm,&mm) / (spbfct(&k,&k)
					     * spbfct(&tmp_int,&tmp_int));
		    work[ls] += tmpd * pow_ri(&w02, &k) * tmp;
		    work[ls + work_dim1] += tmpc * pow_ri(&w02, &k) * tmp;
		}
	    }

	    for (ll = 0 ; ll <= l ; ++ll)
	    {
		d[ll] = work[ll];
		c[ll] = work[ll + work_dim1];
	    }

	    break;

	case 4:

/* SUBSTITUTION OF 1/S TO GENERATE HIGHPASS (HP,BS) */

	    for (mm = 0 ; mm <= (m / 2) ; ++mm)
	    {
		tmp = d[mm];
		d[mm] = d[m - mm];
		d[m - mm] = tmp;
		tmp = c[mm];
		c[mm] = c[m - mm];
		c[m - mm] = tmp;
	    }

/* SUBSTITUTION OF (S**2+W0**2)/(W*S)  BANDPASS,BANDSTOP */

	    for (ll = 0 ; ll <= l ; ++ll)
	    {
		work[ll] = 0.0;
		work[ll + work_dim1] = 0.0;
	    }

	    for (mm = 0 ; mm <= m ; ++mm)
	    {
		tmp_int = m - mm;
		tmpd = d[mm] * pow_ri(&w, &tmp_int);
		tmpc = c[mm] * pow_ri(&w, &tmp_int);

		for (k = 0 ; k <= mm ; ++k)
		{
		    ls = m + mm - (k * 2);
		    tmp_int = mm - k;
		    tmp = spbfct(&mm,&mm) / (spbfct(&k,&k)
					     * spbfct(&tmp_int,&tmp_int));
		    work[ls] += tmpd * pow_ri(&w02, &k) * tmp;
		    work[ls + work_dim1] += tmpc * pow_ri(&w02, &k) * tmp;
		}
	    }

	    for (ll = 0 ; ll <= l ; ++ll)
	    {
		d[ll] = work[ll];
		c[ll] = work[ll + work_dim1];
	    }

	    break;

    }

    spbiln(d, c, ln, b, a, work, error);

    return;
} /* spfblt */



/* SPBILN     11/13/85 */
/* CONVERTS ANALOG H(S) TO DIGITAL H(Z) VIA BILINEAR TRANSFORM */
/*      ANALOG TRANSFER FUNCTION         DIGITAL TRANSFER FUNCTION */
/*          D(L)*S**L+.....+D(0)             B(0)+......+B(L)*Z**-L */
/*    H(S)=---------------------        H(Z)=---------------------- */
/*          C(L)*S**L+.....+C(0)               1+.......+A(L)*Z**-L */
/* H(S) IS ASSUMED TO BE PRE-SCALED AND PRE-WARPED */
/* LN SPECIFIES THE LENGTH OF THE COEFFICIENT ARRAYS */
/* FILTER ORDER L IS COMPUTED INTERNALLY */
/* WORK IS AN INTERNAL ARRAY (2D) SIZED TO MATCH COEF ARRAYS */
/* IERROR=0    NO ERRORS DETECTED IN TRANSFORMATION */
/*        1    ALL ZERO TRANSFER FUNCTION */
/*        2    INVALID TRANSFER FUNCTION; Y(K) COEF=0 */


void spbiln(double *d, double *c, long *ln, double *b, double *a, double *work, long *error)
{
    /* Local variables */
    long i, j, l, zero_func, work_dim1;
    double scale, tmp;
    double atmp;

    zero_func = TRUE;
    for (i = *ln ; i >= 0 && zero_func ; --i)
    {
	if (c[i] != 0.0 || d[i] != 0.0)
	{
	    zero_func = FALSE;
	}
    }

    if ( zero_func )
    {
        *error = 1;
        return;
    }

    work_dim1 = *ln + 1;

    l = i + 1;
    for (j = 0 ; j <= l ; ++j)
    {
	work[j * work_dim1] = 1.0;
    }

    tmp = 1.0;
    for (i = 1 ; i <= l ; ++i)
    {
        tmp = tmp * (double) (l - i + 1) / (double) i;
	work[i] = tmp;
    }

    for (i = 1 ; i <= l ; ++i)
    {
	for (j = 1 ; j <= l ; ++j)
	{
	    work[i + j * work_dim1] = work[i + (j - 1) * work_dim1]
				      - work[i - 1 + j * work_dim1]
				      - work[i - 1 + (j - 1) * work_dim1];
	}
    }

    for (i = l ; i >= 0 ; --i)
    {
	b[i] = 0.0;
	atmp = 0.0;

	for (j = 0 ; j <= l ; ++j)
	{
	    b[i] += work[i + j * work_dim1] * d[j];
	    atmp += (double) work[i + j * work_dim1] * c[j];
	}

	scale = (double) atmp;

	if (i != 0)
	{
	    a[i - 1] = (double) atmp;
	}
    }

    if (scale == 0.0)
    {
	*error = 2;
	return;
    }

    b[0] /= scale;
    for (i = 1 ; i <= l ; ++i)
    {
	b[i] /= scale;
	a[i - 1] /= scale;
    }

    if (l < *ln)
    {
	for (i = l + 1 ; i <= *ln ; ++i)
	{
	    b[i] = 0.0;
	    a[i - 1] = 0.0;
	}
    }

    *error = 0;
    return;
} /* spbiln */




/* SPBFCT     11/14/85 */
/* GENERATES (I1)!/(I1-I2)!=I1*(I1-1)*...*(I1-I2+1). */
/* NOTE: 0!=1 AND SPBFCT(I,I)=SPBFCT(I,I-1)=I!. */


double spbfct(long *i1, long *i2)
{
    /* Local variables */
    long i;
    double ret_val;

    ret_val = 0.0;
    if (*i1 < 0 || *i2 < 0 || *i2 > *i1)
    {
	return(ret_val);
    }

    ret_val = 1.0;
    for (i = *i1 ; i >= (*i1 - *i2 + 1) ; --i)
    {
	ret_val *= (double) i;
    }

    return(ret_val);
} /* spbfct */



double pow_ri(double *ap, long *bp)

{
    double pow, x;
    long n;

    pow = 1;
    x = *ap;
    n = *bp;

    if(n != 0)
    {
	if(n < 0)
	{
	    if(x == 0)
	    {
		return(pow);
	    }
	    n = -n;
	    x = 1/x;
	}

	for( ; ; )
	{
	    if(n & 01)
		pow *= x;

	    if(n >>= 1)
		x *= x;
	    else
		break;
	}
    }

    return(pow);
}


/* SPCFLT     11/13/85 */
/* FILTERS N-POINT DATA SEQUENCE IN PLACE USING ARRAY X */
/* TRANSFER FUNCTION IS COMPOSED OF NS SECTIONS IN CASCADE WITH */
/*       MTH STAGE TRANSFER FUNCTION */
/*            B(0,M)+B(1,M)*Z**(-1)+......+B(LS,M)*Z**(-LS) */
/*     H(Z) = ------------------------------------------- */
/*               1+A(1,M)*Z**(-1)+.......+A(LS,M)*Z**(-LS) */
/* PX RETAINS PAST VALUES OF INPUT X */
/* PY RETAINS PAST VALUES OF OUTPUT Y */
/* IERROR=0    NO ERRORS DETECTED */
/*     1 - NS  OUTPUT AT STAGE [IERROR] EXCEEDS 1.E10 */


void spcflt(double *b, double *a, long *ls, long *ns, double *x, long *n, double *px, double *py, long *error)

{
    /* Local variables */
    long k, m, ll, b_dim1, a_dim1, px_dim1, py_dim1;

    b_dim1 = *ls + 1;
    a_dim1 = *ls;
    px_dim1 = *ls + 1;
    py_dim1 = *ls;

    for (m = 0 ; m < *ns ; ++m)
    {
	for (k = 0 ; k < *n ; ++k)
	{
	    px[m * px_dim1] = x[k];
	    x[k] = b[m * b_dim1] * px[m * px_dim1];

	    for (ll = 1 ; ll <= *ls ; ++ll)
	    {
		x[k] = x[k] + b[ll + m * b_dim1] * px[ll + m * px_dim1]
		       - a[ll - 1 + m * a_dim1] * py[ll - 1 + m * py_dim1];
	    }

	    if (ABS(x[k]) > BIG)
	    {
		*error = m;
		return;
	    }

	    for (ll = *ls ; ll >= 2 ; --ll)
	    {
		px[ll + m * px_dim1] = px[ll - 1 + m * px_dim1];
		py[ll - 1 + m * py_dim1] = py[ll - 2 + m * py_dim1];
	    }

	    px[m * px_dim1 + 1] = px[m * px_dim1];
	    py[m * py_dim1] = x[k];
	}
    }

    *error = 0;
    return;
} /* spcflt */

/* SPFILT     11/13/85 */
/* FILTERS N-POINT DATA SEQUENCE IN PLACE USING ARRAY X */
/* TRANSFER FUNCTION COEFFICIENTS ARE IN ARRAYS B AND A */
/*            B(0)+B(1)*Z**(-1)+.......+B(LB)*Z**(-LB) */
/*     H(Z) = ---------------------------------------- */
/*              1+A(1)*Z**(-1)+.......+A(LA)*Z**(-LA) */
/* PX SAVES PAST VALUES OF INPUT X */
/* PY SAVES PAST VALUES OF OUTPUT Y */
/* IERROR=0    NO ERRORS DETECTED */
/*        1    FILTER RESPONSE EXCEEDS 1.E10 */


void spfilt(double *b, double *a, long lb, long la, Seismogr *s, double *px, double *py)

{
    /* Local variables */
    long k, l;
for(k=0;k<=lb;k++)px[k]=0.0;
for(k=0;k<la;k++)py[k]=0.0;
	
    for (k = 0 ; k <(s->Samp) ;k++)
    {
	px[0] = s->Data[k];
	s->Data[k] = 0.0;

	for (l = 0 ; l <= lb ;l++)
	{
	    s->Data[k] += b[l] * px[l];
	}

	for (l = 0 ; l < la ;l++)
	{
	    s->Data[k] -= a[l] * py[l];
	}

	
	for (l = lb ; l >= 1 ;l--)
	{
	    px[l] = px[l - 1];
	}

	for (l = la - 1 ; l >= 1 ;l--)
	{
	    py[l] = py[l - 1];
	}

	py[0] = s->Data[k];
    }

    } /* spfilt */




/*long filter_disign(int iband,double rate,double fl,double fh,int order,double *a,double *b)
{
long error;
double *c,*d,*work;
double fln=fl*rate/1000.;
double fhn=fh*rate/1000.;
long l,ln,k;
k=1;
l=ln=order;
c=(double *)malloc((order+1)*sizeof(double));
if(!c){return -10;}
d=(double *)malloc((order+1)*sizeof(double));
if(!d){free(c);return -10;}
work=(double *)malloc((order+1)*(order+1)*sizeof(double));
if(!work){free(c);free(d);return -10;}
spbwcf(&l,&k,&ln,d,c,&error);

if(error!=0)
{
free(c);free(d);free(work);
return -error;
}
spfblt(d,c,&ln,&iband,&fln,&fhn,b,a,work,&error);
if(error!=0){
free(c);free(d);free(work);
return -error;
			}

free(d);free(c);free(work);
return error;

}*/
int recalc( double *A,double*B,double *a,double *b,int l,int m,int k)
{
int i,j;
double *c,*d;
c=malloc((l+1)*sizeof(double));
if(!c)return -1;
d=malloc((l+1)*sizeof(double));
if(!d){free((void*)c);return -1;}
for(i=0;i<=l;i++)c[i]=d[i]=0;
if(!k)for(i=0;i<=m;i++){c[i]=a[i];d[i]=b[i];}
else 
{	for(i=0;i<=l;i++)
	for(j=0;j<=m;j++)
	{
		if(i+j<=l)
		{
		d[i+j]+=B[i]*b[j];
		c[i+j]+=A[i]*a[j];
		}
	}

}
for(i=0;i<=l;i++){A[i]=c[i];B[i]=d[i];}
free((void*)c);
free((void*)d);
return 0;
}

long filter_disign(int iband,double rate,double fl,double fh,int order,double *a,double *b)
{
long error;
double A[5],B[5];
double c[5],d[5],work[25];
double fln=fl*rate/1000.;
double fhn=fh*rate/1000.;
long ln,k,i;
if(iband>2){k=(order*2)/4+1;ln=4;}
else {ln=2;k=(order)/2+1;} 
if(order%2==0)k-=1;
//ln=2;k=3;order=5;iband=2;

for(i=0;i<(k*ln);i++)a[i]=b[i]=0;
for(i=1;i<=k;i++)
{
A[0]=1.;  
spbwcf(&order,&i,&ln,d,c,&error);
if(error)return -1;
spfblt(d,c,&ln,&iband,&fln,&fhn,B,A+1,work,&error);
if(error)return -1;
//printf("%lf %lf %lf %lf %lf\n%lf %lf %lf %lf\n",
//	   B[0],B[1],B[2],B[3],B[4],
//	   A[1],A[2],A[3],A[4]);
if(recalc(a,b,A,B,(ln)*k,ln,i-1)<0)return -1;
}
return 0;
}



int draw_spectr(HDC hdc,Spectr sp,RECT w,double amin,double amax,double fmin,double fmax,int color,int scale,int polzer)
{
POINT P;
double ff,df;
HRGN rgn;
RECT r;
int i;
double x,y;
HPEN h_pen,o_pen;
char buf2[20];
char buf3[40];
phisw f;
if(sp.n==0)return -1;
GetClipBox(hdc,&r);
rgn=CreateRectRgn(w.left,w.top,w.right,w.bottom);
if(scale==0||scale==2){f.bottom=amin;
	    f.top=amax;
	    f.left=fmin;
	    f.right=fmax;}
if(scale==1){
	    f.bottom=sp.min;
	    f.top=sp.max;
	    f.left=sp.f1;
	    f.right=sp.f2;
		//if(f.top<-80)f.top=-80;
		//if(f.bottom>-190)f.bottom=-190;
	    }

//if(f.bottom<=0)f.bottom=f.top/10000.;
if(w.left>=w.right)return -1;
if(w.top>=w.bottom)return -1;

if(f.left>=f.right)return -1;
if(f.top<=f.bottom)return -1;
if(polzer==0)
{
if(sp.s.Type=='V'||sp.s.Type=='D'||sp.s.Type=='A')
sprintf(buf3,"%s %s %s",sp.s.Sta,sp.s.Nt,sp.s.Cha);
else sprintf(buf3,"%s %s %s",sp.s.Sta,sp.s.Nt,sp.s.Cha);
}
else if(sp.s.Type=='V')sprintf(buf3,"%s %s %s (Velocity)",sp.s.Sta,sp.s.Nt,sp.s.Cha);
	 else if(sp.s.Type=='D')sprintf(buf3,"%s %s %s (Displacement)",sp.s.Sta,sp.s.Nt,sp.s.Cha);
		else if(sp.s.Type=='A')sprintf(buf3,"%s %s %s (Accseleration)",sp.s.Sta,sp.s.Nt,sp.s.Cha);
		 else sprintf(buf3,"%s %s %s",sp.s.Sta,sp.s.Nt,sp.s.Cha);
sprintf(buf2,"Frequency (Hz)");
Line(hdc,w.left,w.top,w.right,w.top);
Line(hdc,w.left,w.bottom,w.right,w.bottom);
lin_ay(hdc,f,w,buf3,0,"Db");
log_ax(hdc,f,w,buf2,0);
if(color==0)h_pen=CreatePen(PS_SOLID,1,RGB(0,0,0));
else if(color==1)h_pen=CreatePen(PS_SOLID,1,RGB(255,0,0));
else if(color==2)h_pen=CreatePen(PS_SOLID,1,RGB(0,255,0));
else if(color==3)h_pen=CreatePen(PS_SOLID,1,RGB(0,0,255));
else if(color==4)h_pen=CreatePen(PS_SOLID,1,RGB(255,255,0));
else if(color==5)h_pen=CreatePen(PS_SOLID,1,RGB(255,0,255));
else if(color==6)h_pen=CreatePen(PS_SOLID,1,RGB(0,255,255));
else if(color==7)h_pen=CreatePen(PS_SOLID,1,RGB(127,0,0));
else if(color==8)h_pen=CreatePen(PS_SOLID,1,RGB(0,127,0));
else if(color==9)h_pen=CreatePen(PS_SOLID,1,RGB(0,0,127));
else if(color==10)h_pen=CreatePen(PS_SOLID,1,RGB(127,127,0));
else if(color==11)h_pen=CreatePen(PS_SOLID,1,RGB(127,0,127));
else if(color==12)h_pen=CreatePen(PS_SOLID,1,RGB(0,127,127));
else h_pen=CreatePen(PS_SOLID,1,RGB(127,127,127));
o_pen=SelectObject(hdc,h_pen);
SelectClipRgn(hdc,rgn);
for(i=0;i<sp.n;i++)
{
	if(sp.spdata[i]!=0)
y=w.bottom-(sp.spdata[i]-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
	else y=w.bottom;
x=w.left+log10((double)(i+1)/sp.n/2./sp.rate/f.left)/log10(f.right/f.left)*(w.right-w.left);
if(i<=1)MoveToEx(hdc,(int)x,(int)y,&P);
else LineTo(hdc,(int)x,(int)y);
}





SelectObject(hdc,o_pen);
DeleteObject(h_pen);

h_pen=CreatePen(PS_SOLID,3,RGB(0,0,0));
o_pen=SelectObject(hdc,h_pen);

df=0.001;
for(ff=0.0001;ff<10.;ff+=df)
{
	if(sp.spdata[i]!=0)
y=w.bottom-(sublnm(1./ff,2)-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
	else y=w.bottom;
x=w.left+log10(ff/f.left)/log10(f.right/f.left)*(w.right-w.left);
if(ff==0.0001)MoveToEx(hdc,(int)x,(int)y,&P);
else LineTo(hdc,(int)x,(int)y);
if(ff>0.01)df=0.01;
}

df=0.001;
for(ff=0.0001;ff<10.;ff+=df)
{
	if(sp.spdata[i]!=0)
y=w.bottom-(subhnm(1./ff,2)-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
	else y=w.bottom;
x=w.left+log10(ff/f.left)/log10(f.right/f.left)*(w.right-w.left);
if(ff==0.0001)MoveToEx(hdc,(int)x,(int)y,&P);
else LineTo(hdc,(int)x,(int)y);
if(ff>0.01)df=0.01;
}

SelectObject(hdc,o_pen);
DeleteObject(h_pen);

DeleteObject(rgn);
rgn=CreateRectRgn(r.left,r.top,r.right,r.bottom);
SelectClipRgn(hdc,rgn);
DeleteObject(rgn);
return 0;
} 


draw_motion(HDC hdc,Seismogr s1,Seismogr s2, daat tmin,daat tmax,RECT w,double a,double ia,int tip,int rev1,int rev2)
{
POINT P;
double coef;
long start1,end1,start2;//,end2;
int i,j,k;
//int lch;
double x,y;
double xc=0,yc=0;
HPEN o_pen,h_pen;
char buf1[40];
char buf2[40];
//char buf3[20];
phisw f;


			f.top=fabs(get_max(s2,tmin,tmax)*rev2);
			f.bottom=fabs(get_min(s2,tmin,tmax)*rev2);
			if(f.top<f.bottom)f.top=f.bottom;
			f.bottom=-f.top;
			f.bottom/=s2.Sens;
			f.top/=s2.Sens;

            f.right=fabs(get_max(s1,tmin,tmax)*rev1);
			f.left=fabs(get_min(s1,tmin,tmax)*rev1);
			if(f.right<f.left)f.right=f.left;
			f.left=-f.right;
			f.left/=s1.Sens;
			f.right/=s1.Sens;
			if(f.right>f.top){f.top=f.right;f.bottom=-f.right;}
			else {f.right=f.top;f.left=-f.right;}
			//if(f.left>f.bottom)f.left=f.bottom;else f.bottom=f.left;
			//if(f.right<f.top)f.right=f.top;else f.top=f.right;
coef=(double)(w.right-w.left)/(w.bottom-w.top);
if(coef>1.){f.right=f.top*coef;f.left=-f.right;}
else {f.top=f.right/coef;f.bottom=-f.top;}


sprintf(buf1,"[-]  %s %s  [+]",s1.Sta,s1.Cha);
sprintf(buf2,"[-]  %s %s  [+]",s2.Sta,s2.Cha);
lin_ay(hdc,f,w,buf2,1,"");
lin_ax(hdc,f,w,buf1,1,"");
h_pen=CreatePen(PS_SOLID,2,RGB(0,0,0));
o_pen=SelectObject(hdc,h_pen);

start1=getnum_sei(s1,tmin);
end1=getnum_sei(s1,tmax);
start2=getnum_sei(s2,tmin);

k=0;
for(i=(int)start1,j=(int)start2;i<=(int)end1;i++,j++)
{
if(i<0||j<0)continue;
if(i>=s1.Samp||j>=s2.Samp)break;
x=w.left+(s1.Data[i]*rev1/s1.Sens-f.left)/(f.right-f.left)*(w.right-w.left);
y= w.bottom-(s2.Data[j]*rev2/s2.Sens-f.bottom)/(f.top-f.bottom)*(w.bottom-w.top);
if(i==start1||j==start2||i==0||j==0)MoveToEx(hdc,(int)x,(int)y,&P);
else LineTo(hdc,(int)x,(int)y);
xc+=x;
yc+=y;
k+=1;
}


xc/=k;
yc/=k;



if(tip==1){ 
			SelectObject(hdc,o_pen);
			DeleteObject(h_pen);
			h_pen=CreatePen(PS_SOLID,1,RGB(255,0,0));
			o_pen=SelectObject(hdc,h_pen);
			y=yc+(w.bottom-w.top)/2.*cos(M_PI*a/180.);
			x=xc-(w.bottom-w.top)/2.*sin(M_PI*a/180.);
			MoveToEx(hdc,(int)xc,(int)yc,&P);
			LineTo(hdc,(int)x,(int)y);
			SelectObject(hdc,o_pen);
			DeleteObject(h_pen);
			h_pen=CreatePen(PS_SOLID,1,RGB(0,0,255));
			o_pen=SelectObject(hdc,h_pen);
			y=yc-(w.bottom-w.top)/2.*cos(M_PI*a/180.);
			x=xc+(w.bottom-w.top)/2.*sin(M_PI*a/180.);
			MoveToEx(hdc,(int)xc,(int)yc,&P);
			LineTo(hdc,(int)x,(int)y);
			
}

if(tip==2){ 
			SelectObject(hdc,o_pen);
			DeleteObject(h_pen);
			h_pen=CreatePen(PS_SOLID,1,RGB(255,0,0));
			o_pen=SelectObject(hdc,h_pen);
			y=yc+(w.bottom-w.top)/2.*cos(M_PI*ia/180.);
			x=xc+(w.bottom-w.top)/2.*sin(M_PI*ia/180.)*sin(M_PI*a/180.);
	        MoveToEx(hdc,(int)xc,(int)yc,&P);
			LineTo(hdc,(int)x,(int)y);
			SelectObject(hdc,o_pen);
			DeleteObject(h_pen);
			h_pen=CreatePen(PS_SOLID,1,RGB(0,0,255));
			o_pen=SelectObject(hdc,h_pen);
			y=yc-(w.bottom-w.top)/2.*cos(M_PI*ia/180.);
			x=xc-(w.bottom-w.top)/2.*sin(M_PI*ia/180.)*sin(M_PI*a/180.);
			MoveToEx(hdc,(int)xc,(int)yc,&P);
			LineTo(hdc,(int)x,(int)y);
			
			}

if(tip==3){ SelectObject(hdc,o_pen);
			DeleteObject(h_pen);
			h_pen=CreatePen(PS_SOLID,1,RGB(255,0,0));
			o_pen=SelectObject(hdc,h_pen);
			y=yc+(w.bottom-w.top)/2.*cos(M_PI*ia/180.);
			x=xc+(w.bottom-w.top)/2.*sin(M_PI*ia/180.)*cos(M_PI*a/180.);
			MoveToEx(hdc,(int)xc,(int)yc,&P);
			LineTo(hdc,(int)x,(int)y);
			SelectObject(hdc,o_pen);
			DeleteObject(h_pen);
			h_pen=CreatePen(PS_SOLID,1,RGB(0,0,255));
			o_pen=SelectObject(hdc,h_pen);
			y=yc-(w.bottom-w.top)/2.*cos(M_PI*ia/180.);
			x=xc-(w.bottom-w.top)/2.*sin(M_PI*ia/180.)*cos(M_PI*a/180.);
			MoveToEx(hdc,(int)xc,(int)yc,&P);
			LineTo(hdc,(int)x,(int)y);
			
			}

SelectObject(hdc,o_pen);
DeleteObject(h_pen);

return 0;

}



void rot_sei(daat tmin,daat tmax,Seismogr *s1,Seismogr *s2,Seismogr *ss1,Seismogr *ss2,double angle)
{
int i,j;
double ang;
double cang,sang;
int start1,start2;
int end1,end2;
ang=angle*M_PI/180.;
start1=getnum_sei(*ss1,tmin);
start2=getnum_sei(*ss2,tmin);
end1=getnum_sei(*ss1,tmax);
end2=getnum_sei(*ss2,tmax);
cang=cos(ang);
sang=sin(ang);
for(i=start1,j=start2;i<=end1&&j<end2;i++,j++)
{
if(j<0||i<0)continue;
if(j>=ss2->Samp||i>=ss1->Samp)break;
s1->Data[i]=ss1->Data[i]*cang+ss2->Data[j]*sang/ss2->Sens*ss1->Sens;
s2->Data[j]=-ss1->Data[i]*sang/ss1->Sens*ss2->Sens+ss2->Data[j]*cang;
}
s1->Cha[2]='R';
s2->Cha[2]='T';
s1->angl+=angle;
s1->angl+=angle;
sprintf(s1->Comment,"%s [R=%g]",ss1->Comment,angle);
sprintf(s2->Comment,"%s [R=%g]",ss2->Comment,angle);
}


void cor_sei(daat tmin,daat tmax,Seismogr *s1,Seismogr *s2)
{
int i,j;
double ang;
double x,y;
double cang,sang;
int start1,start2;
int end1,end2;
double angle=-(s1->angl);
ang=angle*M_PI/180.;
start1=getnum_sei(*s1,tmin);
start2=getnum_sei(*s2,tmin);
end1=getnum_sei(*s1,tmax);
end2=getnum_sei(*s2,tmax);
cang=cos(ang);
sang=sin(ang);
for(i=start1,j=start2;i<=end1&&j<end2;i++,j++)
{
if(j<0||i<0)continue;
if(j>=s2->Samp||i>=s1->Samp)break;
x=s1->Data[i];
y=s2->Data[j];
s1->Data[i]=x*cang+y*sang/s2->Sens*s1->Sens;
s2->Data[j]=-x*sang/s1->Sens*s2->Sens+y*cang;
}
s1->angl=0;
s2->angl=90;
s1->Cha[2]='N';
s2->Cha[2]='E';
}



