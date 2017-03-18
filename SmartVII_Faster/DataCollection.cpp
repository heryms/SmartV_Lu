#include "StdAfx.h"
#include "DataCollection.h"


CDataCollection::CDataCollection(void)
{
}


CDataCollection::~CDataCollection(void)
{
}

BOOL CDataCollection::ReadParamFile(char* file)
{
	CvFileStorage *fs=cvOpenFileStorage("DeviceList.xml",0,CV_STORAGE_READ);
	int devNum=cvReadIntByName(fs,"opencv_storage","Num",0);
	for (int i=0;i<devNum;i++)
	{
		char aa[10];
		sprintf(aa,"Com%d",i);
		CvFileNode *pseq=cvGetFileNodeByName(fs,0,aa);
		PortParamList tempParam;
		tempParam.Port=cvReadIntByName(fs,pseq,"name",0);
		tempParam.BaudRate=cvReadIntByName(fs,pseq,"baud",0);
		tempParam.DataLen=cvReadIntByName(fs,pseq,"DataLen",0);
	//	tempParam.Dtype=cvReadIntByName(fs,pseq,"Type",0);
		//cout<<a<<endl;
	   int type=cvReadIntByName(fs,pseq,"Type",0);
	   

	}

	return TRUE;
}
BOOL CDataCollection::PortSet(int num)
{
	return TRUE;
}
