#if !defined(_UDP_MSG_H_INCLUDED_)
#define _UDP_MSG_H_INCLUDED_

#include <afxsock.h>		
#include <vector>
using namespace std;
typedef vector<unsigned char> VelodyneGrid;
#define BUFFERLENGTH 512


typedef void (*CallBack_udp)(vector<unsigned char>) ;

class udpReceiveMsg
{

public:
	//////////////////数据传输//////////////////////////////

	//////////////////数据传输//////////////////////////////
	WSADATA w;								/* Used to open Windows connection */
	SOCKET sd;	

	int m_port_number;				        /* The port number to use */

	/* The socket descriptor */
	int server_length;						/* Length of server struct */
	struct sockaddr_in server;				/* Information about the server */
	struct sockaddr_in client;				/* Information about the client */

	CRITICAL_SECTION cs;
	CallBack_udp m_callback;
	///////////////////////////////////////////////////////////////////////////
private:
	VelodyneGrid m_objGrid;
public:
	udpReceiveMsg();
    ~udpReceiveMsg();
	void readConfigIP();
	void initUdpSocket();
	void startListenSocket(void);
	void GetMsg(int bytes_received);
	UINT receivepro();
	void excu_receive(unsigned char* buf,int len);
	void SetUdpCallback(CallBack_udp a){m_callback=a;}
};


#endif