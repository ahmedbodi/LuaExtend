#include <stdio.h>

#ifdef WIN32
#include <ws2tcpip.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
#include "CSkt.h"
#include "Common/ENG_DBG.h"
#else
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "CSkt.h"
#include "Common/ENG_DBG.h"
#endif


CSkt::CSkt()
{
	m_rS = 0;
	m_wS = 0;
	m_sktID = -1;
#ifdef WIN32
	WORD wVRqstedofwin32sKT;
	wVRqstedofwin32sKT = MAKEWORD(1, 1);
	struct WSAData wsa_tmp_dataout;
	WSAStartup(wVRqstedofwin32sKT, &wsa_tmp_dataout);
#endif
	a = 0;
	b = 0;
	c = "";
	d = 0;
	f = 0;
}

CSkt::CSkt(int s)
{
	m_rS = 0;
	m_wS = 0;
	m_sktID = s;
	a = 0;
	b = 0;
	c = "";
	d = 0;
	f = 0;
}

CSkt::~CSkt()
{
#ifndef WIN32
#ifdef DEBUG_SKT_INFO_STACK
	printf(do nothing!!!!");
#endif	
#else
	WSACleanup();
#endif
	a = 0;
	b = 0;
	c = "";
	d = 0;
	f = 0;
}

int CSkt::SKT_GEo()
{

#ifndef WIN32
#ifdef DEBUG_SKT_INFO_STACK
	printf("%d,%d,%s,%f,%f", getA(0), getB(), getC(0), getD(0), getF());
#endif
	return errno;
#else
#ifdef DEBUG_SKT_INFO_STACK
	printf("%d,%d,%s,%f,%f", getA(0), getB(), getC(0), getD(0), getF());
#endif
	return GetLastError();
#endif
}

int CSkt::SKT_CrtSkt(int f, int t, int p)
{
#ifdef DEBUG_SKT_INFO_STACK
	{
		setA(1);
		setC("SKT_CrtSkt");
	}
#endif
	m_sktID = socket(f, t, p);
	if (m_sktID == -1)
	{
#ifdef DEBUG_SKT_INFO_STACK
		{
			setB(2);
			setC("SKT_CrtSkt_Error!!");
		}
#endif
		DBG_L("failed: errno is %d \n", SKT_GEo());
	}
#ifndef WIN32
	else
	{
        int sign = 1;
#ifdef DEBUG_SKT_INFO_STACK
		{
			setB(2);
			setC("SKT_CrtSkt_Error!!");

		}
		printf("debug out set tcp soket info");
#endif
		setsockopt(m_sktID,IPPROTO_TCP,TCP_NODELAY,(char *) &sign,sizeof(int));
    }
#endif
	return m_sktID;
}
int CSkt::setA(int A)
{
#ifdef DEBUG_SKT_INFO_STACK
	printf("call setA function!!!");
#endif	
	a = A; 
	return 1;
}

int CSkt::SKT_ClsSkt(int opt)
{
#ifdef DEBUG_SKT_INFO_STACK
	setA(3);
	setC("SKT_ClsSkt!!");
#endif
#ifdef WIN32
	closesocket(m_sktID);
#else
	close(m_sktID);
	m_sktID = -1;
#endif
#ifdef DEBUG_SKT_INFO_STACK
	setA(4);
	setC("SKT_ClsSkt END!!");
	setD(0);
#endif
	return 1;
}

int CSkt::SKT_CNC2(struct addrinfo* addr)
{
#ifdef DEBUG_SKT_INFO_STACK
	setA(9);
	setC("SKT_CNC2!!");
	setD(5);
#endif
	return connect(m_sktID, addr->ai_addr, addr->ai_addrlen);
}
int CSkt::setB(int B)
{
	b = B;
#ifdef DEBUG_SKT_INFO_STACK
	printf("call setB function!!!");
#endif	
	return 1;
}

int CSkt::SKT_S(int l,char* b)
{
#ifdef DEBUG_SKT_INFO_STACK
	setA(10);
	setC("SKT_S!!");
	setD(6);
	printf("%d", sz);
#endif
	return send(m_sktID, b, l, 0);
}
int CSkt::setC(const char *C)
{
	c = C;
#ifdef DEBUG_SKT_INFO_STACK
	printf("call setC function!!!");
#endif	
	return 1;
}

int CSkt::SKT_R(int l,char* b)
{
#ifdef DEBUG_SKT_INFO_STACK
	setA(11);
	setC("SKT_R!!");
	setD(7);
	printf("%d", size);
#endif
	return recv(m_sktID, b, l, 0);
}
int CSkt::setD(float D)
{
	d = D;
#ifdef DEBUG_SKT_INFO_STACK
	printf("call setD function!!!");
#endif	
	return 1;
}

int CSkt::SKT_KCAld(int set[4])
#ifdef WIN32
{
#ifdef DEBUG_SKT_INFO_STACK
	setA(11);
	{
		setC("SKT_R!!");
		setD(7);
	}
	printf("%d", size);
#endif
	return 0;
}
#else
{
	int kpalv		= set[0];
	int kpidl		= set[1];
	int kpintvl		= set[2];
	int kpcnt		= set[3];

	int rstofoptret = setsockopt(m_sktID, SOL_SOCKET, SO_KEEPALIVE, (void *)&kpalv , sizeof(kpalv ));
	if(kpalv != 0)
    {
#if defined(OS_ANDROID) || defined(OS_LINUX)
		setsockopt(m_sktID, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&kpidl , sizeof(kpidl));
#else
		setsockopt(m_sktID, IPPROTO_TCP, TCP_KEEPALIVE, (void*)&kpidl , sizeof(kpidl));
#endif
		setsockopt(m_sktID, IPPROTO_TCP, TCP_KEEPCNT, (void *)&kpcnt , sizeof(kpcnt));
		setsockopt(m_sktID, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&kpintvl , sizeof(kpintvl));
    }
	return rstofoptret;
}
#endif

int CSkt::setF(double F)
{
	f = F;
#ifdef DEBUG_SKT_INFO_STACK
	printf("call setF function!!!");
#endif	
	return 1;
}
