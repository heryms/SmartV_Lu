#include "stdafx.h"
#include "PathGenerate.h"

double fns[] = { 0.49999988085884732562,   
	1.3511177791210715095,    
	1.3175407836168659241,    
	1.1861149300293854992,    
	0.7709627298888346769,    
	0.4173874338787963957,    
	0.19044202705272903923,   
	0.06655998896627697537,   
	0.022789258616785717418,  
	0.0040116689358507943804, 
	0.0012192036851249883877 } ;

double fds[] = {1.0,                      
	2.7022305772400260215,    
	4.2059268151438492767,    
	4.5221882840107715516,    
	3.7240352281630359588,    
	2.4589286254678152943,    
	1.3125491629443702962,    
	0.5997685720120932908,    
	0.20907680750378849485,   
	0.07159621634657901433,   
	0.012602969513793714191,  
	0.0038302423512931250065 } ;

double gns[] = { 0.50000014392706344801,
	0.032346434925349128728,  
	0.17619325157863254363,   
	0.038606273170706486252,  
	0.023693692309257725361,  
	0.007092018516845033662,  
	0.0012492123212412087428, 
	0.00044023040894778468486,
	-8.80266827476172521e-6,  
	-1.4033554916580018648e-8,
	2.3509221782155474353e-10 } ;

double gds[]  = { 1.0,                    
	2.0646987497019598937,    
	2.9109311766948031235,    
	2.6561936751333032911,    
	2.0195563983177268073,    
	1.1167891129189363902,    
	0.57267874755973172715,   
	0.19408481169593070798,   
	0.07634808341431248904,   
	0.011573247407207865977,  
	0.0044099273693067311209, 
	-0.00009070958410429993314 } ;

void CPathGenerate:: FresnelCS(double *FresnelC,double *FresnelS,double y)
{
	//	FresnelC=new double[3];
	//	FresnelS =new double[3];
	//	memset(FresnelC,0,3*sizeof(double));
	//memset(FresnelS,0,3*sizeof(double));
	double t,twofn,fact,denterm,numterm,sum,ratio,term,sumn,sumd,f,g,U,SinU,CosU,oldterm,eps10,absterm;

	double x = abs(y) ;
	if( x < 1.0)
	{
		t = -((PI/2)*x*x)*((PI/2)*x*x);
		//	% Cosine integral series
		twofn   = 0.0 ;
		fact    = 1.0 ;
		denterm = 1.0 ;
		numterm = 1.0 ;
		sum     = 1.0 ;
		ratio   = 10.0 ; 

		while( ratio > eps1)
		{
			twofn   = twofn + 2.0;
			fact    = fact*twofn*(twofn-1.0);
			denterm = denterm + 4.0;
			numterm = numterm*t;
			term    = numterm/(fact*denterm);
			sum     = sum+term;
			ratio   = abs(term/sum);
		}
		FresnelC[0] = x*sum;

		//% Sine integral series
		twofn   = 1.0 ;
		fact    = 1.0 ;
		denterm = 3.0 ;
		numterm = 1.0 ;
		sum     = 1.0/3.0 ;
		ratio   = 10.0 ;

		while (ratio > eps1)
		{
			twofn   = twofn+2.0;
			fact    = fact*twofn*(twofn-1.0);
			denterm = denterm+4.0;
			numterm = numterm*t;
			term    = numterm/(fact*denterm);
			sum     = sum+term;
			ratio   = abs(term/sum);
		}
		FresnelS[0]= (PI/2)*sum*x*x*x;
	}
	else if(x < 6.0)

		//	% Rational approximation for f
	{
		sumn = 0.0 ;
		sumd = fds[11] ;
		for( int k=11;k>=1;k--)
		{
			sumn = fns[k-1]+x*sumn ;
			sumd = fds[k-1]+x*sumd ;
		}

		f = sumn/sumd;
		//% Rational approximation for  g
		sumn = 0.0 ;
		sumd = gds[11] ;
		for( int k=11;k>=1;k--)
		{
			sumn = gns[k-1]+x*sumn;
			sumd = gds[k-1]+x*sumd;
		}

		g    = sumn/sumd;
		U    = (PI/2)*x*x ;
		SinU = sin(U);
		CosU = cos(U);
		FresnelC[0] = 0.5+f*SinU-g*CosU;
		FresnelS[0]= 0.5-f*CosU-g*SinU;
	}
	else
	{
		//	% x >= 6; asymptotic expansions for  f  and  g
		t = -1/((PI*x*x)*(PI*x*x));
		//	% Expansion for  f
		numterm = -1.0 ;
		term    =  1.0 ;
		sum     =  1.0 ;
		oldterm =  1.0 ;
		ratio   = 10.0 ;
		eps10   = 0.1*eps1;

		while(ratio > eps10)
		{
			numterm = numterm+4.0;
			term    = term*numterm*(numterm-2.0)*t;
			sum     = sum + term;
			absterm = abs(term);
			ratio   = abs(term/sum);
			if (oldterm < absterm)
			{
				//disp('\n\n !!In FresnelCS f not converged to eps');
				ratio = eps10;
			}

			oldterm = absterm;
		}


		f = sum/(PI*x);
		//% Expansion for  g
		numterm = -1.0 ;
		term    =  1.0 ;
		sum     =  1.0 ;
		oldterm =  1.0 ; 
		ratio   = 10.0 ;
		eps10   = 0.1*eps1;

		while(ratio > eps10)
		{
			numterm = numterm+ 4.0;
			term    = term*numterm*(numterm+2.0)*t;
			sum     = sum+term;
			absterm = abs(term);
			ratio   = abs(term/sum);
			if (oldterm < absterm)
			{
				//disp('\n\n!!In FresnelCS g not converged to eps');
				ratio = eps10;
			}
			oldterm = absterm;

		}
		g    = sum/((PI*x)*(PI*x)*x) ;
		U    = (PI/2)*x*x ;
		SinU = sin(U);
		CosU = cos(U);
		FresnelC[0]= 0.5+f*SinU-g*CosU;
		FresnelS[0] = 0.5-f*CosU-g*SinU;
	}


	if(y<0)
	{
		FresnelC[0]= -FresnelC[0] ;
		FresnelS[0]= -FresnelS[0];
	}



}
void CPathGenerate::intXY(double *X,double *Y,int nk,double a,double b,double c)
{
	double absa = abs(a) ;
	double epsi = 0.01   ;// % best thresold

	//X = new double[nk] ;
	//Y = new double[nk] ;
	//memset(X,0,nk*sizeof(double));
	//memset(Y,0,nk*sizeof(double));
	if (absa < epsi )//% case `a` small
	{
		evalXYaSmall(X,Y,nk,a,b,5) ;
	}
	else
	{
		evalXYaLarge(X,Y,nk, a, b ) ;
	}


	double cc = cos(c) ;
	double ss = sin(c) ;

	for (int k=0;k<nk;k++)
	{
		double xx = X[k] ;
		double yy = Y[k] ;
		X[k]= xx*cc-yy*ss ;
		Y[k] = xx*ss+yy*cc ;
	}

}
void CPathGenerate::FresnelCSk(int nk,double t,double *FresnelC,double*FresnelS)
{
	double C[3];
	double S[3];
	FresnelCS(C,S,t);
	FresnelC[0]=C[0];
	FresnelS[0]=S[0];
	if(nk > 1)
	{
		double tt   = PI/2*t*t ;
		double ss   = sin(tt) ;
		double cc   = cos(tt) ;
		FresnelC[1]= ss/PI ;
		FresnelS[1]= (1-cc)/PI ;
		if (nk > 2)
		{
			FresnelC[2] = (t*ss-FresnelS[0])/PI ;
			FresnelS[2] = (FresnelC[0]-t*cc)/PI ;
		}
	}

	//delete C;
	//	delete S;
}

void CPathGenerate::evalXYaLarge(double* X,double* Y,int nk,double a,double b)
{
	double	s=0 ;
	if(a>0)s=1;
	else if(a==0)s=0;
	else if(a<0)s=-1;
	//else s=0;
	double	z       = sqrt(abs(a)/PI) ;
	double	ell     = s*b/sqrt(abs(a)*PI) ;
	double	g       = -0.5*s*b*b/abs(a) ;
	double Cl[3];
	double Sl[3];
	double Cz[3];
	double Sz[3];

	FresnelCSk(nk,ell,Cl,Sl) ;
	FresnelCSk(nk,ell+z,Cz,Sz) ;
	double* dC  =new double[nk]; dC[0]   = Cz[0]- Cl[0] ;
	double* dS  =new double[nk]; dS[0]   = Sz[0]- Sl[0] ;
	double cg      = cos(g)/z ;
	double sg      = sin(g)/z ;
	X[0]   = cg * dC[0] - s * sg * dS[0] ;
	Y[0]   = sg * dC[0] + s * cg * dS[0] ;
	if (nk > 1)
	{
		dC[1]   = Cz[1]- Cl[1] ;
		dC[2]   = Cz[2]- Cl[2] ;
		dS[1]   = Sz[1]- Sl[1] ;
		dS[2]   = Sz[2]- Sl[2] ;
		cg   = cg/z ;
		sg   = sg/z ;
		double DC   = dC[1]-ell*dC[0];
		double DS   = dS[1]-ell*dS[0];
		X[1]= cg * DC - s * sg * DS ;
		Y[1]= sg * DC + s * cg * DS ;
		if (nk > 2)
		{
			cg   = cg/z ;
			sg   = sg/z ;
			DC   = dC[2]+ell*(ell*dC[0]-2*dC[1]) ;
			DS   = dS[2]+ell*(ell*dS[0]-2*dS[1]) ;
			X[2]= cg * DC - s * sg * DS ;
			Y[2]= sg * DC + s * cg * DS ;
		}
	}
	delete dC;
	delete dS;
}
double CPathGenerate::S(double mu,double nu,double b)
{

	double tmp = 1/((mu+nu+1)*(mu-nu+1)) ;
	double res = tmp ;
	for (int n=1;n<=100;n++)
	{
		tmp = tmp * (-b/(2*n+mu-nu+1)) * (b/(2*n+mu+nu+1)) ;
		res = res + tmp ;
		if (abs(tmp) < abs(res) *1e-50)
			break ;

	}
	return res;
}
void CPathGenerate::evalXYazero(double *X,double *Y,int nk,double b)
{
	double sb = sin(b) ;
	double cb = cos(b) ;
	double b2 = b*b ;
	if (abs(b) <1e-3)
	{
		X[0] = 1-b2/6*(1-b2/20*(1-b2/42)) ;
		Y[0] = (b/2)*(1-b2/12*(1-b2/30)) ;
	}
	else
	{
		X[0] = sb/b ;
		Y[0] = (1-cb)/b ;
	}
	double A = b*sb ;
	double D = sb-b*cb ;
	double B = b*D ;            //���������������ʱ�䣡����������
	double C = -b2*sb ;
	for (int k=1;k<nk;k++)
	{
		X[k]=(k*A*S(k+0.5,1.5,b)+B*S(k+1.5,0.5,b)+cb)/(1+k) ;
		Y[k]=(C*S(k+1.5,1.5,b)+sb)/(2+k)+D*S(k+0.5,0.5,b) ;
	}
}
void CPathGenerate::evalXYaSmall(double *X,double *Y,int nk,double a,double b,double p)
{

	double* X0=new double[nk+4*(int)p+2] ;
	double* Y0=new double[nk+4*(int)p+2] ;
	memset(X0,0,(nk+4*(int)p+2)*sizeof(double));
	memset(Y0,0,(nk+4*(int)p+2)*sizeof(double));
	evalXYazero(X0,Y0,nk+4*p+2, b) ;

	for (int j=0;j<nk;j++)
	{
		X[j] = X0[j]-a*Y0[j+2]/2 ;
		Y[j] = Y0[j]+a*X0[j+2]/2 ;
	}


	int 	t  = 1 ;
	double aa = -(a/4)*(a/4) ;
	for (int n=1;n<=p;n++)
	{
		t  = t*aa/(n*(2*n-1)) ;
		double bf = a/(4*n+2) ;
		for(int j=0;j<nk;j++)
		{
			X[j] = X[j] + t*(X0[4*n+j]-bf*Y0[4*n+j+2]) ;
			Y[j] = Y[j] + t*(Y0[4*n+j]+bf*X0[4*n+j+2]) ;

		}
	}

	delete X0;
	delete Y0;
}			

void CPathGenerate::buildClothoid(double &k,double &dk,double &L,double x0,double y0,double theta0,double x1,double y1,double theta1,double tol)
{

	double dx  = x1 - x0 ;
	double dy  = y1 - y0 ;
	double r   = sqrt(dx*dx + dy*dy) ;
	double phi = atan2(dy,dx);
	double phi0 = normalizeAngle(theta0 - phi) ;
	double phi1 = normalizeAngle(theta1 - phi) ;

	double delta = phi1 - phi0 ;

	// % initial point
	double	 Aguess = guessA( phi0, phi1 ) ;
	double A;
	// % Newton iteration
	findA(A,Aguess, delta, phi0, tol ) ;

	double* h=new double;
	double* g=new double;
	memset(h,0,sizeof(double));
	memset(g,0,sizeof(double));
	//% final operation
	intXY(h,g,1, 2*A, delta-A, phi0 ) ;
	L = r/h[0];

	if (L > 0)
	{
		k  = (delta - A)/L ;
		dk = 2*A/L/L ;
	}
	else
		// %      error('negative length') ;
	{ k=inf;
	dk=inf;
	L=inf;}

	delete h;
	delete g;
}
double CPathGenerate::normalizeAngle(double phi_in)
{
	double phi;
	phi = phi_in ;
	while ( phi > PI )
		phi = phi - 2*PI ;
	while ( phi < -PI )
		phi = phi + 2*PI ;
	return phi;

}

void CPathGenerate::findA(double &A,double Aguess,double delta,double phi0,double tol)
{
	double f,df;
	double intC[3];
	double intS[3];
	//double* intC=new double[3];
	//double* intS=new double[3];
	//memset(intC,0,3*sizeof(double));
	//memset(intS,0,3*sizeof(double));
	A = Aguess ;
	for (int iter=1;iter<=100;iter++)
	{
		intXY(intC,intS,3, 2*A, delta-A, phi0 ) ;
		f  = intS[0] ;
		df = intC[2]-intC[1] ;
		A  = A - f/df ;
		if (abs(f) < tol)
			break ;
	}
	//if (abs(f) > tol*10)
	//fprintf( 1, 'Newton iteration fails, f = %g\n', f ) ;
	//fprintf( 1, 'Aguess = %g, A = %g, delta = %g , phi0 = %g\n', Aguess, A, delta, phi0 ) ;
	//delete intC;
	//delete intS;
}


double CPathGenerate::guessA(double phi0,double phi1)
{
	double CF[6] = { 2.989696028701907, 
		0.716228953608281, 
		-0.458969738821509,
		-0.502821153340377, 
		0.261062141752652,
		-0.045854475238709 };
	double X  = phi0/PI ;
	double 	Y  = phi1/PI ;
	double	xy = X*Y ;
	double A  = (phi0+phi1) * ( CF[0] + xy * ( CF[1] + xy * CF[2] ) +
		(CF[3] + xy * CF[4]) * (X*X+Y*Y) + 
		CF[5] * (X*X*X*X+Y*Y*Y*Y) ) ;
	return A;
}

void CPathGenerate::pointsOnClothoid(RoadPoint XY[],double x0,double y0,double theta0,double kappa,double dkappa,double L,int npts)
{
	//	XY=new double[2*npts];
	double C[1];
	double S[1];
	//double* C=new double;
	//double* S=new double;
	//memset(C,0,sizeof(double));
	//memset(S,0,sizeof(double));
	int j=0;
	//FILE *fp=fopen("clothoid.txt","a+");

	for(double t=0;t<=L;)
	{
		intXY(C,S,1, dkappa*t*t, kappa*t, theta0 );
		//XY    = [ XY [ x0 + t*C ; y0 + t*S ] ];

		XY[j].x= x0 + t*C[0];
		XY[j].y=y0 + t*S[0];
		XY[j].angle=theta0+t*kappa+0.5*t*t*dkappa;
		//clothoidpath[j].x=XY[j];
		//clothoidpath[j].y=XY[npts+j];
		//fprintf(fp,"%d\t%f\t%f\n",j, clothoidpath[j].x,clothoidpath[j].y);
		t=t+L/(double)npts;
		j++;
	}
	//delete C;
	//delete S;
	//	fclose(fp);
}







//����

double CPathGenerate::Distance(RoadPoint m1,RoadPoint m2)
{
	return (m1.x-m2.x)*(m1.x-m2.x)+(m1.y-m2.y)*(m1.y-m2.y);
}
double CPathGenerate::cosangle(RoadPoint m1,RoadPoint m2,RoadPoint m3)
{
	return (m2.x-m1.x)*(m2.x-m3.x)+(m2.y-m1.y)*(m2.y-m3.y);
}
double CPathGenerate::multiply(RoadPoint sp,RoadPoint ep,RoadPoint op) 
{ 
	return((sp.x-op.x)*(ep.y-op.y)-(ep.x-op.x)*(sp.y-op.y)); 
} 
//�ж��߶����߶��Ƿ��ཻ
bool CPathGenerate::intersect(LINESEG u,LINESEG v) 
{ 
	return( (max(u.s.x,u.e.x)>=min(v.s.x,v.e.x))&&                     //�ų�ʵ�� 
		(max(v.s.x,v.e.x)>=min(u.s.x,u.e.x))&& 
		(max(u.s.y,u.e.y)>=min(v.s.y,v.e.y))&& 
		(max(v.s.y,v.e.y)>=min(u.s.y,u.e.y))&& 
		(multiply(v.s,u.e,u.s)*multiply(u.e,v.e,u.s)>=0)&&         //����ʵ�� 
		(multiply(u.s,v.e,v.s)*multiply(v.e,u.e,v.s)>=0)); 
} 


//����1��ʾ�ཻ������0��ʾ���ཻ
bool CPathGenerate::CurveInsects(RoadPoint Up[],RoadPoint Down[],LINESEG u)
{

	bool flag_up,flag_down;
	flag_down=flag_up=0;

	for(int i=0;i<fileLength-1;i++)
	{
		LINESEG l;
		l.s=Up[i];
		l.e=Up[i+1];
		if(intersect(l,u)==1)
		{
			flag_up=1;
			break;
		}
	}

	for(int i=0;i<fileLength-1;i++)
	{
		LINESEG l;
		l.s=Down[i];
		l.e=Down[i+1];
		if(intersect(l,u)==1)
		{
			flag_down=1;
			break;
		}
	}
	if(flag_down==0&&flag_up==0)
		return 0;
	else return 1;
	//return 0;
}

//��ȡ�߶ν���
RoadPoint CPathGenerate::getIntersectPoint(RoadPoint u1,RoadPoint u2,RoadPoint v1,RoadPoint v2){
	RoadPoint ret=u1;
	double t=((u1.x-v1.x)*(v1.y-v2.y)-(u1.y-v1.y)*(v1.x-v2.x))
		/((u1.x-u2.x)*(v1.y-v2.y)-(u1.y-u2.y)*(v1.x-v2.x));
	ret.x+=(u2.x-u1.x)*t;
	ret.y+=(u2.y-u1.y)*t;
	return ret;
}

//����㣨x,y�����߶εľ���
double CPathGenerate::PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (py - y1) * (py - y1));
}

//��ֱ֪�������㣬��ֱ�߷���ax+by+c=0(a >= 0)
void  CPathGenerate::makeline(RoadPoint p1,RoadPoint p2,double &a,double &b,double &c)
{
	int sign = 1;
	a=p2.y-p1.y;
	if(a<0)
	{
		sign = -1;
		a=sign*(a);
	}
	b=sign*(p1.x-p2.x);
	c=sign*(p1.y*p2.x-p1.x*p2.y);
}
//�ж��߶κ�ֱ���Ƿ��ཻ
bool  CPathGenerate::Line_Seg_Intersect(double x1,double y1,double x2,double y2,double a,double b,double c)
{
	if ((x1*a+y1*b+c)*(x2*a+y2*b+c)<=0)
	{
		return true;
	}
	else return false;
}

bool CPathGenerate::LineIntersection(double a1,double b1,double c1,double a2,double b2,double c2,RoadPoint &p)
{
	double d=a1*b2-a2*b1;
	if (d==0)
	{
		return false;
	}
	p.x=(c2*b1-c1*b2)/d;
	p.y=(a2*c1-a1*c2)/d;
	return true;
}

//�������ν�ƽ���ߺͶԱߵĽ���
bool CPathGenerate::Angle_Bisector_Intersect(RoadPoint p1,RoadPoint p2,RoadPoint p3,RoadPoint &p)
{
	double A1,B1,C1,A2,B2,C2,A3,B3,C3,A4,B4,C4,A5,B5,C5;
	makeline(p1,p2,A1,B1,C1);
	makeline(p1,p3,A2,B2,C2);
	makeline(p2,p3,A3,B3,C3);

	double Q1,Q2;
	Q1=sqrt(A1*A1+B1*B1);
	Q2=sqrt(A2*A2+B2*B2);
	A4=A1*Q2-A2*Q1;
	B4=B1*Q2-B2*Q1;
	C4=C1*Q2-C2*Q1;

	A5=A1*Q2+A2*Q1;
	B5=B1*Q2+B2*Q1;
	C5=C1*Q2+C2*Q1;

	if (Line_Seg_Intersect(p2.x,p2.y,p3.x,p3.y,A4,B4,C4))
	{
		LineIntersection(A3,B3,C3,A4,B4,C4,p);
	}
	else
	{
		LineIntersection(A3,B3,C3,A5,B5,C5,p);
	}

	return true;
}

double CPathGenerate::AngleNormalnize(double phi)
{
	if(phi>360)phi=phi-360;
	if(phi<0)phi=phi+360;
	return phi/180.0*M_PI;
}


//PID�㷨
double CPathGenerate::MiddlePointPlan(double firMidPoint_x, double firMidPoint_y,double current_position_x,double
	current_position_y,double angle_sensor)
{
	float Ti=0,Td=0;
	//float Kp = 0.4,Kd = Kp*Td/T_PID, Ki = Kp*T_PID/Ti;
	float Kp =0.4,Kd =0, Ki =0;//PID����ϵ��
	static float error_0 =0,error_1= 0,error_2 = 0,delt_u=0,angle_target=0;//error_0��ǰ���,error_1,��һ�����,error_2����һ�����,delt_u��������,angle_targetĿ�곯���
	static float beta = 0,a_theta =0,dis = 0;
	angle_target=atan2((firMidPoint_y-current_position_y),(firMidPoint_x-current_position_x));
	error_0=angle_target-angle_sensor;
	if (error_0<-PI)
		error_0=error_0+2*PI;
	else if (delt_u>PI)
		error_0=error_0-2*PI;

	delt_u=Kp*(error_0-error_1)+Ki*error_0+Kd*(error_0-2*error_1+error_2);
	//if (delt_u<-PI)
	//	delt_u=delt_u+2*PI;
	//else if (delt_u>PI)
	//delt_u=delt_u-2*PI;
	//currentwheelangle+=delt_u;
	if (delt_u>=PI/6)
		delt_u=PI/6;
	else if (delt_u<-PI/6)
		delt_u=-PI/6;

	error_2=error_1;
	error_1=error_0;
	return delt_u;
}


short CPathGenerate::MiddlePiont_Shan(double firMidPoint_x,double firMidPoint_y)
{
	static float a_theta =0;
	static int n=1;
	static float angle_previous=0;
	double a;					//a:����ǰ��ת��
	double delt_b;				//delt_b:������ǰ�����Ԥ�鷽��ļн�  Ԥ�鷽�򣺳����������ĺ�Ԥ��������
	const double L=2.5;	;		//���� L:��ǰ�������.�ݶ�Ϊ2.5��
	double Lf;					//Lforward������������Ԥ���ľ���
	const double delt_y=3;		//���� ��ǰ���е�����������ĵľ��룬y������
	double c;					//������ת��
	double kp=1;				//ת������
	double error=0;
	delt_b = atan(firMidPoint_x /(firMidPoint_y));
	Lf = sqrt((firMidPoint_x*firMidPoint_x) + (firMidPoint_y)*(firMidPoint_y));
	a = atan( 2*L*sin(delt_b) / Lf);
	a_theta = a*180.0*15.0/PI;
	short changeangle;
	changeangle=a_theta+550;                          
	return changeangle;

}