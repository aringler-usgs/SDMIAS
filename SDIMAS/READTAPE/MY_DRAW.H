typedef struct {float left,top,right,bottom;}phisw;
void Line(HDC hdc,int x1,int y1,int x2,int y2)
{
POINT P;
MoveToEx(hdc,x1,y1,&P);
LineTo(hdc,x2,y2);
}

void lin_ax(HDC hdc,phisw f,RECT w,char *name,char typ,char *sn)
{
int i;
char str[20];
float fmd;
float wdx=w.right-w.left;
float fdx=f.right-f.left;
float hor=wdx/fdx;
float minf,maxf;
float dx,fx,x;
HPEN n_pen,o_pen;
float ddx;
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
TextOut(hdc,(w.left+w.right)/2,w.top-3,name,strlen(name));
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
	if(fmd<ddx/2.||fabs(fmd-dx)<ddx/2.){	Line(hdc,x,w.bottom,x,w.bottom+7);
		TextOut(hdc,x,w.bottom+10,str,strlen(str));
		if(typ!=0){
		o_pen=SelectObject(hdc,n_pen);
		Line(hdc,x,w.top,x,w.bottom);
		n_pen=SelectObject(hdc,o_pen);
					 }

		}
		else Line(hdc,x,w.bottom,x,w.bottom+4);
	}
DeleteObject(n_pen);
}

//***************************************************************

void lin_ay(HDC hdc,phisw f,RECT w,char *name,char typ,char *sn)
{
int i;
char str[80];
float fmd;
float wdy=w.bottom-w.top;
float fdy=f.top-f.bottom;
float ver=wdy/fdy;
float minf,maxf;
float dy,fy,y;
HPEN n_pen,o_pen;
float ddy;
Line(hdc,w.left,w.top,w.left,w.bottom);
Line(hdc,w.right,w.top,w.right,w.bottom);
minf=f.bottom;maxf=f.top;
n_pen=CreatePen(PS_DOT,1,RGB(0,0,0));
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
	if(fmd<ddy/2.||fabs(fmd-dy)<ddy/2.){	Line(hdc,w.left,y,w.left-7,y);
	 TextOut(hdc,w.left-10,y,str,strlen(str));
			if(typ!=0){
					o_pen=SelectObject(hdc,n_pen);
		Line(hdc,w.right,y,w.left,y);
				n_pen=SelectObject(hdc,o_pen);
		}

		}
	 else Line(hdc,w.left,y,w.left-4,y);
	}
	DeleteObject(n_pen);
}
void draw_block(HDC hdc,long *dan,long n,RECT r,char *name)
{
int i,x,y;
POINT P;
phisw f;
float max,min;
HPEN n_pen,o_pen;
//float ddx;
n_pen=CreatePen(PS_SOLID,1,RGB(0,0,0));
o_pen=SelectObject(hdc,n_pen);

max=min=dan[0];
for(i=0;i<n;i++)
{
if(dan[i]<min)min=dan[i];
if(dan[i]>max)max=dan[i];
}
if(r.top>=r.bottom)return;
if(r.left>=r.right)return;
f.top=max;
f.bottom=min;
f.left=0;
f.right=n;
if(n==0)return;
if(min>=max)return;
lin_ay(hdc,f,r,"",1,"");
lin_ax(hdc,f,r,name,1,"");
SelectObject(hdc,o_pen);
DeleteObject(n_pen);
n_pen=CreatePen(PS_SOLID,1,RGB(0,0,0));
o_pen=SelectObject(hdc,n_pen);
for(i=0;i<n;i++)
{
y=r.bottom-(dan[i]-min)/(max-min)*(r.bottom-r.top);
x=r.left+(float)(i)/(float)n*(r.right-r.left);
if(i==0)MoveToEx(hdc,x,y,&P);
else LineTo(hdc,x,y);}
SelectObject(hdc,o_pen);
DeleteObject(n_pen);
}






