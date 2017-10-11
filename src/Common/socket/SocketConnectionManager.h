#ifndef __jklasdjkljklejkilewjiowiojfjla982389hjdf_lsdjflksjdfll_H__
#define __jklasdjkljklejkilewjiowiojfjla982389hjdf_lsdjflksjdfll_H__
#include <list>
#include "CSkt.h"
#include "NTMSG.h"
#include <string>
#define NetConState_Connecting         (0)
#define NetConState_ConnectTimeOut	   (1)
#define NetConState_ConnectError	   (2)
#define NetConState_Connected		   (3)
#define NetConState_Disconnected	   (4)

#define NetErrorCode_CreateSocket	   (0)
#define NetErrorCode_ConnectToServer   (1)
#define NetErrorCode_Select		       (2)
#define NetErrorCode_RecvZeroByte      (3)
#define NetErrorCode_SendZeroByte      (4)
#define NetErrorCode_DNSError   	   (10)
#define NetErrorCode_ConnectTimeOut	   (11)



class CSocketConnectionManager
{
public:
	static CSocketConnectionManager* GetInstance();
	CSocketConnectionManager();
	~CSocketConnectionManager();
	void	InitValue();
	int     GetUrlAddrinfoCompatibleIpv6v4(struct addrinfo**out,const char*url, int port);
	int     SetConnectionAlived(int keep[4]/*alive_idle_interval_count*/);
	bool    ConnectToAddrPort(int port, const char* pStrofServerAddr);
	bool    DoConnectToAddrPort(int port ,const char* pStrofServerAddr);    
    void    ReConnectToPreServer();    
    void    CloseConnect();
	void    SendMsgFromNetMsg(NTMSG* msg);
	void    InitEncryptBySeed(long send, long recv);    
    void    DecodeEncryptBuf(char *buf, int len);
	void    EncodeEncryptMsg(NTMSG *msg);
	void    ClearCachedMsg();
	void    RestartConnectMgr();
	bool    CheckEnableOfUpdate();
	void    CheckConnectingTimeOut(int dt);
	bool    UpdateConnectingStatus(int dt);
	bool    UpdateConnectingStatus1(int dt);
	bool    UpdateCheckWriteFlagNTMSG(NTMSG * mtmsg);
	bool    DoDecodeofNTMSG(NTMSG * mtmsg,int );
	NTMSG*  GenNTMSGWhileRead();
	bool    UpdateCheckReadFlag();
	bool    UpdateCheckWriteFlag();
	void	DoWhileSocketError();
	bool	DoConnectToAddr_socket(addrinfo*resaddrinfo,int port);
	bool	ReConnectToUrlPort(const char*url, int port);
	int     GetStateOfNet() const { return m_netConnectionState; }
	NTMSG*   getMsgFromCache();
	void    RemoveMsgFromCache(NTMSG* Msg);
	void    SetNameOfSocketConnet(const char * name){ m_SocketNameForMultSocket = name; }
	void    Update(int dt);
	void    SetNtConState(int stat);
private:
	void    OnConnectTimeOutError();
	void    OnErrorOfErrorCode(int errorID);
	void    OnSuccessWhileConnecting();
	bool    CheckSocketConnectState();	
	char                    m_sADDR[256];
	int                     m_portNumber;
	std::list<NTMSG*>  m_sendMessageList;
	std::list<NTMSG*>  m_recvMessageList;
	CSkt*        m_scmpSocket;
    int                     m_netConnectionState;
    bool                    m_endecodeinited;    
	static CSocketConnectionManager*  m_SocketConnectMgrInstance;
	std::string             m_SocketNameForMultSocket;
	int                     m_tOutTimerSocketConnecting;
	int						m_reconnectcount;
	int						m_retryconnectcount;
	const char*				m_instanceName;
	std::string             m_instanceName2;
};

#endif