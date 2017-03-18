#pragma once
typedef struct tagDataHeader
{
	int headerLength;
	BOOL bGps;
	int gpsLength;
	BOOL bLidar;
	int lidarLength;

	BOOL bVideo;
   int videoLength;

	long frames;
}ckHeader;
#define  CK_SENSOR_1_G_ 0x01
#define  CK_SENSOR_1_L_ 0x02
#define  CK_SENSOR_1_V_ 0x08
#define  CK_FILE_HEADER_LEN_ 20
enum CK_OFFLINE_RES
{
	CK_OK=0,
	CK_NOFILE=1,
	CK_FRAME_FULL=2
};
typedef struct tagOfflineFrame
{
	//int sensor;
	//int headerlen;
	int lidarlen;
	int videolen;
	int gpslen;
	uchar* framedata;

}ckFrame ,*IpckData;
//数据帧操作函数
 ckFrame* ckCreateFrame(int lLen,int vLen,int gLen);
 CK_OFFLINE_RES ckReleaseFrame(ckFrame**frame);
//----thinking---
BOOL ckWriteFrame();
BOOL ckReadFrame();


class COfflineManger
{

public:
	COfflineManger(void);
	~COfflineManger(void);
private:
	//各个传感器数据采集项标记
	BOOL m_bLidarFlags;
	BOOL m_bVideoFlages;
	BOOL m_bGpsFlags;
	int m_nFrameLen;
	BOOL m_bBufferFlag;
	//文件指针记录
	FILE *m_pOfflineFile;
	//数据缓冲帧
private:
	unsigned char* m_pLidarBuffer;
	unsigned char* m_pGpsBuffer;
	unsigned char* m_pVideoBuffer;
	CRITICAL_SECTION m_csData;
	void InitialBuffer();
	void ReleaseBuffer();

public:
	//设置文件头属性
	inline int GetFrameLen(){return m_nFrameLen;}
	inline void EnableLidar(){m_bLidarFlags=TRUE;}
	inline void DisableLidar() {m_bLidarFlags=FALSE;}
    inline void EnableVideo(){m_bVideoFlages=TRUE;}
	inline void DisableVideo(){m_bVideoFlages=FALSE;}
	inline void EnableGps(){m_bGpsFlags=TRUE;}
	inline void DisableGps() {m_bGpsFlags=FALSE;}

private:
	//根据文件属性解析数据
	BOOL AddOfflineFile(CString path);
	void ReadConfig(CString path);
    BOOL RealseFile();
	BOOL GetDataHeader();
public:
	BOOL ReadHeader();
    void Write2File();
	ckFrame* QueryFrames(long pos);
	
	//现实场景模拟控制
private:
	BOOL m_bPlay;
	long m_nCurPos;
	int m_nGpsLen;
	int m_nLidarLen;
	int m_nVideoLen;
	long m_nFramesNum;
	BOOL m_bStopThread;
	HANDLE m_pHandle; 
private:
	BOOL IsHead(unsigned char[],int pos);
	void ProPlay();

public:
	inline void Enable(BOOL flag) {m_bPlay=flag;}
	inline void SetCurrentFrame(long pos) {m_nCurPos=pos;}
	inline long GetCurrentFrame(){return m_nCurPos;}
	inline void SetFrameNum(long num) {m_nFramesNum=num;}
	inline long GetFrameNum(){return m_nFramesNum;}

	//线程控制函数---可能不需要,waiting
public: 
	static UINT OfflineFuncThread(LPVOID Param);
	void CreateThread();
	void ResumeThread();
	void SuspendThread();
	void StartThread();
	void StopThread();
};

