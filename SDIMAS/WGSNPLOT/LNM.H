
/************************************************************************/
/*				SUBLNM					*/
/*  Sublnm returns a value of the NLNM given a period, pn		*/
/*  dva = 0,1, or 2 for displacement, velocity or acceleration		*/
/*									*/
/*  J.Peterson 3/93							*/
/************************************************************************/


double sublnm(double pn, int dva)
{
    int j,k;
    double psd;

/*			Compute psd				*/

    for (j = 0; j < 600; j++)
    {
	if (pn > .1) psd=-162.36+5.64*log10(pn);
	if (pn > .17) psd=-166.70;
	if (pn > .4) psd=-170.00-8.30*log10(pn);
	if (pn > .8) psd=-166.4+28.90*log10(pn);
	if (pn > 1.24) psd=-168.60+52.48*log10(pn);
	if (pn > 2.40) psd=-159.98+29.81*log10(pn);
	if (pn > 4.3) psd=-141.10;
	if (pn > 5.0) psd=-71.36-99.77*log10(pn);
	if (pn > 6.0) psd=-97.26-66.49*log10(pn);
	if (pn > 10.0) psd=-132.18-31.57*log10(pn);
	if (pn > 12.0) psd=-205.27+36.16*log10(pn);
	if (pn > 15.6) psd=-37.65-104.33*log10(pn);
	if (pn > 21.9) psd=-114.37-47.10*log10(pn);
	if (pn > 31.6) psd=-160.58-16.28*log10(pn);
	if (pn > 45.0) psd=-187.50;
	if (pn > 70.0) psd=-216.47+15.70*log10(pn);
	if (pn > 101.0) psd=-185.00;
	if (pn > 154.0) psd=-168.34-7.61*log10(pn);
	if (pn > 328.0) psd=-217.43+11.90*log10(pn);
	if (pn > 600.0) psd=-258.28+26.60*log10(pn);
	if (pn > 10000.0) psd=-346.88+48.75*log10(pn);
    	if (dva == 1) psd = psd+20.0*log10(pn/6.28318531);
    	if (dva == 0) psd = psd+20.0*log10((pn*pn)/39.47842);
    }

    return psd;

}    






/************************************************************************/
/*				SUBHNM					*/
/*  Subhnm returns a value of HNM given a period pn			*/
/*  dva = 0, 1, or 2 for displacement, velocity, or accleration		*/	
/*  J.Peterson 3/93							*/
/************************************************************************/


double subhnm(double pn, int dva)
{
    int j,k;
    double psd;


    for (j = 0; j < 600; j++)
    {
	if (pn > .1) psd=-108.73-17.23*log10(pn);
	if (pn > .22) psd=-150.34-80.50*log10(pn);
	if (pn > .32) psd=-122.31-23.87*log10(pn);
	if (pn > .8) psd=-116.85+32.51*log10(pn);
	if (pn > 3.8) psd=-108.48+18.08*log10(pn);
	if (pn > 4.6) psd=-74.66-32.95*log10(pn);
	if (pn > 6.3) psd=0.66-127.18*log10(pn);
	if (pn > 7.9) psd=-93.37-22.42*log10(pn);
	if (pn > 15.4) psd=73.54-162.98*log10(pn);
	if (pn > 20.0) psd=-151.52+10.01*log10(pn);
	if (pn > 354.8) psd=-206.66+31.63*log10(pn);
    	
    }
    if (dva == 1) psd += 20.0*log10(pn/6.28318531);
    if (dva == 0) psd += 20.0*log10((pn*pn)/39.47842);
    return psd;
}    



