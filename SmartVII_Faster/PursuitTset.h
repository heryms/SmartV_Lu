/************************************************************************/
/* ���Ʋ����࣬
   �̳���Pathgenerate��
   �ܹ����������߳̽��ж�·�ߵĿ��Ʒ���
   �ܹ�ʵʱ��¼��ǰ��ʻ�켣
 */
/************************************************************************/

#pragma once
#include "PathGenerate.h"

typedef struct  
{
	double gauX;
	double gauY;
	double angle;
}ckPursuitPoint;
class CPursuitTset:public CPathGenerate
{
public:
	//long m_npointIndex;   //current postion index
	vector<PointShow> m_trackPointLayer;  //lane point buffer under  local
	vector<GpsLine> m_trackGpsLayer;
private:
	BOOL m_bMapStaus;
	CString m_strMapPath;
 public:
	CPursuitTset(void);
	~CPursuitTset(void);
public:
	BOOL pstLoadTestTrack(CString path);
	///ckPursuitPoint pstGetMapCenter();
    inline CString GetMapName(){return m_strMapPath;} 
};

