#include "stdafx.h"
#include "udpSend.h"

udpSendMsg::udpSendMsg()
{
	m_port_number= 8000;
}

udpSendMsg::~udpSendMsg()
{
	closesocket(sd);
}

void udpSendMsg::initUdpSocket(int port_number, CString ip)
{
	m_port_number= port_number;

	///////////////////////////数据传输初始化/////////////////////////////////////////////
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		//exit(0);
	}
	/* Open a datagram socket */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET)
	{
		fprintf(stderr, "Could not create socket.\n");
		//	WSACleanup();
		//	exit(0);
	}
	/* Clear out server struct */
	memset((void *)&server, '\0', sizeof(struct sockaddr_in));
	/* Set family and port */
	server.sin_family = AF_INET;
	server.sin_port = htons(port_number); // 8002
	server.sin_addr.S_un.S_addr=inet_addr(ip);
	char bOpt = true;  
	setsockopt(sd,SOL_SOCKET,SO_BROADCAST,(char*)&bOpt,sizeof(bOpt));
}


void udpSendMsg::sendUdpMsg(BYTE *pBuffer, int size)
{
	server_length = sizeof(struct sockaddr_in);

	memcpy(buffer, pBuffer, size);

	if (sendto(sd, buffer, size, 0, 
		(struct sockaddr *)&server, server_length) == -1)
	{
		TRACE("Error transmitting data.\n");
	}
}



