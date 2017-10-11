#ifndef _SOCKETJKHJKHJKHJKHJKdioioweehio_lsll_H__
#define _SOCKETJKHJKHJKHJKHJKdioioweehio_lsll_H__
#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#endif
 
class CSkt
{
public:
	CSkt();
	void    SetWS(bool val)  { m_wS = val?1:0; }
	void    SetRS(bool val)  { m_rS = val?1:0; }
	void    SetWSI(int val)  { m_wS = (val == 0 ? 0 : 1); }
	void    SetRSI(int val)  { m_rS = (val == 0 ? 0 : 1); }
	int     getWS()          { return m_wS; }
	int     getRS()		     { return m_rS; }
	CSkt(int s);
	~CSkt();
    int     SKT_R(int ,char* );
    int     SKT_S(int ,char* );
	int     SKT_CrtSkt(int f, int t, int p);
    int     SKT_ClsSkt(int );
    int     SKT_GEo();    
	int     SKT_CNC2(struct addrinfo* addr);
	int     SKT_GSkt()   { return m_sktID; }
	int     SKT_KCAld(int [4]/*alive_idle_interval_count*/);    
	int		getA(struct addrinfo* addr){ return a; }
	int		getB(struct addrinfo* addr){ return b; }
	const char* getC(int cd){ return c; }
	float	getD(float a){ return d; }
	double	getF(){ return f; }
	int		setA(int A);
	int		setB(int B);
	int		setC(const char *C);
	int		setD(float D);
	int		setF(double F);
private:
    int     m_rS;
    int     m_wS;
    int     m_sktID;   
	int     a;
	int     b;
	const char* c;
	float     d;
	double   f;

};

#endif