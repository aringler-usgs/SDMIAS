//#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define  M_PI        3.14159265358979323846
/*
   ASL_00
   PET   IU BHE
   199601010957379900
   16251
   50
   M/S
   3948.58
   0.02
   9.51276e+008
   0.02
   2 4
   0 0
   0 0
   -0.01234 0.01234
   -0.01234 -0.01234
   -39.18 49.12
   -39.18 -49.12
   53.0235 158.65 150 -5 90 0

 */


void ddmm(int year,int jd,int *dd,int *mm)

{
    int i;
    int mon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(year%4==0)mon[1]=29;
    for(i=0;i<12;i++)
    {
        if(jd<mon[i]){*mm=i+1;*dd=jd;break;}
        jd-=mon[i];
    }
}

char aspace=32,lfeed=10,cretn=13;

void dcomp6(int lb,char *ibuf,int *lout,int *iout,int *ierror)       
{
    char  achar[4];
    char test[4]={'1','2','3','4'};

    unsigned char inn;
    int *itest=(int*)test;
    int ichar[128]={
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,2,
        3,4,5,6,7,8,9,10,11,0,0,0,0,0,0,0,
        12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
        28,29,30,31,32,33,34,35,36,37,0,0,0,0,0,0,
        38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,
        54,55,56,57,58,59,60,61,62,63,0,0,0,0,0,0
    };
    int imax = *lout;
    unsigned char isign = 16;
    unsigned char ioflow = 32;
    unsigned char joflow;
    unsigned char mask1 = 15;
    unsigned char mask2 = 31;
    int ibyte = 3;
    int icount = 0;
    int i = 0    ;
    int j = 0    ;
    int k;
    int jsign;
    int itemp;	
    if (*itest==(((52*256+51)*256+50)*256+49)) ibyte = 0;
    *ierror=0;
    for(i=0;i<lb;i++)
    {
        inn=achar[ibyte] = ibuf[i];
        if(achar[ibyte]==cretn)continue;
        if(achar[ibyte]==lfeed)continue;
        if(achar[ibyte]==aspace)break;
        icount++;
        k=(inn)&127;
        k-=1;
        if(k<0)k=0;
        inn = ichar[k];
        jsign=(inn)&isign;
        joflow=(inn)&ioflow;
        itemp=(inn)&mask1;
METKA2:
        if(joflow==0) goto METKA4;
        itemp = itemp * 32;
METKA3:
        i++;
        inn=achar[ibyte] = ibuf[i];
        if(achar[ibyte]==cretn)goto METKA3;
        if(achar[ibyte]==lfeed)goto METKA3;

        icount++;
        k=(inn)&127;
        k-=1;
        if(k<0)k=0;
        inn = ichar[k];

        joflow=(inn)&ioflow;
        k=(inn)&mask2;
        itemp = itemp + k;
        goto METKA2;
METKA4:
        if(jsign!=0)itemp = -itemp;

        iout[j++] = itemp;
        if(j>=imax) goto METKA5;
    }	
METKA5:
    *lout = j;
    if (j > imax) {*lout=imax;
        *ierror = -1;}

}

void      remdif1(int *iy,int nmax)
{
    int k;
    for(k=1;k<nmax;k++)
        iy[k]=iy[k]+iy[k-1];
}


char cmp6[160000];
int wform[160000];
int nchr=160000;
double srate;
double gain,tref,slat,slon,elev,sdep,caz,cdep,unk;
int i;
int jchk;
int yy,mm,dd,jd,hh,mi,ss,ms,npts;
char filename[100];
char sta[10];
char cha[10];
char cmp[10];
char *nt="GS";
char inst[10];
long  idif;
char test[200];
char tmpstr[100];
char compress[10];
long ichk;
long iunits;
int ierror;
double rgain;
double polesi[100];
double polesr[100];
double zerosi[100];
double zerosr[100];
int npoles;
int nzeros;
char rsta[10];
char rcha[10];
char rcmp[10];
char rinst[10];
char rtype[10];
char rdate[10];
char runk[10];
void main(int argc,char *argv[])
{
    FILE *wdata;
    FILE *out;
    FILE *cdata;
    if(argc<3){printf("bad command line option:\nUse gse_wgsn.exe [e-mail waveform] [e-mail calib]\n");return;}
    if((wdata=fopen(argv[1],"r"))==NULL){printf("Cann't open %s file with waveform data\n",argv[1]);return;}
WID1:
    i=0;
    while(!feof(wdata))
    {

        if(fscanf(wdata,"%s",test)!=1){printf("End of Waveform File\n");break;}
        if(strcmp(test,"DAT1")==0){printf("Wrong place for DAT1\n");break;}
        if(strcmp(test,"CHK1")==0){printf("Wrong place for CHK1\n");break;}
        if(strcmp(test,"WID1")==0){
            //fgets(tmpstr,100,wdata);
            if(fscanf(wdata,"  %4d%3d%3d%3d%3d%4d%9d",
                        &yy,&jd,&hh,&mi,&ss,&ms,&npts)!=7)
            {
                printf("Not valid WID1 header line1\n");break;}
                fscanf(wdata,"%s",sta);
                fscanf(wdata,"%s",cha);
                fscanf(wdata,"%s",cmp);
                fscanf(wdata,"%s",tmpstr);
                srate=atof(tmpstr);
                //sscanf(tmpstr,"%lf",&srate);
                fscanf(wdata,"%s",inst);



                if(fscanf(wdata,"%s %d",compress,&idif)!=2)
                {printf("Not valid WID1 header line1\n");break;}
                fgets(tmpstr,100,wdata);
                fgets(tmpstr,100,wdata);
                sscanf(tmpstr+9,"%d",&iunits);
                tmpstr[9]=0;
                sscanf(tmpstr,"%lf",&gain);
                if(sscanf(tmpstr+10,"%7lf%10lf%10lf%10lf%10lf %lf %lf %lf",&tref,&slat,&slon,&elev,&sdep,&caz,&cdep,&unk)!=8)
                {
                    printf("Not valid WID1 header line2\n");break;}
                    goto DAT1;}

    }

END: printf("I cann't find any waveforms\n");
     fclose(wdata);
     return;
DAT1:
     if(fscanf(wdata,"%s",test)!=1)goto END;
     if(strcmp(test,"DAT1")!=0){printf("WID1 contains nodata:looking for next\n");goto WID1;}
     printf("Valid WID1&DAT1 header found\n");
     while(!feof(wdata))
     {
         if(fscanf(wdata,"%s",test)!=1){printf("End of Waveform File\n");break;}
         if(strcmp(test,"DAT1")==0){printf("Wrong place for DAT1\n");break;}
         if(strcmp(test,"WID1")==0){printf("Wrong place for WID1\n");break;}
         if(strcmp(test,"CHK1")==0)goto CHK1;
         if(i+strlen(test)>=nchr){printf("Memory for CMP6 Data fullfilled\n");break;}
         strcpy(&cmp6[i],test);i+=strlen(test);
     }
     goto END;
CHK1:
     cmp6[i]=32;
     fscanf(wdata,"%d",&ichk);
     dcomp6(nchr,cmp6,&npts,wform,&ierror);
     for(i=0;i<idif;i++)
         remdif1(wform,npts);
     jchk=0;
     for(i=0;i<npts;i++)
         jchk+=wform[i];
     if(jchk!=ichk){printf("WID1 check summ error:%d %d\n",jchk,ichk);goto WID1;}
     sprintf(filename,"%04d%03d%02d%02d.%s.%s",yy,jd,hh,mi,sta,cha);
     if((out=fopen(filename,"wb"))==NULL){printf("Cann't open %s file for output\n",filename);fclose(wdata);return;}
     /******************************************************/
     /********* looking for calibration info****************/
     /******************************************************/
     if((cdata=fopen(argv[2],"r"))==NULL)
     {
         printf("Cann't open %s file for output\n",argv[2]);printf("Calibration File Not Avaluable\nDefault response will be set\n");
         //write_default_header(out);
         goto WID1;
     }

     while(!feof(cdata))
     {
         fscanf(cdata,"%s",test);
         if(strcmp(test,"CAL1")==0){
             if(fscanf(cdata,"%s %s %s %s %s %s %s",rsta,rcha,rcmp,rinst,rtype,rdate,runk)!=7)
             {printf("Wrong CAL1 header\n");break;}
             if(strcmp(rsta,sta)==0||strcmp(rcha,cha)==0||strcmp(rtype,"RAZ")==0){printf("Valid calibration found\n");goto CAL1;}
         }
     }

DEFRSP:
     printf("Calibration data for %s %s not available\nDefault response will be set\n",sta,cha);
     //write_default_header(out);
     fclose(cdata);
     goto WID1;
CAL1:
     if(fscanf(cdata,"%d",&npoles)!=1)goto DEFRSP;
     for(i=0;i<npoles;i++)
         if(fscanf(cdata,"%lf %lf",&polesr[i],&polesi[i])!=2)goto DEFRSP;
     if(fscanf(cdata,"%d",&nzeros)!=1)goto DEFRSP;
     for(i=0;i<nzeros;i++)
         if(fscanf(cdata,"%lf %lf",&zerosr[i],&zerosi[i])!=2)goto DEFRSP;
     if(fscanf(cdata,"%lf",&rgain)!=1)goto DEFRSP;
     /* 
        ASL_00
        PET   IU BHE
        199601010957379900
        16251
        50
        M/S
        3948.58
        0.02
        9.51276e+008
        0.02
        2 4
        0 0
        0 0
        -0.01234 0.01234
        -0.01234 -0.01234
        -39.18 49.12
        -39.18 -49.12
        53.0235 158.65 150 -5 90 0

      */
     //write_rsp_header(sta,nt,cha,yy,jd,dd,hh,mi,ss,ms,out,npts,rate,iunits,rgain,tref,gain,tref,nzeros,npols,zerosr,zerosi,polesr,polesr,slat,slon,elev,sdep);
     fprintf(out,"ASL_00\n");
     fprintf(out,"%s %s %s\n",sta,nt,cha);
     ddmm(yy,jd,&dd,&mm);
     fprintf(out,"%04d%02d%02d%02d%02d%02d%d\n",yy,mm,dd,hh,mi,ss,ms*10);
     fprintf(out,"%d\n",npts);
     fprintf(out,"%g\n",1000./srate);
     //if(iunits==0)fprintf(out,"M\n");
     /*else if(iunits==1)*/fprintf(out,"M/S\n");
     //else if(iunits==2)fprintf(out,"M/S**2\n");
     //else fprintf(out,"UNK\n");
     fprintf(out,"%g\n",rgain*gain/tref*2*M_PI);
     fprintf(out,"%g\n",1./tref);
     fprintf(out,"%g\n",(1./gain*1000000000.)*tref/2/M_PI);
     fprintf(out,"%g\n",1./tref);
     fprintf(out,"%d %d\n",nzeros,npoles+1);
     for(i=0;i<nzeros;i++)
         fprintf(out,"%g %g\n",zerosr[i],zerosi[i]);
     for(i=0;i<npoles;i++)
         fprintf(out,"%g %g\n",polesr[i],polesi[i]);
     fprintf(out,"0 0\n");
     fprintf(out,"%g %g %g %g ",slat,slon,elev,-sdep);
     if(cha[2]=='Z')fprintf(out,"0 -90\n");
     else if(cha[2]=='E')fprintf(out,"90 0\n");
     else fprintf(out,"0 0\n");

     fclose(cdata);
     printf("responce OK\n");
WGSNPLOT:
     for(i=0;i<npts;i++)
         fprintf(out,"%d\n",wform[i]);
     printf("Data write To file %s\n",filename);
     printf("looking for Next\n");
     //fclose(wdata);
     fclose(out);
     goto WID1;
}


