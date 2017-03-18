#include "StdAfx.h"
#include "LidarObject.h"

CLidarObject::CLidarObject(void)
{
	m_pOkBuf=NULL;
	m_pPointBuffer=NULL;
}


CLidarObject::~CLidarObject(void)
{
}
float CLidarObject::cal_std(int start,int num,const float *val)//计算val的标准差
{
	float val_average = 0.0;
	for(int i = start;i < start + num; i++)
		val_average += val[i];
	val_average /= num;
	float std = 0.0;
	for(int j = start;j < start + num; j++)
		std += pow(val[j] - val_average,2);
	std = sqrt(std/num);
	return std;
}

//数据预处理，对整体数据进行均值滤波
UINT CLidarObject::MeanFilter(int Num,sPoint *pPoint,int filter)
{


	float* point_r=new float[Num];        // 扫描点高程

	for(int i = 0;i < Num;i++)  // 当扫描点到原点的距离小于0.2时，认为是无效点，采用前一个点的数据
	{
		if(pPoint[i].r < 0.5)
			pPoint[i] = pPoint[i - 1];
	}
	//
	for(int m = 0; m < Num;m ++)
	{
		point_r[m] = pPoint[m].r;
	}
	//进行滑动平均，剔除毛刺  9个点的高层均值作为中间点的高程
	float* temppoint_r=new float[Num];
	for(int n = 0;n < Num; n++)
	{
		if(n < filter || n > (Num-filter))
			temppoint_r[n] = point_r[n];
		else
		{
			temppoint_r[n] = 0;
			for(int q = n - filter; q < n + filter;q++)
				temppoint_r[n]+=point_r[q];
			temppoint_r[n] = temppoint_r[n]/(filter*2);
		}
	}

	for(int m = 0;m < Num; m ++)
	{
		pPoint[m].r = temppoint_r[m];
		//		pPoint[m].z = point_z[m];
	}
	delete[] point_r;
	delete[] temppoint_r;
	return 1;
}
void CLidarObject::AllocPointBuffer()
{
	if (m_nPointNum>0)
	{
	 m_pOkBuf=new unsigned char[m_nPointNum*2+10];
	 m_pPointBuffer=new sPoint[m_nPointNum];
	}
	
}
void CLidarObject::Laser_to_Vehicle_Translator(
	float latove_coor[],
	float latove_A[],
	float trans_coor[],
	Ipp64f *pDat)
{
	float rotationpara[9];	//用于存储转换矩阵的数组

	float A[3];			//存储latove_A转换成的弧度
	for (int i=0; i<3; i++) //将旋转角度转换成旋转弧度
	{
		A[i] = latove_A[i];
	}
	rotationpara[0] = cos(A[2])*cos(A[1]);
	rotationpara[1] = cos(A[2])*sin(A[1])*sin(A[0]) + sin(A[2])*cos(A[0]);
	rotationpara[2] = sin(A[2])*sin(A[0]) - cos(A[2])*sin(A[1])*cos(A[0]);
	rotationpara[3] = -sin(A[2])*cos(A[1]);
	rotationpara[4] = cos(A[2])*cos(A[0]) - sin(A[2])*sin(A[1])*sin(A[0]);
	rotationpara[5] = sin(A[2])*sin(A[1])*cos(A[0]) + cos(A[2])*sin(A[0]);
	rotationpara[6] = sin(A[1]);
	rotationpara[7] = -cos(A[1])*sin(A[0]);
	rotationpara[8] = cos(A[1])*cos(A[0]);

	Ipp64f pSrc1[3*3] = { rotationpara[0], rotationpara[1], rotationpara[2],
		rotationpara[3], rotationpara[4], rotationpara[5],
		rotationpara[6], rotationpara[7], rotationpara[8] };// 转换矩阵

	int src1Width  = 3;			//矩阵宽
	int src1Height = 3;			//height of Matrix

	int src1Stride2 = sizeof(Ipp64f);
	int src1Stride1 = 3*sizeof(Ipp64f);

	Ipp64f pSrc2[3*1] = { trans_coor[0], trans_coor[1], trans_coor[2] };// 在激光空间坐标下坐标

	int src2Width  = 1;
	int src2Height = 3;
	int src2Stride2 = sizeof(Ipp64f);
	int src2Stride1 = sizeof(Ipp64f);

	Ipp64f pDst[3*1];		//mul destination matrix
	int dstStride2 = sizeof(Ipp64f);
	int dstStride1 = sizeof(Ipp64f);

	// 转换矩阵和激光空间坐标下的坐标矩阵相乘，结果存在pDst中
	IppStatus status = ippmMul_mm_64f(pSrc1, src1Stride1, src1Stride2, src1Width, src1Height,
		pSrc2, src2Stride1, src2Stride2, src2Width, src2Height,
		pDst, dstStride1, dstStride2);

	Ipp64f pSrc3[3*1] = 
	{
		latove_coor[0],
		latove_coor[1], 
		latove_coor[2] 
	}; //平移量矩阵

	int src3Width = 1;
	int src3Height = 3;
	int src3Stride2 = sizeof(Ipp64f);
	int src3Stride1 = sizeof(Ipp64f);

	//矩阵相加得到转换后的车体坐标系坐标（pDat）
	IppStatus status1 = ippmAdd_mm_64f(pSrc3, src3Stride1, src3Stride2,
		pDst,  src3Stride1, src3Stride2,
		pDat, dstStride1, dstStride2, src2Width, src2Height);
}
void CLidarObject::Cal_Laser_Point()
{
	//	NLidarNode node=m_dataLidar.m_portArray[port];
	float angle=0.0;
	float radius=0.0;
	float p=0.0;
	float q=0.0;
	short temp1, temp2;

	float latove_A_0[3];   //旋转角
	float latove_coor_0[3];//平移量

	float trans_coor[3]={0,0,0};//原来坐标
	Ipp64f pDat[3]={0.0, 0.0, 0.0};//转换后
	for (int i=0;  i<m_nPointNum;  i++)
	{
		if (m_nPointNum==361)
		{
			angle = (i*0.5) * M_PI/180;
		}
		else if(m_nPointNum==401)
		{
			angle = (40 + i*0.25) * M_PI/180;
		}

		temp1=m_pOkBuf[i*2+1+7] << 8;
		temp2=m_pOkBuf[i*2+7];

		radius = temp1 + temp2 ;

		if(radius >= 8000)
			radius =80000.0;
		radius /=100.0;

		p = radius*cos(angle);
		q = radius*sin(angle);

		float trans_coor[3];

		trans_coor[0] = pDat[0]= p;
		trans_coor[1] = pDat[1]= q;
		trans_coor[2] = pDat[2]= 0.0;

		latove_coor_0[0]=m_Mat[0];
		latove_coor_0[1]=m_Mat[1];
		latove_coor_0[2]=m_Mat[2];

		latove_A_0[0]=m_Mat[3];
		latove_A_0[1]=m_Mat[4];
		latove_A_0[2]=m_Mat[5];
		Laser_to_Vehicle_Translator(latove_coor_0, latove_A_0, trans_coor, pDat);	
		m_pPointBuffer[i].r=radius;
		m_pPointBuffer[i].tht=angle;
		m_pPointBuffer[i].x=pDat[0];
		m_pPointBuffer[i].y=pDat[1];
		m_pPointBuffer[i].z=pDat[2];
	}
}


void CLidarObject::ReleasePointBuffer()
{
	if (m_pOkBuf!=NULL)
	{
		delete[] m_pOkBuf;
	}
	if (m_pPointBuffer!=NULL)
	{
		delete[] m_pPointBuffer;
	}
}
