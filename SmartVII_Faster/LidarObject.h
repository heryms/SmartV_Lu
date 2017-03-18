#pragma once
struct RoadParam 
{
	float stdErr;
	float disMin;
	float disMax;
	int nearPoint;
	float width;
	float leftWidth;
	float rightWidth;

};
class CLidarObject
{

public:
	CLidarObject(void);
	virtual ~CLidarObject(void);

protected:
	//ANGLE_MODE m_eAngleMode;
	int  m_nPointNum;
	//bool m_bDataCapture;
	unsigned char* m_pOkBuf;
	double  m_Mat[6];
	INSPVA  m_Location;
	sPoint *m_pPointBuffer;

public:
	UINT MeanFilter(int Num,sPoint *pPoint,int filter);
	float cal_std(int start,int num,const float *val);
	void  AllocPointBuffer();
	void ReleasePointBuffer();
	inline void SetCurrentLocation(INSPVA pos){m_Location=pos;}
	inline INSPVA GetCurrentLocation(){return m_Location;}
	inline void SetPointNum(int num){m_nPointNum=num;}
	inline void SetMat(double *mat){memcpy(m_Mat,mat,6);}
	void Cal_Laser_Point();
	void Laser_to_Vehicle_Translator(
		float latove_coor[],
		float latove_A[],
		float trans_coor[],
		Ipp64f *pDat);


};

