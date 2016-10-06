#define M_PI 3.14159265359
char *PREVSECTION="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
char *PREVFILE="BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
int GetStr(char *section,char *key,char *defaul,char *buffer,int size,char *fname)
{
FILE *in;
static int prevseek=0;
char direct[200];
char tmpstr[80];
char ffnam[250];
char tmpstr1[80];
int i;
int strl=0;
strcpy(tmpstr,section);
for(i=0;i<strlen(tmpstr);i++)
if(tmpstr[i]=='_'){tmpstr[i]=0;break;}
if(strcmp(tmpstr,PREVSECTION)!=0)prevseek=0;
if(strcmp(PREVFILE,fname)!=0)prevseek=0;
strcpy(PREVFILE,fname);
strcpy(PREVSECTION,tmpstr);

sprintf(tmpstr1,"[%s]",section);
GetWindowsDirectory(direct,200);
sprintf(ffnam,"%s\\%s",direct,fname);

if((in=fopen(ffnam,"r"))==NULL){return 0;}
fseek(in,prevseek,SEEK_SET);
while(!feof(in))
{
prevseek+=strl;
if(fscanf(in,"%s",tmpstr)!=1)break;
strl=strlen(tmpstr)+1;
if(feof(in)){fclose(in);return 0;}
//if(tmpstr[1]!='A'||tmpstr[2]!='N')continue;
if(tmpstr[0]!='[')continue;
if(strcmp(tmpstr,tmpstr1)==0)break;
}
if(feof(in)){fclose(in);prevseek=0;return 0;}
while(fscanf(in,"%s",tmpstr)==1)
{
if(feof(in)){fclose(in);prevseek=0;return 0;}
if(tmpstr[0]=='['){fclose(in);prevseek=0;return 0;}
i=0;
while(tmpstr[i]!=0){if(tmpstr[i]=='=')tmpstr[i]=' ';i+=1;}
sscanf(tmpstr,"%s %s",tmpstr1,buffer);
if(strcmp(tmpstr1,key)==0){fclose(in);return 1;}
}
fclose(in);
return 0;
}

int getrespdate(char *fname,int yy,int dd,char *chan,char *stan,char *nettyp,char *lookup)
{
int yys,dds,yye,dde;
char tmpstr1[80];
char tmpstr2[80];
char tmpstr3[80];
int flag=0;
char startdate[80],enddate[80];
int i=0;
sscanf(chan,"%s",tmpstr1);
sscanf(stan,"%s",tmpstr2);
sscanf(nettyp,"%s",tmpstr3);
for(;;)
{
sprintf(lookup,"%s.%s.%s_%d",tmpstr2,tmpstr3,tmpstr1,i);
if(!GetStr(lookup,"startdate","",startdate,80,fname)){flag=1;break;}
if(!GetStr(lookup,"enddate","",enddate,80,fname)){flag=1;break;}
if(sscanf(startdate,"%d,%d",&yys,&dds)!=2){flag=1;break;}
if(sscanf(enddate,"%d,%d",&yye,&dde)!=2)break;
if((yy*365.25+dd)>=(yys*365.25+dds)&&(yy*365.25+dd)<=(yye*365.25+dde))break;
i++;
}
if(flag)return -1;
else return 0;
}



int getoldsens(char *fname,char *lookup,double *sensetivity,double *period)
{
char type[80];
char sens[80];
char fsens[80];
if(!GetStr(lookup,"lookup","",type,80,fname))return -1;
if(!GetStr(lookup,"sens","",sens,80,fname))return -1;
if(!GetStr(lookup,"fsens","",fsens,80,fname))return -1;
*sensetivity=atof(sens);
*period=atof(fsens);
//if(sscanf(sens,"%g",sensetivity)!=1)return -1;
//if(sscanf(fsens,"%g",period)!=1)return -1;
*period=1./(*period);
if(strcmp(type,"M")==0)return 0;
if(strcmp(type,"M/S")==0){*sensetivity*=2.*M_PI/(*period);return 0;}
if(strcmp(type,"M/S**2")==0){*sensetivity*=4.*M_PI*M_PI/(*period)/(*period);return 0;}
return 0;
}

int get_type(char *lookup,char *type,char *fname)
{
if(!GetStr(lookup,"lookup","",type,80,fname))
return -1;
else return 0;
}

int get_nfactor(char *lookup,double *nfactor,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"nfactor","",tmpstr,80,fname))
return -1;
else {*nfactor=atof(tmpstr);return 0;}
}

int get_nfreq(char *lookup,double *nfreq,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"nfreq","",tmpstr,80,fname))
return -1;
else {*nfreq=atof(tmpstr);return 0;}
}

int get_nzeros(char *lookup,int *nzeros,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"nzeros","",tmpstr,80,fname))
return -1;
else {*nzeros=atoi(tmpstr);return 0;}
}

int get_npoles(char *lookup,int *npoles,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"npoles","",tmpstr,80,fname))
return -1;
else {*npoles=atoi(tmpstr);return 0;}
}

int get_sens(char *lookup,double *sens,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"sens","",tmpstr,80,fname))
return -1;
else {*sens=atof(tmpstr);return 0;}
}

int get_fsens(char *lookup,double *fsens,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"fsens","",tmpstr,80,fname))
return -1;
else {*fsens=atof(tmpstr);return 0;}
}

int get_polei(char *lookup,int n,double *re,double *im,char *fname)
{
char tmpstr[80];
char tmpstr1[80];
sprintf(tmpstr,"p%d",n);
if(!GetStr(lookup,tmpstr,"",tmpstr1,80,fname))
return -1;
else {
	sscanf(tmpstr1,"%lf,%lf",re,im);
	return 0;
}

}

int get_zeroi(char *lookup,int n,double *re,double *im,char *fname)
{
char tmpstr[80];
char tmpstr1[80];
sprintf(tmpstr,"z%d",n);
if(!GetStr(lookup,tmpstr,"",tmpstr1,80,fname))
return -1;
else {
	sscanf(tmpstr1,"%lf,%lf",re,im);
	return 0;
}
}