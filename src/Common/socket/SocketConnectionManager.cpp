#include "assert.h"

#ifndef WIN32
#include <netdb.h>
#else
#include <ws2tcpip.h>
#endif
#include "stdio.h"
#include "SocketConnectionManager.h"

#define CONNECT_TIMEOUT         5000
#ifdef WIN32
#define socklen_t		long
#include "CSkt.h"
#define DWOULDBLOCK		WSAEWOULDBLOCK
#else
#include "strings.h"
#include <sys/select.h>
#include "CSkt.h"
#define DWOULDBLOCK		EINPROGRESS
#endif
#include "stdafx.h"
#include "LuaInterface/LuaInterface.h"
#include "lua.hpp"
void CSocketConnectionManager::OnConnectTimeOutError()
{
	CloseConnect();	
	SetNtConState(NetConState_ConnectTimeOut);
	m_tOutTimerSocketConnecting = -1;
	lua::OnNetFailed(m_SocketNameForMultSocket.c_str(), NetErrorCode_ConnectTimeOut);
}
void CSocketConnectionManager::SetNtConState(int stat)
{
	m_netConnectionState = stat;
}
void CSocketConnectionManager::InitValue()
{
	m_scmpSocket = NULL;
	m_portNumber = -1;
	SetNtConState(NetConState_Disconnected);
	m_tOutTimerSocketConnecting = -1;
	m_endecodeinited = false;
}
CSocketConnectionManager::CSocketConnectionManager()
{    
	InitValue();
	m_reconnectcount	= 0;
	m_retryconnectcount = 0;
	m_instanceName		= "defaultSocket";
	m_instanceName2		= "Socket2";
	memset(m_sADDR, 0, sizeof(m_sADDR));
	m_SocketNameForMultSocket = "";
}

CSocketConnectionManager::~CSocketConnectionManager()
{
    CloseConnect();
    ClearCachedMsg();
}
bool CSocketConnectionManager::CheckEnableOfUpdate()
{
	if (!m_scmpSocket
		|| GetStateOfNet() == NetConState_ConnectTimeOut
		|| GetStateOfNet() == NetConState_ConnectError
		|| GetStateOfNet() == NetConState_Disconnected)
		return false;
	return true;
}

void CSocketConnectionManager::CheckConnectingTimeOut(int dt)
{
	m_tOutTimerSocketConnecting -= dt;
	if (m_tOutTimerSocketConnecting <= 0)
	{
		m_tOutTimerSocketConnecting = -1;
		OnConnectTimeOutError();
	}
}

bool CSocketConnectionManager::UpdateConnectingStatus1(int dt)
{
	if (CONNECT_TIMEOUT != m_tOutTimerSocketConnecting)
	{
		OnSuccessWhileConnecting();
	}
	else
	{
		m_tOutTimerSocketConnecting -= dt;
		return true;
	}
	return false;
}


bool CSocketConnectionManager::UpdateConnectingStatus(int dt)
{
	if (!m_scmpSocket->getWS())
	{
		CheckConnectingTimeOut(dt);
	}
#ifdef DEBUG_SOCKET_INFOMA
	int sizeSendedbufsz = m_scmpSocket->SKT_S(pMessageFromSendList->getSendCap(),pMessageFromSendList->getCurrentBuf());
	if (sizeSendedbufsz >= 0)
		pMessageFromSendList->onSend(sizeSendedbufsz);
	if (sizeSendedbufsz < 0)
	{
#ifdef DEBUG_SOCKET_INFOMA
		DBG_L("send zerobyte by send always socket connet off by server or client \n");
#endif
		OnErrorOfErrorCode(NetErrorCode_SendZeroByte);
		return;
	}
#endif
	else
	{
		if (UpdateConnectingStatus1(dt))
			return true;
	}
	return true;
	 
}

NTMSG*  CSocketConnectionManager::GenNTMSGWhileRead()
{
	NTMSG* pMessageFromRecvList = NULL;
	if (!m_recvMessageList.empty())
	{
		pMessageFromRecvList = m_recvMessageList.back();
		if (pMessageFromRecvList->NTMSG_IsOver())
		{
#ifdef DEBUG_SOCKET_INFOMA
			DBG_L("set to null  \n");
#endif
			pMessageFromRecvList = NULL;
		}
		else
		{
#ifdef DEBUG_SOCKET_INFOMA				
			pMessageFromRecvList = NULL;
#endif
		}
	}

	if (!pMessageFromRecvList)
	{
#ifdef DEBUG_SOCKET_INFOMA
		DBG_L("create an empty to receive data !!!\n");
#endif
		pMessageFromRecvList = new NTMSG();
		m_recvMessageList.push_back(pMessageFromRecvList);
	}
	return pMessageFromRecvList;
}

bool CSocketConnectionManager::UpdateCheckReadFlag()
{
	while (m_scmpSocket->getRS()) {
		NTMSG* pMessageFromRecvList = NULL;
		pMessageFromRecvList = GenNTMSGWhileRead();
		ENG_ASSERT(pMessageFromRecvList->NTMSG_getRdCapSize() > 0, "read buf size error errorr.");
		int DataSizeReceived = m_scmpSocket->SKT_R(pMessageFromRecvList->NTMSG_getRdCapSize(),pMessageFromRecvList->NTMSG_getReadBufFr());
		if (DataSizeReceived <= 0)
		{
#ifdef DEBUG_SOCKET_INFOMA
			DBG_L("receive zero byte meaning socket colose by server or client!!!\n");
#endif
			OnErrorOfErrorCode(NetErrorCode_RecvZeroByte);
#ifdef DEBUG_SOCKET_INFOMA
			DBG_L("send error code NetErrorCode_RecvZeroByte!!!\n");
#endif
			return true;
		}
		DoDecodeofNTMSG(pMessageFromRecvList, DataSizeReceived);
		
		pMessageFromRecvList->NTMSG_CallWhileReceive(DataSizeReceived);
		CheckSocketConnectState();
	}
	return false;
}
bool CSocketConnectionManager::DoDecodeofNTMSG(NTMSG * mtmsg,int datasr)
{
	NTMSG * pMessageFromRecvList = mtmsg;
	if (pMessageFromRecvList->NTMSG_GetSizeAndType() >= 0)
	{
#ifdef DEBUG_SOCKET_INFOMA
		DBG_L("decode msg data !!!\n");
#endif
		DecodeEncryptBuf(pMessageFromRecvList->NTMSG_getCBuf(), datasr);
#ifdef DEBUG_SOCKET_INFOMA
		DBG_L("decode msg data end!!!\n");
#endif
	}
	return true;
}
bool CSocketConnectionManager::UpdateCheckWriteFlagNTMSG(NTMSG*ntmsg)
{
	NTMSG* pMessageFromSendList = ntmsg;
	while (!pMessageFromSendList->NTMSG_IsOver())
	{
		int sizeSendedbufsz = m_scmpSocket->SKT_S(pMessageFromSendList->NTMSG_getSdCapSize(), pMessageFromSendList->NTMSG_getCBuf());
		if (sizeSendedbufsz >= 0)
			pMessageFromSendList->NTMSG_CallWhileSend(sizeSendedbufsz);
		if (sizeSendedbufsz < 0)
		{
#ifdef DEBUG_SOCKET_INFOMA
			DBG_L("send zerobyte by send always socket connet off by server or client \n");
#endif
			OnErrorOfErrorCode(NetErrorCode_SendZeroByte);
			return true;
		}
	}
	return false;
}
bool CSocketConnectionManager::UpdateCheckWriteFlag()
{
	while (m_scmpSocket->getWS())
	{
		if (m_sendMessageList.empty())
		{
			break;
		}
		else
		{
			NTMSG* pMessageFromSendList = m_sendMessageList.front();
			if(UpdateCheckWriteFlagNTMSG(pMessageFromSendList))
				return true;
#ifdef DEBUG_SOCKET_INFOMA
			int sizeSendedbufsz = m_scmpSocket->SKT_S(pMessageFromSendList->getSendCap(),pMessageFromSendList->getCurrentBuf());
			if (sizeSendedbufsz >= 0)
				pMessageFromSendList->onSend(sizeSendedbufsz);
			if (sizeSendedbufsz < 0)
			{
#ifdef DEBUG_SOCKET_INFOMA
				DBG_L("send zerobyte by send always socket connet off by server or client \n");
#endif
				OnErrorOfErrorCode(NetErrorCode_SendZeroByte);
				return true;
			}
#endif

			if (pMessageFromSendList->NTMSG_IsOver())
			{
#ifdef DEBUG_SOCKET_INFOMA
				DBG_L("pMessageFromSendList->isEnd() \n", pMessageFromSendList->isEnd());
#endif
				delete pMessageFromSendList;
			}
			m_sendMessageList.pop_front();
		}
		CheckSocketConnectState();
	}
	return false;
}
void CSocketConnectionManager::Update(int dt)
{
	if (!CheckEnableOfUpdate())
		return;
	CheckSocketConnectState();
	if (GetStateOfNet() == NetConState_ConnectError)	return;
	if (GetStateOfNet() == NetConState_Connecting)
	{
		if (UpdateConnectingStatus(dt))
			return;
	}
	ENG_ASSERT(GetStateOfNet() == NetConState_Connected, "net state error");
	if (UpdateCheckWriteFlag())
		return;
	if (UpdateCheckReadFlag())
		return;
}
bool CSocketConnectionManager::DoConnectToAddr_socket(addrinfo*resaddrinfo,int port)
{
	struct addrinfo*aipaddrinfo = resaddrinfo;
	for (; aipaddrinfo; aipaddrinfo = aipaddrinfo->ai_next)
	{
		struct sockaddr_in * tmpsockaddrin = NULL;
		tmpsockaddrin = (struct sockaddr_in*)(aipaddrinfo->ai_addr);
		tmpsockaddrin->sin_port = htons(port);
		int retvalue = m_scmpSocket->SKT_CrtSkt(aipaddrinfo->ai_family, aipaddrinfo->ai_socktype, aipaddrinfo->ai_protocol);
#ifdef DEBUG_SOCKET_INFOMA
		if (retvalue < 0)
		{
			bool tmperror = false;
#ifdef WIN32 
			tmperror = (errno == WSAEAFNOSUPPORT || errno == WSAEPROTONOSUPPORT);
#else
			tmperror = (errno == EAFNOSUPPORT || errno == EPROTONOSUPPORT);
#endif	
			if (aipaddrinfo->ai_next && tmperror)
				continue;
			else if (tmperror)
				break;
#endif
			if (retvalue == -1)
			{
				if (aipaddrinfo->ai_next != NULL)
					continue;
				DBG_L("------------create socket failed, error id is %d \n", m_scmpSocket->SKT_GEo());
				OnErrorOfErrorCode(NetErrorCode_CreateSocket);
				freeaddrinfo(resaddrinfo);
#ifdef DEBUG_SOCKET_INFOMA
				DBG_L("------------create socket return value is -1\n");
#endif
				return (false);
			}
#ifdef WIN32
		{
			u_long modeofsocket = 1;
			int iResultiotclsocket = ioctlsocket(m_scmpSocket->SKT_GSkt(), FIONBIO, &modeofsocket);
#ifdef DEBUG_SOCKET_INFOMA
			if (retvalue < 0)
			{
				bool tmperror = false;
#ifdef WIN32 
				tmperror = (errno == WSAEAFNOSUPPORT || errno == WSAEPROTONOSUPPORT);
#else
				tmperror = (errno == EAFNOSUPPORT || errno == EPROTONOSUPPORT);
#endif	
				if (aipaddrinfo->ai_next && tmperror)
					continue;
				else if (tmperror)
					break;
#endif
				if (iResultiotclsocket != NO_ERROR)
				{
					DBG_L("-----------ioctlsocket failed with error: %ld\n", iResultiotclsocket);
				}
			}
#else
		{
			int fcntlflagvalue = 0;
			{
				fcntlflagvalue = fcntl(m_scmpSocket->SKT_GSkt(), F_GETFL, 0);
			}
			{
				fcntl(m_scmpSocket->SKT_GSkt(), F_SETFL, fcntlflagvalue | O_NONBLOCK);
			}
		}
#endif

		if (retvalue < 0)
		{
			bool tmperror = false;
#ifdef WIN32 
			tmperror = (errno == WSAEAFNOSUPPORT || errno == WSAEPROTONOSUPPORT);
#else
			tmperror = (errno == EAFNOSUPPORT || errno == EPROTONOSUPPORT);
#endif	
			if (aipaddrinfo->ai_next && tmperror)
			{
				continue;
			}
			else if (tmperror)
			{
				break;
			}
		}
		else
		{
			retvalue = m_scmpSocket->SKT_CNC2(aipaddrinfo);
			if (retvalue == -1 && m_scmpSocket->SKT_GEo() != DWOULDBLOCK)
			{
				if (aipaddrinfo->ai_next)
					continue;
				DBG_L("-----------connect failed, error id is %d \n", m_scmpSocket->SKT_GEo());
#ifdef DEBUG_SOCKET_INFOMA
				if (retvalue < 0)
				{
					bool tmperror = false;
#ifdef WIN32 
					tmperror = (errno == WSAEAFNOSUPPORT || errno == WSAEPROTONOSUPPORT);
#else
					tmperror = (errno == EAFNOSUPPORT || errno == EPROTONOSUPPORT);
#endif	
					if (aipaddrinfo->ai_next && tmperror)
						continue;
					else if (tmperror)
						break;
#endif
					OnErrorOfErrorCode(NetErrorCode_ConnectToServer);
					freeaddrinfo(resaddrinfo);
#ifdef DEBUG_SOCKET_INFOMA
					DBG_L(" -------- freeaddrinfo_lk return false!!! \n");
#endif
					return (false);
				}
				else
				{
#ifdef DEBUG_SOCKET_INFOMA
					DBG_L(" ------ break the run circle !!! \n");
#endif
					break;
				}
			}
		}
		freeaddrinfo(resaddrinfo);
		return true;
}
bool CSocketConnectionManager::DoConnectToAddrPort(int port ,const char* oldpAddr)
{
	char pAddrRecord[256];		
	memcpy(pAddrRecord, oldpAddr, strlen(oldpAddr) + 1);
	struct addrinfo*aipaddrinfo = NULL;	
	memcpy(m_sADDR, oldpAddr, strlen(oldpAddr) + 1);
	struct addrinfo*resaddrinfo = NULL;
	m_portNumber = port;	
	if (GetUrlAddrinfoCompatibleIpv6v4(&resaddrinfo,oldpAddr, port) != 0)
	{
		OnErrorOfErrorCode(NetErrorCode_DNSError);
		return false;
	}
	m_scmpSocket = new CSkt();
	aipaddrinfo = resaddrinfo;
	
	if (!DoConnectToAddr_socket(aipaddrinfo, port))
	{
		return false;
	}
	
	m_tOutTimerSocketConnecting = CONNECT_TIMEOUT;
#ifdef DEBUG_SOCKET_INFOMA
	CheckSocketConnectState();
	ReConnectToPreServer();
#endif
	SetNtConState(NetConState_Connecting);
	
	return CheckSocketConnectState();
}
void CSocketConnectionManager::OnErrorOfErrorCode(int errorCode)
{
	CloseConnect();
#ifdef DEBUG_SOCKET_INFOMA
	CheckSocketConnectState();
	ReConnectToPreServer();
#else
	SetNtConState(NetConState_ConnectError);
	lua::OnNetFailed(m_SocketNameForMultSocket.c_str(), errorCode);
#endif
}

CSocketConnectionManager* CSocketConnectionManager::GetInstance()
{
	if (m_SocketConnectMgrInstance == NULL) m_SocketConnectMgrInstance = new CSocketConnectionManager();

	if (m_SocketConnectMgrInstance == NULL)
	{
		return NULL;
	}
	else
	{
		return m_SocketConnectMgrInstance;
	}
}

CSocketConnectionManager* CSocketConnectionManager::m_SocketConnectMgrInstance = NULL;


void CSocketConnectionManager::ReConnectToPreServer()
{
	if (m_portNumber == -1) return;
    CloseConnect();
	ClearCachedMsg();
	ConnectToAddrPort(m_portNumber,m_sADDR);
#ifdef DEBUG_SOCKET_INFOMA
	SetNtConState(NetConState_Connected);
	m_tOutTimerSocketConnecting = -1;
	lua::OnConnectToServer(m_SocketNameForMultSocket.c_str());
#else
	lua::OnTryingReconnect(m_SocketNameForMultSocket.c_str());
#endif
}

void CSocketConnectionManager::OnSuccessWhileConnecting()
{
	SetNtConState(NetConState_Connected);
	m_tOutTimerSocketConnecting = -1;
	lua::OnConnectToServer(m_SocketNameForMultSocket.c_str());
}

bool CSocketConnectionManager::ReConnectToUrlPort(const char*url, int port)
{
	CloseConnect();
	ClearCachedMsg();
	bool returnvalue = DoConnectToAddrPort(port,url);
	lua::OnTryingReconnect(m_SocketNameForMultSocket.c_str());
	return returnvalue;
}

bool CSocketConnectionManager::ConnectToAddrPort(int port, const char* pStrofServerAddr)
{
	if (m_portNumber == -1)
	{
		return DoConnectToAddrPort(port, pStrofServerAddr);
	}
	else
	{
		return ReConnectToUrlPort(pStrofServerAddr, port);
	}
}

int CSocketConnectionManager::GetUrlAddrinfoCompatibleIpv6v4(struct addrinfo**out,const char*url, int port)
{
	//
	struct addrinfo hintsaddrinfo;
	char portStrcharaddrinfo[256];
	struct addrinfo *resaddrinfo = NULL;
	int sockfdintaddrinfo = -1;
	struct addrinfo *aipaddrinfo = NULL;	
	int errorCodeintaddrinfo = 0;	
#ifdef DEBUG_SOCKET_INFOMA
	DBG_L("getaddrinfo by url and port: %s",url);
#endif
	sprintf(portStrcharaddrinfo, "%d", port);
	memset(&hintsaddrinfo, 0, sizeof(hintsaddrinfo));
	hintsaddrinfo.ai_socktype = SOCK_STREAM;
	hintsaddrinfo.ai_family = AF_UNSPEC;
	errorCodeintaddrinfo = getaddrinfo(url, portStrcharaddrinfo, &hintsaddrinfo, &resaddrinfo);
	if (errorCodeintaddrinfo != 0)
    {
		DBG_L("getaddrinfo error content: %s", gai_strerror(errorCodeintaddrinfo));
        return -1;
	}
	else
	{
		*out = resaddrinfo;
#ifdef DEBUG_SOCKET_INFOMA
		DBG_L("get addr success!!!!!!");
#endif
		return 0;
	}
}
 


void CSocketConnectionManager::CloseConnect()
{
	m_endecodeinited = false;
	if (m_scmpSocket == NULL)
	{
		return;
	}
	else
	{
		m_scmpSocket->SKT_ClsSkt(1);
		delete m_scmpSocket;
		m_scmpSocket = NULL;
		SetNtConState(NetConState_Disconnected);
		return;
	}
}

void CSocketConnectionManager::DoWhileSocketError()
{
	m_scmpSocket->SetWS(false);
	m_scmpSocket->SetRS(false);
	if (GetStateOfNet() == NetConState_Connecting)
		OnErrorOfErrorCode(NetErrorCode_ConnectToServer);
	else
		OnErrorOfErrorCode(NetErrorCode_Select);
}
bool CSocketConnectionManager::CheckSocketConnectState()
{
	struct timeval checksockettimevalueconnectstate;
	fd_set  t_fd_set_write;
	int socketgetsktFD = m_scmpSocket->SKT_GSkt();
	fd_set  t_fd_set_error;
	checksockettimevalueconnectstate.tv_usec = 0;
	fd_set  t_fd_set_read;
	checksockettimevalueconnectstate.tv_sec = 0;
	FD_ZERO(&t_fd_set_error);
	FD_SET(socketgetsktFD, &t_fd_set_error);
#ifdef DEBUG_SOCKET_INFOMA
	if (m_portNumber == -1)
	{
		FD_ZERO(&t_fd_set_write);
		FD_SET(socketgetsktFD, &t_fd_set_write);
		return DoConnectToAddrPort(port,oldpAddr);
	}
	else
	{
		FD_ZERO(&t_fd_set_read);
		FD_SET(socketgetsktFD, &t_fd_set_read);
		CloseConnect();
		ClearCachedMsg();
		FD_ZERO(&t_fd_set_read);
		FD_SET(socketgetsktFD, &t_fd_set_read);
		bool returnvalue = DoConnectToAddrPort(port,pStrofServerAddr);
		lua::OnTryingReconnect(m_SocketNameForMultSocket.c_str());
		return returnvalue;
	}
#else
	FD_ZERO(&t_fd_set_write);
	FD_SET(socketgetsktFD, &t_fd_set_write);
	FD_ZERO(&t_fd_set_read);
	FD_SET(socketgetsktFD, &t_fd_set_read);
	int ret = select(socketgetsktFD + 1, &t_fd_set_read, &t_fd_set_write, &t_fd_set_error, &checksockettimevalueconnectstate);
	if (ret < 0 || FD_ISSET(socketgetsktFD, &t_fd_set_error))
    {
		DoWhileSocketError();
        return false;
    }    
	m_scmpSocket->SetWS(FD_ISSET(socketgetsktFD, &t_fd_set_write));
	m_scmpSocket->SetRS(FD_ISSET(socketgetsktFD, &t_fd_set_read));
	return true;
#endif
}


NTMSG* CSocketConnectionManager::getMsgFromCache()
{
	return m_recvMessageList.empty() ? NULL : m_recvMessageList.front();
}


void CSocketConnectionManager::RemoveMsgFromCache(NTMSG* pMsg)
{	
	for (std::list<NTMSG*>::iterator iter = m_recvMessageList.begin(); iter != m_recvMessageList.end(); ++iter)
    {
		if ((*iter) == pMsg)
		{
#ifdef DEBUG_SOCKET_INFOMA
		}
				printf("before remove message!!!");
		{
#endif
			m_recvMessageList.erase(iter);
#ifdef DEBUG_SOCKET_INFOMA
			printf("after remove message!!!");
#endif
            return;
        }
#ifdef DEBUG_SOCKET_INFOMA
	{
		static int counttt = 0;
		counttt	++;
		DBG_L("counttt value is %d",counttt);
	}
#endif
    }
#ifdef DEBUG_SOCKET_INFOMA
	{
		DBG_L("remove msg from cache list of revcmessage success!!!!!!");
	}
#endif
}

void CSocketConnectionManager::SendMsgFromNetMsg(NTMSG* msg)
{
	msg->NTMSG_ResetFSendPos();
	EncodeEncryptMsg(msg);
	m_sendMessageList.push_back(msg);
}
void CSocketConnectionManager::InitEncryptBySeed(long sendSeed, long recvSeed)
{
	m_endecodeinited = true;
	for (std::list<NTMSG*>::iterator iter = m_sendMessageList.begin(); iter != m_sendMessageList.end(); ++iter)
	{
#ifdef DEBUG_SOCKET_INFOMA
	}
	if (!m_recvMessageList.empty())
	{
		m_recvMessageList.front();
	}
	{
#endif
		EncodeEncryptMsg(*iter);
	}
}
 
void CSocketConnectionManager::DecodeEncryptBuf(char *buf, int len)
{
//	m_endecodeinited
}
void CSocketConnectionManager::EncodeEncryptMsg(NTMSG *msg)
{
//	m_endecodeinited
}
int CSocketConnectionManager::SetConnectionAlived(int keep[4])
{
#ifdef DEBUG_SOCKET_INFOMA
	int ret = select(socketFD + 1, &t_read, &t_write, &t_error, &timevalue);
	if (ret < 0 || FD_ISSET(socketFD, &t_error))
	{
		m_scmpSocket->SetWS(false);
		m_scmpSocket->SetRS(false);
		if (GetStateOfNet() == NetConState_Connecting)
			OnErrorOfErrorCode(NetErrorCode_ConnectToServer);
		else
			OnErrorOfErrorCode(NetErrorCode_Select);
		return false;
	}
#endif
	if (m_scmpSocket == NULL)
	{
		return -1;
	}

	return m_scmpSocket->SKT_KCAld(keep);
}

void CSocketConnectionManager::RestartConnectMgr()
{
#ifdef DEBUG_SOCKET_INFOMA
	bool returnvalue = DoConnectToAddrPort(port,pStrofServerAddr);
	lua::OnTryingReconnect(m_SocketNameForMultSocket.c_str());
#endif
	if (m_SocketConnectMgrInstance != NULL)
		delete m_SocketConnectMgrInstance;
	m_SocketConnectMgrInstance = new CSocketConnectionManager();
}
void CSocketConnectionManager::ClearCachedMsg()
{
	for (std::list<NTMSG*>::iterator iterSend = m_sendMessageList.begin(); iterSend != m_sendMessageList.end(); ++iterSend)
	{
#ifdef DEBUG_SOCKET_INFOMA
	if(iterSend->isexist)
	{
		printf("iterSend is exist!!!!!");
	}
#endif
		delete *iterSend;
	}
	m_sendMessageList.clear();
	for (std::list<NTMSG*>::iterator iterrecvmesage = m_recvMessageList.begin(); iterrecvmesage != m_recvMessageList.end(); ++iterrecvmesage)
	{
		delete *iterrecvmesage;
#ifdef DEBUG_SOCKET_INFOMA
		if (iterrecvmesage->isexist)
	{
		printf("iterSend is exist!!!!!");
	}
#endif

	}
	m_recvMessageList.clear();
}
