void taper(double *fr,int n,int perc)


{
	 int nz,m;
	 double x;

	 nz = (int)(n*perc/100.);
	 x = M_PI/2/nz;
	 for (m = 0; m < nz; m++)
		fr[m]=fr[m]*(float)sin(m*x);
	 for (m = n-nz; m < n; m++)
		fr[m]=fr[m]*(float)sin((n-m)*x);
}



struct _complex response(int nz,int np,double freq,complex z[],complex p[])
{
int i;
struct _complex rsp={1.,0.};
struct _complex jomg;
struct _complex c;
jomg.x=0;
jomg.y=2*M_PI*freq;

for(i=0;i<nz;i++)
{
csub(c,jomg,z[i]);
cmult(rsp,rsp,c);
}

for(i=0;i<np;i++)
{
csub(c,jomg,p[i]);
cdiv(rsp,rsp,c);
}
return rsp;
}



double normalf(Seismogr s,double freq )
{
struct _complex p;
int i;
struct _complex z;
struct _complex rsp={1.,0.};
struct _complex jomg;
struct _complex c;
jomg.x=0;
jomg.y=2*M_PI*freq;
for(i=0;i<s.R.nz;i++)
{
z.x=s.R.re[i];
z.y=s.R.im[i];
csub(c,jomg,z);
cmult(rsp,rsp,c);
}
for(i=0;i<s.R.np;i++)
{
p.x=s.R.re[i+s.R.nz];
p.y=s.R.im[i+s.R.nz];
csub(c,jomg,p);
cdiv(rsp,rsp,c);
}
return cabs(rsp)*s.R.a0;
}



/**********************************************
double lpsro_a=102289;
double lpsro_f=0.04;
int lpsro_n[2]={9,19};
struct _complex lpsro_p[19]={
			     {-4.648,3.463},
			     {-4.648,-3.463},
			     {-0.1179,0.},
			     {-40.73,0.},
			     {-100.,0.},
			     {-0.15,0.},
			     {-264.,0.},
			     {-3.928,0.},
			     {-0.2820,0.},
			     {-0.2010,0.2410},
			     {-0.2010,-0.2410},
			     {-0.1337,0.1001},
			     {-0.1337,-0.1001},
			     {-0.0251,0.},
			     {-0.00924,0.},
			     {-0.8547,0.2555},
			     {-0.8547,-0.2555},
			     {-0.5415,0.6834},
			     {-0.5415,-0.6834}
			     };
struct _complex lpsro_z[9]={
			     {-0.1243,0.},
			     {-47.62,0.},
			     {0.,-1.053},
			     {0.,1.053},
			     {0.,0.},
			     {0.,0.},
			     {0.,0.},
			     {0.,0.},
			     {0.,0.}
			     };
*****************************************************/

double lpsro_a=102289/1.107209;
double lpsro_f=0.04;
int lpsro_n[2]={7,19};
struct _complex lpsro_p[19]={
			     {-4.648,3.463},
			     {-4.648,-3.463},
			     {-0.1179,0.},
			     {-40.73,0.},
			     {-100.,0.},
			     {-0.15,0.},
			     {-264.,0.},
			     {-3.928,0.},
			     {-0.2820,0.},
			     {-0.2010,0.2410},
			     {-0.2010,-0.2410},
			     {-0.1337,0.1001},
			     {-0.1337,-0.1001},
			     {-0.0251,0.},
			     {-0.00924,0.},
			     {-0.8547,0.2555},
			     {-0.8547,-0.2555},
			     {-0.5415,0.6834},
			     {-0.5415,-0.6834}
			     };
struct _complex lpsro_z[7]={
			     {-0.1243,0.},
			     {-47.62,0.},
			     {0.,0.},
			     {0.,0.},
			     {0.,0.},
			     {0.,0.},
			     {0.,0.}
			     };


double kirnos_a=83.559;
double kirnos_f=0.5;
int kirnos_n[2]={3,4};
struct _complex kirnos_z[3]={
	{0.,0.},{0.,0.},{0.,0.}};

struct _complex kirnos_p[4]={
	{-0.127,0.25},{-0.127,-0.25},{-0.263,0.},{-83.6,0.}};

//double wa_a=1.00448;
double wa_a=0.999375;
double wa_f=10.;
int wa_n[2]={2,2};
//struct _complex wa_z[2]={
//	{0.,0.},{0.,0.}};

//struct _complex wa_p[2]={
//	{-6.28,4.71},{-6.28,-4.71}};

struct _complex wa_z[2]={
	{0.,0.},{0.,0.}};

struct _complex wa_p[2]={
	{-5.4192,5.6848},{-5.4192,-5.6848}};


double wwsslp_a=0.84546;
double wwsslp_f=1./15.;
int wwsslp_n[2]={3,4};
struct _complex wwsslp_z[3]={{0.,0.},{0.,0.},{0.,0.}};
struct _complex wwsslp_p[4]={{-0.411,0.0252},{-0.411,-0.0252},
{-0.0486,0.0},{-0.0874,0.0}};

double wwsssp_a=531.98;
double wwsssp_f=1.;
int wwsssp_n[2]={3,5};
struct _complex wwsssp_z[3]={{0.,0.},{0.,0.},{0.,0.}};
struct _complex wwsssp_p[5]={
	{-3.73,6.22},{-3.73,-6.22},{-5.61,0.0},{-13.2,0.0},{-21.1,0.0}
							};



int filter_smgr(Seismogr *s,int type)
{
int n=8;
double x,y;
int stll;
double freq;
int zc=0;
int i;
double A0,F0;
int np,nz;
double *re,*im;
struct _complex c,b,a;
while(s->Samp>=n)n*=2;
re=(double *)malloc(n*sizeof(double));
if(!re)return -1;
im=(double *)malloc(n*sizeof(double));
if(!im){free(re);return -1;}
stll=strlen(s->Comment);
switch(type)
{
case 1:		
A0=lpsro_a;
F0=lpsro_f;
np=lpsro_n[1];
nz=lpsro_n[0];

sprintf(s->Comment+stll," [LPSRO]");
break;
case 2:
A0=kirnos_a;
sprintf(s->Comment+stll," [KIRNOS]");
F0=kirnos_f;
np=kirnos_n[1];
nz=kirnos_n[0];
break;
case 3:
A0=wwsslp_a;
F0=wwsslp_f;
nz=wwsslp_n[0];
np=wwsslp_n[1];
sprintf(s->Comment+stll," [WWSSNLP]");
break;
case 4:
A0=wwsssp_a;
F0=wwsssp_f;
nz=wwsssp_n[0];
np=wwsssp_n[1];
sprintf(s->Comment+stll," [WWSSNSP]");
break;
case 5:
sprintf(s->Comment+stll," [W.A.]");
A0=wa_a;
F0=wa_f;
nz=wa_n[0];
np=wa_n[1];
break;
default : return -1;
}


for(i=0;i<n;i++)
{
if(i<s->Samp)re[i]=s->Data[i];
else re[i]=0.;
im[i]=0.;
}

if(s->Type=='V'){s->Type='D';zc=1;}
else if(s->Type=='A'){s->Type='D';zc=2;}
else zc=0;
s->Sens*=normalf(*s,F0);
s->Fsens=F0;
fft(re,im,n,-1);
for(i=0;i<=n/2;i++)
{
freq=(double)i/(n*(s->Rate)/1000.);
a.x=re[i];
a.y=im[i];
if(type==1)b=response(nz-zc,np,freq,lpsro_z,lpsro_p);
else if(type==2)b=response(nz-zc,np,freq,kirnos_z,kirnos_p);
else if(type==3)b=response(nz-zc,np,freq,wwsslp_z,wwsslp_p);
else if(type==4)b=response(nz-zc,np,freq,wwsssp_z,wwsssp_p);
else b=response(nz-zc,np,freq,wa_z,wa_p);
cmult(c,a,b);
re[i]=c.x*A0;
im[i]=c.y*A0;
if(i!=0&&i!=n/2){re[n-i]=re[i];im[n-i]=-im[i];}
}
fft(re,im,n,1);
for(i=0;i<s->Samp;i++)
{
s->Data[i]=re[i];
}

free((void*)re);
free((void *)im);

re=(double *)malloc((s->R.np+s->R.nz)*sizeof(double));
if(!re)return -1;
im=(double *)malloc((s->R.np+s->R.nz)*sizeof(double));
if(!im){free((void*)re);return -1;}

for(i=0;i<(s->R.np+s->R.nz);i++)
{
re[i]=s->R.re[i];
im[i]=s->R.im[i];
}

free((void*)s->R.re);
free((void*)s->R.im);
zc=0;
s->R.re=(double *)malloc((s->R.np+s->R.nz+nz-zc+np)*sizeof(double));
if(!s->R.re)return -1;
s->R.im=(double *)malloc((s->R.np+s->R.nz+nz-zc+np)*sizeof(double));
if(!s->R.im){free((void*)s->R.re);return -1;}

for(i=0;i<s->R.nz;i++)
{
s->R.re[i]=re[i];
s->R.im[i]=im[i];
}

for(i=0;i<s->R.np;i++)
{
s->R.re[i+s->R.nz+nz-zc]=re[i+s->R.nz];
s->R.im[i+s->R.nz+nz-zc]=im[i+s->R.nz];
}

for(i=0;i<nz-zc;i++)
{
	   if(type==1){x=lpsro_z[i].x;y=lpsro_z[i].y;}
		if(type==2){x=kirnos_z[i].x;y=kirnos_z[i].y;}
		  if(type==3){x=wwsslp_z[i].x;y=wwsslp_z[i].y;}
		   if(type==4){x=wwsssp_z[i].x;y=wwsssp_z[i].y;}
		    if(type==5){x=wa_z[i].x;y=wa_z[i].y;}
s->R.re[i+s->R.nz]=x;
s->R.im[i+s->R.nz]=y;
}

for(i=0;i<np;i++)
{
	   if(type==1){x=lpsro_p[i].x;y=lpsro_p[i].y;}
		if(type==2){x=kirnos_p[i].x;y=kirnos_p[i].y;}
		  if(type==3){x=wwsslp_p[i].x;y=wwsslp_p[i].y;}
		   if(type==4){x=wwsssp_p[i].x;y=wwsssp_p[i].y;}
		    if(type==5){x=wa_p[i].x;y=wa_p[i].y;}
s->R.re[i+s->R.nz+nz-zc+s->R.np]=x;
s->R.im[i+s->R.nz+nz-zc+s->R.np]=y;
}
s->R.nz=s->R.nz+nz-zc;
s->R.np=s->R.np+np;
s->R.f0=F0;
s->R.a0=1./normalf(*s,F0)*s->R.a0;
free((void*)re);
free((void *)im);
return 0;
}

int Spectr_smgr(Seismogr s,Spectr *sp,daat tmin,daat tmax,int polzer)
{
int start,end,i;
double freq;
double psd;
double *re,*im;
int n=256;
int ssamp;
if(!polzer)
{
	start=getnum_sei(s,tmin);
if(start<0)start=0;
if(start>=s.Samp)start=s.Samp-1;
end=getnum_sei(s,tmax);
if(end<0)end=0;
if(end>=s.Samp)end=s.Samp-1;
ssamp=end-start;
if(ssamp<=0){sp->n=0;return -1;}
}
else ssamp=(int)(raznd(tmax,tmin)/s.Rate)*2;
while(ssamp>=n)n*=2;
n/=2;
re=(double *)malloc(n*sizeof(double));
if(!re)return -1;
im=(double *)malloc(n*sizeof(double));
if(!im){free(re);return -1;}
sp->spdata=(double *)malloc(sizeof(double)*n/2);
if(!(sp->spdata)){free(re);free(im);sp->n=0;return -1;}
if(!polzer){for(i=0;i<n;i++)
{
if(i+start<0||i+start>=s.Samp)re[i]=0.;


else re[i]=s.Data[i+start];
im[i]=0.;
}
taper(re,n,10);
fft(re,im,n,-1);
}
for(i=0;i<n/2;i++)sp->spdata[i]=0.;
for(i=1;i<=n/2;i++)
{
freq=(double)i/(n*(s.Rate)/1000.);

if(!polzer){
	psd=(re[i]*re[i]+im[i]*im[i])/normalf(s,freq)/normalf(s,freq);
sp->spdata[i-1]=psd*(2./n*s.Rate/1000.*1.14286);

//	sp->spdata[i-1]=s.Rate*sqrt(re[i]*re[i]+im[i]*im[i])/normalf(s,freq);
//if(s.Type=='V'){sp->spdata[i-1]*=(freq*M_PI*2);}
//else if(s.Type=='D'){sp->spdata[i-1]/=(4*freq*M_PI*freq*M_PI);}
//sp->spdata[i-1]/=s.Sens;
if(s.Type=='V'){sp->spdata[i-1]*=((freq*M_PI*2)*(freq*M_PI*2));}
else if(s.Type=='D'){sp->spdata[i-1]*=((4*freq*M_PI*freq*M_PI)*(4*freq*M_PI*freq*M_PI));}
sp->spdata[i-1]/=(s.Sens*s.Sens);

}
else {sp->spdata[i-1]=normalf(s,freq);/*freq*=2;*/}

if(polzer)sp->spdata[i-1]*=s.Sens;
sp->spdata[i-1]=10.*log10(sp->spdata[i-1]);
if(i==1)sp->min=sp->max=sp->spdata[i-1];
else {
if((sp->min)>(sp->spdata[i-1]))sp->min=sp->spdata[i-1];
if((sp->max)<(sp->spdata[i-1]))sp->max=sp->spdata[i-1];
}	
}

if(!polzer){
for(i=0;i<n/2-5;i++)
sp->spdata[i]=(sp->spdata[i]+sp->spdata[i+1]+sp->spdata[i+2]+sp->spdata[i+3]+sp->spdata[i+4])/5.;

for(i=n/2-1;i>=4;i--)
sp->spdata[i]=(sp->spdata[i]+sp->spdata[i-1]+sp->spdata[i-2]+sp->spdata[i-3]+sp->spdata[i-4])/5.;
}
sp->n=n/2;
sp->rate=s.Rate/1000.;
sp->f1=(double)1./(n/2.*(s.Rate)/1000.);
sp->f2=(double)1./(2*(s.Rate)/1000.);
/*if(polzer){sp->f2*=2;
sp->max*=4;
sp->rate/=2;}*/
if(!polzer){if(sp->max<-80)sp->max=-80;if(sp->min>-190)sp->min=-190;}
sp->s=s;
free(re);
free(im);
}


