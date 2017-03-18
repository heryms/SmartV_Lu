#include "stdafx.h"
#include "MapPath.h"
#include <iostream>
using namespace std;
vector<carPosition*> pt_show;
//#define drawOneLine(x1,x2,y1,y2) glBegin(GL_LINES);\
//	glVertex2f((x1),(y1));glVertex2f((x2),(y2));glEnd();
//#define drawPoint(x,y,pt_size) glBegin(GL_LINES);\
//	glVertex2f(x-pt_size,y-pt_size);glVertex2f(x+pt_size,y-pt_size);\
//	glVertex2f(x-pt_size,y+pt_size);glVertex2f(x+pt_size,y+pt_size);\
//	glVertex2f(x+pt_size,y-pt_size);glVertex2f(x-pt_size,y-pt_size);\
//	glVertex2f(x-pt_size,y+pt_size);glVertex2f(x+pt_size,y+pt_size);glEnd();
//void GLinit()
//{
//	glClearColor(0.0,0.0,0.0,0.0);   //指定清除颜色缓存区时使用黑色
//	//glColor3f(0.1, 0.1, 0.0);   //设置绘图的RGB颜色为白色
//
//	glMatrixMode(GL_PROJECTION);    //指定后续受变换函数影响的矩阵，设置为GL_PROJECTION（投影）
//	glLoadIdentity();   //将当前矩阵初始化为一个单位矩阵
//	//pt_show.at(0)->car_X
//	//gluOrtho2D(-1.0, 1.0, -1.0, 1.0);   //指定二维平面上的裁剪窗口（Clipping window）
//	//gluOrtho2D(pt_show.at(0)->car_X-50,pt_show.at(0)->car_X+50,pt_show.at(0)->car_Y-50,pt_show.at(0)->car_Y+50);
//}
//void display()
//{
//	glClear(GL_COLOR_BUFFER_BIT);
//	glColor4f(0.1, 0.2, 0.1, 0.5);
//	
//	glLoadIdentity();
//	gluLookAt(0,-5,10
//		,0,0,0
//		,0,1,0);
//	//纵向
//	for(int i=1;i<=20;i++){
//		drawOneLine(-0.5*i,-0.5*i,-10,10);
//		drawOneLine(0.5*i,0.5*i,-10,10);
//	}
//	//横向
//	for(int i=1;i<=25;i++){
//		drawOneLine(-12,12,0.5*i,0.5*i);
//		drawOneLine(-12,12,-0.5*i,-0.5*i);
//	}
//
//	//窗口放缩后 模型相应放缩
//	glColor3f(0,1,0);
//	drawOneLine(0,0,-14,14);
//	drawOneLine(-12,12,0,0);
//	//路径
//	glColor3f(1,0,0);
//	double x[20];
//	double y[20];
//	int i=0;
//	for(vector<carPosition*>::iterator it=pt_show.begin();it!=pt_show.end();it++){
//		glVertex2f(((*it)->car_X)-(pt_show.at(0)->car_X),((*it)->car_Y)-(pt_show.at(0)->car_Y));
//		x[i]=((*it)->car_X)-(pt_show.at(0)->car_X);
//		y[i]=((*it)->car_Y)-(pt_show.at(0)->car_Y);
//		i++;
//	}
//	for(int j=0;j<20;j++){
//		drawPoint(x[j]/10,y[j]/10,0.02);
//	}
//	glFlush();
//}
//void reshape(int w,int h)
//{
//	glViewport(0,0,(GLsizei)w,(GLsizei)h);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	//glFrustum(-1,1,-1,1,1.5,20);
//	gluPerspective(60,1,1.5,20);
//	glMatrixMode(GL_MODELVIEW);
//}
double PT::getVLength(vector<PT*>::iterator fPoint,vector<PT*>::iterator sPoint)
{
	double x1=(*fPoint)->pointX;
	double y1=(*fPoint)->pointY;
	double x2=(*sPoint)->pointX;
	double y2=(*sPoint)->pointY;
	double x0=this->pointX;
	double y0=this->pointY;
	//(y2-y1)x+(x1-x2)y+y1x2-x1y2=0		直线方程
	double A=y2-y1;
	double B=x1-x2;
	double C=y1*x2-x1*y2;
	double vLength=abs(A*x0+B*y0+C)/sqrt(A*A+B*B);
	return vLength;
}

void MapCreater::Init(const char *shpName,double D,int stepLength,float bufferRadius)
{
	this->shpName=shpName;   //为什么依然可以赋值
	this->D=D;
	this->stepLength=stepLength;
	this->bufferRadius=bufferRadius;
}

void MapCreater::ReadPoints(const char * fileName)
{
	FILE *pFIle;
	long lSize;
	char *buffer;
	size_t result;
	pFIle=fopen(fileName,"rb");
	if(pFIle==NULL){
		fputs("File error",stderr);
		exit(1);
	}
	fseek(pFIle,0,SEEK_END);
	lSize=ftell(pFIle);
	rewind(pFIle);
	int charLength=sizeof(char);
	//both are OK
	//buffer=(char*)malloc(sizeof(char)*lSize);
	buffer=new char[sizeof(char)*lSize];
	//buffer=new char();	//this is wrong
	if(buffer==NULL){
		fputs("Memory error",stderr);
		exit(2);
	}
	result=fread(buffer,1,lSize,pFIle);
	if(result!=lSize){
		fputs("Reading error",stderr);
		exit(3);
	}
	int dataNum=0;
	while(dataNum!=result/sizeof(PT)){
		PT *point=new PT();
		memcpy(point,buffer+dataNum*16,16);
		data.push_back(point);
		++dataNum;
		//cout<<"第"<<++dataNum<<"项数据"<<endl;
	}
	fclose(pFIle);
	free(buffer);
}

void MapCreater::DPLine(vector<PT*>::iterator first,vector<PT*>::iterator second)
{
	//得到分界点
	double maxD=0;
	vector<PT*>::iterator it=first;
	for(it=first;it!=second;it++){
		double vLength=(*it)->getVLength(first,second);
		if(vLength>maxD){
			maxD=vLength;
			m_it1=it;
		}
	}			
	ptStack1.push_back(m_it1);
	ptStack2.push_back(second);
	if(maxD<D){
		vector<PT*>::iterator temp1=first;
		vector<PT*>::iterator temp2=second;
		//压缩后的数据存数新数组
		PT* pt1=new PT();
		pt1->pointX=(*first)->pointX;
		pt1->pointY=(*first)->pointY;
		outData.push_back(pt1);
		cout<<"保留首末点，压缩结束"<<endl;
	}
	else{
		cout<<"迭代"<<endl;
		DPLine(first,m_it1);		
		ptStack1.pop_back();
		ptStack2.pop_back();
		if(!ptStack1.empty()){
			DPLine(ptStack1.back(),ptStack2.back());
			ptStack1.pop_back();
			ptStack2.pop_back();
		}
	}
}

void MapCreater::GetLocalPath(carPosition *curP,vector<carPosition *> &dstP)
{
	OGRRegisterAll();
	OGRDataSource * poDS;
	poDS=OGRSFDriverRegistrar::Open(shpName);
	if(NULL==poDS){
		cout<<"打开全局路径失败"<<endl;
		exit(1);
	}
	OGRLayer * poLayer=poDS->GetLayer(0);
	OGRFeature * poFeature;
	//poLayer=ResetReading();
	int featureCount=poLayer->GetFeatureCount();
	double x0,y0;	//当前路径上的点
	while((poFeature=poLayer->GetNextFeature())!=NULL){
		OGRGeometry * poGeometry;
		poGeometry = poFeature->GetGeometryRef();
		if(poGeometry==NULL
			||wkbFlatten(poGeometry->getGeometryType()!=wkbLineString)){
				cout<<"路网数据格式不正确"<<endl;
				exit(1);
		}
		//创建当前车辆位置矢量点及其缓冲区
		OGRPoint *poPoint = (OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
		OGRPolygon *ptBuffer=(OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
		poPoint->setX(curP->car_X);
		poPoint->setY(curP->car_Y);
		//半径为bufferRadius的缓冲区
		ptBuffer=(OGRPolygon*)poPoint->Buffer(bufferRadius,30);
		OGRLineString *poLineString=(OGRLineString*)poGeometry;
		//求缓冲区与路径的交点
		OGRLineString *dstLine=(OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
		if(poLineString->Intersects(ptBuffer)){
			dstLine=(OGRLineString*)poLineString->Intersection(ptBuffer);
			x0=(dstLine->getX(0)+dstLine->getX(1))/2;
			y0=(dstLine->getY(0)+dstLine->getY(1))/2;
			cout<<x0<<"   "<<y0<<endl;
			cout<<dstLine->getNumPoints();
			OGRPoint * dstPoint=(OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
			dstPoint->setX(x0);
			dstPoint->setY(y0);
			OGRPoint * nextPoint=(OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
			int pointNum=100/this->stepLength;
			for(int i=0;i<pointNum;i++){
				getNextPoint(dstPoint,poLineString,this->stepLength,nextPoint);
				carPosition *car_position=new carPosition();
				car_position->car_X=nextPoint->getX();
				car_position->car_Y=nextPoint->getY();
				dstP.push_back(car_position);
				dstPoint->setX(car_position->car_X);
				dstPoint->setY(car_position->car_Y);
			}
		}
	}

	//CreatShapePoint(dstP);
}

void MapCreater::CreateGlobalMap(const char *ptFile,int mode)
{
	if(mode==FROM_TXT){
		ReadPoints(ptFile);
	}
	for(size_t i=0;i<data.size()-1;){
		if(abs(data[i]->pointX-data[i+1]->pointX)>20||abs(data[i]->pointY-data[i+1]->pointY)>20){
			data.erase(data.begin()+i);
			cout<<"剔除脏点"<<endl;
		}
		else{
			i++;
		}
	}
	DPLine(data.begin(),--data.end());
	CreatShapefile();
}
//创建全局地图（矢量线）
void MapCreater::CreatShapefile()
{
	OGRRegisterAll();
	const char* strDriverName="ESRI Shapefile";
	//输出文件名
	const char* strVectorFile=this->shpName;
	//只用静态成员函数可以通过类名调用
	OGRSFDriver *pDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(strDriverName);
	if(pDriver==NULL){
		cout<<"驱动不可用"<<endl;
	}
	//创建数据
	OGRDataSource* oDS=pDriver->CreateDataSource(strVectorFile,NULL);
	if(oDS==NULL){
		cout<<"创建矢量文件失败"<<endl;
	}
	//创建图层
	OGRLayer *oLayer=oDS->CreateLayer("shpLine",NULL,wkbLineString,NULL);
	if(oLayer==NULL){
		cout<<"图层创建失败"<<endl;
	}
	//创建属性字段
	OGRFieldDefn oFileID("FieldID",OFTInteger);
	oLayer->CreateField(&oFileID);
	//创建属性字段
	OGRFieldDefn oFileName("FieldName",OFTString);
	oFileName.SetWidth(100);
	oLayer->CreateField(&oFileName);
	//构造函数中的解释：
	//he OGRFeatureDefn maintains a reference count,
	//but this starts at zero. 
	//It is mainly intended to represent a count of OGRFeature's based on this definition.
	//作用：维护要素对象的引用计数
	OGRFeatureDefn *poDefn = oLayer->GetLayerDefn();
	//创建要素
	OGRFeature *oFeatureLineY=OGRFeature::CreateFeature(poDefn);
	oFeatureLineY->SetField(0,1);
	oFeatureLineY->SetField(1,"YLine");
	OGRLineString *oLine1=(OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
	int pIndex=0;
	for(vector<PT*>::iterator it=outData.begin();it!=outData.end();it++){
		oLine1->setPoint(pIndex,(*it)->pointX,(*it)->pointY);
		pIndex++;
	}

	oFeatureLineY->SetGeometry(oLine1);
	oLayer->CreateFeature(oFeatureLineY);
	OGRFeature::DestroyFeature(oFeatureLineY);

	//不关闭数据集无法生成矢量图形
	OGRDataSource::DestroyDataSource(oDS);
}

void MapCreater::ShowLocalPath(vector<carPosition*> &pathPoint)
{
	for(vector<carPosition*>::iterator it=pathPoint.begin();it!=pathPoint.end();it++){
		pt_show.push_back(*it);
	}

}
//创建局部路径部分
void MapCreater::ReadShapefile()
{
	OGRRegisterAll();
	OGRDataSource *poDS;
	//Singleton manager for OGRSFDriver instances
	poDS=OGRSFDriverRegistrar::Open(this->shpName,false);
	if(poDS==NULL){
		cout<<"Open failed"<<endl;
		exit(1);
	}
	OGRLayer *poLayer;
	//Fetch a layer by index.
	poLayer=poDS->GetLayer(0);
	OGRFeature *poFeature;
	//Reset feature reading to start on the first feature. 
	poLayer->ResetReading();
	while((poFeature=poLayer->GetNextFeature())!=NULL){
		//OGRFeature 的概要信息
		OGRFeatureDefn *poFDefn=poLayer->GetLayerDefn();
		int iField;
		int i=poFDefn->GetFieldCount();	//得到字段数目
		for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )  
		{  
			OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );  
			//根据字段值得类型，选择对应的输出  
			if( poFieldDefn->GetType() == OFTInteger )  
				printf( "%d,", poFeature->GetFieldAsInteger( iField ) );  
			else if( poFieldDefn->GetType() == OFTReal )  
				printf( "%.3f,", poFeature->GetFieldAsDouble(iField) );  
			else if( poFieldDefn->GetType() == OFTString )  
				printf( "%s,", poFeature->GetFieldAsString(iField) );  
			else  
				printf( "%s,", poFeature->GetFieldAsString(iField) );  
		}  

		OGRGeometry *poGeometry;  

		poGeometry = poFeature->GetGeometryRef();  
		if( poGeometry != NULL   
			&& wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )  
		{  
			OGRPoint *poPoint = (OGRPoint *) poGeometry;  

			printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );  
		}  
		else if(poGeometry!=NULL
			&&wkbFlatten(poGeometry->getGeometryType()==wkbLineString))
		{
			OGRLineString *poLineString=(OGRLineString*)poGeometry;
			int PointsNum=poLineString->getNumPoints();
			double x1=poLineString->getX(0);
			double y1=poLineString->getY(0);
			cout<<"point in linestring,X:"<<x1<<"    Y:"<<y1<<endl;
			//	OGRLineString *oLine1=(OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
			OGRPoint *currentLocation=(OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
			//Fetch point at given distance along curve. 
			//计算曲线上距起点20米的点坐标
			poLineString->Value(20,currentLocation);
			cout<<"My Location"<<currentLocation->getX()<<" , "<<currentLocation->getY()<<endl;
		}
		else  
		{  
			printf( "no point geometry\n" );  
		}    
		OGRFeature::DestroyFeature( poFeature ); 
	}
	OGRDataSource::DestroyDataSource( poDS ); 
}
/*****************************************************************************
* @brief : 定位点的位置，如果在[m,m+1]之间则return m 
* @author : panan
* @date : 2014/3/19 15:11
* @version : ver 1.0
* @inparam : 待定位点
* @outparam : 前一点在点列中的标号
*****************************************************************************/
int MapCreater::LocatePoint(OGRPoint * poPoint,OGRLineString * poLineString)
{
	//可能有运算精度的损失
	//缓冲区与线的交点不在线上
	for(int i=0;i<poLineString->getNumPoints()-1;i++){
		double x1=poLineString->getX(i);
		double y1=poLineString->getY(i);
		double x2=poLineString->getX(i+1);
		double y2=poLineString->getY(i+1);
	//	if((y2-y1)*(x2-poPoint->getX())==(x2-x1)*(y2-poPoint->getY())){		//共线
		if(abs((y2-y1)*(x2-poPoint->getX())-(x2-x1)*(y2-poPoint->getY()))<0.01){		//共线
			if(x1<=poPoint->getX()<=x2||x2<=poPoint->getX()<=x1){		//不在延长线上
				return i;
			}
		}
	}
	cout<<"该点不在这条线上"<<endl;
	return -1;
}
/*****************************************************************************
* @brief : 得到下一点
* @author : panan
* @date : 2014/3/19 15:44
* @version : ver 1.0
* @inparam : 当前已知点，步长，下一点
* @outparam : 下一点，是否找到下一点
*****************************************************************************/
BOOL MapCreater::getNextPoint(OGRPoint * currentPT,OGRLineString * poLineString,double stepLength,OGRPoint * nextPT)
{
	double dfDeltaX,dfDeltaY;
	double dfLength=0;
	int m=LocatePoint(currentPT,poLineString);
	if(stepLength>0){		//计算前进方向的下一点
		double dist=sqrt((currentPT->getX()-poLineString->getX(m))*(currentPT->getX()-poLineString->getX(m))
			+(currentPT->getY()-poLineString->getY(m))*(currentPT->getY()-poLineString->getY(m)));
		stepLength+=dist;	//转换为求m点前方stepLength的点
		for(int i = m; i < poLineString->getNumPoints()-1; i++ )  
		{  
			double      dfDeltaX, dfDeltaY, dfSegLength;  
			dfDeltaX = poLineString->getX(i+1) - poLineString->getX(i);  
			dfDeltaY = poLineString->getY(i+1) - poLineString->getY(i);  
			dfSegLength = sqrt(dfDeltaX*dfDeltaX + dfDeltaY*dfDeltaY);  
			if (dfSegLength > 0)  
			{  
				if( (dfLength <= stepLength) && ((dfLength + dfSegLength) >=   
					stepLength) )  
				{  
					double dfRatio; 
					dfRatio = (stepLength - dfLength) / dfSegLength;  
					//计算返回点坐标
					nextPT->setX( poLineString->getX(i) * (1 - dfRatio)  
						+ poLineString->getX(i+1)* dfRatio );  
					nextPT->setY( poLineString->getY(i) * (1 - dfRatio)  
						+ poLineString->getY(i+1) * dfRatio );  
// 					nextPT->setX((poLineString->getX(i+1)-poLineString->getX(i))*dfRatio);
// 					nextPT->setY((poLineString->getY(i+1)-poLineString->getY(i))*dfRatio);
					return TRUE;  
				}  
				dfLength += dfSegLength;  
			}  
		}
	}
	else{		//计算

	}
	return FALSE;
}
//创建局部路径shp点文件
void MapCreater::CreatShapePoint(vector<carPosition *> & points)
{
	OGRRegisterAll();
	const char* strDriverName="ESRI Shapefile";
	//输出文件名
	const char* strVectorFile="LocalPoints2.shp";
	//只用静态成员函数可以通过类名调用
	OGRSFDriver *pDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(strDriverName);
	if(pDriver==NULL){
		cout<<"驱动不可用"<<endl;
	}
	//创建数据
	OGRDataSource* oDS=pDriver->CreateDataSource(strVectorFile,NULL);
	if(oDS==NULL){
		cout<<"创建矢量文件失败"<<endl;
	}
	//创建图层
	OGRLayer *oLayer=oDS->CreateLayer("shpPoint",NULL,wkbPoint,NULL);
	if(oLayer==NULL){
		cout<<"图层创建失败"<<endl;
	}
	//创建属性字段
// 	OGRFieldDefn oFileID("FieldID",OFTInteger);
// 	oLayer->CreateField(&oFileID);
	//构造函数中的解释：
	//he OGRFeatureDefn maintains a reference count,
	//but this starts at zero. 
	//It is mainly intended to represent a count of OGRFeature's based on this definition.
	//作用：维护要素对象的引用计数
	OGRFeatureDefn *poDefn = oLayer->GetLayerDefn();
	//创建要素
	OGRFeature *oLocalPoints=OGRFeature::CreateFeature(poDefn);
	oLocalPoints->SetField(0,1);
	/*OGRPoint *oPoints=(OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);*/
	//int pIndex=0;
	for(vector<carPosition *>::iterator it=points.begin();it!=points.end();it++){
		OGRPoint *oPoints=(OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
		oPoints->setX((*it)->car_X);
		oPoints->setY((*it)->car_Y);
		oLocalPoints->SetGeometry(oPoints);
		oLayer->CreateFeature(oLocalPoints);
	//	pIndex++;
	}
	
	
	OGRFeature::DestroyFeature(oLocalPoints);

	//不关闭数据集无法生成矢量图形
	OGRDataSource::DestroyDataSource(oDS);
}
//析构函数
MapCreater::~MapCreater()
{
	//删除PT
	for(vector<PT*>::iterator it=data.begin();it!=data.end();it++){
		if(*it!=NULL){
			delete (*it);
			*it=NULL;
		}
	}
	//释放vector
	vector<PT*> v1;
	v1.swap(data);
	for(vector<PT*>::iterator it=outData.begin();it!=outData.end();it++){
		if(*it!=NULL){
			delete (*it);
			*it=NULL;
		}
	}
	vector<PT*> v2;
	v2.swap(outData);
}