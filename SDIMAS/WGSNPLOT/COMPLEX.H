#ifndef __COMPLEX_H
#define __COMPLEX_H


#if __STDC__ || defined(HP700) || defined(SUN4)
typedef struct complex
		{ double x;
		  double y;} complex;
#else
typedef struct complex complex;
#endif

#define COMPLEX_DEFINED

double _caccr=0., _cacci=0.;    /* pracovni promenne pro makra    */

#define cabs2(c)        ((c).x * (c).x + (c).y * (c).y) /* vraci |c|*|c|  */
#define cabs(c)	        sqrt ((double)  cabs2 (c))      /* vraci |c|      */
#define cadd(c,b,a)     ((c).x = (a).x + (b).x, (c).y = (a).y + (b).y)
							/* c = a + b      */
#define csub(c,a,b)     ((c).x = (a).x - (b).x, (c).y = (a).y - (b).y)
							/* c = a - b      */
#define cconj(b,a)	((b).x = (a).x, (b).y = - (a).y)
							/* b = a*         */
#define cmult(c,b,a)	(_caccr = (a).x * (b).x - (a).y * (b).y,     \
			 (c).y = (a).x * (b).y + (a).y * (b).x,      \
			 (c).x = _caccr)                /* c = a * b      */
#define cnull(a)	((a).x = (a).y = 0)             /* c = 0          */
#define ccopy(b,a)	((b).x = (a).x, (b).y = (a).y)  /* b = a          */
#define cdiv(c,a,b)	(_caccr = (_cacci = 1 / cabs2 (b)) *              \
			((a).x * (b).x + (a).y * (b).y),                  \
			(c).y = _cacci * ((a).y * (b).x - (a).x * (b).y), \
			(c).x = _caccr)                /*  c = a / b     */
#define cneg(b,a)       ((b).x = -(a).x, (b).y = -(a).y)/*  b = -a        */
//#define re(c)		((c).x)
//#define im(c)		((c).y)

#define cisReal(z)      ((z).y==0.0)
#define cdist2(z1,z2)   (((z1).x-(z2).x)*((z1).x-(z2).x) + ((z1).y-(z2).y)*((z1).y-(z2).y))
#define cdist(z1,z2)    sqrt(cdist2(z1,z2))

#define cEQ(c1,c2)    ((c1).x==(c2).x && (c1).y==(c2).y)
#define cNE(c1,c2)    !cEQ(c1,c2)

#ifdef ANSI
double carg (const complex *c);
#else
double carg ();
#endif
  /* Vraci argument komplexniho cisla v rozzsahu <0, 2*PI) */

#ifdef ANSI
void cctop (complex *b, const complex *a);
#else
void cctop ();
#endif
  /*  Cartesian to polar, a->b (re(b) = modul, im(b)= argument [rad]) */

#ifdef ANSI
void cptoc (complex *b, const complex *a);
#else
void cptoc ();
#endif
  /*  polar to Cartesian, a->b (re(a) = modul, im(a) = argument [rad]) */

#ifdef ANSI
void clog (complex *b, const complex *a);
#else
void clog ();
#endif
  /* b = log (a) */

#ifdef ANSI
void cexp (complex *b, const complex *a);
#else
void cexp ();
#endif
  /*  b = exp (a)  */

#endif
