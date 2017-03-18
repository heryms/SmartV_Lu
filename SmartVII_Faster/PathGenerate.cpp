#include "stdafx.h"
#include "PathGenerate.h"
#include"lcm/velodynegrid_t.hpp"
#include "ckCoordinate.h"
#include "ckMapBaseObject.h"
#define  MIN_TRACKRADS   40000
//#include "ckShiftHeader.h"
//当前车辆位置
extern LocationStatus g_curLocation;
extern VelodyneGrid m_vectorGird;
extern ckLcmType::velodynegrid_t m_velodynegrid;

CPathGenerate::CPathGenerate(void)
{	
	flag_IsCloseTo=FALSE;
	CurrentNetPoint=0;
	CurrentNetStatus=0;

	flag_Boundary=0;
	flag_Obstacle=0;
	flag_path=1;
	m_pThreadHandle=NULL;
	m_bStopThread=FALSE;
	m_RoadDataBuffer=NULL;
	m_invertedRoadDataBuffer=NULL;
	fileLength=0;
	m_ultiLength=0;
	m_GENetFileLength=0;
	tarI=GRID_I-1;
	tarJ=GRID_J-1;
	flag_Intersect=0;
	VeloAccCount=0;

	flag_path_HalfGrid=TRUE;
	OffsetVal=0;
	m_ultiLen_linshi=0;
	m_lastUltiLen=0;
	beginPointGPSPos=0;
	startIndex=0;
	currentIndex=0;
	tarIndex=0;

	Obs_Avoidance=0;
	Obs_Avoid_Time=0;
	PJDISTANCE=6;

	EndMode=FALSE;

	SideWalkModeTime=0;

	InitializeCriticalSection(&m_planCs);

}
CPathGenerate::~CPathGenerate(void)
{
	//delete m_pThreadHandle;
	StopThread();	
	if(m_RoadDataBuffer!=NULL)
	{
      delete[] m_RoadDataBuffer;
	  m_RoadDataBuffer=NULL;
	}
	
	if (m_invertedRoadDataBuffer!=NULL)
	{
		delete[] m_invertedRoadDataBuffer;
		m_invertedRoadDataBuffer=NULL;
	}
	DeleteCriticalSection(&m_planCs);
	/*delete[] m_ultiPath;
	delete[] m_pNodes;
	delete[] m_clothoidPoint;*/
}

int CPathGenerate::LongLat2XY(double longitude,double latitude,double &X,double &Y)
{
	int ProjNo=0; int ZoneWide; ////带宽 
	double longitude1,latitude1, longitude0,latitude0, X0,Y0, xval,yval;
	double a,f, e2,ee, NN, T,C,A, M, iPI;
	iPI = 0.0174532925199433; ////3.1415926535898/180.0;
	ZoneWide = 3;////3度带宽
	//ZoneWide = 6; ////6度带宽 
	a=6378245.0; f=1.0/298.3; //54年北京坐标系参数 
	ProjNo = (int)(longitude / ZoneWide+0.5) ; 
	longitude0 = ProjNo * ZoneWide ; 
	longitude0 = longitude0 * iPI ;
	latitude0=0; 
	longitude1 = longitude * iPI ; //经度转换为弧度
	latitude1 = latitude * iPI ; //纬度转换为弧度
	e2=2*f-f*f;
	ee=e2*(1.0-e2);
	NN=a/sqrt(1.0-e2*sin(latitude1)*sin(latitude1));
	T=tan(latitude1)*tan(latitude1);
	C=ee*cos(latitude1)*cos(latitude1);
	A=(longitude1-longitude0)*cos(latitude1); 	

	M=a*((1-e2/4-3*e2*e2/64-5*e2*e2*e2/256)*latitude1-(3*e2/8+3*e2*e2/32+45*e2*e2
		*e2/1024)*sin(2*latitude1)
		+(15*e2*e2/256+45*e2*e2*e2/1024)*sin(4*latitude1)-(35*e2*e2*e2/3072)*sin(6*latitude1));
	xval = NN*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ee)*A*A*A*A*A/120);
	yval = M+NN*tan(latitude1)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
		+(61-58*T+T*T+600*C-330*ee)*A*A*A*A*A*A/720);
	//X0 = 1000000L*(ProjNo+1)+500000L; //6度带
	//X0 = 1000000L*ProjNo+500000L;  //3度带
	X0 = 500000L;  //3度带,不算带号
	Y0 = 0; 
	xval = xval+X0; yval = yval+Y0; 

	X= xval;
	Y= yval;

	return 1;
}
int CPathGenerate::XY2LongLat(double X,double Y,double& longitude,double& latitude)
{
	int ProjNo; int ZoneWide; ////带宽 
	double longitude1,latitude1, longitude0,latitude0(0), X0,Y0, xval,yval;
	double e1,e2,f,a, ee, NN, T,C, M, D,R,u,fai, iPI;
	iPI = 0.0174532925199433; ////3.1415926535898/180.0; 
	///a = 6378245.0; f = 1.0/298.3; //54年北京坐标系参数 
	////a=6378140.0; f=1/298.257; //80年西安坐标系参数
	a = 6378137.0; f = 1.0/298.257223563;//WGS84坐标系参数
	ProjNo = (int)(X/1000000L) ; //查找带号
	// 	ZoneWide = 6; ////6度带宽 
	// 	longitude0 = (ProjNo-1) * ZoneWide + ZoneWide / 2; //计算每带中央子午线经度
	ZoneWide=3;   ////3度带宽
	longitude0 =  ProjNo * ZoneWide;
	longitude0 = longitude0 * iPI ; //中央经线

	X0 = ProjNo*1000000L+500000L; 
	Y0 = 0; 
	xval = X-X0; yval = Y-Y0; //带内大地坐标
	e2 = 2*f-f*f;
	e1 = (1.0-sqrt(1-e2))/(1.0+sqrt(1-e2));
	ee = e2/(1-e2);
	M = yval;
	u = M/(a*(1-e2/4-3*e2*e2/64-5*e2*e2*e2/256));
	fai = u+(3*e1/2-27*e1*e1*e1/32)*sin(2*u)+(21*e1*e1/16-55*e1*e1*e1*e1/32)*sin(4*u)
		+(151*e1*e1*e1/96)*sin(6*u)+(1097*e1*e1*e1*e1/512)*sin(8*u);
	C = ee*cos(fai)*cos(fai);
	T = tan(fai)*tan(fai);
	NN = a/sqrt(1.0-e2*sin(fai)*sin(fai));// 字串1 
	R = a*(1-e2)/sqrt((1-e2*sin(fai)*sin(fai))*(1-e2*sin(fai)*sin(fai))*(1-e2*sin(fai)*sin(fai)));
	D = xval/NN;
	//计算经度(Longitude) 纬度(Latitude)
	longitude1 = longitude0+(D-(1+2*T+C)*D*D*D/6+(5-2*C+28*T-3*C*C+8*ee+24*T*T)*D*D*D*D*D/120)/cos(fai);
	latitude1 = fai -(NN*tan(fai)/R)*(D*D/2-(5+3*T+10*C-4*C*C-9*ee)*D*D*D*D/24+(61+90*T+298*C+45*T*T-256*ee-3*C*C)*D*D*D*D*D*D/720); 
	int g=0;
	//转换为度 DD
	longitude = longitude1 / iPI; 
	latitude = latitude1 / iPI;
	return 1;
}

void CPathGenerate::VeloAccumulation()
{
	OriginPt oriPt;
	oriPt.X=m_velodynegrid.position[0];
	oriPt.Y=m_velodynegrid.position[1];
	oriPt.Angle=m_velodynegrid.orientation[0]*PI/180;

	double newX,newY;
	int I,J;
	if (VeloAccCount>=5)
	{
		VeloAccPoints.erase(VeloAccPoints.begin(),VeloAccPoints.begin()+m_velodynegrid.grid_nums*2);
		//VeloAccPoints.shrink_to_fit();
	}
	for (int i=0;i<m_velodynegrid.grid_nums;i++)
	{
		VeloGrid gridPoint0;
		if ((m_velodynegrid.velo_grid[i]>>4)&1)
		{
			gridPoint0.isObs=TRUE;

		}
		else gridPoint0.isObs=FALSE;

		I=(2*i)%m_velodynegrid.width;
		J=(2*i)/m_velodynegrid.width;
		ckCoordinate::MaptoWorld(oriPt,m_velodynegrid.wid_step*(I-m_velodynegrid.width/2),m_velodynegrid.height_step*J,newX,newY);
		gridPoint0.GauX=newX;
		gridPoint0.GauY=newY;
		gridPoint0.angle=oriPt.Angle;
		VeloAccPoints.push_back(gridPoint0);

		VeloGrid gridPoint1;
		if (m_velodynegrid.velo_grid[i]&1)
		{
			gridPoint1.isObs=TRUE;
		}
		else gridPoint1.isObs=FALSE;

		I=(2*i+1)%m_velodynegrid.width;
		J=(2*i+1)/m_velodynegrid.width;
		ckCoordinate::MaptoWorld(oriPt,m_velodynegrid.wid_step*(I-m_velodynegrid.width/2),m_velodynegrid.height_step*J,newX,newY);
		gridPoint1.GauX=newX;
		gridPoint1.GauY=newY;
		gridPoint1.angle=oriPt.Angle;
		VeloAccPoints.push_back(gridPoint1);
	}

	VeloAccCount++;

}

void CPathGenerate::StartCPathGenerate()
{
#pragma region 读路网点的情况
	if (m_eType==NET)
	{

		while (!m_bStopThread)
		{
			//Sleep(50);
			WaitForSingleObject(g_VeloRcvHandle,INFINITE);


			//RoadPoint pp1,pp2,pp3,pp4;
			//pp1.x=3;
			//pp1.y=-1;
			//pp2.x=1.5;
			//pp2.y=2;
			//pp3.x=-1;
			//pp3.y=-1;
			//Angle_Bisector_Intersect(pp1,pp2,pp3,pp4);
			//TRACE("PP4=%f  %f\n",pp4.x,pp4.y);

			tarI=GRID_I-1;
			tarJ=GRID_J-1;
			flag_Intersect=0;

			PJDISTANCE=6;//什么是拼接距离？？
			////仿真用8.25///////////////////////////////	

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//for (int i=0;i<GRID_I*GRID_J;i++)
			//{
			//	obs_grid[i]=0;
			//}
			////for (int i=0;i<GRID_I;i++)
			////{
			////	/*obs_grid[120*100+i]=1;*/
			////	obs_grid[98*100+i]=1;
			////}

			//for (int j=GRID_J-1;j>=0;j--)
			//{
			//	obs_grid[j*100+42]=1;
			//	obs_grid[j*100+57]=1;

			//}

			//g_CurrentLocation.x=427462.531;      //427474.531234
			//g_CurrentLocation.y=4413649.18;      //4413638.184688
			//g_CurrentLocation.angle=m_roadNetBuffer[0].angle;//弧度


			//g_CurrentLocation.x=m_roadNetBuffer[0].x;      //427474.531234
			//g_CurrentLocation.y=m_roadNetBuffer[0].y;      //4413638.184688
			//g_CurrentLocation.angle=m_roadNetBuffer[0].angle;//弧度

			g_CurrentLocation.x=m_velodynegrid.position[0];
			g_CurrentLocation.y=m_velodynegrid.position[1];
			g_CurrentLocation.angle=m_velodynegrid.orientation[0]*PI/180;


			double newX,newY;
			OriginPt oriPoint;//坐标转换的原点
			oriPoint.X=g_CurrentLocation.x;
			oriPoint.Y=g_CurrentLocation.y;
			oriPoint.Angle=g_CurrentLocation.angle;

#pragma region 获取网格障碍物信息

			//网格四个角的坐标
			GridLeftDown.x=g_CurrentLocation.x-GRID_I/2*GRID_WIDTH_X*sin(g_CurrentLocation.angle+PI/2);
			GridLeftDown.y=g_CurrentLocation.y-GRID_I/2*GRID_WIDTH_X*cos(g_CurrentLocation.angle+PI/2);
			GridLeftDown.angle=g_CurrentLocation.angle;

			GridRightDown.x=g_CurrentLocation.x+GRID_I/2*GRID_WIDTH_X*sin(g_CurrentLocation.angle+PI/2);
			GridRightDown.y=g_CurrentLocation.y+GRID_I/2*GRID_WIDTH_X*cos(g_CurrentLocation.angle+PI/2);
			GridRightDown.angle=g_CurrentLocation.angle;

			GridLeftUp.x=GridLeftDown.x+GRID_J*GRID_WIDTH_Y*sin(GridLeftDown.angle);
			GridLeftUp.y=GridLeftDown.y+GRID_J*GRID_WIDTH_Y*cos(GridLeftDown.angle);
			GridLeftUp.angle=g_CurrentLocation.angle;

			GridRightUp.x=GridRightDown.x+GRID_J*GRID_WIDTH_Y*sin(GridRightDown.angle);
			GridRightUp.y=GridRightDown.y+GRID_J*GRID_WIDTH_Y*cos(GridRightDown.angle);
			GridRightUp.angle=g_CurrentLocation.angle;



			//获取网格障碍物信息
			for (int i=0;i<m_velodynegrid.grid_nums;i++)
			{
				if ((m_velodynegrid.velo_grid[i]>>4)&1)//??
				{
					obs_grid[i*2]=1;
				}
				else obs_grid[i*2]=0;

				if (m_velodynegrid.velo_grid[i]&1)
				{
					obs_grid[i*2+1]=1;
				}
				else obs_grid[i*2+1]=0;
			}

			////获取叠加后的障碍物信息////////////////////////////////////////////////////////
			//VeloAccumulation();
			//int countTol=0;
			//if (VeloAccCount>=5)
			//{
			//	countTol=1;
			//}
			//int obs_grid_count[GRID_I*GRID_J]={0}; 
			//int I,J;
			//for (int i=0;i<VeloAccPoints.size();i++)
			//{
			//	ckCoordinate::WorldtoMap(oriPoint,VeloAccPoints[i].GauX,VeloAccPoints[i].GauY,newX,newY);
			//	I=newX/GRID_WIDTH_X+GRID_I/2;
			//	J=newY/GRID_WIDTH_Y;
			//	if (VeloAccPoints[i].isObs==TRUE)
			//	{
			//		obs_grid_count[J*100+I]+=1;
			//	}
			//}
			//for (int i=0;i<GRID_I*GRID_J;i++)
			//{
			//	if (obs_grid_count[i]>countTol)
			//	{
			//		obs_grid[i]=1;
			//	}
			//	else obs_grid[i]=0;
			//}
			//////////////////////////////////////////////////////////////////////////////////
#pragma endregion 获取网格障碍物信息


#pragma region 路径规划的起始点
			////路径规划起始点：m_beginPoint，以上一路径中与当前点最近的点作为起始点，获取其坐标和角度
			//int minGps=MAXD;
			//int beginIndex=0;
			//if (m_ultiLength==0)//if (1)//
			//{
			//	m_beginPoint.x=g_CurrentLocation.x;
			//	m_beginPoint.y=g_CurrentLocation.y;
			//	if (g_CurrentLocation.angle<=PI/2)
			//	{
			//		m_beginPoint.angle=PI/2-g_CurrentLocation.angle;
			//	}
			//	else m_beginPoint.angle=5*PI/2-g_CurrentLocation.angle;
			//}
			//else
			//{
			//	//有可能改成与m_ultiPath_HalfGrid_Gussian[]比较
			//	for(int i=0;i<m_ultiLength;i++)
			//	{
			//		double dis=(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)*(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)
			//			+(g_CurrentLocation.y-m_ultiPath_Gussian[i].y)*(g_CurrentLocation.y-m_ultiPath_Gussian[i].y);
			//		if(dis<minGps)
			//		{
			//			minGps=dis;
			//			beginIndex=i;
			//		}
			//	}

			//	m_beginPoint.x=m_ultiPath_Gussian[beginIndex].x;
			//	m_beginPoint.y=m_ultiPath_Gussian[beginIndex].y;
			//	m_beginPoint.angle=m_ultiPath_Gussian[beginIndex].angle;
			//}


			//路径规划起始点，以上一路径中与当前点最近的点，距离为6米的点作为起始点，获取其坐标和角度
			double minGps=MAXD;
			currentIndex=0;//当前车体位置在上一路径中的序号
			startIndex=0;  //规划起始点在上一路径中的序号

			//TRACE("m_ultiLength=%d   ",m_ultiLength);

			if (m_ultiLength==0)//if(1)//
			{
				m_beginPoint.x=g_CurrentLocation.x;
				m_beginPoint.y=g_CurrentLocation.y;
				if (g_CurrentLocation.angle<=PI/2)
				{
					m_beginPoint.angle=PI/2-g_CurrentLocation.angle;
				}
				else m_beginPoint.angle=5*PI/2-g_CurrentLocation.angle;
			}
			else
			{
				for(int i=0;i<m_ultiLength;i++)
				{
					double dis=(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)*(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)
						+(g_CurrentLocation.y-m_ultiPath_Gussian[i].y)*(g_CurrentLocation.y-m_ultiPath_Gussian[i].y);
					if(dis<minGps)
					{
						minGps=dis;
						currentIndex=i;
					}
				}

				//TRACE("currentIndex=%d",currentIndex);

				for (int i=currentIndex+1;i<m_ultiLength;i++)
				{
					double dis=sqrt(Distance(m_ultiPath_Gussian[i],m_ultiPath_Gussian[currentIndex]));
					if (dis>=PJDISTANCE)
					{
						startIndex=i;
						break;
					}
				}

				if (startIndex==0)
				{
					startIndex=m_ultiLength-1;
				}

				//TRACE("    startIndex=%d\n",startIndex);

				m_beginPoint.x=m_ultiPath_Gussian[startIndex].x;
				m_beginPoint.y=m_ultiPath_Gussian[startIndex].y;
				m_beginPoint.angle=m_ultiPath_Gussian[startIndex].angle;
			}

#pragma endregion 路径规划的起始点


			//返回编号为当前点的上一个点在数组中的索引号
			CurrentNetPoint=GetCurrentPosition(CurrentNetPoint);


#pragma region 下一路网点和起始点连线与网格的关系flag_Intersect，得到可通行判断的起点OriginStart和终点OriginEnd
			////计算下一路网点和起始点连线的斜率
			//double delta_x=m_roadNetBuffer[CurrentNetPoint+1].x-m_beginPoint.x;
			//double delta_y=m_roadNetBuffer[CurrentNetPoint+1].y-m_beginPoint.y;
			//double k=delta_y/delta_x;

			RoadPoint nextNetPoint;
			nextNetPoint.x=m_roadNetBuffer[CurrentNetPoint+1].x;
			nextNetPoint.y=m_roadNetBuffer[CurrentNetPoint+1].y;

			if (sqrt(Distance(m_beginPoint,nextNetPoint))<6)
			{
				nextNetPoint.x=m_roadNetBuffer[CurrentNetPoint+2].x;
				nextNetPoint.y=m_roadNetBuffer[CurrentNetPoint+2].y;
			}

			//double xxxx=sqrt(Distance(g_CurrentLocation,nextNetPoint));
			//double yyyy=PointToSegDist(GridLeftUp.x,GridLeftUp.y,GridLeftDown.x,GridLeftDown.y,GridRightDown.x,GridRightDown.y);

			//GridPoint start,end;//用于可通行点判断的起点和终点两个网格点，X,Y是网格的列和行编号

			RoadPoint intersectPoint;
			//求出起始点在网格中的哪个点start(或者连线与网格底部的交点)
			LINESEG u,v;
			u.s=GridLeftDown;
			u.e=GridRightDown;
			v.s=m_beginPoint;
			v.e=nextNetPoint;
			if (intersect(u,v))  //判断连线是否与网格底线相交,
			{
				XXX11=11;
				//如果相交，以交点所在的行列号作为判断是否可通行的起点
				 OriginStart.Y=0;
				//求连线与网格底线的交点
				intersectPoint=getIntersectPoint(GridLeftDown,GridRightDown,m_beginPoint,nextNetPoint);
				OriginStart.X=sqrt(Distance(GridLeftDown,intersectPoint))/GRID_WIDTH_X;
			}
			else
			{
				XXX11=90;
				//如果不相交，计算起始点所在的行列号作为判断是否可通行的起点

				ckCoordinate::WorldtoMap(oriPoint,m_beginPoint.x,m_beginPoint.y,newX,newY);
				OriginStart.X=newX/GRID_WIDTH_X+GRID_I/2;
				OriginStart.Y=newY/GRID_WIDTH_Y;
			}

			//求出下一路网点和起始点连线与网格顶部的交点end（如果下一路网点和当前点很近了？）
			u.s=GridLeftUp;
			u.e=GridRightUp;
			v.s=m_beginPoint;
			v.e=nextNetPoint;
			if (intersect(u,v))  //判断连线是否与网格顶部线相交
			{
				flag_Intersect=1;

				//如果相交，以交点所在的行列号作为判断是否可通行的终点
				OriginEnd.Y=GRID_J-1;
				//求连线与网格顶线的交点
				intersectPoint=getIntersectPoint(GridLeftUp,GridRightUp,m_beginPoint,nextNetPoint);
				OriginEnd.X=sqrt(Distance(GridLeftUp,intersectPoint))/GRID_WIDTH_X;
			}
			else
			{
				//判断是否与左边界相交
				u.s=GridLeftDown;
				u.e=GridLeftUp;
				if (intersect(u,v))
				{
					flag_Intersect=2;
					//如果相交，以交点所在的行列号作为判断是否可通行的终点
					OriginEnd.X=0;
					//求连线与网格左边界的交点
					intersectPoint=getIntersectPoint(GridLeftDown,GridLeftUp,m_beginPoint,nextNetPoint);
					OriginEnd.Y=sqrt(Distance(GridLeftDown,intersectPoint))/GRID_WIDTH_Y;

				}
				else   
				{
					//判断是否与右边界相交
					u.s=GridRightDown;
					u.e=GridRightUp;
					if (intersect(u,v))
					{
						flag_Intersect=3;
						//如果相交，以交点所在的行列号作为判断是否可通行的终点
						OriginEnd.X=GRID_I-1;
						//求连线与网格右边界的交点
						intersectPoint=getIntersectPoint(GridRightDown,GridRightUp,m_beginPoint,nextNetPoint);
						OriginEnd.Y=sqrt(Distance(GridRightDown,intersectPoint))/GRID_WIDTH_Y;
					}
					else//下一路网点有可能在网格内，也有可能在网格下方
					{
						//计算下一路网点所在的行列号作为判断是否可通行的终点

						ckCoordinate::WorldtoMap(oriPoint,nextNetPoint.x,nextNetPoint.y,newX,newY);
						OriginEnd.X=newX/GRID_WIDTH_X+GRID_I/2;
						OriginEnd.Y=newY/GRID_WIDTH_Y;

						double A,B,C;
						makeline(m_beginPoint,nextNetPoint,A,B,C);//下一路网点和起始点连线的直线
						//直线是否和网格顶端相交
						if (Line_Seg_Intersect(GridLeftUp.x,GridLeftUp.y,GridRightUp.x,GridRightUp.y,A,B,C))
						{
							flag_Intersect=4;
							
						}
						else if (Line_Seg_Intersect(GridLeftDown.x,GridLeftUp.y,GridRightUp.x,GridRightUp.y,A,B,C))//直线是否和网格左边界相交
						{
							flag_Intersect=5;

						}
						else if (newY>=0)
						{ 
							flag_Intersect=6;//直线和网格右边界相交
							

						}
						else if (newY<0&&newX<0)//下一路网点在网格左下方
						{
							flag_Intersect=7;
						}
						else flag_Intersect=8;//下一路网点在网格右下方
					}
				}
			}

#pragma endregion 
			//根据下一路网点和网格数据寻找可通行点
			FindAvailableTar(obs_grid);


			if (tarI==0&&tarJ==0)//未找到可通行点
			{
				flag_path=FALSE;				
				continue;
			}



			ckCoordinate::MaptoWorld(oriPoint,(tarI-GRID_I/2)*GRID_WIDTH_X,tarJ*GRID_WIDTH_Y,newX,newY);
			m_tarPoint.x=newX;
			m_tarPoint.y=newY;


			RoadPoint RP;
			double deltaAngle = abs(m_roadNetBuffer[CurrentNetPoint].angle-m_beginPoint.angle);
			if(deltaAngle>3.0*PI/2.0) deltaAngle=abs(deltaAngle-2.0*PI);
			if (deltaAngle>PI/6.0)
			{
				Angle_Bisector_Intersect(m_beginPoint,m_tarPoint,nextNetPoint,RP);
				//计算方向
				double delta_x=RP.x-m_beginPoint.x;
				double delta_y=RP.y-m_beginPoint.y;
				double angle=atan(delta_x/delta_y);
				if(delta_y>0&&delta_x>0)
				{
					angle=angle;
				}
				else if(delta_y<0&&delta_x>0)
				{
					angle=angle+PI;
				}
				else if(delta_x<0&&delta_y<0)
				{
					angle=angle+PI;
				}
				else if(delta_x<0&&delta_y>0)
				{
					angle=angle+2*PI;
				}

				if (angle<PI/2)
				{
					m_tarPoint.angle=PI/2-angle;
				}
				else m_tarPoint.angle=5*PI/2-angle;
			}
			else
			{
				//以当前路网点和下一路网点的连线方向作为目标点的方向(角度)            //9.16用高斯坐标的弧度
				if (m_roadNetBuffer[CurrentNetPoint].angle<PI/2)
				{
					m_tarPoint.angle=PI/2-m_roadNetBuffer[CurrentNetPoint].angle;
				}
				else m_tarPoint.angle=5*PI/2-m_roadNetBuffer[CurrentNetPoint].angle;
			}
			
			



			////以当前路网点和下一路网点的连线方向作为目标点的方向(角度)            //9.16用高斯坐标的弧度
			//if (m_roadNetBuffer[CurrentNetPoint].angle<PI/2)
			//{
			//	m_tarPoint.angle=PI/2-m_roadNetBuffer[CurrentNetPoint].angle;
			//}
			//else m_tarPoint.angle=5*PI/2-m_roadNetBuffer[CurrentNetPoint].angle;

			//SYSTEMTIME st2;
			//GetLocalTime(&st2);
			//int t2=st2.wMilliseconds;
			/*int deltaT1=t2-t1;*/

			int flag_Av=1;
			if (tarJ<=OriginStart.Y)
			{
				flag_Av=0;
			}

			while(flag_Av==0||!PathExtend())
			{
				flag_Av=1;
				FindTarInGrid(tarI,tarJ);

				if (tarI<0||tarJ<0)
				{
					flag_path=FALSE;
					break;
				}
				if (tarI==0&&tarJ==0)
				{
					flag_path=FALSE;
					break;
				}

				if (tarJ<=OriginStart.Y)
				{
					continue;
				}

				ckCoordinate::MaptoWorld(oriPoint,(tarI-GRID_I/2)*GRID_WIDTH_X,tarJ*GRID_WIDTH_Y,newX,newY);
				m_tarPoint.x=newX;
				m_tarPoint.y=newY;

				if (deltaAngle>PI/6.0)
				{
					Angle_Bisector_Intersect(m_beginPoint,m_tarPoint,nextNetPoint,RP);
					//计算方向
					double delta_x=RP.x-m_beginPoint.x;
					double delta_y=RP.y-m_beginPoint.y;
					double angle=atan(delta_x/delta_y);
					if(delta_y>0&&delta_x>0)
					{
						angle=angle;
					}
					else if(delta_y<0&&delta_x>0)
					{
						angle=angle+PI;
					}
					else if(delta_x<0&&delta_y<0)
					{
						angle=angle+PI;
					}
					else if(delta_x<0&&delta_y>0)
					{
						angle=angle+2*PI;
					}
					if (angle<PI/2)
					{
						m_tarPoint.angle=PI/2-angle;
					}
					else m_tarPoint.angle=5*PI/2-angle;
				}
				

				
				if ((tarI-GRID_I/2)*(tarI-GRID_I/2)+tarJ*tarJ<900)//如果目标点距离起始点距离小于6米
				{
					flag_path=FALSE;
					//IsPathExtend=0;
					break;
				}
			}

			//SYSTEMTIME st3;
			//GetLocalTime(&st3);
			//int t3=st3.wMilliseconds;
			/*int deltaT2=t3-t1;*/


			ResetEvent(g_VeloRcvHandle);
		}
	}
#pragma endregion 读路网点的情况
#pragma region 读GPS轨迹点的情况
	else if (m_eType==TRACK)
	{

#pragma region 新写的GPS轨迹偏移，暂不使用逐行或逐列扫描
		//新写的GPS轨迹偏移，暂不使用逐行或逐列扫描

		while (!m_bStopThread)
		{
			
			//Sleep(50);
			WaitForSingleObject(g_VeloRcvHandle,INFINITE);
			tarI=GRID_I-1;
			tarJ=GRID_J-1;
			flag_Intersect=0;

			//double curSpeed=sqrt(g_curLocation.eVelocity*g_curLocation.eVelocity+g_curLocation.nVelocity*g_curLocation.nVelocity+g_curLocation.upVelocity*g_curLocation.upVelocity);
			PJDISTANCE=10;

			//g_CurrentLocation.x=m_RoadDataBuffer[45].x;
			//g_CurrentLocation.y=m_RoadDataBuffer[45].y;
			//g_CurrentLocation.angle=AngleNormalnize(m_RoadDataBuffer[45].angle);

			//当前位置
			g_CurrentLocation.x=m_velodynegrid.position[0];
			g_CurrentLocation.y=m_velodynegrid.position[1];
			g_CurrentLocation.angle=m_velodynegrid.orientation[0]*PI/180;

			//TRACE("g_Cur.x=%f  ",g_CurrentLocation.x);
			//TRACE("g_Cur.y=%f  ",g_CurrentLocation.y);
			//TRACE("ultiPath0.x=%f  ",m_ultiPath_Gussian[0].x);
			//TRACE("ultiPath0.y=%f  ",m_ultiPath_Gussian[0].y);

			double newX,newY;
			OriginPt oriPoint;//坐标转换的原点
			oriPoint.X=g_CurrentLocation.x;
			oriPoint.Y=g_CurrentLocation.y;
			oriPoint.Angle=g_CurrentLocation.angle;

#pragma region 获取网格障碍物信息

			//网格四个角的坐标
			GridLeftDown.x=g_CurrentLocation.x-GRID_I/2*GRID_WIDTH_X*sin(g_CurrentLocation.angle+PI/2);
			GridLeftDown.y=g_CurrentLocation.y-GRID_I/2*GRID_WIDTH_X*cos(g_CurrentLocation.angle+PI/2);
			GridLeftDown.angle=g_CurrentLocation.angle;

			GridRightDown.x=g_CurrentLocation.x+GRID_I/2*GRID_WIDTH_X*sin(g_CurrentLocation.angle+PI/2);
			GridRightDown.y=g_CurrentLocation.y+GRID_I/2*GRID_WIDTH_X*cos(g_CurrentLocation.angle+PI/2);
			GridRightDown.angle=g_CurrentLocation.angle;

			GridLeftUp.x=GridLeftDown.x+GRID_J*GRID_WIDTH_Y*sin(GridLeftDown.angle);
			GridLeftUp.y=GridLeftDown.y+GRID_J*GRID_WIDTH_Y*cos(GridLeftDown.angle);
			GridLeftUp.angle=g_CurrentLocation.angle;

			GridRightUp.x=GridRightDown.x+GRID_J*GRID_WIDTH_Y*sin(GridRightDown.angle);
			GridRightUp.y=GridRightDown.y+GRID_J*GRID_WIDTH_Y*cos(GridRightDown.angle);
			GridRightUp.angle=g_CurrentLocation.angle;



			//获取网格障碍物信息
			for (int i=0;i<m_velodynegrid.grid_nums;i++)
			{
				if ((m_velodynegrid.velo_grid[i]>>4)&1)
				{
					obs_grid[i*2]=1;
				}
				else obs_grid[i*2]=0;

				if (m_velodynegrid.velo_grid[i]&1)
				{
					obs_grid[i*2+1]=1;
				}
				else obs_grid[i*2+1]=0;
			}

			//////获取叠加后的障碍物信息////////////////////////////////////////////////////////
			//VeloAccumulation();
			//int countTol=0;
			//if (VeloAccCount>=50)
			//{
			//	countTol=0;
			//}
			//int obs_grid_count[GRID_I*GRID_J]={0}; 
			//int I,J;
			//for (int i=0;i<VeloAccPoints.size();i++)
			//{
			//	ckCoordinate::WorldtoMap(oriPoint,VeloAccPoints[i].GauX,VeloAccPoints[i].GauY,newX,newY);
			//	I=newX/GRID_WIDTH_X+GRID_I/2;
			//	J=newY/GRID_WIDTH_Y;
			//	if (VeloAccPoints[i].isObs==TRUE)
			//	{
			//		obs_grid_count[J*100+I]+=1;
			//	}
			//}
			//for (int i=0;i<GRID_I*GRID_J;i++)       
			//{
			//	if (obs_grid_count[i]>countTol)
			//	{
			//		obs_grid[i]=1;
			//	}
			//	else obs_grid[i]=0;
			//}
			//////////////////////////////////////////////////////////////////////////////////
#pragma endregion 获取网格障碍物信息

			BOOL flag_througth=FALSE;
			double minGps=MAXD;

#pragma region 终点模式
/////////////////终点模式//////////////////////////////////
			if (tarIndex==(fileLength-1))
			{
				EndMode=TRUE;
				double minGps1=MAXD;
				for(int i=0;i<m_ultiLength;i++)
				{
					double dis=(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)*(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)
						+(g_CurrentLocation.y-m_ultiPath_Gussian[i].y)*(g_CurrentLocation.y-m_ultiPath_Gussian[i].y);
					if(dis<minGps1)
					{
						minGps1=dis;
						currentIndex=i;
					}
				}

				m_beginPoint.x=m_ultiPath_Gussian[currentIndex].x;
				m_beginPoint.y=m_ultiPath_Gussian[currentIndex].y;
				m_beginPoint.angle=m_ultiPath_Gussian[currentIndex].angle;

			}
///////////////////////////////////////////////////////
#pragma endregion 终点模式
			else
			{

#pragma region 路径规划的起始点
				////路径规划起始点：m_beginPoint，以上一路径中与当前点最近的点作为起始点，获取其坐标和角度
				//int minGps=MAXD;
				//int beginIndex=0;
				//if (m_ultiLength==0)//if (1)//
				//{
				//	m_beginPoint.x=g_CurrentLocation.x;
				//	m_beginPoint.y=g_CurrentLocation.y;
				//	if (g_CurrentLocation.angle<=PI/2)
				//	{
				//		m_beginPoint.angle=PI/2-g_CurrentLocation.angle;
				//	}
				//	else m_beginPoint.angle=5*PI/2-g_CurrentLocation.angle;
				//}
				//else
				//{
				//	//有可能改成与m_ultiPath_HalfGrid_Gussian[]比较
				//	for(int i=0;i<m_ultiLength;i++)
				//	{
				//		double dis=(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)*(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)
				//			+(g_CurrentLocation.y-m_ultiPath_Gussian[i].y)*(g_CurrentLocation.y-m_ultiPath_Gussian[i].y);
				//		if(dis<minGps)
				//		{
				//			minGps=dis;
				//			beginIndex=i;
				//		}
				//	}

				//	m_beginPoint.x=m_ultiPath_Gussian[beginIndex].x;
				//	m_beginPoint.y=m_ultiPath_Gussian[beginIndex].y;
				//	m_beginPoint.angle=m_ultiPath_Gussian[beginIndex].angle;
				//}


				//路径规划起始点，以上一路径中与当前点最近的点，距离为6米的点作为起始点，获取其坐标和角度
				minGps=MAXD;
				currentIndex=0;//当前车体位置在上一路径中的序号
				startIndex=0;  //规划起始点在上一路径中的序号

				//TRACE("m_ultiLength=%d   ",m_ultiLength);

				if (m_ultiLength==0)
				{
					m_beginPoint.x=g_CurrentLocation.x;
					m_beginPoint.y=g_CurrentLocation.y;
					m_beginPoint.angle=AngleTransfer(g_CurrentLocation.angle);

					//if (g_CurrentLocation.angle<=PI/2)
					//{
					//	m_beginPoint.angle=PI/2-g_CurrentLocation.angle;
					//}
					//else m_beginPoint.angle=5*PI/2-g_CurrentLocation.angle;
				}
				else
				{
					for(int i=0;i<m_ultiLength;i++)
					{
						double dis=(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)*(g_CurrentLocation.x-m_ultiPath_Gussian[i].x)
							+(g_CurrentLocation.y-m_ultiPath_Gussian[i].y)*(g_CurrentLocation.y-m_ultiPath_Gussian[i].y);
						if(dis<minGps)
						{
							minGps=dis;
							currentIndex=i;
						}
					}

					//TRACE("currentIndex=%d",currentIndex);

					for (int i=currentIndex+1;i<m_ultiLength;i++)
					{
						double dis=sqrt(Distance(m_ultiPath_Gussian[i],m_ultiPath_Gussian[currentIndex]));
						if (dis>=PJDISTANCE)
						{
							startIndex=i;
							break;
						}
					}

					if (startIndex==0)
					{
						startIndex=m_ultiLength-1;
					}

					//TRACE("    startIndex=%d\n",startIndex);

					m_beginPoint.x=m_ultiPath_Gussian[startIndex].x;
					m_beginPoint.y=m_ultiPath_Gussian[startIndex].y;
					m_beginPoint.angle=m_ultiPath_Gussian[startIndex].angle;
				}

#pragma endregion 路径规划的起始点


#pragma region 行人检测
				//SideWalkMode=1;
				//if (SideWalkMode==1)
				//{
				//	if (SideWalkModeTime==0)
				//	{
				//		HumanExist=FALSE;//本来是False
				//		SideWalkAccess=TRUE;

				//		dataCount=0;
				//		SideWalkModeTime=0;
				//		LastHumanLocationLeft=MAXD;
				//		LastHumanLocationRight=MAXD;
				//		HumanLocationLeft=MAXD;//左边行人位置
				//		HumanLocationRight=MAXD;//右边行人位置

				//		m_beginPoint.x=g_CurrentLocation.x;
				//		m_beginPoint.y=g_CurrentLocation.y;
				//		m_beginPoint.angle=AngleTransfer(g_CurrentLocation.angle);


				//		GpsLine trackPt;
				//		trackPt.Gnx=m_beginPoint.x;
				//		trackPt.Gny=m_beginPoint.y;
				//		trackPt.azimuth=AngleTransfer(m_beginPoint.angle);
				//		beginPointGPSPos=GetCloseTrackPoint(trackPt);

				//		double dis35=0;
				//		for (int i=beginPointGPSPos+1;i<fileLength;i++)
				//		{

				//			dis35+=sqrt(Distance(m_RoadDataBuffer[i],m_RoadDataBuffer[i-1]));

				//			if (dis35>=35)
				//			{
				//				tarIndex=i;
				//				break;
				//			}
				//		}


				//		//对GPS点加密

				//		RoadPoint addData[1000];
				//		for (int i=0;i<tarIndex-beginPointGPSPos;i++)
				//		{
				//			double lineangle=ckMapBaseObject::AmizthCaltulate(m_RoadDataBuffer[beginPointGPSPos+i].x,m_RoadDataBuffer[beginPointGPSPos+i].y,m_RoadDataBuffer[beginPointGPSPos+i+1].x,m_RoadDataBuffer[beginPointGPSPos+i+1].y);

				//			int m=sqrt(Distance(m_RoadDataBuffer[beginPointGPSPos+i+1],m_RoadDataBuffer[beginPointGPSPos+i]))/0.5;

				//			if (m<=1)
				//			{
				//				addData[dataCount].x=m_RoadDataBuffer[beginPointGPSPos+i].x;
				//				addData[dataCount].y=m_RoadDataBuffer[beginPointGPSPos+i].y;
				//				dataCount++;
				//			}
				//			else
				//			{
				//				for (int j=0;j<m;j++)
				//				{
				//					addData[dataCount].x=m_RoadDataBuffer[beginPointGPSPos+i].x+0.5*j*sin(lineangle);
				//					addData[dataCount].y=m_RoadDataBuffer[beginPointGPSPos+i].y+0.5*j*cos(lineangle);
				//					dataCount++;
				//				}
				//			}
				//		}
				//		addData[dataCount]=m_RoadDataBuffer[tarIndex];


				//		for (int i=0;i<=dataCount;i++)
				//		{
				//			GpsForSideWalkMode[i]=addData[i];
				//		}

				//		//获取道路大致宽度（左右）
				//		//向左探测获取左边宽度
				//		int testLeftLine;
				//		int pointNum=flag_TrackBufferInObsGrid(GpsForSideWalkMode,0,dataCount,-1,testLeftLine);
				//		if (pointNum==MAXD)
				//		{
				//			leftWide=MAXD;
				//		}
				//		else
				//		{
				//			//leftLine=testLeftLine;

				//			for (int i=pointNum;i<=(dataCount-5);i++)
				//			{
				//				addData[i]=GpsForSideWalkMode[i+5];
				//			}

				//			pointNum=flag_TrackBufferInObsGrid(addData,0,dataCount-5,-1,testLeftLine);

				//			if (pointNum==MAXD)
				//			{
				//				leftWide=MAXD;
				//			}
				//			else leftWide=testLeftLine;

				//		}


				//		//向右探测获取右边宽度
				//		int testRightLine;
				//		pointNum=flag_TrackBufferInObsGrid(GpsForSideWalkMode,0,dataCount,1,testRightLine);
				//		if (pointNum==MAXD)
				//		{
				//			rightWide=MAXD;
				//		}
				//		else
				//		{
				//			//rightLine=testRightLine;

				//			for (int i=pointNum;i<=(dataCount-5);i++)
				//			{
				//				addData[i]=GpsForSideWalkMode[i+5];
				//			}

				//			pointNum=flag_TrackBufferInObsGrid(addData,0,dataCount-5,1,testRightLine);

				//			if (pointNum==MAXD)
				//			{
				//				rightWide=MAXD;
				//			}
				//			else rightWide=testRightLine;

				//		}

				//	}
				//	else
				//	{
				//		LastHumanLocationLeft=HumanLocationLeft;
				//		LastHumanLocationRight=HumanLocationRight;


				//		//向左探测
				//		int testLeftLine;
				//		int pointNum=flag_TrackBufferInObsGrid(GpsForSideWalkMode,0,dataCount,-1,testLeftLine);
				//		TRACE("leftWide=%d\n",leftWide);
				//		TRACE("testLeftLine=%d\n",testLeftLine);
				//		if (pointNum==MAXD)
				//		{
				//			HumanLocationLeft=MAXD;
				//		}
				//		else
				//		{
				//			if (leftWide-testLeftLine>=7)
				//			{
				//				HumanLocationLeft=testLeftLine;
				//				if (HumanExist==FALSE)
				//				{
				//					HumanExist=TRUE;
				//					SideWalkAccess=FALSE;
				//					//AfxMessageBox("有行人");
				//					TRACE("有行人");
				//				}

				//			}

				//		}


				//		//向右探测
				//		int testRightLine;
				//		pointNum=flag_TrackBufferInObsGrid(GpsForSideWalkMode,0,dataCount,1,testRightLine);
				//		TRACE("rightWide=%d\n",rightWide);
				//		TRACE("testRightLine=%d\n",testRightLine);
				//		if (pointNum==MAXD)
				//		{
				//			HumanLocationRight=MAXD;
				//		}
				//		else
				//		{
				//			if (rightWide-testRightLine>=7)
				//			{
				//				HumanLocationRight=testRightLine;
				//				if (HumanExist==FALSE)
				//				{
				//					HumanExist=TRUE;
				//					SideWalkAccess=FALSE;
				//					//AfxMessageBox("有行人!!");
				//					TRACE("有行人!!");
				//				}
				//			}
				//		}

				//		if (HumanExist==TRUE)
				//		{
				//			//判断是否可以通行
				//			BOOL LeftAccess=FALSE;
				//			if ((leftWide-HumanLocationLeft<=2)&&(leftWide-LastHumanLocationLeft<=2))//说明左边没人
				//			{
				//				LeftAccess=TRUE;
				//			}
				//			else
				//			{
				//				if ((HumanLocationLeft>LastHumanLocationLeft)&&HumanLocationLeft>10)
				//				{
				//					LeftAccess=TRUE;
				//				}
				//			}

				//			BOOL RightAccess=FALSE;
				//			if ((rightWide-HumanLocationRight<=2)&&(rightWide-LastHumanLocationRight<=2))
				//			{
				//				RightAccess=TRUE;
				//			}
				//			else
				//			{
				//				if ((HumanLocationRight>LastHumanLocationRight)&&HumanLocationRight>10)
				//				{
				//					RightAccess=TRUE;
				//				}
				//			}

				//			if (LeftAccess==TRUE&&RightAccess==TRUE)
				//			{
				//				SideWalkAccess=TRUE;  //可以通行了
				//				HumanExist=FALSE;
				//				AfxMessageBox("可通行");
				//			}
				//		}
				//	}

				//	SideWalkModeTime++;


				//	//if (SideWalkModeTime>2&&HumanExist==FALSE)
				//	//{
				//	//	//没有行人
				//	//	SideWalkAccess=TRUE;
				//	//	AfxMessageBox("没有行人");
				//	//	
				//	//}

				//	if (SideWalkAccess==FALSE)
				//	{
				//		//flag_path=FALSE;
				//		m_ultiLength=0;
				//		
				//	}
				//	if (SideWalkModeTime>15&&HumanExist==FALSE)
				//	{
				//		AfxMessageBox("无人，可通行！");
				//	}
				//	Sleep(2000);
				//	ResetEvent(g_VeloRcvHandle);
				//	continue;

				//}
				//else
				//{
				//	if (SideWalkModeTime>0)
				//	{
				//		SideWalkModeTime=0;
				//	}
				//}
#pragma endregion 行人检测


				minGps=MAXD;
				//int CurrentGpsPosition=0;  //保存与当前位置最近的点的序号
				////CurrentGpsPosition=GetCurrentPosition(CurrentGpsPosition);//查找当前坐标在整个gps路径里面的位置，取最近点为它的位置...
				//for(int i=0;i<fileLength;i++)
				//{
				//	double dis=(g_CurrentLocation.x-m_RoadDataBuffer[i].x)*(g_CurrentLocation.x-m_RoadDataBuffer[i].x)
				//		+(g_CurrentLocation.y-m_RoadDataBuffer[i].y)*(g_CurrentLocation.y-m_RoadDataBuffer[i].y);
				//	if(dis<minGps)
				//	{
				//		minGps=dis;
				//		CurrentGpsPosition=i;
				//	}
				//}

				////beginPointGPSPos=0;//规划起始点位置最近的GPS点的序号
				//for(int i=0;i<fileLength;i++)
				//{
				//	double dis=(m_beginPoint.x-m_RoadDataBuffer[i].x)*(m_beginPoint.x-m_RoadDataBuffer[i].x)
				//		+(m_beginPoint.y-m_RoadDataBuffer[i].y)*(m_beginPoint.y-m_RoadDataBuffer[i].y);
				//	if(dis<minGps)
				//	{
				//		minGps=dis;
				//		beginPointGPSPos=i;
				//	}
				//}

				GpsLine trackPoint;
				trackPoint.Gnx=m_beginPoint.x;
				trackPoint.Gny=m_beginPoint.y;
				trackPoint.azimuth=AngleTransfer(m_beginPoint.angle);
				beginPointGPSPos=GetCloseTrackPoint(trackPoint);
				//if (beginPointGPSPos==-1)
				//{
				//	flag_path=FALSE;
				//	m_ultiLength=0;

				//}
				//TRACE("beginPointGPSPos=%d",beginPointGPSPos);
				tarIndex=0;//获取20米距离的GPS轨迹点
				int dis6=0;//
				double dis=0;
				for (int i=beginPointGPSPos+1;i<fileLength;i++)
				{
					dis+=sqrt(Distance(m_RoadDataBuffer[i],m_RoadDataBuffer[i-1]));
					if (dis>=6&&dis6==0)
					{
						dis6=i;
					}
					if (dis>=20)
					{
						tarIndex=i;
						break;
					}
				}
				if (tarIndex==0)
				{
					tarIndex=fileLength-1;  //如果dis6还是0？
				}
				//TRACE("   tarIndex=%d\n",tarIndex);

				//////11.07避障尝试//////////////////////////////////////////
				//if (Obs_Avoidance==1)
				//{
				//	if (m_ultiLength>=700)
				//	{
				//		m_ultiLength=0;
				//	}
				//	//m_ultiPath_Gussian[m_ultiLength-1];
				//	int nearestGPSPt=0;
				//	minGps=MAXD;
				//	for (int i=beginPointGPSPos;i<=tarIndex;i++)
				//	{
				//		double dis=(m_ultiPath_Gussian[m_ultiLength-1].x-m_RoadDataBuffer[i].x)*(m_ultiPath_Gussian[m_ultiLength-1].x-m_RoadDataBuffer[i].x)
				//			+(m_ultiPath_Gussian[m_ultiLength-1].y-m_RoadDataBuffer[i].y)*(m_ultiPath_Gussian[m_ultiLength-1].y-m_RoadDataBuffer[i].y);
				//		if(dis<minGps)
				//		{
				//			minGps=dis;
				//			nearestGPSPt=i;
				//		}
				//	}

				//	double disPt=sqrt(minGps);
				//	double delta_x=m_ultiPath_Gussian[m_ultiLength-1].x-m_RoadDataBuffer[nearestGPSPt].x;
				//	double delta_y=m_ultiPath_Gussian[m_ultiLength-1].y-m_RoadDataBuffer[nearestGPSPt].y;
				//	double lineangle=ckMapBaseObject::AmizthCaltulate(m_RoadDataBuffer[nearestGPSPt].x,m_RoadDataBuffer[nearestGPSPt].y,m_ultiPath_Gussian[m_ultiLength-1].x,m_ultiPath_Gussian[m_ultiLength-1].y);


				//	m_tarPoint.x=m_RoadDataBuffer[tarIndex].x+disPt*sin(lineangle);
				//	m_tarPoint.y=m_RoadDataBuffer[tarIndex].y+disPt*cos(lineangle);

				//	//for (int val=3;val>0;val--)
				//	//{
				//	//	m_tarPoint.x=m_RoadDataBuffer[tarIndex].x+disPt*sin(lineangle);
				//	//	m_tarPoint.y=m_RoadDataBuffer[tarIndex].y+disPt*cos(lineangle);
				//	//}
				//	//double disOfObs=Distance(fistObs,g_CurrentLocation);



				//	//TRACE("delta_x=%f",delta_x);
				//	//TRACE("  delta_y=%f\n",delta_y);


				//	//ckCoordinate::WorldtoMap(oriPoint,m_ultiPath_Gussian[m_ultiLength-1].x,m_ultiPath_Gussian[m_ultiLength-1].y,newX,newY);

				//	//TRACE("newX1=%f",newX);
				//	//TRACE("  newY1=%f\n",newY);

				//	//ckCoordinate::WorldtoMap(oriPoint,m_RoadDataBuffer[nearestGPSPt].x,m_RoadDataBuffer[nearestGPSPt].y,newX,newY);

				//	//TRACE("newX2=%f",newX);
				//	//TRACE("  newY2=%f\n",newY);




				//	//ckCoordinate::WorldtoMap(oriPoint,m_tarPoint.x,m_tarPoint.y,newX,newY);

				//	//TRACE("newX3=%f",newX);
				//	//TRACE("  newY3=%f\n",newY);


				//	//ckCoordinate::WorldtoMap(oriPoint,m_RoadDataBuffer[tarIndex].x,m_RoadDataBuffer[tarIndex].y,newX,newY);

				//	//TRACE("newX4=%f",newX);
				//	//TRACE("  newY4=%f\n",newY);

				//	double newAngle=m_RoadDataBuffer[tarIndex].angle;
				//	if (newAngle<=PI/2)
				//	{
				//		m_tarPoint.angle=PI/2-newAngle;
				//	}
				//	else m_tarPoint.angle=5*PI/2-newAngle;

				//	m_beginPoint.x=m_ultiPath_Gussian[m_ultiLength-1].x;
				//	m_beginPoint.y=m_ultiPath_Gussian[m_ultiLength-1].y;
				//	m_beginPoint.angle=m_ultiPath_Gussian[m_ultiLength-1].angle;

				//	if (Distance(m_beginPoint,m_tarPoint)<25)
				//	{
				//		flag_path=TRUE;
				//		ResetEvent(g_VeloRcvHandle);
				//		continue;
				//	}

				//	if (PathExtendSpecial())
				//	{

				//		Obs_Avoid_Time++;
				//		TRACE("Obs_Avoid_Time=%d\n",Obs_Avoid_Time);

				//		double delta_x1=fistObs.x-g_CurrentLocation.x;
				//		double delta_y1=fistObs.y-g_CurrentLocation.y;
				//		double lineangle1=ckMapBaseObject::AmizthCaltulate(g_CurrentLocation.x,g_CurrentLocation.y,fistObs.x,fistObs.y);

				//		RoadPoint test1;
				//		test1.x=g_CurrentLocation.x+1.0*sin(g_CurrentLocation.angle);
				//		test1.y=g_CurrentLocation.y+1.0*cos(g_CurrentLocation.angle);

				//		//double d_angle=abs(lineangle1-g_CurrentLocation.angle);

				//		//if (d_angle>3.0*PI/2.0) d_angle=abs(d_angle-2.0*PI); //?也 有可能是大弯道

				//		double distanceOfTest=Distance(test1,fistObs);
				//		double distanceOfObs=Distance(fistObs,g_CurrentLocation);

				//		if ((distanceOfTest>distanceOfObs)&&Distance(fistObs,g_CurrentLocation)>=64)
				//		{
				//			Obs_Avoidance=2;
				//			Obs_Avoid_Time=0;
				//			m_beginPoint.x=m_ultiPath_Gussian[startIndex].x;
				//			m_beginPoint.y=m_ultiPath_Gussian[startIndex].y;
				//			m_beginPoint.angle=m_ultiPath_Gussian[startIndex].angle;
				//		}

				//		

				//		//if (Obs_Avoid_Time==9)
				//		//{
				//		//	
				//		//}
				//		ResetEvent(g_VeloRcvHandle);
				//		continue;
				//	}
				//	else
				//	{
				//		Obs_Avoidance=2;
				//		Obs_Avoid_Time=0;
				//		m_beginPoint.x=m_ultiPath_Gussian[startIndex].x;
				//		m_beginPoint.y=m_ultiPath_Gussian[startIndex].y;
				//		m_beginPoint.angle=m_ultiPath_Gussian[startIndex].angle;
				//	}

				//}
				/////////////////////////////////////////////////////////


				//对GPS点加密
				int dataCount=0;
				RoadPoint testData[200];
				for (int i=0;i<tarIndex-beginPointGPSPos;i++)
				{
					double delta_x=m_RoadDataBuffer[beginPointGPSPos+i+1].x-m_RoadDataBuffer[beginPointGPSPos+i].x;
					double delta_y=m_RoadDataBuffer[beginPointGPSPos+i+1].y-m_RoadDataBuffer[beginPointGPSPos+i].y;
					double lineangle=atan(delta_x/delta_y);
					if(delta_y>0&&delta_x>0)
					{
						lineangle=lineangle;
					}
					else if(delta_y<0&&delta_x>0)
					{
						lineangle=lineangle+PI;
					}
					else if(delta_x<0&&delta_y<0)
					{
						lineangle=lineangle+PI;
					}
					else if(delta_x<0&&delta_y>0)
					{
						lineangle=lineangle+2*PI;
					}

					int m=sqrt(Distance(m_RoadDataBuffer[beginPointGPSPos+i+1],m_RoadDataBuffer[beginPointGPSPos+i]))/1.0;

					if (m<=1)
					{
						testData[dataCount].x=m_RoadDataBuffer[beginPointGPSPos+i].x;
						testData[dataCount].y=m_RoadDataBuffer[beginPointGPSPos+i].y;
						dataCount++;
					}
					else
					{
						for (int j=0;j<m;j++) 
						{
							testData[dataCount].x=m_RoadDataBuffer[beginPointGPSPos+i].x+1.0*j*sin(lineangle);
							testData[dataCount].y=m_RoadDataBuffer[beginPointGPSPos+i].y+1.0*j*cos(lineangle);
							dataCount++;
						}
					}
				}
				testData[dataCount]=m_RoadDataBuffer[tarIndex];

				//直接判断beginPointGPSPos到tarIndex的轨迹能否可通行
				//int IsAccess=flag_SegPointsInObsGrid(TRUE,m_RoadDataBuffer,CurrentGpsPosition,tarIndex);
				int IsAccess=flag_SegPointsInObsGrid(TRUE,testData,0,dataCount);
				if(IsAccess==MAXD)
				{

					flag_path=TRUE;
					//int pointIndex=CurrentGpsPosition;
					m_tarPoint.x=m_RoadDataBuffer[tarIndex].x;
					m_tarPoint.y=m_RoadDataBuffer[tarIndex].y;
					//double newAngle=AngleNormalnize(m_RoadDataBuffer[tarIndex].angle);
					double newAngle=m_RoadDataBuffer[tarIndex].angle;
					if (newAngle<=PI/2)
					{
						m_tarPoint.angle=PI/2-newAngle;
					}
					else m_tarPoint.angle=5*PI/2-newAngle;

					if(PathExtend())
					{
						OffsetVal=0;
						ResetEvent(g_VeloRcvHandle);
						continue;
					}
				}
				else
				{
					//fistObs.x=testData[IsAccess].x;
					//fistObs.y=testData[IsAccess].y;
				}


				//TRACE("BBB");
				//int minIndex=IsAccess>dis6?IsAccess:dis6;
				int minIndex=dis6;
				flag_througth=FALSE;

				//偏移轨迹点来判断能否通行
				for (int i=tarIndex;i>=minIndex;i=i-1)
				{
					//BOOL flag_througth=FALSE;
					RoadPoint offsetPoint;

					////一定半径的圆上的随机点来判断(半径设为1.5米)
					//double r=1.5;
					//srand((unsigned)time(NULL));
					//int randomAngle=rand()%360;
					//offsetPoint.x=m_RoadDataBuffer[i].x+r*sin(AngleNormalnize(randomAngle));
					//offsetPoint.y=m_RoadDataBuffer[i].y+r*cos(AngleNormalnize(randomAngle));

					if (OffsetVal!=0)
					{
						//offsetPoint.x=m_RoadDataBuffer[i].x+OffsetVal*0.3*sin(AngleNormalnize(m_RoadDataBuffer[i].angle)+PI/2);
						//offsetPoint.y=m_RoadDataBuffer[i].y+OffsetVal*0.3*cos(AngleNormalnize(m_RoadDataBuffer[i].angle)+PI/2);
						//offsetPoint.angle=AngleNormalnize(m_RoadDataBuffer[i].angle);
						offsetPoint.x=m_RoadDataBuffer[i].x+OffsetVal*0.3*sin(m_RoadDataBuffer[i].angle+PI/2);
						offsetPoint.y=m_RoadDataBuffer[i].y+OffsetVal*0.3*cos(m_RoadDataBuffer[i].angle+PI/2);
						offsetPoint.angle=m_RoadDataBuffer[i].angle;
						if (offsetPoint.angle<=PI/2)
						{
							offsetPoint.angle=PI/2-offsetPoint.angle;
						}
						else offsetPoint.angle=5*PI/2-offsetPoint.angle;

						m_tarPoint=offsetPoint;


						if (PathExtend())
						{
							flag_path=TRUE;
							flag_througth=TRUE;
							break;
						}

					}

					int offsetVal=-6;
					while (abs(offsetVal)!=0)
					{
						//offsetPoint.x=m_RoadDataBuffer[i].x+offsetVal*0.3*sin(AngleNormalnize(m_RoadDataBuffer[i].angle)+PI/2);
						//offsetPoint.y=m_RoadDataBuffer[i].y+offsetVal*0.3*cos(AngleNormalnize(m_RoadDataBuffer[i].angle)+PI/2);
						//offsetPoint.angle=AngleNormalnize(m_RoadDataBuffer[i].angle);
						offsetPoint.x=m_RoadDataBuffer[i].x+offsetVal*0.3*sin(m_RoadDataBuffer[i].angle+PI/2);
						offsetPoint.y=m_RoadDataBuffer[i].y+offsetVal*0.3*cos(m_RoadDataBuffer[i].angle+PI/2);
						offsetPoint.angle=m_RoadDataBuffer[i].angle;

						if (offsetPoint.angle<=PI/2)
						{
							offsetPoint.angle=PI/2-offsetPoint.angle;
						}
						else offsetPoint.angle=5*PI/2-offsetPoint.angle;

						m_tarPoint=offsetPoint;
						if (PathExtend())
						{
							flag_path=TRUE;
							flag_througth=TRUE;
							OffsetVal=offsetVal;
							break;
						}

						if (offsetVal==-1)
						{
							offsetVal=6;
							continue;
						}                                                     
						if (offsetVal<0)
						{
							offsetVal++;
						}
						else offsetVal--;
					}

					if (flag_througth==TRUE)
					{
						break;
					}
				}
			}

				if (flag_througth==FALSE||EndMode==TRUE)
				{
#pragma region 原来的搜索方式
					//TRACE("CCC");
#pragma region 目标GPS点和起始点连线与网格的关系flag_Intersect，得到可通行判断的起点OriginStart和终点OriginEnd

					//获取作为目标点的GPS轨迹点的坐标
					RoadPoint tarGPSPoint;
					tarGPSPoint.x=m_RoadDataBuffer[tarIndex].x;  //fileLength-1需要修改
					tarGPSPoint.y=m_RoadDataBuffer[tarIndex].y;
					//获取目标点的方向
					//double iniAngle=AngleNormalnize(m_RoadDataBuffer[CurrentGpsPosition+STEPNUM].angle);
					//double iniAngle=AngleNormalnize(m_RoadDataBuffer[tarIndex].angle);
					double iniAngle=m_RoadDataBuffer[tarIndex].angle;
					if (iniAngle<=PI/2)
					{
						tarGPSPoint.angle=PI/2-iniAngle;
					}
					else tarGPSPoint.angle=5*PI/2-iniAngle;

					m_tarPoint=tarGPSPoint;
					//RoadPoint nextNetPoint;
					//nextNetPoint.x=tarGPSPoint.x;
					//nextNetPoint.y=tarGPSPoint.y;

					RoadPoint intersectPoint;
					//求出起始点在网格中的哪个点start(或者连线与网格底部的交点)
					LINESEG u,v;
					u.s=GridLeftDown;
					u.e=GridRightDown;
					v.s=m_beginPoint;
					v.e=tarGPSPoint;
					if (intersect(u,v))  //判断连线是否与网格底线相交,
					{
						//如果相交，以交点所在的行列号作为判断是否可通行的起点
						OriginStart.Y=0;
						//求连线与网格底线的交点
						intersectPoint=getIntersectPoint(GridLeftDown,GridRightDown,m_beginPoint,tarGPSPoint);
						OriginStart.X=sqrt(Distance(GridLeftDown,intersectPoint))/GRID_WIDTH_X;
					}
					else
					{
						//如果不相交，计算起始点所在的行列号作为判断是否可通行的起点

						////起始点到网格底部的距离
						//double dis1=PointToSegDist(m_beginPoint.x,m_beginPoint.y,GridLeftDown.x,GridLeftDown.y,GridRightDown.x,GridRightDown.y);
						//OriginStart.Y=dis1/GRID_WIDTH_Y;

						////起始点到网格左边界的距离
						//double dis2=PointToSegDist(m_beginPoint.x,m_beginPoint.y,GridLeftDown.x,GridLeftDown.y,GridLeftUp.x,GridLeftUp.y);
						//OriginStart.X=dis2/GRID_WIDTH_X;

						ckCoordinate::WorldtoMap(oriPoint,m_beginPoint.x,m_beginPoint.y,newX,newY);
						OriginStart.X=newX/GRID_WIDTH_X+GRID_I/2;
						OriginStart.Y=newY/GRID_WIDTH_Y;
					}



					//求出下一路网点和起始点连线与网格顶部的交点end（如果下一路网点和当前点很近了？）
					u.s=GridLeftUp;
					u.e=GridRightUp;
					v.s=m_beginPoint;
					v.e=tarGPSPoint;
					if (intersect(u,v))  //判断连线是否与网格顶部线相交
					{
						flag_Intersect=1;

						//如果相交，以交点所在的行列号作为判断是否可通行的终点
						OriginEnd.Y=GRID_J-1;
						//求连线与网格顶线的交点
						intersectPoint=getIntersectPoint(GridLeftUp,GridRightUp,m_beginPoint,tarGPSPoint);
						OriginEnd.X=sqrt(Distance(GridLeftUp,intersectPoint))/GRID_WIDTH_X;
					}
					else
					{
						//判断是否与左边界相交
						u.s=GridLeftDown;
						u.e=GridLeftUp;
						if (intersect(u,v))
						{
							flag_Intersect=2;
							//如果相交，以交点所在的行列号作为判断是否可通行的终点
							OriginEnd.X=0;
							//求连线与网格左边界的交点
							intersectPoint=getIntersectPoint(GridLeftDown,GridLeftUp,m_beginPoint,tarGPSPoint);
							OriginEnd.Y=sqrt(Distance(GridLeftDown,intersectPoint))/GRID_WIDTH_Y;

						}
						else   
						{
							//判断是否与右边界相交
							u.s=GridRightDown;
							u.e=GridRightUp;
							if (intersect(u,v))
							{
								flag_Intersect=3;
								//如果相交，以交点所在的行列号作为判断是否可通行的终点
								OriginEnd.X=GRID_I-1;
								//求连线与网格右边界的交点
								intersectPoint=getIntersectPoint(GridRightDown,GridRightUp,m_beginPoint,tarGPSPoint);
								OriginEnd.Y=sqrt(Distance(GridRightDown,intersectPoint))/GRID_WIDTH_Y;
							}
							else//下一路网点在网格内
							{
								//计算下一路网点所在的行列号作为判断是否可通行的终点
								//下一路网点到网格底部的距离
								//double dis1=PointToSegDist(nextNetPoint.x,nextNetPoint.y,GridLeftDown.x,GridLeftDown.y,GridRightDown.x,GridRightDown.y);
								//OriginEnd.Y=dis1/GRID_WIDTH_Y;

								////下一路网点到网格左边界的距离
								//double dis2=PointToSegDist(nextNetPoint.x,nextNetPoint.y,GridLeftDown.x,GridLeftDown.y,GridLeftUp.x,GridLeftUp.y);
								//OriginEnd.X=dis2/GRID_WIDTH_X;

								ckCoordinate::WorldtoMap(oriPoint,tarGPSPoint.x,tarGPSPoint.y,newX,newY);
								OriginEnd.X=newX/GRID_WIDTH_X+GRID_I/2;
								OriginEnd.Y=newY/GRID_WIDTH_Y;

								double A,B,C;
								makeline(m_beginPoint,tarGPSPoint,A,B,C);//下一路网点和起始点连线的直线
								//直线是否和网格顶端相交
								if (Line_Seg_Intersect(GridLeftUp.x,GridLeftUp.y,GridRightUp.x,GridRightUp.y,A,B,C))
								{
									flag_Intersect=4;
								}
								else if (Line_Seg_Intersect(GridLeftDown.x,GridLeftUp.y,GridRightUp.x,GridRightUp.y,A,B,C))//直线是否和网格左边界相交
								{
									flag_Intersect=5;
								}
								else flag_Intersect=6;//直线和网格右边界相交

							}
						}
					}

#pragma endregion 

					//根据目标GPS点和网格数据寻找可通行点
					FindAvailableTar(obs_grid);

					

					if (tarI<0||tarJ<0)
					{
						flag_path=FALSE;				
						continue;
					}
					if (tarI==0&&tarJ==0)//未找到可通行点
					{
						flag_path=FALSE;				
						continue;
					}

					ckCoordinate::MaptoWorld(oriPoint,(tarI-GRID_I/2)*GRID_WIDTH_X,tarJ*GRID_WIDTH_Y,newX,newY);
					m_tarPoint.x=newX;
					m_tarPoint.y=newY;
					//m_tarPoint.angle=tarGPSPoint.angle;

					minGps=MAXD;
					int tarPt=0;
					for (int pointCount=beginPointGPSPos;pointCount<=tarIndex;pointCount++)
					{
						double dis1=(m_tarPoint.x-m_RoadDataBuffer[pointCount].x)*(m_tarPoint.x-m_RoadDataBuffer[pointCount].x)
							+(m_tarPoint.y-m_RoadDataBuffer[pointCount].y)*(m_tarPoint.y-m_RoadDataBuffer[pointCount].y);
						if(dis1<minGps)
						{
							minGps=dis1;
							tarPt=pointCount;
						}
					}
					//以目标GPS点方向作为规划路径目标点的方向(弧度)
					if (m_RoadDataBuffer[tarPt].angle<=PI/2)
					{
						m_tarPoint.angle=PI/2-m_RoadDataBuffer[tarPt].angle;
					}
					else m_tarPoint.angle=5*PI/2-m_RoadDataBuffer[tarPt].angle;

					int flag_tooclose=0;

					if( (tarJ-OriginStart.Y)*(tarJ-OriginStart.Y)+(tarI-OriginStart.X)*(tarI-OriginStart.X)<900||tarJ<=OriginStart.Y)
					{
						flag_tooclose=1;
					}

					while(flag_tooclose==1||!PathExtend())
					{
						flag_tooclose=0;
						FindTarInGrid(tarI,tarJ);

						if (tarI<0||tarJ<0)
						{
							flag_path=FALSE;
							break;
						}
						if (tarI==0&&tarJ==0)
						{
							flag_path=FALSE;
							break;
						}

						if( (tarJ-OriginStart.Y)*(tarJ-OriginStart.Y)+(tarI-OriginStart.X)*(tarI-OriginStart.X)<900||tarJ<=OriginStart.Y)
						{
							flag_tooclose=1;
							continue;
						}

						//if (tarJ<=OriginStart.Y)
						//{
						//	continue;
						//}

						ckCoordinate::MaptoWorld(oriPoint,(tarI-GRID_I/2)*GRID_WIDTH_X,tarJ*GRID_WIDTH_Y,newX,newY);
						m_tarPoint.x=newX;
						m_tarPoint.y=newY;
						m_tarPoint.angle=tarGPSPoint.angle;


						minGps=MAXD;
						int tarPt=0;
						for (int pointCount=beginPointGPSPos;pointCount<=tarIndex;pointCount++)
						{
							double dis1=(m_tarPoint.x-m_RoadDataBuffer[pointCount].x)*(m_tarPoint.x-m_RoadDataBuffer[pointCount].x)
								+(m_tarPoint.y-m_RoadDataBuffer[pointCount].y)*(m_tarPoint.y-m_RoadDataBuffer[pointCount].y);
							if(dis1<minGps)
							{
								minGps=dis1;
								tarPt=pointCount;
							}
						}
						//以目标GPS点方向作为规划路径目标点的方向(弧度)
						if (m_RoadDataBuffer[tarPt].angle<=PI/2)
						{
							m_tarPoint.angle=PI/2-m_RoadDataBuffer[tarPt].angle;
						}
						else m_tarPoint.angle=5*PI/2-m_RoadDataBuffer[tarPt].angle;



						if ((tarI-GRID_I/2)*(tarI-GRID_I/2)+tarJ*tarJ<900)//如果目标点距离起始点距离小于6米
						{
							flag_path=FALSE;
							//IsPathExtend=0;
							break;
						}
					}
#pragma endregion 原来的搜索方式
				}


			ResetEvent(g_VeloRcvHandle);
		}


		////////////////////////////////////////////////////
#pragma endregion 新写的GPS轨迹偏移，暂不使用逐行或逐列扫描


#pragma region 注释的以前的代码
		//int d_target,d_start;//用来存起点和终点在gps路径或路网数据中间的位置
		//int CurrentGpsPosition=0;  //保存与当前位置最近的点的序号
		////试验地  g_CurrentLocation需要获取

		////g_CurrentLocation.x=m_RoadDataBuffer[30].x;
		////g_CurrentLocation.y=m_RoadDataBuffer[30].y;
		////g_CurrentLocation.angle=AngleNormalnize(m_RoadDataBuffer[30].angle);




		////查找当前坐标在整个gps路径里面的位置，取最近点为它的位置.
		//g_CurrentLocation.x=g_curLocation.gauX;
		//g_CurrentLocation.y=g_curLocation.gauY;
		//g_CurrentLocation.angle=AngleNormalnize(g_curLocation.azimuth);

		//CurrentGpsPosition=GetCurrentPosition(CurrentGpsPosition);

		////判断是否倒走
		//if(cos((g_CurrentLocation.angle-m_RoadDataBuffer[CurrentGpsPosition].angle)*PI/180)<0)  //AngleNormalnize()
		//{
		//	CurrentGpsPosition = fileLength-1-CurrentGpsPosition;
		//	for(int i=0;i<fileLength;i++)
		//	{
		//		m_RoadDataBuffer[i]=m_invertedRoadDataBuffer[i];
		//	}
		//}
		//while(!m_bStopThread)
		//{
		//	Sleep(50);
		//   // WaitForSingleObject(g_VeloRcvHandle,50);
		//	g_CurrentLocation.x=g_curLocation.gauX;
		//	g_CurrentLocation.y=g_curLocation.gauY;
		//	g_CurrentLocation.angle=AngleNormalnize(g_curLocation.azimuth);
		//	CurrentGpsPosition=GetCurrentPosition(CurrentGpsPosition);
		//	d_target=CurrentGpsPosition+STEPNUM;
		//	if (d_target>=fileLength)
		//	{
		//		if (CurrentGpsPosition>=fileLength-1)
		//		{
		//			m_bStopThread=TRUE;
		//			break;
		//		}
		//		d_target=fileLength-1;
		//	}
		//	//?在终点要停止
		//	if(Distance(g_CurrentLocation,m_RoadDataBuffer[fileLength-1])<100)
		//	{
		//		m_bStopThread=1;
		//	}
		//	d_start=CurrentGpsPosition;
		//	m_beginPoint=g_CurrentLocation;
		//	m_tarPoint=m_RoadDataBuffer[d_target];

		//	//////////////////////////////////////////////////////////////////
		//	m_beginPoint.angle = AngleNormalnize(90-m_beginPoint.angle)*PI/180;
		//	m_tarPoint.angle = AngleNormalnize(90-m_tarPoint.angle)*PI/180;
		//	/////////////////////////////////////////////////////////////////

		//	while(!flag_path)
		//	{
		//		PathExtend();
		//	}
		//	//获取当前点在最终得到的路径里面的位置
		//	int currentNum=0;
		//	for(int i=0;i<m_ultiLength;i++)
		//	{
		//		double x=m_ultiPath[i].x-g_CurrentLocation.x;
		//		int y=m_ultiPath[i].y-g_CurrentLocation.y;
		//		if(x*x+y*y<10)
		//		{
		//			currentNum=i;
		//			break;
		//		}
		//	}

		//	//for(int i=currentNum;i<m_ultiLength;i++)
		//	//{
		//	//	double x=m_ultiPath[i].x-g_CurrentLocation.x;
		//	//	double y=m_ultiPath[i].y-g_CurrentLocation.y;
		//	//	double angle=m_ultiPath[i].angle-AngleNormalnize(90-g_CurrentLocation.angle)*PI/180;

		//	//	if(((x*x+y*y)-36)>0&&abs(x)<1000&&abs(y)<1000)
		//	//	{
		//	//		double dx=(x)*cos(g_CurrentLocation.angle/180*PI)-y*sin(g_CurrentLocation.angle/180*PI);
		//	//		double dy=x*sin(g_CurrentLocation.angle/180*PI)+y*cos(g_CurrentLocation.angle/180*PI);
		//	//	    m_PurePursuitTar.x=dx;
		//	//		m_PurePursuitTar.y=dy;
		//	//		m_TagWheelAngle=MiddlePiont_Shan(m_PurePursuitTar.x,m_PurePursuitTar.y);
		//	//		break;
		//	//	}
		//	//}

		//	if(currentNum>m_ultiLength/5||(CurrentGpsPosition-d_target)>=0)
		//	{			
		//		//flag_path=0;
		//		currentNum=0;
		//	}
		//	double angle=(m_TagWheelAngle-550.0)/15.0;
		//	/*g_CurrentLocation.angle+=angle;
		//	g_CurrentLocation.x+=0.6*sin(g_CurrentLocation.angle/180*PI);
		//	g_CurrentLocation.y+=0.6*cos(g_CurrentLocation.angle/180*PI);*/
		//	ResetEvent(g_VeloRcvHandle);

		//}
	
#pragma endregion 注释的以前的代码
	}
#pragma endregion 读GPS轨迹点的情况
}

void CPathGenerate::readGPSDataFile(CString path)
{
	//path->GpsLength=0;
	FILE *m_pGpsFile=fopen(path,"rb");
	//获得文件的长度以及保存所有信息到buffer里面
	fseek(m_pGpsFile,0,SEEK_END);
	int Len = ftell(m_pGpsFile);
	fileLength=Len/sizeof(GpsLine);
	fseek(m_pGpsFile,0,SEEK_SET);
	GpsLine *m_gpsDataBuffer=new GpsLine[fileLength];
	fread(m_gpsDataBuffer,sizeof(GpsLine),fileLength,m_pGpsFile);
	//判断是否已经存在GPS数据
	m_RoadDataBuffer=NULL;
	if (m_RoadDataBuffer!=NULL)
	{
		delete[] m_RoadDataBuffer;
		m_RoadDataBuffer=NULL;
	}
	m_RoadDataBuffer=new RoadPoint[fileLength];
	if (m_invertedRoadDataBuffer!=NULL)
	{
		delete[] m_invertedRoadDataBuffer;
		m_invertedRoadDataBuffer=NULL;
	}
	m_invertedRoadDataBuffer=new RoadPoint[fileLength];
	for(int i=0;i<fileLength;i++)
	{
		m_RoadDataBuffer[i].x=m_gpsDataBuffer[i].Gnx;
		m_RoadDataBuffer[i].y=m_gpsDataBuffer[i].Gny;
		m_RoadDataBuffer[i].angle=AngleNormalnize(m_gpsDataBuffer[i].azimuth);
	}
	//把倒走的信息存入这里面
	for(int i =0;i<fileLength;i++)
	{
		m_invertedRoadDataBuffer[i].x = m_RoadDataBuffer[fileLength-1-i].x;
		m_invertedRoadDataBuffer[i].y = m_RoadDataBuffer[fileLength-1-i].y;
		m_invertedRoadDataBuffer[i].angle = AngleNormalnize(m_RoadDataBuffer[fileLength-1-i].angle + 180);
	}
	CalMapCenter();
	delete[] m_gpsDataBuffer;
	fclose(m_pGpsFile);
}

void CPathGenerate::readRoadNetFile(CString path)
{
	//打开路网文件
	FILE *m_roadNetFile=fopen(path,"r");
	int length=0;

	RoadNet m_roadnet[100]={0};
	while(!feof(m_roadNetFile))
	{
		fscanf(m_roadNetFile,"%d\t%lf\t%lf\t%lf\t%d\n",&m_roadnet[length].num,&m_roadnet[length].longitude,
			&m_roadnet[length].latitude,&m_roadnet[length].height,&m_roadnet[length].roadprop);
		length++;
	}

	/////////////////////////////
	m_netFileLength=length;
	memcpy(m_roadNetBuffer,m_roadnet,length*sizeof(RoadNet));
	/////////////////////////////


	///////////////////////////////////////////////////////////////
	if (m_RoadDataBuffer!=NULL)
	{
		delete[] m_RoadDataBuffer;
		m_RoadDataBuffer=NULL;
	}
	fileLength=length;
	m_RoadDataBuffer=new RoadPoint[length];
	//int fileLength=0;
	///////////////////////////////////////////////////////////////

	RoadPoint luwanggaosi[100];
	for(int i=0;i<length;i++)
	{
		LongLat2XY(m_roadnet[i].longitude,m_roadnet[i].latitude,luwanggaosi[i].x,luwanggaosi[i].y);

		///////////////////////////////////////////////
		m_roadNetBuffer[i].x=luwanggaosi[i].x;
		m_roadNetBuffer[i].y=luwanggaosi[i].y;
		///////////////////////////////////////////////
	}
	for(int i=0;i<length;i++)
	{
		if (i==length-1)
		{
			m_roadNetBuffer[i].angle=0;///////////////////

			int m=sqrt(Distance(luwanggaosi[i],luwanggaosi[i+1]))/1.0;
			m_RoadDataBuffer[i].x=luwanggaosi[i].x;
			m_RoadDataBuffer[i].y=luwanggaosi[i].y;
			m_RoadDataBuffer[i].angle=0;
			break;
		}
		else
		{
			double delta_x=luwanggaosi[i+1].x-luwanggaosi[i].x;
			double delta_y=luwanggaosi[i+1].y-luwanggaosi[i].y;
			double angle=atan(delta_x/delta_y);
			if(delta_y>0&&delta_x>0)
			{
				angle=angle;
			}
			else if(delta_y<0&&delta_x>0)
			{
				angle=angle+PI;
			}
			else if(delta_x<0&&delta_y<0)
			{
				angle=angle+PI;
			}
			else if(delta_x<0&&delta_y>0)
			{
				angle=angle+2*PI;
			}
			luwanggaosi[i].angle=angle;

			m_roadNetBuffer[i].angle=angle;///////////////////

			int m=sqrt(Distance(luwanggaosi[i],luwanggaosi[i+1]))/1.0;
			m_RoadDataBuffer[i].x=luwanggaosi[i].x;
			m_RoadDataBuffer[i].y=luwanggaosi[i].y;
			m_RoadDataBuffer[i].angle=angle*180/PI;
			/*fileLength++;
			for(int j=0;j<m;j++)
			{

				m_RoadDataBuffer[fileLength+j].x=luwanggaosi[i].x+1.0*j*sin(angle);
				m_RoadDataBuffer[fileLength+j].y=luwanggaosi[i].y+1.0*j*cos(angle);
				m_RoadDataBuffer[fileLength+j].angle=angle*180/PI;


			}
			fileLength+=m;*/
		}
		

	}
	CalMapCenter();
	fclose(m_roadNetFile);
}
void CPathGenerate::CalMapCenter()
{
	double Xmin=m_RoadDataBuffer[0].x,Ymin=m_RoadDataBuffer[0].y,Xmax=m_RoadDataBuffer[0].x,Ymax=m_RoadDataBuffer[0].y;
	for (int i=0;i<fileLength;i++)
	{
		if (m_RoadDataBuffer[i].x<Xmin) Xmin = m_RoadDataBuffer[i].x;
		else if (m_RoadDataBuffer[i].x>Xmax) Xmax = m_RoadDataBuffer[i].x;

		if (m_RoadDataBuffer[i].y<Ymin) Ymin = m_RoadDataBuffer[i].y;
		else if (m_RoadDataBuffer[i].y>Ymax) Ymax = m_RoadDataBuffer[i].y;
	}

	initialPoint.x = (Xmin+Xmax)/2;
	initialPoint.y = (Ymin+Ymax)/2;
}
//此处传点进来要注意坐标系已经转换成功

int CPathGenerate::LoadMapFile(CString path)
{
m_eType=TRACK;
	//int pos=path.ReverseFind('.');
	//CString str=path.Right(path.GetLength()-pos-1);
	//if (str=="txt")
	//{
	//	readRoadNetFile(path);
	//	m_eType=NET;
	//}
	//else if (str=="cs")
	//{
	//	readGPSDataFile(path);
	//	m_eType=TRACK;
	//}
	return 1;
}
void CPathGenerate::initializepath()
{
	m_pNodes[0].fatherNode=0;
	m_pNodes[0].point=m_beginPoint;
	m_pNodes[0].cost=0;

	m_pNodes[POINTNUM].fatherNode=0;
	m_pNodes[POINTNUM].point=m_beginPoint;
	m_pNodes[POINTNUM].cost=0;
	m_pNodes[2*POINTNUM].fatherNode=0;
	m_pNodes[2*POINTNUM].point=m_beginPoint;
	m_pNodes[2*POINTNUM].cost=0;
	m_pNodes[3*POINTNUM].fatherNode=0;
	m_pNodes[3*POINTNUM].point=m_beginPoint;
	m_pNodes[3*POINTNUM].cost=0;
}


	BOOL CPathGenerate::PathExtendSpecial()
{
	DWORD maxTimes=0;
	//给节点和点的存储空间赋存储空间并且初始化
	initializepath();
	RoadPoint path[20];
	//clothoid参数
	double k,dk,L;

	//clothiod线段优先生成
	//buildClothoid(k,dk,L,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
	//pointsOnClothoid(m_clothoidPoint,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,k,dk,L,CLOTHOIDPOINTNUM);
	buildClothoid(k,dk,L,m_beginPoint.x,m_beginPoint.y,m_beginPoint.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
	pointsOnClothoid(m_clothoidPoint,m_beginPoint.x,m_beginPoint.y,m_beginPoint.angle,k,dk,L,CLOTHOIDPOINTNUM);


	double accumAngle = 0.0;
	for(int z=0;z<CLOTHOIDPOINTNUM-1;z++)
	{
		double deltaAngle = abs(m_clothoidPoint[z+1].angle-m_clothoidPoint[z].angle);
		if(deltaAngle>3.0*PI/2.0) deltaAngle=abs(deltaAngle-2.0*PI);
		accumAngle += deltaAngle;
	}


	findUltipath(m_pNodes,m_clothoidPoint,0);

	if(flag_PointsInObsGrid(m_ultiPath_linshi,CLOTHOIDPOINTNUM))
	{
		flag_path=1;
		//break;
		//tarI=GRID_I-1;
		//tarJ=GRID_J-1;

		double newX,newY;
		OriginPt oriPoint;//坐标转换的原点
		oriPoint.X=g_CurrentLocation.x;
		oriPoint.Y=g_CurrentLocation.y;
		oriPoint.Angle=g_CurrentLocation.angle;

		EnterCriticalSection(&m_planCs);
		for (int i=0;i<m_ultiLength;i++)
		{
			m_lastPath_Gussian[i]=m_ultiPath_Gussian[i];
			//m_lastPath[i]=m_ultiPath[i];
			ckCoordinate::WorldtoMap(oriPoint,m_lastPath_Gussian[i].x,m_lastPath_Gussian[i].y,newX,newY);
			m_lastPath[i].x=newX;
			m_lastPath[i].y=newY;
		}
		m_lastUltiLen=m_ultiLength;


		for (int i=currentIndex;i<m_lastUltiLen;i++)
		{
			m_ultiPath_Gussian[i-currentIndex]=m_lastPath_Gussian[i];
			m_ultiPath[i-currentIndex]=m_lastPath[i];
		}

		for (int i=0;i<m_ultiLen_linshi;i++)
		{
			m_ultiPath_Gussian[m_lastUltiLen-currentIndex+i]=m_ultiPath_Gussian_linshi[i];
			m_ultiPath[m_lastUltiLen-currentIndex+i]=m_ultiPath_linshi[i];
		}

		m_ultiLength=m_lastUltiLen-currentIndex+m_ultiLen_linshi;

		m_ultiPath_Gussian[m_ultiLength-1].changeangle=accumAngle;//保存累计弧度变化

		double d_angle=abs(m_tarPoint.angle-m_beginPoint.angle);

		if (d_angle>3.0*PI/2.0) d_angle=abs(d_angle-2.0*PI); //?也有可能是大弯道


		LeaveCriticalSection(&m_planCs);

		return TRUE;
	}

	return FALSE;


}

BOOL CPathGenerate::PathExtend()
{
	DWORD maxTimes=0;
	//给节点和点的存储空间赋存储空间并且初始化
	initializepath();
	 RoadPoint path[20];
	//clothoid参数
	double k,dk,L;

        //clothiod线段优先生成
		//buildClothoid(k,dk,L,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
		//pointsOnClothoid(m_clothoidPoint,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,k,dk,L,CLOTHOIDPOINTNUM);
	buildClothoid(k,dk,L,m_beginPoint.x,m_beginPoint.y,m_beginPoint.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
	pointsOnClothoid(m_clothoidPoint,m_beginPoint.x,m_beginPoint.y,m_beginPoint.angle,k,dk,L,CLOTHOIDPOINTNUM);


		double accumAngle = 0.0;
		for(int z=0;z<CLOTHOIDPOINTNUM-1;z++)
		{
			double deltaAngle = abs(m_clothoidPoint[z+1].angle-m_clothoidPoint[z].angle);
			if(deltaAngle>3.0*PI/2.0) deltaAngle=abs(deltaAngle-2.0*PI);
			accumAngle += deltaAngle;
		}


		//if(accumAngle>2.0*PI/2.0)
		//{ 
		//	//flag_path=0;//判定路径生成不成功
		//	//m_ultiLength=0;
		//	//break;
		//	return FALSE;
		//}

		
		findUltipath(m_pNodes,m_clothoidPoint,0);//m_ultiPath
		RoadPoint boundaryUp[1000];
		RoadPoint boundaryDown[1000];
		LINESEG u;

		//TRACE("clothoid 1111  ");

		if(flag_PointsInObsGrid(m_ultiPath_linshi,CLOTHOIDPOINTNUM))
		{
			flag_path=1;
			//break;
			//tarI=GRID_I-1;
			//tarJ=GRID_J-1;

			double newX,newY;
			OriginPt oriPoint;//坐标转换的原点
			oriPoint.X=g_CurrentLocation.x;
			oriPoint.Y=g_CurrentLocation.y;
			oriPoint.Angle=g_CurrentLocation.angle;

			EnterCriticalSection(&m_planCs);

			double d_angle=abs(m_tarPoint.angle-m_beginPoint.angle);

			if (d_angle>3.0*PI/2.0) d_angle=abs(d_angle-2.0*PI); //?也有可能是大弯道

			if (d_angle<DELTAANGLE&&accumAngle>PI/20&&(tarJ-OriginStart.Y<=30))
			{
				/*AfxMessageBox("道路阻断！！");*/
				flag_path=FALSE;
				RoadAccess=FALSE;
				m_ultiLength=0;
				return TRUE;

			}

			for (int i=0;i<m_ultiLength;i++)
			{
				m_lastPath_Gussian[i]=m_ultiPath_Gussian[i];
				//m_lastPath[i]=m_ultiPath[i];
				ckCoordinate::WorldtoMap(oriPoint,m_lastPath_Gussian[i].x,m_lastPath_Gussian[i].y,newX,newY);
				m_lastPath[i].x=newX;
				m_lastPath[i].y=newY;
			}
			m_lastUltiLen=m_ultiLength;


			for (int i=currentIndex;i<startIndex;i++)
			{
				m_ultiPath_Gussian[i-currentIndex]=m_lastPath_Gussian[i];
				m_ultiPath[i-currentIndex]=m_lastPath[i];
			}

			for (int i=0;i<m_ultiLen_linshi;i++)
			{
				m_ultiPath_Gussian[startIndex-currentIndex+i]=m_ultiPath_Gussian_linshi[i];
				m_ultiPath[startIndex-currentIndex+i]=m_ultiPath_linshi[i];
			}

			m_ultiLength=startIndex-currentIndex+m_ultiLen_linshi;

			m_ultiPath_Gussian[m_ultiLength-1].changeangle=accumAngle;//保存累计弧度变化

			//double d_angle=abs(m_tarPoint.angle-m_beginPoint.angle);

			//if (d_angle>3.0*PI/2.0) d_angle=abs(d_angle-2.0*PI); //?也有可能是大弯道
			
			//if (d_angle<DELTAANGLE&&accumAngle>PI/20)
			//{
			//	if (Obs_Avoidance==2)
			//	{
			//		Obs_Avoidance=0;
			//	}
			//	else
			//	{
			//		//避障模式
			//		Obs_Avoidance=1;

			//		int nearestGPSPt=0;
			//		double minGps=MAXD;
			//		for (int i=beginPointGPSPos;i<=tarIndex;i++)
			//		{
			//			double dis=(m_ultiPath_Gussian[m_ultiLength-1].x-m_RoadDataBuffer[i].x)*(m_ultiPath_Gussian[m_ultiLength-1].x-m_RoadDataBuffer[i].x)
			//				+(m_ultiPath_Gussian[m_ultiLength-1].y-m_RoadDataBuffer[i].y)*(m_ultiPath_Gussian[m_ultiLength-1].y-m_RoadDataBuffer[i].y);
			//			if(dis<minGps)
			//			{
			//				minGps=dis;
			//				nearestGPSPt=i;
			//			}
			//		}

			//		double disPt=sqrt(minGps);
			//		double delta_x=m_ultiPath_Gussian[m_ultiLength-1].x-m_RoadDataBuffer[nearestGPSPt].x;
			//		double delta_y=m_ultiPath_Gussian[m_ultiLength-1].y-m_RoadDataBuffer[nearestGPSPt].y;
			//		double lineangle=ckMapBaseObject::AmizthCaltulate(m_RoadDataBuffer[nearestGPSPt].x,m_RoadDataBuffer[nearestGPSPt].y,m_ultiPath_Gussian[m_ultiLength-1].x,m_ultiPath_Gussian[m_ultiLength-1].y);

			//		for (int val=6;val>0;val--)
			//		{
			//			m_tarPoint.x=m_RoadDataBuffer[nearestGPSPt].x+(disPt+val*0.2)*sin(lineangle);
			//			m_tarPoint.y=m_RoadDataBuffer[nearestGPSPt].y+(disPt+val*0.2)*cos(lineangle);

			//			buildClothoid(k,dk,L,m_beginPoint.x,m_beginPoint.y,m_beginPoint.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
			//			pointsOnClothoid(m_clothoidPoint,m_beginPoint.x,m_beginPoint.y,m_beginPoint.angle,k,dk,L,CLOTHOIDPOINTNUM);


			//			double accumAngle = 0.0;
			//			for(int z=0;z<CLOTHOIDPOINTNUM-1;z++)
			//			{
			//				double deltaAngle = abs(m_clothoidPoint[z+1].angle-m_clothoidPoint[z].angle);
			//				if(deltaAngle>3.0*PI/2.0) deltaAngle=abs(deltaAngle-2.0*PI);
			//				accumAngle += deltaAngle;
			//			}

			//			findUltipath(m_pNodes,m_clothoidPoint,0);//m_ultiPath

			//			if(flag_PointsInObsGrid(m_ultiPath_linshi,CLOTHOIDPOINTNUM))
			//			{
			//				for (int i=0;i<m_ultiLen_linshi;i++)
			//				{
			//					m_ultiPath_Gussian[startIndex-currentIndex+i]=m_ultiPath_Gussian_linshi[i];
			//					m_ultiPath[startIndex-currentIndex+i]=m_ultiPath_linshi[i];
			//				}

			//				break;
			//			}

			//		}
			//	}
			//	
			//}

			//if (Obs_Avoidance==2)
			//{
			//	Obs_Avoidance==0;
			//}
			//TRACE("d_angle=%f  ",d_angle);
			//TRACE("accumAngel=%f\n",accumAngle);



			LeaveCriticalSection(&m_planCs);

			return TRUE;
		}

		return FALSE;

////		//生成随机点
////		RoadPoint random=GetRandomPoint(m_beginPoint);//之后抛点规则可能带来很大的问题，需要进一步改进
////
////		DWORD nearest=FindNearestPoint(random,i);
////       memcpy(path,GeneratePath(nearest,random),sizeof(RoadPoint)*20);
////		//path=GeneratePath(nearest,random);
////	    m_pNodes[i].point=path[STEPNUM];
////		m_pNodes[i].fatherNode=nearest;
////		m_pNodes[i].cost=m_pNodes[nearest].cost+STEPNUM*STEP;
////		//AddNodes(path,i);
////		//m_pNodes[i].fatherNode=i;
////		//增加节点数
//////		memcpy(m_pogBuffer+i*STEPNUM,path,sizeof(RoadPoint)*STEPNUM);//注意可能有问题
////
////		if(flag_PointsInObsGrid(path,STEPNUM))
////		{
////			i++;
////		}
////		
////		if(maxTimes>MAXITER)
////		{
////			flag_path=0;
////			break;
////		}
////	//	delete path;
////		maxTimes++;
//	}

}


BOOL CPathGenerate::PathExtendWithoutObs()
{
	DWORD maxTimes=0;
	//给节点和点的存储空间赋存储空间并且初始化
	initializepath();
	RoadPoint path[20];
	//clothoid参数
	double k,dk,L;

	//clothiod线段优先生成
	buildClothoid(k,dk,L,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
	pointsOnClothoid(m_clothoidPoint,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,k,dk,L,CLOTHOIDPOINTNUM);

	double accumAngle = 0.0;
	for(int z=0;z<CLOTHOIDPOINTNUM-1;z++)
	{
		double deltaAngle = abs(m_clothoidPoint[z+1].angle-m_clothoidPoint[z].angle);
		if(deltaAngle>3.0*PI/2.0) deltaAngle=abs(deltaAngle-2.0*PI);
		accumAngle += deltaAngle;
	}
	//if(accumAngle>2.0*PI/2.0)
	//{ 
	//	//flag_path=0;//判定路径生成不成功
	//	//m_ultiLength=0;
	//	//break;
	//	return FALSE;
	//}


	findUltipath(m_pNodes,m_clothoidPoint,0);//m_ultiPath

	return TRUE;

}


BOOL CPathGenerate::PathExtendHalfGrid()
{
	DWORD maxTimes=0;
	//给节点和点的存储空间赋存储空间并且初始化
	initializepath();
	RoadPoint path[20];
	//clothoid参数
	double k,dk,L;

	//clothiod线段优先生成
	buildClothoid(k,dk,L,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,m_tarPoint.x,m_tarPoint.y,m_tarPoint.angle,0.05);
	pointsOnClothoid(m_clothoidPoint,m_pNodes[0].point.x,m_pNodes[0].point.y,m_pNodes[0].point.angle,k,dk,L,CLOTHOIDPOINTNUM);

	double accumAngle = 0.0;
	for(int z=0;z<CLOTHOIDPOINTNUM-1;z++)
	{
		double deltaAngle = abs(m_clothoidPoint[z+1].angle-m_clothoidPoint[z].angle);
		if(deltaAngle>3.0*PI/2.0) deltaAngle=abs(deltaAngle-2.0*PI);
		accumAngle += deltaAngle;
	}
	//if(accumAngle>2.0*PI/2.0)
	//{ 
	//	//flag_path=0;//判定路径生成不成功
	//	//m_ultiLength=0;
	//	//break;
	//	return FALSE;
	//}


	findUltipathHalfGrid(m_pNodes,m_clothoidPoint,0);//m_ultiPath
	RoadPoint boundaryUp[1000];
	RoadPoint boundaryDown[1000];
	LINESEG u;


	if(flag_PointsInObsGrid(m_ultiPath_HalfGrid,CLOTHOIDPOINTNUM))
	{
		flag_path_HalfGrid=TRUE;
		//break;
		//tarI=GRID_I-1;
		//tarJ=GRID_J-1;
		//TRACE("clothoid 2222\n");
		return TRUE;
	}

	return FALSE;

}



DWORD CPathGenerate::FindNearestPoint(RoadPoint random,DWORD n)
{
	double min=100000;
	DWORD return_i;
	for(int i=0;i<n;i++)
	{
		if(Distance(m_pNodes[i].point,random)<min)
		{
			min=Distance(m_pNodes[i].point,random);
			return_i=i;
		}
	}
	/*for(int i=POINTNUM;i<POINTNUM+n;i++)
	{
	if(Distance(m_pNodes[i].point,random)<min)
	{
	min=Distance(m_pNodes[i].point,random);
	return_i=i;
	}
	}
	for(int i=POINTNUM*2;i<POINTNUM*2+n;i++)
	{
	if(Distance(m_pNodes[i].point,random)<min)
	{
	min=Distance(m_pNodes[i].point,random);
	return_i=i;
	}
	}
	for(int i=POINTNUM*3;i<POINTNUM*3+n;i++)
	{
	if(Distance(m_pNodes[i].point,random)<min)
	{
	min=Distance(m_pNodes[i].point,random);
	return_i=i;
	}
	}*/

	return return_i;
}

RoadPoint* CPathGenerate::GeneratePath(DWORD nearest,RoadPoint end)
{
	RoadPoint path[STEPNUM];
	

	//获得从最近的那个点到随机点的二十步步长
	path[0]=m_pNodes[nearest].point;
	for(int k=0;k<STEPNUM-1;k++)
	{
		path[k+1].changeangle=path[k].changeangle+MiddlePointPlan(end.x,end.y,path[k].x,path[k].y,
			path[k].angle);
		if(path[k+1].changeangle>0.5)path[k+1].changeangle=0.5;
		if(path[k+1].changeangle<-0.5)path[k+1].changeangle=-0.5;
		path[k+1].angle=AngleNormalnize((path[k].angle+(double)STEP*tan(path[k+1].changeangle)/2.5)/PI*180)*PI/180;
		path[k+1].x=path[k].x+cos(path[k+1].angle)*(double)STEP;
		path[k+1].y=path[k].y+sin(path[k+1].angle)*(double)STEP;

	}
	return path;
}

BOOL CPathGenerate::flag_BoundaryObstacle(RoadPoint Up[],RoadPoint Down[],LINESEG u,RoadPoint paths[],int pointnum)
{

	bool flag_boundary=0;
	bool flag_obstacle=0;
	for(int f=0;f<(pointnum-1);f++)
	{
		u.s=paths[f];
		u.e=paths[f+1];

		if(CurveInsects(Up,Down,u)==1)
		{
			//flag_boundary=1;
		}

	}



	flag_obstacle=flag_PointsInObsGrid(paths,pointnum);

	if(flag_obstacle==0&&flag_boundary==0)
		return 0;
	else return 1;
	return 1;
}

void CPathGenerate::findUltipath(NodePoint *nodes,RoadPoint *clothoid,DWORD finalI)
{

	//int h=finalI;
	//int f=0;
	//RoadPoint path[POINTNUM];
	//while(nodes[h].fatherNode!=0)
	//{
	//	path[f]=nodes[h].point;
	//	h=nodes[h].fatherNode;
	//	f++;
	//}
	//path[f] = nodes[0].point;

	//m_ultiLen_linshi=f+1+CLOTHOIDPOINTNUM;
	m_ultiLen_linshi=CLOTHOIDPOINTNUM;
	//m_ultiPath=new RoadPoint[m_ultiLength];

	//EnterCriticalSection(&m_planCs);

	double newX,newY;
	OriginPt oriPoint;
	oriPoint.X=g_CurrentLocation.x;
	oriPoint.Y=g_CurrentLocation.y;
	oriPoint.Angle=g_CurrentLocation.angle;

	//for(int i=0;i<f+1;i++)
	//{
	//	m_ultiPath_Gussian_linshi[i]=path[f-i];
	//	ckCoordinate::WorldtoMap(oriPoint,m_ultiPath_Gussian_linshi[i].x,m_ultiPath_Gussian_linshi[i].y,newX,newY);
	//	m_ultiPath_linshi[i].x=newX;
	//	m_ultiPath_linshi[i].y=newY;
	//	


	//	//m_ultiPath_Gussian[i].x= (m_ultiPath[i].x*cos(g_CurrentLocation.angle)+m_ultiPath[i].y*sin(g_CurrentLocation.angle))+g_CurrentLocation.x;
	//	//m_ultiPath_Gussian[i].y= -m_ultiPath[i].x*sin(g_CurrentLocation.angle)+m_ultiPath[i].y*cos(g_CurrentLocation.angle)+g_CurrentLocation.y;


	//	//m_ultiPath_Gussian[i]=m_ultiPath[i];


	//	//m_ultiPath_Gussian[i].angle=m_ultiPath[pointNum].angle+g_CurrentLocation.angle;    //角度转换
	//}
	for(int i=0;i<CLOTHOIDPOINTNUM;i++)
	{
		m_ultiPath_Gussian_linshi[i]=clothoid[i];

		ckCoordinate::WorldtoMap(oriPoint,m_ultiPath_Gussian_linshi[i].x,m_ultiPath_Gussian_linshi[i].y,newX,newY);
		m_ultiPath_linshi[i].x=newX;
		m_ultiPath_linshi[i].y=newY;


		//m_ultiPath_Gussian[f+1+i].x= (m_ultiPath[f+1+i].x*cos(g_CurrentLocation.angle)+m_ultiPath[f+1+i].y*sin(g_CurrentLocation.angle))+g_CurrentLocation.x;
		//m_ultiPath_Gussian[f+1+i].y= -m_ultiPath[f+1+i].x*sin(g_CurrentLocation.angle)+m_ultiPath[f+1+i].y*cos(g_CurrentLocation.angle)+g_CurrentLocation.y;
		//m_ultiPath_Gussian[f+1+i].angle=m_ultiPath[f+1+i].angle+g_CurrentLocation.angle;    //角度转换


		//m_ultiPath_Gussian[f+1+i]=m_ultiPath[f+1+i];

	}



	//m_ultiPath[0].changeangle=g_curLocation.azimuth;


	//double newX,newY;
	//for (int i=0;i<m_ultiLength-1;i++)
	//{
	//	GussianToCar(m_ultiPath_Gussian[i].x,m_ultiPath_Gussian[i].y,&newX,&newY);
	//	m_ultiPath[i].x=newX;
	//	m_ultiPath[i].y=newY;
	//}




	//LeaveCriticalSection(&m_planCs);
//	delete path;

}



void CPathGenerate::findUltipathHalfGrid(NodePoint *nodes,RoadPoint *clothoid,DWORD finalI)
{
	int h=finalI;
	int f=0;
	RoadPoint path[POINTNUM];
	while(nodes[h].fatherNode!=0)
	{
		path[f]=nodes[h].point;
		h=nodes[h].fatherNode;
		f++;
	}
	path[f] = nodes[0].point;

	m_ultiLength=f+1+CLOTHOIDPOINTNUM;
	//m_ultiPath=new RoadPoint[m_ultiLength];
	EnterCriticalSection(&m_planCs);

	double newX,newY;
	OriginPt oriPoint;
	oriPoint.X=g_CurrentLocation.x;
	oriPoint.Y=g_CurrentLocation.y;
	oriPoint.Angle=g_CurrentLocation.angle;

	for(int i=0;i<f+1;i++)
	{
		m_ultiPath_HalfGrid_Gussian[i]=path[f-i];
		ckCoordinate::WorldtoMap(oriPoint,m_ultiPath_HalfGrid_Gussian[i].x,m_ultiPath_HalfGrid_Gussian[i].y,newX,newY);
		m_ultiPath_HalfGrid[i].x=newX;
		m_ultiPath_HalfGrid[i].y=newY;
	}
	for(int i=0;i<CLOTHOIDPOINTNUM;i++)
	{
		m_ultiPath_Gussian[f+1+i]=clothoid[i];

		ckCoordinate::WorldtoMap(oriPoint,m_ultiPath_HalfGrid_Gussian[f+1+i].x,m_ultiPath_HalfGrid_Gussian[f+1+i].y,newX,newY);
		m_ultiPath_HalfGrid[f+1+i].x=newX;
		m_ultiPath_HalfGrid[f+1+i].y=newY;

	}

	LeaveCriticalSection(&m_planCs);

}

void CPathGenerate::AddNodes(RoadPoint *path,int i)
{
	//m_pNodes[POINTNUM+i].point=path[STEPNUM/4];
	//m_pNodes[POINTNUM+i].fatherNode=m_pNodes[i].fatherNode;
	//int father=m_pNodes[POINTNUM+i].fatherNode;
	//m_pNodes[POINTNUM+i].cost=m_pNodes[father].cost+STEPNUM*STEP/4;

	//m_pNodes[POINTNUM*2+i].point=path[2*STEPNUM/4];
	//m_pNodes[POINTNUM*2+i].fatherNode=POINTNUM+i;
	//father=m_pNodes[POINTNUM*2+i].fatherNode;
	//m_pNodes[POINTNUM*2+i].cost=m_pNodes[father].cost+STEPNUM*STEP/4;

	//m_pNodes[POINTNUM*3+i].point=path[3*STEPNUM/4];
	//m_pNodes[POINTNUM*3+i].fatherNode=POINTNUM*2+i;
	//father=m_pNodes[POINTNUM*3+i].fatherNode;
	//m_pNodes[POINTNUM*3+i].cost=m_pNodes[father].cost+STEPNUM*STEP/4;


}

BOOL CPathGenerate::flag_PointsInObsGrid(RoadPoint *pointArr,DWORD length) 
{
	//RoadPoint currentLocation=g_CurrentLocation;
	for (int i=0;i<length;i++)
	{
		
		//double dis1=PointToSegDist(pointArr[i].x, pointArr[i].y, GridLeftDown.x, GridLeftDown.y, GridRightDown.x, GridRightDown.y);//点到网格底部的距离
		//int y=dis1/GRID_WIDTH_Y;

		//double dis2=PointToSegDist(pointArr[i].x, pointArr[i].y, GridLeftDown.x, GridLeftDown.y, GridLeftUp.x, GridLeftUp.y);//点到网格左边界的距离
		//int x=dis2/GRID_WIDTH_X;

		int x=(pointArr[i].x+GRID_I*GRID_WIDTH_X/2)/GRID_WIDTH_X;
		int y=pointArr[i].y/GRID_WIDTH_Y;

			//GridPoint p0,p1;
			//p0.X=x-BUFFERWIDTHFORCLOTHOID;
			//p0.Y=y;
			//p1.X=x+BUFFERWIDTHFORCLOTHOID;
			//p1.Y=y;
			//BOOL flag=Cal_PassLine(p0,p1,obs_grid);
			//if (flag==FALSE)
			//{
			//	return FALSE;
			//}

		for (int m=x-BUFFERWIDTHFORCLOTHOID;m<x+BUFFERWIDTHFORCLOTHOID;m++)
		{
			if (y<0||y>=GRID_J)
			{
				continue;
			}
			if (m<0||m>GRID_I)
			{
				continue;
			}
			if (obs_grid[y*100+m]==1)
			{
				//double newX,newY;
				//OriginPt orpt;
				//orpt.X=g_CurrentLocation.x;
				//orpt.Y=g_CurrentLocation.y;
				//orpt.Angle=g_CurrentLocation.angle;
				//ckCoordinate::MaptoWorld(orpt,pointArr[i].x,pointArr[i].y,newX,newY);
				//fistObs.x=newX;
				//fistObs.y=newY;
				return FALSE;
			}
		}

	}
	return TRUE;
}


int CPathGenerate::flag_SegPointsInObsGrid(BOOL isGussian,RoadPoint *pointArr,int beginIndex,DWORD length) 
{
	OriginPt oriPt;
	oriPt.X=g_CurrentLocation.x;
	oriPt.Y=g_CurrentLocation.y;
	oriPt.Angle=g_CurrentLocation.angle;

	double NewX,NewY;

	for (int i=beginIndex;i<beginIndex+length;i++)
	{
		int x,y;
		if (isGussian)
		{
			ckCoordinate::WorldtoMap(oriPt,pointArr[i].x,pointArr[i].y,NewX,NewY);
			x=(NewX+GRID_I*GRID_WIDTH_X/2)/GRID_WIDTH_X;
			y=NewY/GRID_WIDTH_Y;
		}
		else
		{
			x=(pointArr[i].x+GRID_I*GRID_WIDTH_X/2)/GRID_WIDTH_X;
			y=pointArr[i].y/GRID_WIDTH_Y;
		}
		

		//GridPoint p0,p1;
		//p0.X=x-BUFFERWIDTHFORCLOTHOID;
		//p0.Y=y;
		//p1.X=x+BUFFERWIDTHFORCLOTHOID;
		//p1.Y=y;
		//BOOL flag=Cal_PassLine(p0,p1,obs_grid);
		//if (flag==FALSE)
		//{
		//	return FALSE;
		//}

		for (int m=x-BUFFERWIDTHFORCLOTHOID;m<x+BUFFERWIDTHFORCLOTHOID;m++)
		{
			if (y<0||y>=GRID_J)
			{
				continue;
			}
			if (m<0||m>GRID_I)
			{
				continue;
			}
			if (obs_grid[y*100+m]==1)
			{
				return i;
			}
		}

	}
	return MAXD;
}


int CPathGenerate::flag_TrackBufferInObsGrid(RoadPoint *pointArr,int beginIndex,int length,int LeftOrRight,int &LineNum) 
{
	OriginPt oriPt;
	oriPt.X=g_CurrentLocation.x;
	oriPt.Y=g_CurrentLocation.y;
	oriPt.Angle=g_CurrentLocation.angle;

	double NewX,NewY;

	for (int i=beginIndex;i<beginIndex+length;i++)
	{
		int x,y;

		ckCoordinate::WorldtoMap(oriPt,pointArr[i].x,pointArr[i].y,NewX,NewY);
		x=(NewX+GRID_I*GRID_WIDTH_X/2)/GRID_WIDTH_X;
		y=NewY/GRID_WIDTH_Y;



		//GridPoint p0,p1;
		//p0.X=x-BUFFERWIDTHFORCLOTHOID;
		//p0.Y=y;
		//p1.X=x+BUFFERWIDTHFORCLOTHOID;
		//p1.Y=y;
		//BOOL flag=Cal_PassLine(p0,p1,obs_grid);
		//if (flag==FALSE)
		//{
		//	return FALSE;
		//}


		if (LeftOrRight==-1)//向左
		{
			for (int m=x;m>x-50;m--)
			{
				if (y<0||y>=GRID_J)
				{
					continue;
				}
				if (m<0||m>GRID_I)
				{
					continue;
				}
				if (obs_grid[y*100+m]==1)
				{
					LineNum=(x-m);
					return i;
				}
			}
		}
		else
		{
			for (int m=x;m<x+50;m++)
			{
				if (y<0||y>=GRID_J)
				{
					continue;
				}
				if (m<0||m>GRID_I)
				{
					continue;
				}
				if (obs_grid[y*100+m]==1)
				{
					LineNum=m-x;
					return i;
				}
			}
		}


	}
	return MAXD;
}

//返回1说明目标点附近有障碍物，返回0说明没有
BOOL CPathGenerate::flag_TargetonobsGrid(RoadPoint tar)
{
	RoadPoint currentLocation=g_CurrentLocation;
	double cy=tar.x-currentLocation.x;
	double cx=tar.y-currentLocation.y;

	double tar_x= (cx*cos(currentLocation.angle/180*PI)-cy*sin(currentLocation.angle/180*PI));
	double tar_y= cx*sin(currentLocation.angle/180*PI)+cy*cos(currentLocation.angle/180*PI);

	int gridX,gridY;
	int dx=10;
	int dy_plus=20;
	int dy_minus=30;

	gridX = (tar_x+ GRID_I*GRID_WIDTH/2)/GRID_WIDTH;
	gridY = (tar_y)/GRID_WIDTH;
	if(gridY>GRID_J||gridX>GRID_I)return 0;

	for(int i=-dx;i<dx;i++)
		for(int j=-dy_minus;j<dy_plus;j++)
		{
			if(obs_grid_simu[i+gridX][j+gridY]==1)
				return 1;
		}

		return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////靠右行驶算法部分
///////////////////////////////////////////////////////////////////////////////////////////////


//线程部分
void CPathGenerate::CreateThread()
{
	if(m_pThreadHandle != NULL)
		return;

	//char buffer[_MAX_PATH];
	//::GetModuleFileName(AfxGetInstanceHandle(), buffer, _MAX_PATH);
	//CString strHomePath = GetFilePath(buffer);

	//	m_pStream = fopen(strHomePath+strFilename, "ab+");

	m_pThreadHandle = AfxBeginThread(PlanThreadPro, 
		this,
		THREAD_PRIORITY_HIGHEST, //THREAD_PRIORITY_NORMAL,
		0,
		CREATE_SUSPENDED);

	if(m_pThreadHandle==NULL) 
	{
		AfxMessageBox("Create thread error!");
		return;
	}

	m_bStopThread = FALSE;
	m_threadMode = MODESUSPEND;

}

void CPathGenerate::StartThread()
{
	if(m_pThreadHandle == NULL)
		CreateThread();

	if(m_pThreadHandle != NULL)
		m_pThreadHandle->ResumeThread();
	else
		return;

	m_threadMode = MODERUN;
}

void CPathGenerate::SuspendThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODERUN)
		m_pThreadHandle->SuspendThread();
	else 
		return;

	m_threadMode = MODESUSPEND;
}

void CPathGenerate::ResumeThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODESUSPEND)
		m_pThreadHandle->ResumeThread();
	else
		return;

	m_threadMode = MODERUN;
}

void CPathGenerate::StopThread()
{
	if(m_pThreadHandle == NULL)
		return;

	m_bStopThread = TRUE;
	//SetEvent(g_VeloRcvHandle);
	while(m_pThreadHandle)
		Sleep(1);

	m_threadMode = MODESTOP;
}

UINT CPathGenerate::PlanThreadPro(LPVOID lpParam)
{

	CPathGenerate *pThread = (CPathGenerate *)lpParam;

	pThread->StartCPathGenerate();

	pThread->m_pThreadHandle = NULL;
	pThread->m_threadMode = MODENULL;
	pThread->m_bStopThread = FALSE;

	return 0;
}
RoadPoint CPathGenerate::GetRandomPoint(RoadPoint p)
{
	RoadPoint random;
	srand((unsigned)time(NULL));
	int rN=1+rand()%9;
	srand((unsigned)time(NULL)+1);
	int thetaN=-10+rand()%20;
	if(thetaN==0)thetaN=1;
	double delta_theta=PI/2/thetaN;
	double R=20/rN;
	random.x=R*sin(p.angle+delta_theta)+p.x;
	random.y=R*cos(p.angle+delta_theta)+p.y;



	return random;
}
//pt 角度单位为弧度
int lastIndex=0;
int CPathGenerate::GetCloseTrackPoint(GpsLine pt)
{
	bool closeflag=false;//记录是否靠近节点
	vector<int> selectindexs;
	for (int i=0;i<fileLength;i++)
	{
		double dis=(pt.Gnx-m_RoadDataBuffer[i].x)*(pt.Gnx-m_RoadDataBuffer[i].x)
			+(pt.Gny-m_RoadDataBuffer[i].y)*(pt.Gny-m_RoadDataBuffer[i].y);
		if(dis<MIN_TRACKRADS)
		{
			//判断为备选点
            selectindexs.push_back(i);
		}
	}
	if (selectindexs.empty())
	{
		return -1;
	}
	//根据车体航向角约束
	float vercurx=sin(pt.azimuth);
	float vercury=cos(pt.azimuth);
	float mindis=MIN_TRACKRADS;
	int resindex=-1;
	for (int j=0;j<selectindexs.size();j++)
	{
		int idx=selectindexs[j];
		float verroadx=sin(m_RoadDataBuffer[idx].angle);
		float verroady=cos(m_RoadDataBuffer[idx].angle);
		if ((vercurx*verroadx+vercury*verroady)>0)
		{
			double disrad=(pt.Gnx-m_RoadDataBuffer[idx].x)*(pt.Gnx-m_RoadDataBuffer[idx].x)
				+(pt.Gny-m_RoadDataBuffer[idx].y)*(pt.Gny-m_RoadDataBuffer[idx].y);
			if (lastIndex==0)
			{	
				if (disrad<mindis)
				{
					mindis=disrad;
					resindex=idx;
				}
			}
			else 
			{
				if (disrad<mindis&&(abs(lastIndex-idx)<50))
				{
					mindis=disrad;
					resindex=idx;
				}
			}
		
		}
	}
	lastIndex=resindex;
	return resindex;
}
//搜索上一个经过的点，若靠近距离小于两米则更新下一个结点
int CPathGenerate::GetCurrentPosition(int currentposi)
{
	bool closeflag=false;//记录是否靠近节点
	//根据车体航向角约束
	float vercurx=sin(g_CurrentLocation.angle);
	float vercury=cos(g_CurrentLocation.angle);

	//先判定是否进入节点区域，若已接近则退出循环并返回当前点
	for(int i=0;i<fileLength;i++)
	{
		float verroadx=sin(m_RoadDataBuffer[i].angle);
		float verroady=cos(m_RoadDataBuffer[i].angle);
		double dis=(g_CurrentLocation.x-m_RoadDataBuffer[i].x)*(g_CurrentLocation.x-m_RoadDataBuffer[i].x)
			+(g_CurrentLocation.y-m_RoadDataBuffer[i].y)*(g_CurrentLocation.y-m_RoadDataBuffer[i].y);
		if(dis<MINDIS)
		{
		/*	if (vercurx*verroadx+vercury*verroady>0)
			{*/if (currentposi>i)
			{
				continue;
			}
			else
			{
				currentposi=i;
				closeflag=true;
				flag_IsCloseTo=TRUE;
				break;
			}
			
			//}
		}
	}
	if (closeflag)
	{
		return currentposi;
	}
	else  //车辆在结点之间
	{
		flag_IsCloseTo=FALSE;
		for (int i=0;i<fileLength-1;i++)
		{
			double dx0=g_CurrentLocation.x-m_RoadDataBuffer[i].x;
			double dy0=g_CurrentLocation.y-m_RoadDataBuffer[i].y;
			double dx1=g_CurrentLocation.x-m_RoadDataBuffer[i+1].x;
			double dy1=g_CurrentLocation.y-m_RoadDataBuffer[i+1].y;
			float verroadx=sin(AngleNormalnize(m_RoadDataBuffer[i].angle));
			float verroady=cos(AngleNormalnize(m_RoadDataBuffer[i].angle));
			if (dx0*dx1+dy0*dy1<0)//判定在哪两个点之间
			{
	
				if (vercurx*verroadx+vercury*verroady>0)
				{
					currentposi=i;
					break;
				}
			}
		}
		return currentposi;
	}

}


double CPathGenerate::AngleTransfer(double angle)
{
	if (angle<=PI/2)
	{
		return PI/2-angle;
	}
	else return 5*PI/2-angle;
}

BOOL CPathGenerate::Cal_PassLine(GridPoint p0,GridPoint p1,int* obs)
{

	int dx, // x差值
		dy, // y差值
		dx2, // dx,dy * 2
		dy2, 
		x_inc, // x步长
		y_inc, // y步长
		error, // 误差
		index; 
	int xStart=p0.X,yStart=p0.Y;
	dx = p1.X-p0.X;
	dy = p1.Y-p0.Y;
	//
	//if (obs.size()!=10000)
	//{
	//	AfxMessageBox("障碍物网格错误");
	//	return FALSE;
	//}
	//判定直线方向，不考虑Y为负值
	if (dx>=0)
	{
		x_inc = 1;//网格填充对应

	} 
	else
	{
		x_inc = -1;
		dx = -dx; 

	} 
	y_inc=1;//
	dx2 = dx << 1;
	dy2 = dy << 1;
	if (dx > dy)//斜率小于1的情况
	{
		error = dy2 - dx; 
		for (index=0; index < dx; index++)
		{
			if (error >= 0) 
			{
				error-=dx2;
				yStart+=y_inc;
			} 
			error+=dy2;
			xStart+=x_inc;
			//判断是否超过边界
			if (xStart<0||xStart>=GRID_I||yStart<0||yStart>=GRID_J)
			{
				//obs[yStart*100+xStart]=0;
			   continue;
			}
			//判定网格对应位置是否有障碍物
			if (obs[yStart*100+xStart]!=0)
			{
				TT7=xStart;
				TT8=yStart;
				return FALSE;
			}
		} 

	} 
	else 
	{//斜率大于等于1的情况

		error = dx2 - dy;
		for (index=0; index < dy; index++)
		{
			if (error >= 0)
			{
				error-=dy2;
				xStart+=x_inc;

			} 
			error+=dx2;
			yStart+=y_inc;
			//判断是否超过边界
			if (xStart<0||xStart>=GRID_I||yStart<0||yStart>=GRID_J)
			{
				//obs[yStart*100+xStart]=0;
		        continue;
			}
			//判定网格对应位置是否有障碍物
			if (obs[yStart*100+xStart]!=0)
			{
				return FALSE;
			}

		} 

	} 
	return TRUE;
}
vector<GridPoint> grids;

BOOL CPathGenerate::GetBufferGrid(double x1,double y1,double x2,double y2)
{
	/*vector<GridPoint> gr;
	grids.swap(gr);*/

	if (y2<y1)
	{
		return FALSE;
	}

	double tanTheta=(y2-y1)/(x2-x1);

	double Theta=atan2(y2-y1,x2-x1);

	double delta_x=BUFFERWIDTH*GRID_WIDTH_X/sin(Theta);


	int Xleft;
	int Xright;
	int Xpleft=50-delta_x/0.1;
	int Xpright=50+delta_x/0.1;


	int Yulti=100;
	if (y2<100*0.2)
	{
		Yulti=y2/0.2;
	}

	for (int Y=1;Y<=Yulti;Y++)
	{
		double y=Y*0.2;
		double x=y/tanTheta;//!

		Xleft=(x-delta_x+5)/0.1;
		Xright=(x+delta_x+5)/0.1;

		if (Xright>Xpright)
		{
			for (int i=Xpleft;i<=Xright;i++)
			{
				if (i>=0&&i<100)
				{
					/*GridPoint g;
					g.X=i;
					g.Y=Y-1;
					grids.push_back(g);*/
					if (obs_grid[(Y-1)*100+i]==1)
					{
						return FALSE;
					}
				}
			}
		}
		else if (Xleft<Xpleft)
		{
			for (int i=Xleft;i<=Xpright;i++)
			{
				if (i>=0&&i<100)
				{
					/*GridPoint g;
					g.X=i;
					g.Y=Y-1;
					grids.push_back(g);*/
					if (obs_grid[(Y-1)*100+i]==1)
					{
						return FALSE;
					}
				}
			}
		}
		else
		{
			for (int i=Xpleft;i<=Xpright;i++)
			{
				if (i>=0&&i<100)
				{
					/*GridPoint g;
					g.X=i;
					g.Y=Y-1;
					grids.push_back(g);*/
					if (obs_grid[(Y-1)*100+i]==1)
					{
						return FALSE;
					}
				}
			}
		}

		Xpleft=Xleft;
		Xpright=Xright;

	}

	return TRUE;
}
//同时计算多根先产生缓冲区效果，计算方式为X坐标横向移动
BOOL CPathGenerate::SearchBestTarget(GridPoint p0,GridPoint p1,int* obs,int width)
{
	//int index=0; //
	/*int start=p0.X-width;
	int end=p0.X+width;*/
	BOOL pass=TRUE;
	for (int i=-width;i<width;i++)
	{
		GridPoint temp0,temp1;
		//if (abs(p1.Y-p0.Y)>=abs(p1.X-p0.X))
		//{
			temp0.X=p0.X+i;
			temp0.Y=p0.Y;
			temp1.X=p1.X+i;
			temp1.Y=p1.Y;
		//}
		//else
		//{
		/*	temp0.X=p0.X;
			temp0.Y=p0.Y+i;
			temp1.X=p1.X;
			temp1.Y=p1.Y+i;*/
		//}
		
		pass=Cal_PassLine(temp0,temp1 ,obs);
		if (!pass)
		{
			return FALSE; //缓冲区不能通过
		}
	}

 	return TRUE;

}
//判定可否通行
BOOL CPathGenerate::flag_Available(double x1,double y1,double x2,double y2)   //1可通行，0不行
{
	return GetBufferGrid(x1,y1,x2,y2);
	/*for (int i=0;i<grids.size();i++)
	{
		if (obs_grid[grids[i].Y*100+grids[i].X]==1)
		{
			return FALSE;
		}
	}
	return TRUE;*/
}

void CPathGenerate::FindAvailableTar(int* obj)  ////找到局部规划目标点
{
	//判断是否可通行
	if (flag_Intersect!=7 && flag_Intersect!=8 && SearchBestTarget(OriginStart,OriginEnd,obj,BUFFERWIDTH))//判断是否可通行  tarJ==GRID_J-1？
	{
		//记录此时目标点在网格中的行列编号
		tarI=OriginEnd.X;
		tarJ=OriginEnd.Y;
		return;
	}

	RoadPoint startP,endP;//起点和终点（局部坐标表示）
	startP.x=OriginStart.X;
	startP.y=OriginStart.Y;
	endP.x=OriginEnd.X;
 	endP.y=OriginEnd.Y;

	RoadPoint intersectPoint;

	GridPoint end;


	//如果不可通行，则在网格中找end点
#pragma region 下一路网点与当前点连线(直线)与网格顶部相交
	if(flag_Intersect==1||flag_Intersect==4)//下一路网点与当前点连线(直线)与网格顶部相交，此时对网格的每个行j，从连线与j行的交点TopX,先向右后向左搜索可通行的点
	{
		int TopX; //每一行与连线交点的列号
		RoadPoint leftP,rightP;//每一行的左右端点(局部坐标表示)
		int beginNum=GRID_J-1;
		if (flag_Intersect==4)
		{
			beginNum=OriginEnd.Y;
		}

		for (int j=beginNum;j>TOLDISTANCE/GRID_WIDTH_Y;j=j-1)   //Lu 11.04  GRID_J/5
		{

			//计算连线与j行的交点的列号
			leftP.x=0;
			leftP.y=j;
			rightP.x=GRID_I-1;
			rightP.y=j;
			intersectPoint=getIntersectPoint(leftP,rightP,startP,endP);
			TopX=(int)intersectPoint.x;//对于每个行j,计算此时连线与行j的交点的列号

			for (int i=TopX-1;i>GRID_I/4;i--)  //从TopX向左搜索网格中的可通行点
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;
					return;
				}
			}

			for (int i=TopX+1;i<GRID_I*3/4;i++)  //从TopX向右搜索网格中的可通行点
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;
					return;
				}
			}

		}
	}
#pragma endregion 


#pragma region 下一路网点与当前点连线(直线)与网格左右边界相交
	else if (flag_Intersect!=7 && flag_Intersect!=8)
	{
		RoadPoint upP,downP;//每一列的上下端点（局部坐标）
		int BeginY=OriginEnd.Y;
		int BeginNum=OriginEnd.X;

		if (flag_Intersect==3||flag_Intersect==6) //终点在右上方的情况
		{
			for (int i=BeginNum;i>=0;i--) 
			{
				if (i<=OriginStart.X)  //要修改
				{
					for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GRID_J*7/8;j++)   //Lu 10.27
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				else
				{
					upP.x=i;
					upP.y=GRID_J-1;
					downP.x=i;
					downP.y=0;
					intersectPoint=getIntersectPoint(downP,upP,startP,endP);
					BeginY=(int)intersectPoint.y;//对于每个列i,计算此时连线与列i的交点的行号

					for (int j=BeginY+1;j<GRID_J*7/8;j++)    //从BeginY向上搜索网格中的可通行点
					{
						if (BeginY<TOLDISTANCE/GRID_WIDTH_Y) //Lu 10.27
						{
							continue;
						}

						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
					for (int j=BeginY-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)  //从BeginY向下搜索网格中的可通行点   //Lu 10.27
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}

			}
		}
		else if (flag_Intersect==2||flag_Intersect==5)//目标点在左上方的情况
		{
			for (int i=BeginNum;i<GRID_I-1;i++)
			{
				if (i>OriginStart.X)
				{
					for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GRID_J*7/8;j++)  //Lu 10.27  GRID_J/8
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				else
				{
					upP.x=i;
					upP.y=GRID_J-1;
					downP.x=i;
					downP.y=0;
					intersectPoint=getIntersectPoint(downP,upP,startP,endP);
					BeginY=(int)intersectPoint.y;//对于每个列i,计算此时连线与列i的交点的行号

					for (int j=BeginY+1;j<GRID_J*7/8;j++) //从BeginY向上搜索网格中的可通行点
					{
						if (BeginY<TOLDISTANCE/GRID_WIDTH_Y) //Lu 10.27
						{
							continue;
						}
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
					for (int j=BeginY-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--) //从BeginY向下搜索网格中的可通行点
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				
			}
		}
	}
#pragma endregion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region 下一路网点在网格下方
	else
	{
		if (flag_Intersect==7)//下一路网点在网格左下方
		{
			for (int i=0;i<GRID_I;i++)
			{
				for (int j=OriginStart.Y+10;j<GRID_J;j++)
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
					{
						tarI=i;
						tarJ=j;
						return;
					}
				}
			}
		}
		else//右下方
		{
			for (int i=GRID_I-1;i>=0;i++)
			{
				for (int j=OriginStart.Y+10;j<GRID_J;j++)
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
					{
						tarI=i;
						tarJ=j;
						return;
					}
				}
			}
		}
	}
#pragma endregion 下一路网点在网格下方

	tarI=0;//未找到可通行点。
	tarJ=0;
	return;
}

void CPathGenerate::FindAvailableTar(int* obj,int GridI,int GridJ)  ////找到局部规划目标点
{
	//判断是否可通行
	if (SearchBestTarget(OriginStart,OriginEnd,obj,BUFFERWIDTH))//判断是否可通行  tarJ==GRID_J-1？
	{
		//记录此时目标点在网格中的行列编号
		tarI=OriginEnd.X;
		tarJ=OriginEnd.Y;
		return;
	}

	RoadPoint startP,endP;//起点和终点（局部坐标表示）
	startP.x=OriginStart.X;
	startP.y=OriginStart.Y;
	endP.x=OriginEnd.X;
	endP.y=OriginEnd.Y;

	RoadPoint intersectPoint;

	GridPoint end;


	//如果不可通行，则在网格中找end点
#pragma region 下一路网点与当前点连线(直线)与网格顶部相交
	if(flag_Intersect==1||flag_Intersect==4)//下一路网点与当前点连线(直线)与网格顶部相交，此时对网格的每个行j，从连线与j行的交点TopX,先向右后向左搜索可通行的点
	{
		int TopX; //每一行与连线交点的列号
		RoadPoint leftP,rightP;//每一行的左右端点(局部坐标表示)
		int beginNum=GridJ-1;
		if (flag_Intersect==4)
		{
			beginNum=OriginEnd.Y;
		}

		for (int j=beginNum;j>TOLDISTANCE/GRID_WIDTH_Y;j=j-1)   //Lu 10.27
		{

			//计算连线与j行的交点的列号
			leftP.x=0;
			leftP.y=j;
			rightP.x=GridI-1;
			rightP.y=j;
			intersectPoint=getIntersectPoint(leftP,rightP,startP,endP);
			TopX=(int)intersectPoint.x;//对于每个行j,计算此时连线与行j的交点的列号

			for (int i=TopX+1;i<GridI*3/4;i++)  //从TopX向右搜索网格中的可通行点
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;
					return;
				}
			}
			for (int i=TopX-1;i>GridI/4;i--)  //从TopX向左搜索网格中的可通行点
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;
					return;
				}
			}
		}
	}
#pragma endregion 


#pragma region 下一路网点与当前点连线(直线)与网格左右边界相交
	else  //目标点与当前点连线与x轴夹角小于45度情况，此时对网格的每个列i，从连线与i列的交点BeginY,先向右后向左搜索可通行的点
	{
		RoadPoint upP,downP;//每一列的上下端点（局部坐标）
		int BeginY=OriginEnd.Y;
		int BeginNum=OriginEnd.X;

		if (flag_Intersect==3||flag_Intersect==6) //终点在右上方的情况
		{
			for (int i=BeginNum;i>=0;i--) 
			{
				if (i<=OriginStart.X)  //要修改
				{
					for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GridJ*7/8;j++)   //Lu 10.27
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				else
				{
					upP.x=i;
					upP.y=GridJ-1;
					downP.x=i;
					downP.y=0;
					intersectPoint=getIntersectPoint(downP,upP,startP,endP);
					BeginY=(int)intersectPoint.y;//对于每个列i,计算此时连线与列i的交点的行号

					for (int j=BeginY+1;j<GridJ*7/8;j++)    //从BeginY向上搜索网格中的可通行点
					{
						if (BeginY<(TOLDISTANCE/GRID_WIDTH_Y)) //Lu 10.27
						{
							continue;
						}

						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
					for (int j=BeginY-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)  //从BeginY向下搜索网格中的可通行点   //Lu 10.27
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}

			}
		}
		else if (flag_Intersect==2||flag_Intersect==5)//目标点在左上方的情况
		{
			for (int i=BeginNum;i<GridI-1;i++)
			{
				if (i>OriginStart.X)
				{
					for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GridJ*7/8;j++)  //Lu 10.27  GRID_J/8
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				else
				{
					upP.x=i;
					upP.y=GridJ-1;
					downP.x=i;
					downP.y=0;
					intersectPoint=getIntersectPoint(downP,upP,startP,endP);
					BeginY=(int)intersectPoint.y;//对于每个列i,计算此时连线与列i的交点的行号

					for (int j=BeginY+1;j<GridJ*7/8;j++) //从BeginY向上搜索网格中的可通行点
					{
						if (BeginY<TOLDISTANCE/GRID_WIDTH_Y) //Lu 10.27
						{
							continue;
						}
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
					for (int j=BeginY-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--) //从BeginY向下搜索网格中的可通行点
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obj,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}

			}
		}
	}
#pragma endregion
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	tarI=0;//未找到可通行点。
	tarJ=0;
	return;
}

void CPathGenerate::FindTarInGrid(int I,int J)
{
	
	GridPoint end;
	RoadPoint intersectPoint;
	RoadPoint startP,endP;//起点和终点（局部坐标表示）
	startP.x=OriginStart.X;
	startP.y=OriginStart.Y;
	endP.x=OriginEnd.X;
	endP.y=OriginEnd.Y;

	XXX1=I;
	XXX2=J;

#pragma region 下一路网点与当前点连线(直线)与网格顶部相交
	if (flag_Intersect==1||flag_Intersect==4)
	{
		int TopX;
		RoadPoint leftP,rightP;//每一行的左右端点(局部坐标表示)
		//计算连线与j行的交点的列号
		leftP.x=0;
		leftP.y=J;
		rightP.x=GRID_I-1;
		rightP.y=J;
		intersectPoint=getIntersectPoint(leftP,rightP,startP,endP);
		TopX=(int)intersectPoint.x;//判断下一路网点和当前点的连线与第J行的交点的列号

		RR1=intersectPoint;
		XXX=TopX;

#pragma region 把第J行剩下的列编号的网格点判断是否可通行
		if (I>=TopX)
		{
			
			for (int i=I+1;i<GRID_I*3/4;i++)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{	
					tarI=i;
					tarJ=J;

					XXX3=i;

					return;
				}
			}
			for (int i=TopX-1;i>GRID_I/4;i--)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=J;

					XXX4=i;

					return;
				}
			}
		}
		else
		{
			for (int i=I-1;i>GRID_I/4;i--)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=J;

					XXX5=i;

					return;
				}
			}

		}
#pragma endregion 把第J行剩下的列编号的网格点判断是否可通行


#pragma region 从第J-1行开始寻找可通行点
		for (int j=J-1;j>TOLDISTANCE/GRID_WIDTH_Y;j=j-1)
		{
			
			leftP.x=0;
			leftP.y=j;
			rightP.x=GRID_I-1;
			rightP.y=j;
			intersectPoint=getIntersectPoint(leftP,rightP,startP,endP);
			TopX=(int)intersectPoint.x;//判断下一路网点和当前点的连线与第J行的交点的列号

			RR2=intersectPoint;
			TT1=TopX;


			for (int i=TopX-1;i>GRID_I/4;i--)
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;

					XXX8=i;
					XXX9=j;

					return;
				}
			}


			for (int i=TopX+1;i<GRID_I*3/4;i++)
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;

					XXX6=i;
					XXX7=j;

					return;
				}
			}

		}
#pragma endregion 从第J-1行开始寻找可通行点
	}
#pragma endregion


#pragma region 下一路网点与当前点连线(直线)与网格左右边界相交
	else if (flag_Intersect!=7&&flag_Intersect!=8)
	{ 
		XXX10=10;

#pragma region 把第I列剩下的行编号的点判断可否通行
		RoadPoint upP,downP;//每一列的上下端点（局部坐标）
		upP.x=I;
		upP.y=GRID_J-1;
		downP.x=I;
		downP.y=0;
		intersectPoint=getIntersectPoint(downP,upP,startP,endP);
		int tJ=(int)intersectPoint.y;
		if (J>=tJ)
		{
			for (int j=J+1;j<GRID_J*7/8;j++)
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=I;
					tarJ=j;
					return;
				}
			}
			for (int j=tJ-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=I;
					tarJ=j;
					return;
				}
			}
		}
		else
		{
			for (int j=J-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--) 
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=I;
					tarJ=j;
					return;
				}
			}
		}
#pragma endregion


#pragma region 下一路网点在右上方
			if(flag_Intersect==3||flag_Intersect==6)
			{
				//int newBeginJ;
				for (int i=I-1;i>0;i--)
				{
					if (I<=OriginStart.X)
					{
						for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GRID_J*7/8;j++)   //Lu 10.27
						{
							end.X=i;
							end.Y=j;
							if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
							{
								tarI=i;
								tarJ=j;
								return;
							}
						}
					}
					else
					{
						upP.x=i;
						upP.y=GRID_J-1;
						downP.x=i;
						downP.y=0;
						intersectPoint=getIntersectPoint(downP,upP,startP,endP);
						int newBeginJ=(int)intersectPoint.y;

						for (int j=newBeginJ+1;j<GRID_J*7/8;j++)
						{
							if (j<TOLDISTANCE/GRID_WIDTH_Y)
							{
								continue;
							}
							end.X=i;
							end.Y=j;
							if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
							{
								tarI=i;
								tarJ=j;
								return;
							}
						}
						for (int j=newBeginJ-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)
						{
							end.X=i;
							end.Y=j;
							if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
							{
								tarI=i;
								tarJ=j;
								return;
							}
						}
					}
					
				}
			}
#pragma endregion

#pragma region 下一路网点在左上方
			else if (flag_Intersect==2||flag_Intersect==5)
			{
				for (int i=I+1;i<GRID_I;i++)
				{
					if (I>=OriginStart.X)
					{
						for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GRID_J*7/8;j++)   //Lu 10.27
						{
							end.X=i;
							end.Y=j;
							if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
							{
								tarI=i;
								tarJ=j;
								return;
							}
						}
					}
					else
					{
						upP.x=i;
						upP.y=GRID_J-1;
						downP.x=i;
						downP.y=0;
						intersectPoint=getIntersectPoint(downP,upP,startP,endP);
						int newBeginJ=(int)intersectPoint.y;

						for (int j=newBeginJ+1;j<GRID_J*7/8;j++)
						{
							if (j<TOLDISTANCE/GRID_WIDTH_Y)
							{
								continue;
							}
							end.X=i;
							end.Y=j;
							if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
							{
								tarI=i;
								tarJ=j;
								return;
							}
						}
						for (int j=newBeginJ-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)
						{
							end.X=i;
							end.Y=j;
							if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
							{	
								tarI=i;
								tarJ=j;
								return;
							}
						}
					}
				}
			}
#pragma endregion

	}
#pragma endregion
	
#pragma region 下一路网点在网格下方
	else
	{
		for (int j=J+1;j<GRID_J;j++)
		{
			end.X=I;
			end.Y=j;
			if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
			{
				tarI=I;
				tarJ=j;
				return;
			}
		}



		if (flag_Intersect==7)//下一路网点在网格左下方
		{
			for (int i=I+1;i<GRID_I;i++)
			{
				for (int j=OriginStart.Y+10;j<GRID_J;j++)
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
					{
						tarI=i;
						tarJ=j;
						return;
					}
				}
			}
		}
		else//右下方
		{
			for (int i=I-1;i>=0;i++)
			{
				for (int j=OriginStart.Y+10;j<GRID_J;j++)
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
					{
						tarI=i;
						tarJ=j;
						return;
					}
				}
			}
		}
	}
#pragma endregion 下一路网点在网格下方



	//未找到可通行点
	tarJ=0;
	tarI=0;

}

void CPathGenerate::FindTarInGrid(int I,int J,int GridI,int GridJ)
{

	GridPoint end;
	RoadPoint intersectPoint;
	RoadPoint startP,endP;//起点和终点（局部坐标表示）
	startP.x=OriginStart.X;
	startP.y=OriginStart.Y;
	endP.x=OriginEnd.X;
	endP.y=OriginEnd.Y;

	XXX1=I;
	XXX2=J;

#pragma region 下一路网点与当前点连线(直线)与网格顶部相交
	if (flag_Intersect==1||flag_Intersect==4)
	{
		int TopX;
		RoadPoint leftP,rightP;//每一行的左右端点(局部坐标表示)
		//计算连线与j行的交点的列号
		leftP.x=0;
		leftP.y=J;
		rightP.x=GridI-1;
		rightP.y=J;
		intersectPoint=getIntersectPoint(leftP,rightP,startP,endP);
		TopX=(int)intersectPoint.x;//判断下一路网点和当前点的连线与第J行的交点的列号

		RR1=intersectPoint;
		XXX=TopX;

#pragma region 把第J行剩下的列编号的网格点判断是否可通行
		if (I>=TopX)
		{

			for (int i=I+1;i<GridI*3/4;i++)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{	
					tarI=i;
					tarJ=J;

					XXX3=i;

					return;
				}
			}
			for (int i=TopX-1;i>GridI/4;i--)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=J;

					XXX4=i;

					return;
				}
			}
		}
		else
		{
			for (int i=I-1;i>GridI/4;i--)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=J;

					XXX5=i;

					return;
				}
			}

		}
#pragma endregion 把第J行剩下的列编号的网格点判断是否可通行


#pragma region 从第J-1行开始寻找可通行点
		for (int j=J-1;j>GridJ/5;j=j-1)
		{

			leftP.x=0;
			leftP.y=j;
			rightP.x=GridI-1;
			rightP.y=j;
			intersectPoint=getIntersectPoint(leftP,rightP,startP,endP);
			TopX=(int)intersectPoint.x;//判断下一路网点和当前点的连线与第J行的交点的列号

			RR2=intersectPoint;
			TT1=TopX;

			for (int i=TopX+1;i<GridI*3/4;i++)
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;

					XXX6=i;
					XXX7=j;

					return;
				}
			}
			for (int i=TopX-1;i>GridI/4;i--)
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=i;
					tarJ=j;

					XXX8=i;
					XXX9=j;

					return;
				}
			}
		}
#pragma endregion 从第J-1行开始寻找可通行点
	}
#pragma endregion


#pragma region 下一路网点与当前点连线(直线)与网格左右边界相交
	else
	{ 
		XXX10=10;

#pragma region 把第I列剩下的行编号的点判断可否通行
		RoadPoint upP,downP;//每一列的上下端点（局部坐标）
		upP.x=I;
		upP.y=GridJ-1;
		downP.x=I;
		downP.y=0;
		intersectPoint=getIntersectPoint(downP,upP,startP,endP);
		int tJ=(int)intersectPoint.y;
		if (J>=tJ)
		{
			for (int j=J+1;j<GridJ*7/8;j++)
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=I;
					tarJ=j;
					return;
				}
			}
			for (int j=tJ-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=I;
					tarJ=j;
					return;
				}
			}
		}
		else
		{
			for (int j=J-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--) 
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
				{
					tarI=I;
					tarJ=j;
					return;
				}
			}
		}
#pragma endregion


#pragma region 下一路网点在右上方
		if(flag_Intersect==3||flag_Intersect==6)
		{
			//int newBeginJ;
			for (int i=I-1;i>0;i--)
			{
				if (I<=OriginStart.X)
				{
					for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GridJ*7/8;j++)   //Lu 10.27
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				else
				{
					upP.x=i;
					upP.y=GridJ-1;
					downP.x=i;
					downP.y=0;
					intersectPoint=getIntersectPoint(downP,upP,startP,endP);
					int newBeginJ=(int)intersectPoint.y;

					for (int j=newBeginJ+1;j<GridJ*7/8;j++)
					{
						if (j<TOLDISTANCE/GRID_WIDTH_Y)
						{
							continue;
						}
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
					for (int j=newBeginJ-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}

			}
		}
#pragma endregion

#pragma region 下一路网点在左上方
		else if (flag_Intersect==2||flag_Intersect==5)
		{
			for (int i=I+1;i<GridI;i++)
			{
				if (I>=OriginStart.X)
				{
					for (int j=TOLDISTANCE/GRID_WIDTH_Y;j<GridJ*7/8;j++)   //Lu 10.27
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
				else
				{
					upP.x=i;
					upP.y=GridJ-1;
					downP.x=i;
					downP.y=0;
					intersectPoint=getIntersectPoint(downP,upP,startP,endP);
					int newBeginJ=(int)intersectPoint.y;

					for (int j=newBeginJ+1;j<GridJ*7/8;j++)
					{
						if (j<TOLDISTANCE/GRID_WIDTH_Y)
						{
							continue;
						}
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
						{
							tarI=i;
							tarJ=j;
							return;
						}
					}
					for (int j=newBeginJ-1;j>TOLDISTANCE/GRID_WIDTH_Y;j--)
					{
						end.X=i;
						end.Y=j;
						if(SearchBestTarget(OriginStart,end,obs_grid,BUFFERWIDTH))
						{	
							tarI=i;
							tarJ=j;
							return;
						}
					}
				}
			}
		}
#pragma endregion

	}
#pragma endregion

	//未找到可通行点
	tarJ=0;
	tarI=0;

}

RoadPoint CPathGenerate::FindTarMoveX(int moveWidth)
{
	RoadPoint Tar_Point;
	Tar_Point.x=1000000;
	Tar_Point.y=1000000;

	//获取下一路网点相对于平移后的起始点的局部坐标
	double dx=m_roadNetBuffer[CurrentNetPoint+1].x-g_CurrentLocation.x;
	double dy=m_roadNetBuffer[CurrentNetPoint+1].y-g_CurrentLocation.y;
	double cx=dx*cos(g_CurrentLocation.angle)-dy*sin(g_CurrentLocation.angle)-moveWidth*GRID_WIDTH_X;
	double cy=dx*sin(g_CurrentLocation.angle)+dy*cos(g_CurrentLocation.angle);

	int NEW_GRID_I;  //新的网格的列数
	if (moveWidth>0)  //往右平移
	{
		//填充网格
		for (int j=0;j<100;j++)
		{
			for (int i=1;i<=moveWidth;i++)
			{
				obs_grid[j*100+99+i]=1;
			}
		}
		NEW_GRID_I=GRID_I+moveWidth;
	}
	else   //往左平移，此时不填充网格，直接裁右边不对称部分
	{
		NEW_GRID_I=GRID_I+moveWidth;
	}

	GridPoint start,end;//用于可通行点判断的起点和终点两个网格点，X,Y是网格的列和行编号
	start.X=NEW_GRID_I/2;
	start.Y=0;

#pragma region 越界情况判断
	//越界情况判断
	if (GRID_J*GRID_WIDTH_Y/cy*cx>NEW_GRID_I/2*GRID_WIDTH_X)//Y为GRID_J时，根据目标点与起始点（原点）计算的X大于网格右边界
	{
		end.X=NEW_GRID_I-1;
		end.Y=(NEW_GRID_I/2*GRID_WIDTH_X*cy/cx)/GRID_WIDTH_Y;
	}
	else if (GRID_J*GRID_WIDTH_Y/cy*cx<-(NEW_GRID_I/2*GRID_WIDTH_X))//Y为GRID_J时，根据目标点与起始点（原点）计算的X小于网格左边界
	{
		end.X=0;
		end.Y=(-(NEW_GRID_I/2*GRID_WIDTH_X)*cy/cx)/GRID_WIDTH_Y;
	}
	else
	{
		end.X=(GRID_J*GRID_WIDTH_Y/cy*cx)/GRID_WIDTH_X+NEW_GRID_I/2;
		end.Y=GRID_J-1;
	}
#pragma endregion 越界情况判断

	if (tarJ==GRID_J-1&&SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))//判断是否可通行  tarJ==GRID_J-1？
	{
		Tar_Point.x=GRID_J*GRID_WIDTH_Y/cy*cx;
		Tar_Point.y=GRID_J*GRID_WIDTH_Y;

		//记录此时目标点在网格中的行列编号
		tarI=end.X;
		tarJ=end.Y;

		return Tar_Point;
	}

#pragma region 下一路网点与当前点连线与x轴夹角大于45度情况
	if(abs(cy)>=abs(cx))//目标点和原点连线与x轴夹角大于45度的情况，此时对网格的每个行j，从连线与j行的交点TopX,先向右后向左搜索可通行的点
	{
		int TopX;


		for (int j=GRID_J-1;j>GRID_J/4;j=j-1)
		{
			TopX=(j*GRID_WIDTH_Y/cy*cx)/GRID_WIDTH_X+NEW_GRID_I/2;//对于每个行j,计算此时连线与行j的交点的编号

			for (int i=TopX+1;i<NEW_GRID_I*3/4;i++)  //从TopX向右搜索网格中的可通行点
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=j;
					return Tar_Point;
				}
			}
			for (int i=TopX-1;i>NEW_GRID_I/4;i--)  //从TopX向左搜索网格中的可通行点
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=j;
					return Tar_Point;
				}
			}
		}
	}
#pragma endregion 下一路网点与当前点连线与x轴夹角大于45度情况
#pragma region 下一路网点与当前点连线与x轴夹角小于45度情况
	else  //目标点与当前点连线与x轴夹角小于45度情况，此时对网格的每个列i，从连线与i列的交点BeginY,先向右后向左搜索可通行的点
	{
		int BeginY=end.Y;
		int BeginX=end.X;
		if (cx>0) //目标点在右上方的情况
		{
			for (int i=BeginX;i>=0;i--)  //BeginX此时为GRID_I-1
			{
				BeginY=(i-NEW_GRID_I/2)*GRID_WIDTH_X/cx*cy/GRID_WIDTH_Y;//对于每个列i,计算此时连线与列i的交点编号

				for (int j=BeginY+1;j<GRID_J*3/4;j++)    //从BeginY向上搜索网格中的可通行点
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=i;
						tarJ=j;
						return Tar_Point;
					}
				}
				for (int j=BeginY-1;j>GRID_J/4;j--)  //从BeginY向下搜索网格中的可通行点
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=i;
						tarJ=j;
						return Tar_Point;
					}
				}

			}
		}
		else//目标点在左上方的情况
		{
			for (int i=BeginX;i<NEW_GRID_I-1;i++)  //BeginX此时为0
			{
				BeginY=(i-NEW_GRID_I/2)*GRID_WIDTH_X/cx*cy/GRID_WIDTH_Y;

				for (int j=BeginY+1;j<GRID_J*3/4;j++) //从BeginY向上搜索网格中的可通行点
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=i;
						tarJ=j;
						return Tar_Point;
					}
				}
				for (int j=BeginY-1;j>GRID_J/4;j--) //从BeginY向下搜索网格中的可通行点
				{
					end.X=i;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=i;
						tarJ=j;
						return Tar_Point;
					}
				}
			}
		}
	}
#pragma endregion 目标点与当前点连线与x轴夹角小于45度情况
	
	

return Tar_Point; //未找到可通行点。Tar_Point.x=1000000;Tar_Point.y=1000000;

}

RoadPoint CPathGenerate::FindTarInGridMoveX(int I,int J,int moveWidth)
{
	//计算下一路网点相对于平移的起始点的局部坐标
	double dx=m_roadNetBuffer[CurrentNetPoint+1].x-g_CurrentLocation.x;
	double dy=m_roadNetBuffer[CurrentNetPoint+1].y-g_CurrentLocation.y;
	double cx=dx*cos(g_CurrentLocation.angle)-dy*sin(g_CurrentLocation.angle)-moveWidth*GRID_WIDTH_X;
	double cy=dx*sin(g_CurrentLocation.angle)+dy*cos(g_CurrentLocation.angle);

	int NEW_GRID_I;  //新的网格的列数
	if (moveWidth>0)  //往右平移
	{
		////填充网格
		//for (int j=0;j<100;j++)
		//{
		//	for (int i=1;i<=moveWidth;i++)
		//	{
		//		obs_grid[j*100+99+i]=1;
		//	}
		//}
		NEW_GRID_I=GRID_I+moveWidth;
	}
	else   //往左平移，此时不填充网格，直接裁右边不对称部分
	{
		NEW_GRID_I=GRID_I+moveWidth;
	}

	RoadPoint Tar_Point;
	GridPoint start,end;
	start.X=NEW_GRID_I/2;
	start.Y=0;

#pragma region 下一路网点和当前点连线与x轴夹角大于45度
	if (abs(cy)>=abs(cx))
	{
		int TopX;
		TopX=(J*GRID_WIDTH_Y/cy*cx)/GRID_WIDTH_X+NEW_GRID_I/2;  //判断下一路网点和当前点的连线与第J行的交点的编号

#pragma region 把第J行剩下的列编号的网格点判断是否可通行
		if (I>=TopX)
		{
			for (int i=I+1;i<NEW_GRID_I*3/4;i++)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=J*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=J;
					return Tar_Point;
				}
			}
			for (int i=TopX-1;i>NEW_GRID_I/4;i--)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=J*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=J;
					return Tar_Point;
				}
			}
		}
		else
		{
			for (int i=I-1;I>NEW_GRID_I/4;i--)
			{
				end.X=i;
				end.Y=J;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=J*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=J;
					return Tar_Point;
				}
			}

		}
#pragma endregion 把第J行剩下的列编号的网格点判断是否可通行


#pragma region 从第J-1行开始寻找可通行点
		for (int j=J-1;j>GRID_J/4;j=j-1)
		{
			TopX=(j*GRID_WIDTH_Y/cy*cx)/GRID_WIDTH_X+NEW_GRID_I/2;

			for (int i=TopX+1;i<NEW_GRID_I*3/4;i++)
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=j;
					return Tar_Point;
				}
			}
			for (int i=TopX-1;i>NEW_GRID_I/4;i--)
			{
				end.X=i;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(i-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=i;
					tarJ=j;
					return Tar_Point;
				}
			}
		}
#pragma endregion 从第J-1行开始寻找可通行点
	}
#pragma endregion 下一路网点和当前点连线与x轴夹角大于45度
#pragma region 下一路网点和当前点连线和x轴夹角小于45度
	else
	{ 
		//目标点和当前点连线斜率小于45度的情况

#pragma region 把第I列剩下的行编号的点判断可否通行
		int tJ=(I-NEW_GRID_I/2)*GRID_WIDTH_X/cx*cy/GRID_WIDTH_Y;
		if (J>=tJ)
		{
			for (int j=J+1;J<GRID_J*3/4;J++)
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=I;
					tarJ=j;
					return Tar_Point;
				}
			}
			for (int j=tJ-1;j>GRID_J/4;j--)//原来是j++
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=I;
					tarJ=j;
					return Tar_Point;
				}
			}
		}
		else
		{
			for (int j=J-1;j>GRID_J/4;j--)  //原来是j++
			{
				end.X=I;
				end.Y=j;
				if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
				{
					double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
					double y=j*GRID_WIDTH_Y;
					Tar_Point.x=x;
					Tar_Point.y=y;	
					tarI=I;
					tarJ=j;
					return Tar_Point;
				}
			}
		}
#pragma endregion 把第I列剩下的行编号的点判断可否通行

#pragma region 下一路网点在右上方
		if(cx>0)
		{
			int newBeginJ;
			for (int i=I-1;i>0;i--)
			{
				int newBeginJ=(i-NEW_GRID_I/2)*GRID_WIDTH_X/cx*cy/GRID_WIDTH_Y;
				for (int j=newBeginJ+1;j<GRID_J*3/4;j++)
				{
					end.X=I;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=I;
						tarJ=j;
						return Tar_Point;
					}
				}
				for (int j=newBeginJ-1;j>GRID_J/4;j--)
				{
					end.X=I;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=I;
						tarJ=j;
						return Tar_Point;
					}
				}
			}
		}
#pragma endregion 下一路网点在右上方
#pragma region 下一路网点在左上方
		else
		{
			int newBeginJ;
			for (int i=I+1;i<NEW_GRID_I;i++)
			{
				int newBeginJ=(i-NEW_GRID_I/2)*GRID_WIDTH_X/cx*cy/GRID_WIDTH_Y;
				for (int j=newBeginJ+1;j<GRID_J*3/4;j++)
				{
					end.X=I;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=I;
						tarJ=j;
						return Tar_Point;
					}
				}
				for (int j=newBeginJ-1;j>GRID_J/4;j--)
				{
					end.X=I;
					end.Y=j;
					if(SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))
					{
						double x=(I-NEW_GRID_I/2)*GRID_WIDTH_X;
						double y=j*GRID_WIDTH_Y;
						Tar_Point.x=x;
						Tar_Point.y=y;	
						tarI=I;
						tarJ=j;
						return Tar_Point;
					}
				}
			}
		}
#pragma endregion 下一路网点在左上方

	}
#pragma endregion 下一路网点和当前点连线和x轴夹角小于45度

	//未找到可通行点
	tarJ=0;
	tarI=0;
}



RoadPoint CPathGenerate::FindTarByMoveX(int moveWidth)
{
	RoadPoint Tar_Point;
	Tar_Point.x=1000000;
	Tar_Point.y=1000000;

	//获取下一路网点的局部坐标
	double dx=m_roadNetBuffer[CurrentNetPoint+1].x-g_CurrentLocation.x;
	double dy=m_roadNetBuffer[CurrentNetPoint+1].y-g_CurrentLocation.y;
	double cx=dx*cos(g_CurrentLocation.angle)-dy*sin(g_CurrentLocation.angle);
	double cy=dx*sin(g_CurrentLocation.angle)+dy*cos(g_CurrentLocation.angle);

	double k=cy/(cx-moveWidth*GRID_WIDTH_X);

	GridPoint start,end;//用于可通行点判断的起点和终点两个网格点，X,Y是网格的列和行编号
	start.X=GRID_I/2+moveWidth;
	start.Y=0;

	//越界情况判断
	if ((GRID_J*GRID_WIDTH_Y/k + moveWidth*GRID_WIDTH_X)>GRID_I/2*GRID_WIDTH_X)//Y为GRID_J时，根据目标点与起始点（非原点）计算的X大于网格右边界
	{
		end.X=GRID_I-1;
		end.Y=((GRID_I/2-moveWidth)*GRID_WIDTH_X*k)/GRID_WIDTH_Y;
	}
	else if ((GRID_J*GRID_WIDTH_Y/k+moveWidth*GRID_WIDTH_X)<-(GRID_I/2*GRID_WIDTH_X))//Y为GRID_J时，根据目标点与起始点（非原点）计算的X小于网格左边界
	{
		end.X=0;
		end.Y=-(GRID_I/2+moveWidth)*k*GRID_WIDTH_X/GRID_WIDTH_Y;

	}
	else
	{
		end.X=(GRID_J*GRID_WIDTH_Y/k)/GRID_WIDTH_X+GRID_I/2+moveWidth;
		end.Y=GRID_J-1;
	}

	if (SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))//判断是否可通行
	{
		Tar_Point.x=(end.X-GRID_I/2)*GRID_WIDTH_X;
		Tar_Point.y=end.Y*GRID_WIDTH_Y;

		//记录此时目标点在网格中的行列编号
		tarI=end.X;
		tarJ=end.Y;

		return Tar_Point;
	}

	for (int j=GRID_J-1;j>GRID_J/4;j--)
	{
		for (int i=GRID_I/2;i<GRID_I*3/4;i++)
		{
			end.X=i;
			end.Y=j;
			if (SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))//判断是否可通行
			{
				Tar_Point.x=(end.X-GRID_I/2)*GRID_WIDTH_X;
				Tar_Point.y=end.Y*GRID_WIDTH_Y;

				//记录此时目标点在网格中的行列编号
				tarI=end.X;
				tarJ=end.Y;

				return Tar_Point;
			}

		}
		for (int i=GRID_I/2-1;i>GRID_I/4;i--)
		{
			end.X=i;
			end.Y=j;
			if (SearchBestTarget(start,end,obs_grid,BUFFERWIDTH))//判断是否可通行
			{
				Tar_Point.x=(end.X-GRID_I/2)*GRID_WIDTH_X;
				Tar_Point.y=end.Y*GRID_WIDTH_Y;

				//记录此时目标点在网格中的行列编号
				tarI=end.X;
				tarJ=end.Y;

				return Tar_Point;
			}
		}
	}

	return Tar_Point;
}

void CPathGenerate::GetRightLine()
{
	//获得格网点右侧距车最近的路沿线
	for (int i=0;i<100;i++)
	{
		LineInRight[i]=0;
	}

	for(int i=0;i<GRID_J;i++)
	{
		for(int j=GRID_I/2;j<GRID_I;j++)
		{
			if(obs_grid[i*100+j]==1)//初始得到这条线
			{
				LineInRight[i]=j;
				break;
			}
		}
		if(LineInRight[i]==GRID_I/2)//若从0开始就存在为1的点，说明是障碍物或弯道部分，应往左继续延伸获取。
		{
			int m=GRID_I/2;
			while(obs_grid[i*100+m]==1&&m>0)
			{
				LineInRight[i]=m;
				m--;
			}
		}
	}
	//循环结束后即得到一系列点。
}

double CPathGenerate::GetTarAngle()
{
	double k,A,B,C;
	int count;
	if (flag_IsCloseTo==TRUE)
	{
		double delta_x=(LineInRight[GRID_J-1]-LineInRight[GRID_J-2])*GRID_WIDTH_X;
		double delta_y=GRID_WIDTH_Y;
		return atan2(delta_y,delta_x);
	}
	else
	{
		 /*k=((GRID_J-1)*GRID_WIDTH_Y-0)/((LineInRight[GRID_J-1]-LineInRight[0])*GRID_WIDTH_X);

		A=-1.0;
		B=(LineInRight[GRID_J-1]-LineInRight[0])*GRID_WIDTH_X/((GRID_J-1)*GRID_WIDTH_Y);
		C=(LineInRight[0]-GRID_I/2)*GRID_WIDTH_X;

		count=0;
		for (int j=0;j<100;j++)
		{
			double x=(LineInRight[j]-GRID_I/2)*GRID_WIDTH_X;
			double y=j*GRID_WIDTH_Y;
			double L=abs(A*x+B*y+C)/sqrt(A*A+B*B);

			if (L>TOL_LINE)
			{
				count++;
			}

		}

		if (count<10)
		{
			return atan2(k,1.0);
		}*/

		int J1=GRID_J-1;
		int J2=0;
		while(J1>J2)
		{
			while(LineInRight[J1]==0)
			{
				J1--;
				if (J1<=J2)
				{
					return NULL;
				}
			}
			while(LineInRight[J2]==0)
			{
				J2++;
				if (J1<=J2)
				{
					return NULL;
				}
			}

			k=(J1-J2)*GRID_WIDTH_Y/((LineInRight[J1]-LineInRight[J2])*GRID_WIDTH_X);

			double x1=(LineInRight[J1]-GRID_I/2)*GRID_WIDTH_X;
			double y1=J1*GRID_WIDTH_Y;

			double x2=(LineInRight[J2]-GRID_I/2)*GRID_WIDTH_X;
			double y2=J2*GRID_WIDTH_Y;

			A=-1.0;
			B=(x1-x2)/(y1-y2);
			C=(x2*y1-x1*y2)/(y1-y2);

			double x=0.0;
			double y=0.0;
			double L=0.0;

			count=0;
			for (int j=J2+1;j<J1;j++)
			{
				x=(LineInRight[j]-GRID_I/2)*GRID_WIDTH_X;
				y=j*GRID_WIDTH_Y;
				L=abs(A*x+B*y+C)/sqrt(A*A+B*B);

				if (L>TOL_LINE&&LineInRight[j]!=0)
				{
					count++;
				}
			}
			if(count<10)
			{
				//double agg=atan2(k,1.0);
				return atan2(k,1.0);
			}
			else
			{
				J1--;
				J2++;
			}
		}
	}
	
}


double CPathGenerate::iterGetAngle(int J1,int J2)
{
	while(LineInRight[J1]==0)
	{
		J1--;
		if (J1<=J2)
		{
			return NULL;
		}
	}
	while(LineInRight[J2]==0)
	{
		J2++;
		if (J1<=J2)
		{
			return NULL;
		}
	}
	if (J1<=J2)
	{
		return NULL;
	}
	
	double k=(J1-J2)*GRID_WIDTH_Y/((LineInRight[J1]-LineInRight[J2])*GRID_WIDTH_X);

	double x1=(LineInRight[J1]-GRID_I/2)*GRID_WIDTH_X;
	double y1=J1*GRID_WIDTH_Y;

	double x2=(LineInRight[J2]-GRID_I/2)*GRID_WIDTH_X;
	double y2=J2*GRID_WIDTH_Y;

	double A=-1.0;
	double B=(x1-x2)/(y1-y2);
	double C=(x2*y1-x1*y2)/(y1-y2);

	double x=0.0;
	double y=0.0;
	double L=0.0;

	int count=0;
	for (int j=J2+1;j<J1;j++)
	{
		x=(LineInRight[j]-GRID_I/2)*GRID_WIDTH_X;
		y=j*GRID_WIDTH_Y;
		L=abs(A*x+B*y+C)/sqrt(A*A+B*B);

		if (L>TOL_LINE&&LineInRight[j]!=0)
		{
			count++;
		}
	}
	if(count<10)
	{
		double agg=atan2(k,1.0);
		return atan2(k,1.0);
	}
	else
	{
		return iterGetAngle(J1-1,J2+1);
	}
}


int CPathGenerate::IsCloseToNet(int currentposi)
{

	//此处为中段开始处理的临时方式


	
	////////////////////////////////////////////////////////

	double dis1=(g_CurrentLocation.x-m_roadNetBuffer[currentposi+1].x)*(g_CurrentLocation.x-m_roadNetBuffer[currentposi+1].x)
		+(g_CurrentLocation.y-m_roadNetBuffer[currentposi+1].y)*(g_CurrentLocation.y-m_roadNetBuffer[currentposi+1].y);
	if(dis1<400)
	{
		currentposi++;
		flag_IsCloseTo=TRUE;
		CurrentNetStatus=m_roadNetBuffer[currentposi].roadprop;
		
	}
	else
	{
		double dis2=(g_CurrentLocation.x-m_roadNetBuffer[currentposi].x)*(g_CurrentLocation.x-m_roadNetBuffer[currentposi].x)
			+(g_CurrentLocation.y-m_roadNetBuffer[currentposi].y)*(g_CurrentLocation.y-m_roadNetBuffer[currentposi].y);
		if(dis2>400)
		{

			flag_IsCloseTo=FALSE;
		}
	}
	
	return currentposi;
}


//double CPathGenerate::GetAngleFromGEarthPoints()
//{
//	//RoadPoint Tar;
//	////ReadGoogleEarthPointsFile();
//
//	//bool closeflag=false;//记录是否靠近节点
//	////先判定是否进入节点区域，若已接近则退出循环并返回当前点
//	//for(int i=0;i<m_GENetFileLength;i++)
//	//{
//	//	double dis=(g_CurrentLocation.x-m_GoogleEarthRoadNet[i].x)*(g_CurrentLocation.x-m_GoogleEarthRoadNet[i].x)
//	//		+(g_CurrentLocation.y-m_GoogleEarthRoadNet[i].y)*(g_CurrentLocation.y-m_GoogleEarthRoadNet[i].y);
//	//	if(dis<MINDIS)
//	//	{
//	//		//currentposi=i;
//	//		closeflag=true;
//	//		//flag_IsCloseTo=TRUE;
//	//		break;
//	//	}
//	//}
//	//if (closeflag)
//	//{
//	//	return;
//	//}
//	//else  //车辆在结点之间
//	//{
//	//	flag_IsCloseTo=FALSE;
//	//	for (int i=0;i<fileLength-1;i++)
//	//	{
//	//		double dx0=g_CurrentLocation.x-m_GoogleEarthRoadNet[i].x;
//	//		double dy0=g_CurrentLocation.y-m_GoogleEarthRoadNet[i].y;
//	//		double dx1=g_CurrentLocation.x-m_GoogleEarthRoadNet[i+1].x;
//	//		double dy1=g_CurrentLocation.y-m_GoogleEarthRoadNet[i+1].y;
//	//		if (dx0*dx1+dy0*dy1<0)//判定在哪两个点之间
//	//		{
//	//			//根据车体航向角约束
//	//			float vercurx=sin(g_CurrentLocation.angle);
//	//			float vercury=cos(g_CurrentLocation.angle);
//	//			float verroadx=sin(AngleNormalnize(m_RoadDataBuffer[i].angle));
//	//			float verroady=cos(AngleNormalnize(m_RoadDataBuffer[i].angle));
//	//			if (vercurx*verroadx+vercury*verroady>0)
//	//			{
//	//				currentposi=i;
//	//				break;
//	//			}
//	//		}
//	//	}
//	//	return currentposi;
//	//}
//
//}


void CPathGenerate::GussianToCar(double Gx,double Gy,double *cx,double *cy)
{
	double dx=Gx-g_CurrentLocation.x;
	double dy=Gy-g_CurrentLocation.y;
	*cx=dx*cos(g_CurrentLocation.angle)-dy*sin(g_CurrentLocation.angle);
	*cy=dx*sin(g_CurrentLocation.angle)+dy*cos(g_CurrentLocation.angle);

}



