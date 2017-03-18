#include "StdAfx.h"
#include "MissionLayer.h"


CMissionLayer::CMissionLayer(void)
{
}


CMissionLayer::~CMissionLayer(void)
{
	if (m_vectorMission.size()>0)
	{
		vector<ckMissionPoint>().swap(m_vectorMission);
	}
}
void CMissionLayer::MissionMsg(EMISSIONTYPE e)
{

}

