#include "stdafx.h"
#include "TxtMgr.h"
#include "IO/CFSys.h"
#include <stdlib.h>
#include <ctime>
using namespace std;

// ////////////////////////////////////////////////////////////////////////////////////////////
TxtMgr::TxtMgr()
{
	m_LT = "english";
}

TxtMgr::~TxtMgr()
{
	release();
}

TxtMgr* TxtMgr::Inst()
{
	static TxtMgr* i = NULL;
	if(!i)
	{
		i = MARC_NEW TxtMgr;
	}
	return i;
}

