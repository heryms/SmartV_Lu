#include "StdAfx.h"

#include "ckCoordinate.h"

#include "ckMapBaseObject.h"



BOOL ckMapBaseObject::GetMapCentor(ckPointS &centor)
{
	
	if(m_MifPoint.size()!=0)
	{
		double top=0.0,right=0.0;
		double left=m_MifPoint.at(0).y;
		double bottom=m_MifPoint.at(0).x;
		for (int i=0;i<m_MifPoint.size();i++)
		{
			ckMifPoint pt=m_MifPoint.at(i);
			if (pt.y<left)
			{
				left=pt.y;
			}
			if (pt.y>right)
			{
				right=pt.y;
			}
			if (pt.x<bottom)
			{
				bottom=pt.x;
			}
			if (pt.x>top)
			{
				top=pt.x;
			}
		}
		m_fullExtent.min[0]=left;
		m_fullExtent.min[1]=bottom;
		m_fullExtent.max[0]=right;
		m_fullExtent.max[1]=top;
		centor.x=(top+bottom)/2.0;
		centor.y=(left+right)/2.0;
		return TRUE;
	}
	else
		return FALSE;
}
void ckMapBaseObject::Refresh()
{
	
}
//×ª»»»ñµÃ»æÍ¼Í¼²ã
UINT ckMapBaseObject::Convert2PointLayer(vector<ckMifPoint> ptM,vector<ckPointS>&ptS)
{
	if (ptM.size()==0)
	{
		return 0;
	}

	for (int i=0;i<ptM.size();i++)
	{
		ckPointS pt;
		pt	.x=ptM[i].x-m_centerPoint.x;
        pt.y=ptM[i].y-m_centerPoint.y;
		ptS.push_back(pt);
	}
	return 1;
}
UINT ckMapBaseObject::Convert2LineLayer(vector<ckMifPoint>ptM,vector<ckMifLine>line,vector<ckPloyLineS>& lineS)
{
	if (line.size()==0||ptM.size()==0)
	{
		return 0;
	}
	
	for (int i=0;i<line.size();i++)
	{
		ckPloyLineS linetemp;
		int s,e;
		s=line.at(i).snode;
		e=line.at(i).enode;
		if (s<e)
		{
			for (int i=s;i<=e;i++)
			{
				ckPointS pt;
				pt.x=ptM.at(i).x-m_centerPoint.x;
				pt.y=ptM.at(i).y-m_centerPoint.y;
				linetemp.ptData.push_back(pt);
			}
		}
		else
		{
			for (int i=s;i>=e;i--)
			{
				ckPointS pt;
				pt.x=ptM.at(i).x-m_centerPoint.x;
				pt.y=ptM.at(i).y-m_centerPoint.y;
				linetemp.ptData.push_back(pt);
			}
		}
		//CalLineAzmith(linetemp);
		lineS.push_back(linetemp);
	}
	return 1;
}
UINT ckMapBaseObject::Convert2LaneLayer(vector<ckMifPoint>ptM,vector<ckRoadLane>line,vector<ckPloyLineS>& lineS)
{
	if (line.size()==0||ptM.size()==0)
	{
		return 0;
	}
	ckPointS centor;
//	GetMapCentor(centor);
	for (int i=0;i<line.size();i++)
	{
		ckPloyLineS linetemp;
		int s,e;
		s=line.at(i).snode;
		e=line.at(i).enode;
		if (s<e)
		{
			for (int i=s;i<=e;i++)
			{
				ckPointS pt;
				pt.x=ptM.at(i).x-m_centerPoint.x;
				pt.y=ptM.at(i).y-m_centerPoint.y;
				linetemp.ptData.push_back(pt);
			}
		}
		else
		{
			for (int i=s;i>=e;i--)
			{
				ckPointS pt;
				pt.x=ptM.at(i).x-m_centerPoint.x;
				pt.y=ptM.at(i).y-m_centerPoint.y;
				linetemp.ptData.push_back(pt);
			}
		}
		linetemp.type=line.at(i).iscross;
		lineS.push_back(linetemp);
	}
	return 1;
}
void ckMapBaseObject::CalLineAzmith(vector<GpsLine> &line)
{
	ASSERT(!line.empty());
	for (int i=0;i<line.size()-1;i++)
	{
		double angle=AmizthCaltulate(line[i].Gnx,line[i].Gny,line[i+1].Gnx,line[i+1].Gny);
	   line[i].azimuth=angle;
	}
	line[line.size()-1].azimuth=line[line.size()-2].azimuth;
}
UINT ckMapBaseObject::Convert2TrackLayer(vector<ckRoadLane> line,vector<GpsLine> &trackLine)
{
	if (line.size()==0||m_LanePoint.size()==0)
	{
		return 0;
	}
	for (int i=0;i<line.size();i++)
	{
		vector<GpsLine> linetemp;
		int s,e;
		s=line.at(i).snode;
		e=line.at(i).enode;
		if (s<e)
		{
			for (int j=s;j<=e;j++)
			{
				GpsLine pt;
				pt.Gnx=m_LanePoint.at(j).x;
				pt.Gny=m_LanePoint.at(j).y;
				linetemp.push_back(pt);
			}
		}
		else
		{
			for (int l=s;l>=e;l--)
			{
				GpsLine pt;
				pt.Gnx=m_LanePoint.at(l).x;
				pt.Gny=m_LanePoint.at(l).y;
				linetemp.push_back(pt);
			}
		}
		CalLineAzmith(linetemp);
		vector<GpsLine>::iterator it;
		for (it=linetemp.begin();it<linetemp.end();it++)
		{
			trackLine.push_back(*it);
		}
	}
	return 1;
}
UINT ckMapBaseObject::Convert2IndexLayer(vector<ckLineSegment> lines,vector<PointShow>&ptcloud)
{
	for (int i=0;i<lines.size();i++)
	{
		PointShow pt;
		ckMifPoint ptMif=m_MifPoint.at(lines.at(i).lineinfo.snode);
		pt.x=ptMif.x-m_centerPoint.x;
		pt.y=ptMif.y-m_centerPoint.y;
		pt.z=20.0f;
		pt.type=lines.at(i).lineinfo.id;
		ptcloud.push_back(pt);
	}
	return 1;
}