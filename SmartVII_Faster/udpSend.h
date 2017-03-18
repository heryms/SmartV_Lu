#if !defined(_UDP_SEND_MSG_H_INCLUDED_)
#define _UDP_SEND_MSG_H_INCLUDED_

#include <WinSock2.h>		// MFC socket extensions


class udpSendMsg
{

private:
	//////////////////Êý¾Ý´«Êä//////////////////////////////
	WSADATA w;								/* Used to open Windows connection */
	SOCKET sd;	

	int m_port_number;				        /* The port number to use */

	/* The socket descriptor */
	int server_length;						/* Length of server struct */

	struct sockaddr_in server;				/* Information about the server */
	struct sockaddr_in client;				/* Information about the client */


	char buffer[1000/*BUFFERLENGTH*/];
	///////////////////////////////////////////////////////////////////////////

public:
	udpSendMsg();
    ~udpSendMsg();

	void initUdpSocket(int port_number, CString ip);
	void sendUdpMsg(BYTE *pBuffer, int size);

/////////////////////////////////////////////////////////////////
	void sendVideoMsg(BYTE command,BYTE *pBuffer, int size);
	void sendLidarMsg(BYTE command,BYTE *pBuffer, int size);
	void sendGISMsg(BYTE command,BYTE *pBuffer, int size);
	void sendControlMsg(BYTE command, BYTE *pBuffer, int size);
	void sendLedMsg(BYTE command, BYTE *pBuffer, int size);
};

#endif