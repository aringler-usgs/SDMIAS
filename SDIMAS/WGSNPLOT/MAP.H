double calc_mb_ch(char *sta,char *nt,char *cha,char *comment,char *mag)
{
double mb=0.;
double at;
//double amp;
//double dist;
int i;
	for(i=0;i<Nampli;i++)
	{
	if(strcmp(sta,SA[i].Sta)==0&&strcmp(nt,SA[i].Nt)==0&&strcmp(cha,SA[i].Cha)==0&&strcmp(comment,SA[i].Comment)==0)
	{
	
		if(strcmp(SA[i].Wtype,mag)==0&&SA[i].amplit>0&&SA[i].period>0)
	{if(SA[i].Type=='V')at=SA[i].amplit/2/M_PI*1000000.;
		else if(SA[i].Type=='D')at=SA[i].amplit/SA[i].period*1000000.*2.*M_PI;
		else continue;
		mb=log10(at)+2.3 *log10(get_dist(SA[i].Sta,SA[i].Nt,DEP)*M_PI/180.*6371.)-2;
		
	}

	}
	}
mb=(int)(mb*10)/10.;
	return mb;


}



double calc_mb(char *mag)
{
	double mpv=0.;
	double at;
	//double dist;
	int k=0;
	int i;
	for(i=0;i<Nampli;i++)
	{
	if(strcmp(SA[i].Wtype,mag)==0&&SA[i].amplit>0&&SA[i].period>0)
	{if(SA[i].Type=='V')at=SA[i].amplit/2/M_PI*1000000.;
		else if(SA[i].Type=='D')at=SA[i].amplit/SA[i].period*1000000.*2.*M_PI;
		else continue;
		mpv+=log10(at)+2.3 *log10(get_dist(SA[i].Sta,SA[i].Nt,DEP)*M_PI/180.*6371.)-2;
		k+=1;
	}
	}

	if(k<=0)mpv=0.;
	else mpv/=k;
	return mpv;
}

double calc_ms_ch(char *sta,char *nt,char *cha,char *comment,char *mag)
{
double ms=0.;
double at;
//double amp;
//double dist;
int i;
	for(i=0;i<Nampli;i++)
	{
	if(strcmp(sta,SA[i].Sta)==0&&strcmp(nt,SA[i].Nt)==0&&strcmp(cha,SA[i].Cha)==0&&strcmp(comment,SA[i].Comment)==0)
	{
	if(strcmp(SA[i].Wtype,mag)==0&&SA[i].amplit>0&&SA[i].period>0)
	{if(SA[i].Type=='V')at=SA[i].amplit/2./M_PI*1000000.;
		else if(SA[i].Type=='D')at=SA[i].amplit/SA[i].period*1000000.;
		else continue;
		ms=log10(at)+1.66 *log10(get_dist(SA[i].Sta,SA[i].Nt,DEP))+3.3;
	
	}
	}
	}

	ms=(int)(ms*10)/10.;
	return ms;


}




double calc_ms(char *mag)
{
	double msh=0.;
	double at;
	//double dist;
	int k=0;
	int i;
	for(i=0;i<Nampli;i++)
	{

	if(strcmp(SA[i].Wtype,mag)==0&&SA[i].amplit>0&&SA[i].period>0)
	{if(SA[i].Type=='V')at=SA[i].amplit/2./M_PI*1000000.;
		else if(SA[i].Type=='D')at=SA[i].amplit/SA[i].period*1000000.;
		else continue;
		msh+=log10(at)+1.66 *log10(get_dist(SA[i].Sta,SA[i].Nt,DEP))+3.3;
		k+=1;
	}
	}

	if(k<=0)msh=0.;
	else msh/=k;
	return msh;
}

double calc_ml_ch(char *sta,char *nt,char *cha,char *comment,char *mag)
{
double ml=0.;
double at;
double amp;
double dist;
int i;
	for(i=0;i<Nampli;i++)
	{
	if(strcmp(sta,SA[i].Sta)==0&&strcmp(nt,SA[i].Nt)==0&&strcmp(cha,SA[i].Cha)==0&&strcmp(comment,SA[i].Comment)==0)
	{dist=get_dist(SA[i].Sta,SA[i].Nt,DEP);
	amp=SA[i].amplit*1000000.;
	if(strcmp(SA[i].Wtype,mag)==0&&SA[i].amplit>0.&&SA[i].period>0.&&dist>0.)
	if(SA[i].Type=='D'){at=amp;
	  ml=log10(at)+2.56 *log10(dist/180.*M_PI*6371.)-1.67;
	}

	}
	}
ml=(int)(ml*10)/10.;
	return ml;


}

double calc_ml(char *mag)
{
	double ml=0.;
	double at;
	double amp;
	double dist;
	int k=0;
	int i;
	for(i=0;i<Nampli;i++)
	{
	dist=get_dist(SA[i].Sta,SA[i].Nt,DEP);
	amp=SA[i].amplit*1000000.;
	if(strcmp(SA[i].Wtype,mag)==0&&SA[i].amplit>0.&&SA[i].period>0.&&dist>0.)
	{ if(SA[i].Type=='D'){at=amp;
	  ml+=log10(at)+2.56 *log10(dist/180.*M_PI*6371.)-1.67;
	  k+=1;}

	}
	}

	if(k<=0)ml=0.;
	else ml/=k;
	return ml;
}




int condek (double fic,double lamc,double alf,double fi,double lam,double *x,double *y)
{
double grad=M_PI/180.;
double x1,x2,x3,y1,y2,y3,z1,z2,z3;
double R=6371.;
fic*=grad;
lamc*=grad;
alf*=grad;
fi*=grad;
lam*=grad;
x1=R*cos(fi)*cos(lam);
y1=R*cos(fi)*sin(lam);
z1=R*sin(fi);
x2=x1*cos(lamc)+y1*sin(lamc);
y2=-x1*sin(lamc)+y1*cos(lamc);
z2=z1;
x3=x2*sin(fic)-z2*cos(fic);
z3=x2*cos(fic)+z2*sin(fic);
y3=y2;
*x=x3*cos(alf)-y3*sin(alf);
*y=x3*sin(alf)+y3*cos(alf);
x3=*x;
*x=*y;
*y=-x3;
if(z3 <=0.)return -1;
else return 0;
}

double cal_azim(double fi1,double lam1,double fi2,double lam2)
{
	double x,y,azim;

condek(fi1,lam1,0,fi2,lam2,&x,&y);
azim=90-atan2(y,x)*180./M_PI;
while(azim<0)azim+=360.;
return azim;
}

void filam(double f,double l,double dist,double aza,double *fi,double *lam)
{
double grad=M_PI/180.;
double RZ=6371.;
double x2,y2,z2;
double x1,y1,z1;
double x,y,z;
aza*=grad;
f*=grad;
l*=grad;
//dist=M_PI/4.*RZ;
dist/=RZ;
x=cos(f)*cos(l);
y=cos(f)*sin(l);
z=sin(f);
x1=x*cos(l)+y*sin(l);
y1=-x*sin(l)+y*cos(l);
z1=z;
x2=-z1*cos(f)+x1*sin(f);
y2=y1;
z2=z1*sin(f)+x1*cos(f);
aza=-M_PI-aza;

x2=sin(dist)*cos(aza);
y2=sin(dist)*sin(aza);
z2=cos(dist);
x1=-(-z2*cos(-f)+x2*sin(-f));
y1=y2;
z1=-(z2*sin(-f)+x2*cos(-f));
x=x1*cos(-l)+y1*sin(-l);
y=-x1*sin(-l)+y1*cos(-l);
z=z1;
*fi=asin(z)/grad;
*lam=atan2(y,x)/grad;

}

int get_source(double *fic,double *lamc)
{
int i;
double dist;
double mindist=1000000000.;
double aza;
double fi,lam;
for(i=0;i<Ntrass;i++)
{
dist=get_dist(S[i].Sta,S[i].Nt,DEP);
if(S[i].Azim!=0&&dist<mindist&&dist>0.){mindist=dist;aza=S[i].Azim;fi=S[i].Lat;lam=S[i].Lon;}
}

if(mindist!=1000000000.)
filam(fi,lam,mindist*M_PI/180.*6371,aza,fic,lamc);
return 0;
}



int draw_circle(HDC hdc,double fic,double lamc,RECT r,double mas,Seismogr s)
{
int i;
double dist,d;
POINT p;
double fi,lam;
double par;
double x,y;
HPEN s_pen,o_pen;
s_pen=CreatePen(PS_SOLID,2,RGB(0,0,255));
o_pen=SelectObject(hdc,s_pen);
dist=get_dist(s.Sta,s.Nt,DEP)/180.*M_PI*6371;
if(dist>0.)
for(i=0;i<360;i++)
{
filam(s.Lat,s.Lon,dist,(double)i,&fi,&lam);
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par<0||i==0){MoveToEx(hdc,(int)x,(int)y,&p);continue;}
else LineTo(hdc,(int)x,(int)y);
}
if(dist<=0.)dist=100;
if(s.Azim>0.)
for(d=0.;d<=dist*1.1;d+=dist/20.)
{filam(s.Lat,s.Lon,d,s.Azim,&fi,&lam);
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par<0||d==0.)
{
MoveToEx(hdc,(int)x,(int)y,&p);continue;}
else LineTo(hdc,(int)x,(int)y);
}




SelectObject(hdc,o_pen);
DeleteObject(s_pen);
s_pen=CreatePen(PS_SOLID,2,RGB(255,0,0));
o_pen=SelectObject(hdc,s_pen);

if(s.Azim>0.)
for(d=0.;d<=dist*1.1;d+=dist/20.)
{filam(s.Lat,s.Lon,d,s.Azim+180.,&fi,&lam);
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par<0||d==0.)
{
MoveToEx(hdc,(int)x,(int)y,&p);continue;}
else LineTo(hdc,(int)x,(int)y);
}


SelectObject(hdc,o_pen);
DeleteObject(s_pen);

return 0;
}





int Draw_map(HDC hdc,double mas,double fic,double lamc,RECT r)
{
int i;
HRGN rgn;
int num;
int pup=0;
POINT p,pp[3];
HPEN o_pen,g_pen,b_pen;
HBRUSH o_br,h_br;
FILE *in;
char tmpstr[80];
char header[100];
short int *filam;
double fi,lam;
int dfi=10;
int dlam=10;
double x,y;
int par;

rgn=CreateRectRgn(r.left,r.top,r.right,r.bottom);


g_pen=CreatePen(PS_SOLID,1,RGB(0,127,0));
o_pen=SelectObject(hdc,g_pen);
SelectClipRgn(hdc,rgn);

sprintf(tmpstr,"%s\\coasts.bin",DIRNAME);
in=fopen(tmpstr,"rb");
if(!in)return 0;
num=_filelength(_fileno(in));
filam=(short int*)malloc(num);
if(!filam)return 0;
num/=2;
fread(filam,num,2,in);
for(i=0;i<num;i+=2)
{
if(filam[i]==10000){pup=0;continue;}
fi=filam[i]/60.;
lam=filam[i+1]/60.;
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par<0){pup=0;continue;}
if(!pup)MoveToEx(hdc,(int)x,(int)y,&p);
else LineTo(hdc,(int)x,(int)y);
pup=1;
}

free((void *)filam);
fclose(in);
SelectObject(hdc,o_pen);
DeleteObject(g_pen);
b_pen=CreatePen(PS_SOLID,1,RGB(0,0,255));
o_pen=SelectObject(hdc,b_pen);
sprintf(tmpstr,"%s\\rivers.bin",DIRNAME);
in=fopen(tmpstr,"rb");
if(!in)return 0;
num=_filelength(_fileno(in));
filam=(short int*)malloc(num);
if(!filam)return 0;
num/=2;
fread(filam,num,2,in);
for(i=0;i<num;i+=2)
{
if(filam[i]==10000){pup=0;continue;}
fi=filam[i]/60.;
lam=filam[i+1]/60.;
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par<0){pup=0;continue;}
if(!pup)MoveToEx(hdc,(int)x,(int)y,&p);
else LineTo(hdc,(int)x,(int)y);
pup=1;
}

free((void *)filam);
fclose(in);


SelectObject(hdc,o_pen);
DeleteObject(b_pen);
/******************/
b_pen=CreatePen(PS_SOLID,2,RGB(255,0,255));
o_pen=SelectObject(hdc,b_pen);
sprintf(tmpstr,"%s\\plates.bin",DIRNAME);
in=fopen(tmpstr,"rb");
if(!in)return 0;
num=_filelength(_fileno(in));
filam=(short int*)malloc(num);
if(!filam)return 0;
num/=2;
fread(filam,num,2,in);
for(i=0;i<num;i+=2)
{
if(filam[i]==10000){pup=0;continue;}
fi=filam[i]/60.;
lam=filam[i+1]/60.;
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par<0){pup=0;continue;}
if(!pup)MoveToEx(hdc,(int)x,(int)y,&p);
else LineTo(hdc,(int)x,(int)y);
pup=1;
}

free((void *)filam);
fclose(in);


SelectObject(hdc,o_pen);
DeleteObject(b_pen);


/******************/


for(fi=-90.;fi<=90;fi+=dfi)
for(lam=-180.;lam<=180;lam+=dlam/4)
{
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par <0){MoveToEx(hdc,(int)x,(int)y,&p);continue;}
if(lam==-180.)MoveToEx(hdc,(int)x,(int)y,&p);
else LineTo(hdc,(int)x,(int)y);
}

for(lam=-180.;lam<180;lam+=dlam)
for(fi=-90.;fi<=90;fi+=dfi/4.)
{
par=condek(fic,lamc,0,fi,lam,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
if(par <0){MoveToEx(hdc,(int)x,(int)y,&p);continue;}
if(fi==-90.)MoveToEx(hdc,(int)x,(int)y,&p);
else LineTo(hdc,(int)x,(int)y);

}

Ellipse(hdc,(r.left+r.right)/2-10-PRINTPARAM*10,(r.bottom+r.top)/2-10-PRINTPARAM*10,(r.left+r.right)/2+10+PRINTPARAM*10,(r.bottom+r.top)/2+10+PRINTPARAM*10);
MoveToEx(hdc,(r.left+r.right)/2-10-PRINTPARAM*10,(r.bottom+r.top)/2,&p);
LineTo(hdc,(r.left+r.right)/2-5-PRINTPARAM*5,(r.bottom+r.top)/2);
MoveToEx(hdc,(r.left+r.right)/2+10+PRINTPARAM*10,(r.bottom+r.top)/2,&p);
LineTo(hdc,(r.left+r.right)/2+5+PRINTPARAM*5,(r.bottom+r.top)/2);
MoveToEx(hdc,(r.left+r.right)/2,(r.bottom+r.top)/2+10+PRINTPARAM*10,&p);
LineTo(hdc,(r.left+r.right)/2,(r.bottom+r.top)/2+5+PRINTPARAM*5);
MoveToEx(hdc,(r.left+r.right)/2,(r.bottom+r.top)/2-10-PRINTPARAM*10,&p);
LineTo(hdc,(r.left+r.right)/2,(r.bottom+r.top)/2-5-PRINTPARAM*5);

h_br=CreateSolidBrush(RGB(255,0,0));
o_br=SelectObject(hdc,h_br);

for(i=0;i<Ntrass;i++)
{
draw_circle(hdc,fic,lamc,r,mas,S[i]);
par=condek(fic,lamc,0,S[i].Lat,S[i].Lon,&x,&y);
x=x/mas+(r.right-r.left)/2+r.left;
y=-y/mas+(r.bottom-r.top)/2+r.top;
pp[0].x=(int)x-5-PRINTPARAM*10;
pp[0].y=(int)y+3+PRINTPARAM*5;
pp[1].x=(int)x+5+PRINTPARAM*10;
pp[1].y=(int)y+3+PRINTPARAM*5;
pp[2].x=(int)x;
pp[2].y=(int)y-5-PRINTPARAM*10;
if(par>=0)Polygon(hdc,pp,3);
}
SelectObject(hdc,o_br);
DeleteObject(h_br);


SelectClipRgn(hdc,NULL);
SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);
sprintf(header,"Time=%s LATTITUDE=%6.2lf LONGTITUDE=%7.2lf DEPTH=%g",cdatostr(TOR,tmpstr,1),FIC,LAMC,DEP);
TextOut(hdc,(r.right+r.left)/2,r.top-5-PRINTPARAM*10,header,strlen(header));
SetTextAlign(hdc,TA_CENTER|TA_TOP);
sprintf(header,"MPV=%3.1f MPH=%3.1f MSH=%3.1f MSV=%3.1f MLRV=%3.1f MLRH=%3.1f MLQ=%3.1f MWA=%3.1f",
calc_mb("MPV"),calc_mb("MPH"),calc_mb("MSH"),calc_mb("MSV"),calc_ms("MLRV"),calc_ms("MLRH"),calc_ms("MLQ"),calc_ml("MWA"));
TextOut(hdc,(r.right+r.left)/2,r.bottom+5+PRINTPARAM*10,header,strlen(header));
DeleteObject(rgn);
	return 1;
}



