#include "stdafx.h"
#include "CMemToFile.h"

CMemToFile::CMemToFile(const char *a, int b, const char *c)
	: FileBaseStream(CEFilePtr(), ESM::FAM_READ | ESM::FAM_WRITE)
	, MemStream(a, b, true)
{
	m_fn = c;
	setMode(ESM::FAM_READ | ESM::FAM_WRITE);
}

