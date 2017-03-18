#include "StdAfx.h"
#include "PursuitTset.h"


CPursuitTset::CPursuitTset(void)
{
}


CPursuitTset::~CPursuitTset(void)
{
}
//将存储在
BOOL CPursuitTset::pstLoadTestTrack(CString path)
{
	if (1!=LoadMapFile(path))
	{
		AfxMessageBox("导入地图数据错误");
		return FALSE;
	}
	if (m_trackPointLayer.size()!=0)
	{
		m_trackPointLayer.clear();
	}
	//构建显示图层
	for (int i=0;i<fileLength;i++)
	{
		PointShow pt;
		pt.x=m_RoadDataBuffer[i].x-initialPoint.x;
		pt.y=m_RoadDataBuffer[i].y-initialPoint.y;
		pt.z=0;
		pt.type=0;
		m_trackPointLayer.push_back(pt);
		GpsLine ptline;
		ptline.Gnx=m_RoadDataBuffer[i].x;
		ptline.Gny=m_RoadDataBuffer[i].y;
		ptline.azimuth=m_RoadDataBuffer[i].angle;
		m_trackGpsLayer.push_back(ptline);
	}

	return TRUE;
}