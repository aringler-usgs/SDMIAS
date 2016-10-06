#include <math.h>
#include <stdio.h>
typedef struct {long day;double msec;}daat;
double raznd(daat d1,daat d2)
{
double d;
double ms;
d=d1.day-d2.day;
ms=d1.msec-d2.msec;
d*=86400000.;
d+=ms;
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
if(!flag)sprintf(str,"%04d%02d%02d%02d%02d%06ld",yy,mm,dd,hh,mi,(long)(msec*10));
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
