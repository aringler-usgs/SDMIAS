
int getstandate(char *fname,int yy,int dd,char *chan,char *stan,char *nettyp,char *lookup)
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
if(!GetStr(lookup,"a","",startdate,80,fname)){flag=1;break;}
if(!GetStr(lookup,"b","",enddate,80,fname)){flag=1;break;}
if(sscanf(startdate,"%d,%d",&yys,&dds)!=2){flag=1;break;}
if(sscanf(enddate,"%d,%d",&yye,&dde)!=2)break;
if((yy*365.25+dd)>=(yys*365.25+dds)&&(yy*365.25+dd)<=(yye*365.25+dde))break;
i++;
}
if(flag)return -1;
else return 0;
}

int get_lat(char *lookup,double *lat,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"c","",tmpstr,80,fname))return -1;
*lat=atof(tmpstr);
return 0;
}
int get_lon(char *lookup,double *lon,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"d","",tmpstr,80,fname))return -1;
*lon=atof(tmpstr);
return 0;
}
int get_elev(char *lookup,double *elev,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"e","",tmpstr,80,fname))return -1;
*elev=atof(tmpstr);
return 0;
}

int get_depth(char *lookup,double *depth,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"f","",tmpstr,80,fname))return -1;
*depth=atof(tmpstr);
return 0;
}

int get_azim(char *lookup,double *azim,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"g","",tmpstr,80,fname))return -1;
*azim=atof(tmpstr);
return 0;
}

int get_dip(char *lookup,double *dip,char *fname)
{
char tmpstr[80];
if(!GetStr(lookup,"h","",tmpstr,80,fname))return -1;
*dip=atof(tmpstr);
return 0;
}


