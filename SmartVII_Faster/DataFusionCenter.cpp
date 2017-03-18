/************************************************************************/
/* 集成统一数据转换函数，对实时获取的带时间坐标的传感数据进行空间转换
  准备对数据进行传感识别*/
/************************************************************************/
#include "StdAfx.h"

#include "DataFusionCenter.h"

#include "ckCoordinate.h"
#include "ControlDlg.h"
#define min_curvature 1
//#include "ckShiftHeader.h"

ckLcmType::velodynegrid_t m_velodynegrid;
VelodyneGrid m_vectorGird;
//ckLcmType::LaneInfo_t g_laneInfo;
//VelodyneLcm g_LidarGrid;
DataFusionCenter::DataFusionCenter(void)
{
	m_nActivateStep=0;
	m_pThreadHandle=NULL;
	m_arrHandle[0]=g_lidarHandle1;
	m_arrHandle[1]=g_lidarHandle2;
	m_arrHandle[2]=g_lidarHandle3;
	m_arrHandle[3]=g_lidarHandle4;
	m_bCanOn=false;
	m_bGpsOn=false;
	m_bLidarOn=false;
	m_strGpsFile="";
	m_bRecordGps=FALSE;
	m_bpstMapStatus=FALSE;
	//m_showPath=new CShowPathPlanDlg();
	/*m_arrHandle[4]=g_laneHandle;
	m_arrHandle[5]=g_canHandle;*/
	g_curLocation.gauX=0;
	g_curLocation.gauY=0;
	m_showPath=NULL;
	m_ectlType=MANUAL;
	m_RoadBuffer=NULL; //测试用
	m_RoadLaneBuffer=NULL;
	m_nRoadLaneLength=0;
	m_brakeStatus=0; //初始状态不刹车
	m_nIdexPos=0;
	//m_laneStatus=ckLaneStatus();
	//m_proadlaneRcv=new cxLanedetect::LaneInfoRcv();
//	m_lcmObj=NULL;
	//m_pLocation=new ckLocation::Location_t();
	//_CrtSetBreakAlloc(28902);
}
DataFusionCenter::~DataFusionCenter(void)
{
//	delete m_proadlaneRcv;
	//	delete m_pLocation;
	if (!m_RoadBuffer)
	{
		delete[] m_RoadBuffer;
		m_RoadBuffer=NULL;
	}
	if (m_RoadLaneBuffer!=NULL)
	{
		delete[] m_RoadLaneBuffer;
		m_RoadLaneBuffer=NULL;
	}
	StopThread();
}

#pragma region Lcm DataPro

// roadlane callback function
//void roadlane_message(const LaneInfo_t *msg)
//{
//	//data pro
//	g_laneInfo=*msg;
//}
//
//void roadlane_thread(void *param)
//{
//	LcmLaneHandler* handle=(LcmLaneHandler*)param;
//	while(0==handle->grab());
//}
//车道线监听函数
BOOL DataFusionCenter::StartRoadLaneLcm()
{

	//char path[MAX_PATH];
	//::GetCurrentDirectory(MAX_PATH,(LPSTR)path);
	//// path="roadparam.ini";
	//string strpath(path);
	//strpath+="\\IPConfig.ini";
	//int ipstatus=GetPrivateProfileInt("Roadlane","this",0,strpath.c_str());
	//if (ipstatus==0)//在本机进行数据广播
	//{
	//	
	//	if (!m_lancLcmHandle.initialLcm("","CKLCMROADLANE",roadlane_message))
	//	{
	//		return FALSE;
	//	}
	//}
	//else
	//{
	//	char ipaddr[50];
	//	DWORD strnum=GetPrivateProfileString("Roadlane","addr","udpm://192.168.127.2:8801",ipaddr,sizeof(ipaddr),strpath.c_str()); 
	//	if (!m_lancLcmHandle.initialLcm(ipaddr,"CKLCMROADLANE",roadlane_message))
	//	{
	//		return FALSE;
	//	}
	//}
	//_beginthread(roadlane_thread,0,&m_lancLcmHandle);
	return TRUE;
		
}
void velodyne_message(const velodynegrid_t*msg)
{
	//data pro
	m_velodynegrid=*msg;
	SetEvent(g_VeloRcvHandle);
   // g_LidarGrid=*msg;
	//TRACE("packaget time %d\n",msg->timestamp);
	
}
void velodyne_thread(void *param)
{
	LcmVeloHandler* handle=(LcmVeloHandler*)param;
	while(0==handle->grab());
}
//激光监听函数
BOOL DataFusionCenter::StartVelodyneLcm()
{
	char path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,(LPSTR)path);
	// path="roadparam.ini";
	string strpath(path);
	strpath+="\\IPConfig.ini";
	int ipstatus=GetPrivateProfileInt("Velodyne","this",0,strpath.c_str());
	if (ipstatus==0)//在本机进行数据广播
	{
		
		if (!m_veloLcmHandle.initialLcm("","CKVELODYNE",velodyne_message))
		{
			return FALSE;
		}
	}
	else
	{


		char ipaddr[50];
		DWORD strnum=GetPrivateProfileString("Velodyne","addr","udpm://192.168.127.2:8801",ipaddr,sizeof(ipaddr),strpath.c_str()); 
		if (!m_veloLcmHandle.initialLcm(ipaddr,"CKVELODYNE",velodyne_message))
		{
			return FALSE;
		}
	}
	
	_beginthread(velodyne_thread,0,&m_veloLcmHandle);
	return TRUE;
}
#pragma endregion Lcm DataPro
//void RoadLane_
void DataFusionCenter::WriteGpsTrack(GpsLine pt)
{
	if (m_strGpsFile!="")
	{
		FILE *fp=fopen(m_strGpsFile,"a+");
		fprintf(fp,"%.10lf\t%.10lf \t%.10lf\r\n",pt.Gnx,pt.Gny,pt.azimuth);
		fclose(fp);
	}
	if (m_RoadBuffer!=NULL)
	{
		delete[] m_RoadBuffer;
		m_RoadBuffer=NULL;
	}
	if (m_calLine.size()!=0)
	{
		vector<GpsLine> rel;
		m_calLine.swap(rel);
	}
}

void DataFusionCenter::CartoGuassian(double &ptX,double &ptY,GpsLine pos)
{
	double x=ptX;
	double y=ptY;
	ptX= (x*cos(pos.azimuth/180*M_PI)+y*sin(pos.azimuth/180*M_PI))+pos.Gnx;
	ptY= -x*sin(pos.azimuth/180*M_PI)+y*cos(pos.azimuth/180*M_PI)+pos.Gny;
}

//激光暂时不改
void DataFusionCenter::ActiveLidarPro()
{
	
	SetEvent(g_gpsHandle);
}
//没有使用,
BOOL DataFusionCenter::ActivateDataPro()
{	
	return TRUE;
}
//加载地图文件
void DataFusionCenter::LoadMap(CString path)
{
	m_dataPath.LoadMapFile(path);
}
//加载测试轨迹
void DataFusionCenter::LoadTestTrack(CString path)
{
	int pos=path.ReverseFind('.');
	CString str=path.Right(path.GetLength()-pos-1);
	if (str!="cs")
	{
		AfxMessageBox("请加载CS格式地图数据!");
		return;
	}
	//if(!m_dataPursuitPath.pstLoadTestTrack(path))
	//{
	//	AfxMessageBox("地图加载异常");
	//	return;
	//}
	m_bpstMapStatus=TRUE;  //标记已经正确加载地图数据

}
//改进版纯追随控制算法，坐标系参考为车辆后轮转轴中间
bool DataFusionCenter::FindPursuitPoint(vector<GpsLine> linebuffer,GpsLine curPoint,GpsLine &TargetPoint,float Speed)
{
	vector<GpsLine> line;
	if (linebuffer.empty())
	{
		return false;
	}
	else
	{
		for (int i=0;i<linebuffer.size();i++)
		{
			GpsLine pt;
			pt.Gnx=linebuffer[i].Gnx-curPoint.Gnx;
			pt.Gny=linebuffer[i].Gny-curPoint.Gny;
			line.push_back(pt);
		}
	}
	double delD=0.0;//车体追随偏差
	double minr=(line[0].Gnx)*(line[0].Gnx)+
		        (line[0].Gny)*(line[0].Gny);
	int index=0;

    for (int i=0;i<line.size();i++)
	{
		double dx=line[i].Gnx;
		double dy=line[i].Gny;
		double dis=dx*dx+dy*dy;
		if (dis<minr)//计算路线内最邻近点
		{
			minr=dis;
			index=i;
		}
	}
	//计算轨迹偏差
	ckVector va,vb;
	//va.x=line[index+1].Gnx-line[index].Gnx;
	//va.y=line[index+1].Gny-line[index].Gny;
	vb.x=-line[index].Gnx;
	vb.y=-line[index].Gny;

	//delD=sqrt(
	//	pow(vb.x,2)+pow(vb.x,2)-
	//	pow((va.x*vb.x+va.y*vb.y),2)/(pow(va.x,2)+pow(va.x,2)));
	delD=sqrt(pow(vb.x,2)+pow(vb.x,2));
	//计算自适应预瞄距离
	float Lmin=6.0;  //最小转弯半径
	float Kla=2.25 ; //比例系数
	float Vchange=2.25;  //速度阈值 单位为m/s
	float Kct=1.2;         //偏差的距离增益
	float radious=6.0;   //预瞄半径
	if (Speed<=Vchange)
	{
		radious=Lmin;
			//+delD*Kct;
	}
	else
	{
		radious=Speed*Kla;
			//+delD*Kct;
	}	

	//for (int i=0;i<m_nRoadLength;i++)
	//{
	//	GpsLine  point;
	//	double cx=m_RoadBuffer[i].x-m_curLocation.gauX;
	//	double cy=m_RoadBuffer[i].y-m_curLocation.gauY;

	//	point.Gnx= (cx*cos(m_curLocation.azimuth/180*PI)-cy*sin(m_curLocation.azimuth/180*PI));
	//	point.Gny= cx*sin(m_curLocation.azimuth/180*PI)+cy*cos(m_curLocation.azimuth/180*PI);
	//	localline.push_back(point);
	//}
	//计算线与扫描半径的交点
	//
	//double radious;
//	GpsLine targ;
	bool isfind=false;
	//记录车辆当前的方向适量
	double towardx=sin(CPathGenerate::AngleNormalnize(curPoint.azimuth));
	double towardy=cos(CPathGenerate::AngleNormalnize(curPoint.azimuth));
	for (int i=0;i<line.size()-1;i++)
	{
		double delGaux0=line[i].Gnx;
		double delGauy0=line[i].Gny;
		double delGaux1=line[i+1].Gnx;
		double delGauy1=line[i+1].Gny;
		double r0=calulate_radios_err(delGaux0,delGauy0,radious);
		double r1=calulate_radios_err(delGaux1,delGauy1,radious);
		if (fabs(r0)<0.01)
		{
			if (towardx*delGaux0+towardy*delGauy0>0)
			{
				TargetPoint=line[i];
				isfind=true;
				break;
			}

		}
		if (fabs(r1)<0.01)
		{//夹角小于90度，判定为当前预描点
			if (towardx*delGaux1+towardy*delGauy1>0)
			{
				TargetPoint=line[i+1];
				isfind=true;
				break;
			}
		}
		if (r0*r1<0) //判定到达圆的边界处
		{
			GpsLine tag0= Newton_divide(line[i],line[i+1],radious);
			double ptx=tag0.Gnx;
			double pty=tag0.Gny;
			if (towardx*ptx+towardy*pty>0)
			{
				TargetPoint=tag0;
				isfind=true;
				break;
			}
		}
	}
	if (isfind)
	{
		TargetPoint.Gnx=TargetPoint.Gnx+curPoint.Gnx;
		TargetPoint.Gny=TargetPoint.Gny+curPoint.Gny;
	}
	return isfind;

}
void DataFusionCenter::SendLcmPos(GpsLine pos)
{
	Location_t lcmLocation;
	lcmLocation.timestamp=(int64_t)1000;
	lcmLocation.position[0]=pos.Gnx;
	lcmLocation.position[1]=pos.Gny;
	lcmLocation.position[2]=0;
	lcmLocation.orientation[0]=0;
	lcmLocation.orientation[1]=0;
	lcmLocation.orientation[2]=0;
	lcmLocation.velocity[0]=1.0;
	lcmLocation.velocity[1]=2.0;
	lcmLocation.velocity[2]=3.0;
	m_gpsLcmHandle.sendLcm(&lcmLocation);
}
BOOL DataFusionCenter::InitialLocationLcm()
{
	char path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,(LPSTR)path);
	// path="roadparam.ini";
	string strpath(path);
	strpath+="\\IPConfig.ini";
	int ipstatus=GetPrivateProfileInt("Location","this",0,strpath.c_str());
	if (ipstatus==0)//在本机进行数据广播
	{
		if(!m_gpsLcmHandle.intialSend("","CKLOCATION"))
		{
			return FALSE;
		}
	}
	else
	{
		char ipaddr[50];
		DWORD strnum=GetPrivateProfileString("Location","addr","",ipaddr,sizeof(ipaddr),strpath.c_str());
		if(!m_gpsLcmHandle.intialSend(ipaddr,"CKLOCATION"))
		{
			return FALSE;
		}
	}

	return TRUE;
}
#define distri_cur 0.1
bool DataFusionCenter::ywFindPursuitPoint(vector<GpsLine> line,GpsLine curPoint,GpsLine &TargetPoint,float Speed)
{
	double delD=0.0;//车体追随偏差
	double minr=(line[0].Gnx)*(line[0].Gnx)+
		        (line[0].Gny)*(line[0].Gny);
	int index=0;
	bool isfind=false;

	for (int i=0;i<line.size();i++)
	{
		double dx=line[i].Gnx-curPoint.Gnx;
		double dy=line[i].Gny-curPoint.Gny;
		double dis=dx*dx+dy*dy;
		if (dis<minr)//计算路线内最邻近点
		{
			minr=dis;
			index=i;
		}
	}
	//搜索预描点,返回的预描点坐标为局部坐标
	bool ds_flag=true;
	double ds=0;
	double min_distance=12;
	for(int i=index+1;i<line.size()-1;i++)
	{
		GpsLine line_pre=GautoMycar_oid(curPoint,line[i-1]);
		GpsLine line_i=GautoMycar_oid(curPoint,line[i]);
		GpsLine line_next=GautoMycar_oid(curPoint,line[i+1]);
		double k=0;
		double dk=0;
		double L=0;
		double KEnd=0;
		if(ds_flag)
		{
			ds=ds+sqrt((line_i.Gnx-line_next.Gnx)*(line_i.Gnx-line_next.Gnx)+(line_i.Gny-line_next.Gny)*(line_i.Gny-line_next.Gny));
		}
		if(ds>min_distance&&ds<20)
		{
			ds_flag=false;
			double tangentvalue1=atan2(line_i.Gnx-line_pre.Gnx,line_i.Gny-line_pre.Gny);
			double tangentvalue2=atan2(line_next.Gnx-line_i.Gnx,line_next.Gny-line_i.Gny);
			line_i.azimuth=(tangentvalue1+tangentvalue2)/2;
			m_dataPursuitPath.buildClothoid(k,dk,L,0,0,0,line_i.Gny,line_i.Gnx,line_i.azimuth,0.05);
			KEnd=k+dk*L;
			if(min_distance==6)
			{
				TargetPoint=line_i;
				isfind=true;
				m_nIdexPos=i;
				break;
			}
			if(fabs(k)<distri_cur&&fabs(KEnd)<distri_cur)
			{
				TargetPoint=line_i;
				isfind=true;
				m_nIdexPos=i;
				break;
			}
			else if(min_distance==12)
			{
				min_distance=6;
				i=index;
				ds_flag=true;
				ds=0;
			}
		}
	}
	return isfind;
}



int flag=0;//判断当前是否有离散化过的Clothoid曲线
bool speedflag=false;

void DataFusionCenter::ywNew_ClothoidTrack(GpsLine car_curpoint,GpsLine targetpoint)
{
	double k,dk,L;
	k=0;dk=0;L=0;
	double KEnd=0;
	double distance=0;
	double detaD=0;
	double detaD_min=1000;
	int nearest_num=0;
	double thegema=0;
	double LWheel=2.5;
	double ValueCurvature=0;
	double ds=0;
	double cur_k=0;
	double AngleWheel=0;
	double s_TagWheelAngle=0;
	double s_CurrentWheelAngle=0;
	double TimeStart=0;
	double fir_k,fir_dk,fir_L;
	double speed; //车速
	fir_k=0;fir_dk=0;fir_L=0;
	//double Cofficient=0;
   // RoadPoint fir_m_xy[CLOTHOIDPOINTNUM];
	if(flag==0)
		{
			fir_targetpoint.Gnx=targetpoint.Gnx;
			fir_targetpoint.Gny=targetpoint.Gny;
			fir_targetpoint.azimuth=targetpoint.azimuth;
			ZeroMemory(m_xy,sizeof(RoadPoint)*CLOTHOIDPOINTNUM);
			m_dataPursuitPath.buildClothoid(k,dk,L,0,0,0,fir_targetpoint.Gny,fir_targetpoint.Gnx,fir_targetpoint.azimuth,0.05);
			m_dataPursuitPath.pointsOnClothoid(m_xy,0,0,0,k,dk,L,CLOTHOIDPOINTNUM);
			//std::copy(m_xy,m_xy+CLOTHOIDPOINTNUM,fir_m_xy);
			/*for(int i=0;i<CLOTHOIDPOINTNUM;i++)
			{
			fir_m_xy[i]=m_xy[i];
			}*/
			fir_k=k;
			fir_dk=dk;
			fir_L=L;
			flag=1;
		}
	m_dataPursuitPath.buildClothoid(k,dk,L,0,0,0,targetpoint.Gny,targetpoint.Gnx,targetpoint.azimuth,0.05);
	//Cofficient=targetpoint.azimuth*100/PI;
	//汽车当前位置离预描点的距离
	distance=sqrt((0-fir_targetpoint.Gnx)*(0-fir_targetpoint.Gnx)+(0-fir_targetpoint.Gny)*(0-fir_targetpoint.Gny));
	KEnd=k+dk*L;
	if (fabs(k)<min_curvature&&fabs(KEnd)<min_curvature)
	{
		ValueCurvature=k;
		thegema=atan(ValueCurvature*LWheel);
		AngleWheel = thegema*180.0*15.0/PI;
		s_TagWheelAngle=AngleWheel+550;
		if(s_TagWheelAngle<0)
		{
			s_TagWheelAngle=90;
		}
		if(s_TagWheelAngle>1080)
		{
			s_TagWheelAngle=1000;
		}
		TimeStart=GetTickCount();
		if (!speedflag)
		{
			m_dataCan.SendCarAngle(true,s_TagWheelAngle);
		}
		else
		{
				m_dataCan.SendCarSpeed(true,0, speed);
				speedflag=false;
		}
	
		s_CurrentWheelAngle=m_dataCan.m_nCarAngle;
		if(distance>1)
		{

		}
		else
		{
			flag=0;
		}
		//m_dataPursuitPath.MiddlePiont_Shan(targetpoint.Gny,fabs(targetpoint.Gnx));
		//return;
	}
	else
	{
		//判断汽车当前位置离fir_clothoid曲线上哪个点最近
		for(int i=0;i<CLOTHOIDPOINTNUM;i++)
		{
			detaD=sqrt((0-m_xy[i].x)*(0-m_xy[i].x)+(0-m_xy[i].y)*(0-m_xy[i].y));
			if(detaD<detaD_min)
			{
				detaD_min=detaD;
				nearest_num=i;
			}
		}
		//clothoid曲线上离汽车最近的点到clothoid曲线起点的距离
		for(int i=0;i<nearest_num;i++)
		{
			ds=ds+sqrt((m_xy[i].x-m_xy[i+1].x)*(m_xy[i].x-m_xy[i+1].x)+(m_xy[i].y-m_xy[i+1].y)*(m_xy[i].y-m_xy[i+1].y));
		}
		cur_k=dk*ds+k;
		ValueCurvature=cur_k;
		thegema=atan(ValueCurvature*LWheel);
		AngleWheel = thegema*180.0*15.0/PI;
		s_TagWheelAngle=AngleWheel+550;
		TimeStart=GetTickCount();
		m_dataCan.SendCarAngle(true,s_TagWheelAngle);
		s_CurrentWheelAngle=m_dataCan.m_nCarAngle;
		if(distance>1)
		{	
		}
		else
		{
			flag=0;
		}
	}
	//FILE *fp=fopen("Clothoid.txt","a+");
	//fprintf(fp,"%f,%f,%f,%f,%f,%f,%f\r\n",k,targetpoint.Gnx,targetpoint.Gny,targetpoint.azimuth,car_curpoint.Gnx,car_curpoint.Gny,car_curpoint.azimuth);
	////fprintf(fp,"%d,%d,%f,%f,%f,%f\r\n",s_TagWheelAngle,s_CurrentWheelAngle,fir_targetpoint.Gnx,fir_targetpoint.Gny,fir_targetpoint.azimuth);
	//fclose(fp);
}



GpsLine DataFusionCenter::GautoMycar_oid(GpsLine car_curpoint,GpsLine daizhuan_point)
{
	GpsLine jubu_system;
	double x=daizhuan_point.Gnx-car_curpoint.Gnx;
	double y=daizhuan_point.Gny-car_curpoint.Gny;
	jubu_system.Gnx=x*cos(car_curpoint.azimuth/180*3.1415926)-y*sin(car_curpoint.azimuth/180*3.1415926);
	jubu_system.Gny=x*sin(car_curpoint.azimuth/180*3.1415926)+y*cos(car_curpoint.azimuth/180*3.1415926);
	return jubu_system;
}
bool side=false;
bool brake=false;
void DataFusionCenter::ProFusionData()
{
	//initial
	//g_LidarGrid.timestamp=0;
	long timestamp=0;;
	while (!m_bStopThread)
	{

		WaitForSingleObject(g_gpsHandle,INFINITE);
		if (m_dataLocation.m_insStatus!=NOSINGAL)
		{
			m_curLocation.azimuth=m_dataLocation.m_insPva.Azimuth;
			m_curLocation.week=m_dataLocation.m_nWeek;
			m_curLocation.eVelocity=m_dataLocation.m_insPva.EVelocity;
			m_curLocation.nVelocity=m_dataLocation.m_insPva.NVelocity;
			m_curLocation.upVelocity=m_dataLocation.m_insPva.UpVelocity;
			m_curLocation.pitch=m_dataLocation.m_insPva.Pitch;
			m_curLocation.roll=m_dataLocation.m_insPva.Roll;
			double lat=m_dataLocation.m_insPva.Lat;
			double lon=m_dataLocation.m_insPva.Lon;
		//	TRACE("speed:up %lf,north %lf,,east %lf\n",m_curLocation.upVelocity,m_curLocation.nVelocity,m_curLocation.eVelocity);
			double gaux=0,gauy=0;		
			ckCoordinate::LongLat2XY(lon,lat,gaux,gauy);
			m_curLocation.gauX=gaux;		
			m_curLocation.gauY=gauy;
			g_curPostion.Gnx=lon	;
			g_curPostion.Gny=lat;
			g_curPostion.azimuth=m_curLocation.azimuth;
			//lcm位置广播赋值
			Location_t lcmLocation;
			lcmLocation.timestamp=(int64_t)m_dataLocation.m_insPva.Second*1000;
		    lcmLocation.position[0]=gaux;
			lcmLocation.position[1]=gauy;
			lcmLocation.position[2]=m_dataLocation.m_insPva.Hgt;
			lcmLocation.orientation[0]=m_dataLocation.m_insPva.Azimuth;
			lcmLocation.orientation[1]=m_dataLocation.m_insPva.Pitch;
			lcmLocation.orientation[2]=m_dataLocation.m_insPva.Roll;
			lcmLocation.velocity[0]=m_dataLocation.m_insPva.NVelocity;
			lcmLocation.velocity[1]=m_dataLocation.m_insPva.EVelocity;
			lcmLocation.velocity[2]=m_dataLocation.m_insPva.UpVelocity;
			m_gpsLcmHandle.sendLcm(&lcmLocation);
			g_curLocation=m_curLocation;
#pragma region Self-Driving
           short  angle=547;
			if (g_ctlTypes==SELFDRIVING)
			{	
				  
				vector<GpsLine> localline;

		
			/*	if ()
				{
				}*/
				//如果识别出车道线，优先使用车道线
				if (m_laneCentor.m_blineStatus)
				{
				   m_nRoadLaneLength=m_laneCentor.m_nptNum;
			
				   for (int j=0;j<m_nRoadLaneLength;j++)      
				   {
					   GpsLine pt;
					   pt.Gnx=m_laneCentor.m_gpsBuffer[j].Gnx;
					   pt.Gny=m_laneCentor.m_gpsBuffer[j].Gny;
					   pt.azimuth=m_laneCentor.m_gpsBuffer[j].azimuth;
					   localline.push_back(pt);
				   }
				   //GpsLine curPoint,targetPoint;
				   //curPoint.Gnx=0;
				   //curPoint.Gny=0;
				   //curPoint.azimuth=0;
				   ////转换到局部坐标系
				   //float speed=m_dataCan.m_nCarSpeed/100/3.6;
				   //bool isfind=false;
				   //isfind=FindPursuitPoint(localline,curPoint,targetPoint,speed);	
				   //if (isfind)
				   //{
					  //   angle=   CPathGenerate::MiddlePiont_Shan(targetPoint.Gnx,targetPoint.Gny);	
					  // m_dataCan.SendCarAngle(true,angle);
				   //} 		
			//	   m_laneCentor.m_blineStatus=false;
				}
				else
				{
					if (m_dataPath.GetPathFlag())
					{
						m_nRoadLength=m_dataPath.GetFinalpathLength();
						if (m_RoadBuffer!=NULL)
						{
							delete[] m_RoadBuffer;
							m_RoadBuffer=NULL;
						}
						m_RoadBuffer=new RoadPoint[m_nRoadLength];
						memcpy(m_RoadBuffer,m_dataPath.GetGauPath(),m_nRoadLength*sizeof(RoadPoint));
			 	    }
					for (int i=0;i<m_nRoadLength;i++)      
					{
						GpsLine pt;
						pt.Gnx=m_RoadBuffer[i].x;
						pt.Gny=m_RoadBuffer[i].y;
						pt.azimuth=m_RoadBuffer[i].angle;
						localline.push_back(pt);
					}
			 	}	
				GpsLine curPoint,targetPoint;
				curPoint.Gnx=m_curLocation.gauX;
				curPoint.Gny=m_curLocation.gauY;
				curPoint.azimuth=m_curLocation.azimuth;
				//转换到局部坐标系
				float speed=m_dataCan.m_nCarSpeed/100/3.6;
				bool isfind=false;
				isfind=FindPursuitPoint(localline,curPoint,targetPoint,speed);	
				if (isfind)
				{

					double x=0,y=0;
					OriginPt org;
					org.X=curPoint.Gnx;
					org.Y=curPoint.Gny;
					org.Angle=curPoint.azimuth/180.0*M_PI;
					ckCoordinate::WorldtoMap(org,targetPoint.Gnx,targetPoint.Gny,x,y);
					angle=   CPathGenerate::MiddlePiont_Shan(x,y);	
					m_dataCan.SendCarAngle(true,angle);
				} 		
			
			
		}
#pragma endregion Self-Driving
			//采集GPS数据
		if (m_bRecordGps)
		{
			GpsLine pt;
			pt.Gnx=gaux;
			pt.Gny=gauy;
			pt.azimuth=m_curLocation.azimuth;
			WriteGpsTrack(pt);
		}
#pragma region 测试Pursuit专用
		if (g_ctlTypes==TRACKMODEL)
		{
			vector<GpsLine> localline;
			localline=m_sideWay;
			GpsLine curPoint,targetPoint;
			curPoint.Gnx=m_curLocation.gauX;
			curPoint.Gny=m_curLocation.gauY;
			curPoint.azimuth=m_curLocation.azimuth;
			//转换到局部坐标系
			float speed=m_dataCan.m_nCarSpeed/100/3.6;
			bool isfind=false;
			isfind=FindPursuitPoint(localline,curPoint,targetPoint,speed);	
			if (isfind)
			{

				double x=0,y=0;
				OriginPt org;
				org.X=curPoint.Gnx;
				org.Y=curPoint.Gny;
				org.Angle=curPoint.azimuth/180.0*M_PI;
				ckCoordinate::WorldtoMap(org,targetPoint.Gnx,targetPoint.Gny,x,y);
				angle=   CPathGenerate::MiddlePiont_Shan(x,y);	
				m_dataCan.SendCarAngle(true,angle);
			} 		
		}

#pragma endregion 测试Pursuit专用
//#pragma region UpdateView
//		////画路径规划图
//
//		if (m_showPath!=NULL&&(gaux!=0)&&(gauy!=0))
//		{
//			m_showPath->currentLocation.x=gaux;
//			m_showPath->currentLocation.y=gauy;
//
//			if (m_dataPath.IsTargetPoint())
//			{
//				m_showPath->targetPointNum=m_dataPath.GetNetNum()+1;
//			}
//			else
//			{
//				m_showPath->targetPointNum=m_dataPath.GetNetNum()+1;
//			}
//			PostMessage(m_showPath->GetSafeHwnd(),WM_PAINT,0,0);
//		}
//		
//#pragma endregion UpdateView
		
		m_dataCan.SendCarAngle(false,0);
		//	Sleep(5);
		m_dataCan.SendCarSpeed(false,0,0);
	//	Sleep(5);
		}
		ResetEvent(g_gpsHandle);
	}
}

UINT DataFusionCenter::SensorFusionPro(LPVOID lpParam)
{
	DataFusionCenter *pFusion = (DataFusionCenter *)lpParam;

	pFusion->ProFusionData();

	pFusion->m_pThreadHandle = NULL;
	pFusion->m_threadMode = MODENULL;
	pFusion->m_bStopThread = FALSE;

  return 0;
}
//激活CAN卡，开始接受数据
BOOL DataFusionCenter::ActiveCanPro()
{
	//m_dataCan.ConnectCan();
	int flag=m_dataCan.GetConnectStatus();
	if (flag==1)
	{
		m_dataCan.CreateThread();
		m_dataCan.StartThread();
		m_bCanOn=true;
		return TRUE;
	}
	else
	{
		AfxMessageBox("请确认CAN已打开");
	}

}
BOOL DataFusionCenter::ConnectAllSensor()
{

	if (!m_dataLocation.OpenPort())
	{
		AfxMessageBox("GPS数据未正确打开");

	}
	if (!m_dataCan.ConnectCan())
	{
		AfxMessageBox("CAN数据未正确打开");
	}
	ResetEvent(g_gpsHandle);
	CreateThread();
	StartThread(); 
	return TRUE;
}
BOOL DataFusionCenter::StartFusionThread()
{

	//m_dataLocation.CreateThread();
	//m_dataLocation.StartThread();
	//m_dataLidar.CreateThread();
	//m_dataLidar.StartThread();
	return TRUE;
}
//连接GPS，发送LOG指令	
BOOL DataFusionCenter::ActivateGPSPro()
{
	//判断串口是否打开
	BOOL flag=GetGpsPortStatus();
	if (flag)
	{
		m_dataLocation.StartDataOutput();
		m_dataLocation.CreateThread();
		m_dataLocation.StartThread();
		m_bGpsOn=true;
	}
	else
	{
		AfxMessageBox("GPS串口未打开");
		return FALSE;
	}
	return TRUE;
}
//获得GPS串口当前状态
BOOL DataFusionCenter::GetGpsPortStatus()
{
	return m_dataLocation.GetPortStatus();
}
void DataFusionCenter::StopGpsPro()
{
	BOOL flag=GetGpsPortStatus();
	if (flag)
	{
		m_dataLocation.StopDataOutput();
		m_dataLocation.StopThread();
		m_dataLocation.ClosePort();
	}
    else
	{
		AfxMessageBox("GPS串口未打开");
		return ;
	}
	
}
void DataFusionCenter::CreateThread()
{
	if(m_pThreadHandle != NULL)
		return;
	m_pThreadHandle = AfxBeginThread(SensorFusionPro, 
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
void DataFusionCenter::StartThread()
{
	if(m_pThreadHandle == NULL)
		CreateThread();

	if(m_pThreadHandle != NULL)
		m_pThreadHandle->ResumeThread();
	else
		return;

	m_threadMode = MODERUN;
}
void DataFusionCenter::ResumeThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODERUN)
		m_pThreadHandle->SuspendThread();
	else 
		return;

	m_threadMode = MODESUSPEND;
}
void DataFusionCenter::StopThread()
{
	if(m_pThreadHandle == NULL)
		return;

	m_bStopThread = TRUE;

	while(m_pThreadHandle)
		Sleep(1);

	m_threadMode = MODESTOP;
}
void DataFusionCenter::SuspendThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODERUN)
		m_pThreadHandle->SuspendThread();
	else 
		return;

	m_threadMode = MODESUSPEND;
}
void DataFusionCenter::DestoryLidarPro()
{
	//for (int i=0;i<4;i++)
	//{
	//	m_dataLidar.m_lidarThread[i].CtnsDataOutputStop();
	//	m_dataLidar.m_lidarThread[i].StopThread();
	//}
}
void DataFusionCenter::DestoryGpsPro()
{
	m_dataLocation.StopDataOutput();
	m_dataLocation.StopThread();
}
void DataFusionCenter::DestoryCanPro()
{
	m_dataCan.StopThread();
}
void DataFusionCenter::ReadIpConfig()
{

}
//计算返回与圆的径向误差
double  DataFusionCenter::calulate_radios_err(double x,double y,double radios)
{
	return (x*x+y*y-radios*radios);
}
//float midx=0,midy=0;
//牛顿二分法计算交点
//此处gpsline为局部坐标
GpsLine DataFusionCenter::Newton_divide(GpsLine p0,GpsLine p1,float radious) 
{
	//计算误差
	double x0=p0.Gnx;
	double y0=p0.Gny;
	double x1=p1.Gnx;
	double y1=p1.Gny;
	double midx=(x0+x1)/2;
	double midy=(y0+y1)/2;
	GpsLine midpoint;
	midpoint.Gnx=midx;
	midpoint.Gny=midy;
	double r0=calulate_radios_err(x0,y0,radious);
	double r1=calulate_radios_err(x1,y1,radious);
	double r2=calulate_radios_err(midx,midy,radious);
	if (fabs(r0)<=0.01)
	{
		return p0;

	}
	if (fabs(r1)<=0.01)
	{
		return p1;
	}
	if (fabs(r2)<=0.01)
	{
		return midpoint;
	}
	if (r0*r2<=0)
	{
		return Newton_divide(p0,midpoint,radious);
	}
	else if(r1*r2<=0)
	{
		return Newton_divide(midpoint,p1,radious);
	}
}
//规划部分//1111
void DataFusionCenter::StartPathPlan()
{
	//CString filter;  
	//filter="路网文件(*.txt)|*.txt|GPS文件(*.cs)|*.cs||";  
	//CFileDialog fdlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter);  
	////fdlg.m_ofn.lpstrFilter="GPS File(*.cs)|*.cs|Road Net(*.txt)|*.txt||";
	//if(fdlg.DoModal()!=IDOK)
	//{
	//	AfxMessageBox("OPEN GPS FILE ERROR!");
	//	return;
	//}
	//m_dataPath.gpsFileName=fdlg.GetPathName();
	//m_dataPath.LoadMapFile(fdlg.GetPathName());

	m_dataPath.LoadMapFile("");

	m_dataPath.StopThread();
	ResetEvent(g_VeloRcvHandle);
	m_dataPath.CreateThread(); 
	m_dataPath.StartThread();

}
void DataFusionCenter::StartVelodyneListen()
{  
  recvmsg.SetUdpCallback(OutData_obstacle);
  recvmsg.initUdpSocket();
  recvmsg.startListenSocket();
}
void DataFusionCenter::ShowPlanPath()
{

	if(m_showPath==NULL)
	{
		//m_showPath->Create(IDD_SHOW_PATHPLAN,m_hWnd);
		m_showPath=new CShowPathPlanDlg();
		if(m_showPath->Create(IDD_SHOW_PATHPLAN)==FALSE)
		{
			AfxMessageBox("CREATE PATHDLG ERROR!");
			m_showPath=NULL;
			return;
		}
		m_showPath->IsWindowCreate=1;

		CRect clientRect,dlgRect;
		GetClientRect(m_hWnd,&clientRect);
		GetWindowRect(m_hWnd,&dlgRect);

		m_showPath->MoveWindow(dlgRect.left,dlgRect.top,dlgRect.Width()/2,dlgRect.Height());
	}

	//if(m_showPath->IsWindowOpen==0)
	//{
		
	////	m_showPath->IsWindowOpen=1;
	//	m_showPath->m_roadDataLength=m_dataPath.fileLength;
	//	if (m_showPath->m_RoadDataBuffer!=NULL)
	//	{
	//		delete[] m_showPath->m_RoadDataBuffer;
	//		m_showPath->m_RoadDataBuffer=NULL;
	//	}

	//	if (m_dataPath.m_eType==NET)
	//	{
	//		m_showPath->bNetFlag=TRUE;
	//		m_showPath->m_nNetLength=m_dataPath.m_netFileLength;
	//		if (m_showPath->m_RoadNetBuffer!=NULL)
	//		{
	//			delete[] m_showPath->m_RoadNetBuffer;
	//			m_showPath->m_RoadNetBuffer=NULL;
	//		}
	//		m_showPath->m_RoadNetBuffer=new RoadNet[m_showPath->m_nNetLength];
	//		memcpy(m_showPath->m_RoadNetBuffer,m_dataPath.m_roadNetBuffer,m_showPath->m_nNetLength*sizeof(RoadNet));
	//	}
	//	m_showPath->m_RoadDataBuffer=new RoadPoint[m_showPath->m_roadDataLength];
	//	memcpy(m_showPath->m_RoadDataBuffer,m_dataPath.m_RoadDataBuffer,m_dataPath.fileLength*sizeof(RoadPoint));
	//	m_showPath->initialPoint = m_dataPath.GetInitialpoint();

		m_showPath->ShowWindow(SW_SHOW);
		
	//}

}
void DataFusionCenter::CalTargetPoint(double &X,double& Y,double pursuit)
{
		///RoadPoint  path[100];
	RoadPoint* finalpath=m_dataPath.GetFinalPath();
	int length=m_dataPath.GetFinalpathLength();
	//RoadPoint *path=new RoadPoint[length];
	//memcpy(path,finalpath,length*sizeof(RoadPoint));
	////转换全局坐标到局部路径
	//vector<RoadPoint> local_path;

	//for (int i=0;i<)
	//{
	//}

	//delete[] path;
}
void DataFusionCenter::OutData_obstacle(vector<unsigned char> grid)
{
	m_vectorGird=grid;
//	TRACE("Grid sizes %d\n",grid.size());
	SetEvent(g_VeloRcvHandle);
}
void DataFusionCenter::DrawObjectGrid()
{
	vector<unsigned char> grid;
	//m_vectorGird.swap(grid);
	grid=m_vectorGird;
	if (grid.size()!=0)
	{
		glPointSize(5);
		glColor3f(1,0,0);
		glBegin(GL_POINTS);
		for (int i=0;i<100;i++)
		{
			for (int j=0;j<100;j++)
			{
				if (grid[i*100+j]==1)
				{
					//float x=j*0.1-5;
					float x=j*0.2-10;
					float y=i*0.2;
					glVertex3f(x,y,0);
				}
			}
		}
		glEnd();
		glFlush();
	}
}
void DataFusionCenter::DrawLocalPath()
{
	///RoadPoint  path[100];
	RoadPoint* finalpath=m_dataPath.GetFinalPath();
	int length=m_dataPath.GetFinalpathLength();
	if (length==0)
	{
		return;
	}
	RoadPoint *path=new RoadPoint[length];
	memcpy(path,finalpath,length*sizeof(RoadPoint));
   glColor3f(0,1,0);
   glPointSize(5);
   glBegin(GL_POINTS);
	for(int i=0;i<length;i++)
	{
		/*double x=path[i].x-g_curLocation.gauX;
		double y=path[i].y-g_curLocation.gauY;
		double angle=path[i].angle-CPathGenerate::AngleNormalnize(90-g_curLocation.azimuth)*PI/180;
		double dx=(x)*cos(g_curLocation.azimuth/180*PI)-y*sin(g_curLocation.azimuth/180*PI);
		double dy=x*sin(g_curLocation.azimuth/180*PI)+y*cos(g_curLocation.azimuth/180*PI);*/
		glVertex3f(path[i].x,path[i].y,0);
			/*	m_PurePursuitTar.x=dx;
			m_PurePursuitTar.y=dy;
			m_TagWheelAngle=MiddlePiont_Shan(m_PurePursuitTar.x,m_PurePursuitTar.y);*/
	//		break;
		
	}
	glEnd();
	delete[] path;

}
//调用场景模拟对话框进行测试
void DataFusionCenter::ShowCtlSceneDlg()
{
	CControlDlg dlg;
	//if (dlg.InitCanInfo(m_dataCan))
	//{
		dlg.DoModal();
//	}
	
}
UINT DataFusionCenter::UpdateRoadLane()
{
	//if (!m_laneBuffer.empty())
	//{
	//	m_laneBuffer.clear();
	//	m_drawlaneBuffer.clear();
	//}
	//float ydel=0;
	//float x=0;
	//while(ydel<40)
	//{
	//	x=g_laneInfo.centerPolyPara[0]+ydel*g_laneInfo.centerPolyPara[1]+ydel*ydel*g_laneInfo.centerPolyPara[2];
	//	ydel+=0.5;
	//	GpsLine pt;
	//	pt.Gnx=x;
	//	pt.Gny=ydel;
	//	m_laneBuffer.push_back(pt);
	//	PointShow pts;
	//	pts.x=x;
	//	pts.y=ydel;
	//	pts.z=0;
	//	m_drawlaneBuffer.push_back(pts);
	//}
	//Sleep(100);
	return 1;
}
