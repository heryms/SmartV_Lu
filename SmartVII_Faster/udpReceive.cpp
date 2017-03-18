#include "stdafx.h"
#include <process.h>
#include "udpReceive.h"
#include "math.h"

bool socketEixt =false;
void SocketThread(void*);



udpReceiveMsg::udpReceiveMsg()
{
	m_port_number= 8801;
	InitializeCriticalSection(&cs);
}

udpReceiveMsg::~udpReceiveMsg()
{
	socketEixt = true;
	Sleep(10);
	closesocket(sd);
	WSACleanup();
	DeleteCriticalSection(&cs);
}

void udpReceiveMsg::readConfigIP()
{
	char path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,(LPSTR)path);
	// path="roadparam.ini";
	string strpath(path);
	strpath+="\\IPConfig.ini";
	m_port_number=GetPrivateProfileInt("Velodyne","port",8800,strpath.c_str());
}
void udpReceiveMsg::initUdpSocket()
{
	readConfigIP();
	///////////////////////////数据传输初始化/////////////////////////////////////////////
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		TRACE("Could not open Windows connection.\n");
		//exit(0);
	}

	/* Open a datagram socket */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET)
	{
		TRACE("Could not create socket.\n");
		//WSACleanup();
		//exit(0);
	}

	/* Clear out server struct */
	memset((void *)&server, '\0', sizeof(struct sockaddr_in));

	/* Set family and port */
	server.sin_family = AF_INET;
	server.sin_port = htons(m_port_number); // 8002

	/* Set server address */

	server.sin_addr.S_un.S_addr=INADDR_ANY;
	///////////////////////////////////////////////////////////////////////////

	/* Bind address to socket */
	if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
	{
		TRACE("Could not bind name to socket.\n");
	
	}

}
UINT recvThread(LPVOID lParam)
{
	udpReceiveMsg *pMsg= (udpReceiveMsg *)lParam;
	pMsg->receivepro();
	return 1;
}
UINT udpReceiveMsg::receivepro()
{
	unsigned char buf[2048];
	int iDataLen;
	SYSTEMTIME st;
	int len=(int)sizeof(struct sockaddr_in);


	while(!socketEixt)
	{
		memset(buf,0,2048);
		if((iDataLen = recvfrom(sd, (char *)buf, 2048, 0,
			(struct sockaddr *)&client,&len)) != ERROR)
		{
			GetLocalTime(&st);
		//	TRACE("last value %d \n",st.wMilliseconds);
			//判断是否为激光数据头
		   if (buf[0]==4&&buf[1]==1)
		   {
			   //EnterCriticalSection(&cs);
			   excu_receive(buf+2,iDataLen-2);
			   
			   //LeaveCriticalSection(&cs);
		   }
			// m_callback(obstacledata,iDataLen);
			//sprintf(tmp, "\n\r\n\r%2d:%2d:%2d\n\r", st.wHour,st.wMinute,st.wSecond);
		}
	}

	

	return 0;
}
void udpReceiveMsg::excu_receive(unsigned char* buf,int len)
{
	int rec=0;
	for (int i=0;i<len;i+=2)
	{
		rec+=buf[i+1];
		
	}
	
	/*if(rec!=1250)
	return;*/
	int gridnum=0;
	for(int j=0;j<len;j+=2)
	{
		unsigned char a[8]={0};

		for(int i=0;i<8;i++)
		{
			a[i] =((buf[j] &0x80)>>7);
			buf[j]<<=1;
		}
		int num;
		num=buf[j+1];
		for(int n=0;n<num;n++)
		{
			//memcpy(pData+8*(k+n),a,8);
			for (int k=0;k<8;k++)
			{
				m_objGrid.push_back(a[k]);
			}
			gridnum+=8;
		  
		}
	}
	//回调传出网格
	EnterCriticalSection(&cs);
	m_callback(m_objGrid);
	LeaveCriticalSection(&cs);
	vector<unsigned char> del;
	m_objGrid.swap(del);
}
void udpReceiveMsg::startListenSocket(void)
{
	//	_beginthread(recvThread,0,(void *)this);
	::AfxBeginThread(recvThread, this);
}



