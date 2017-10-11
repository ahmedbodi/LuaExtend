#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <vector>
#include "GameApp.h"
#include "TimeProfiler.h"


#if defined(OS_ANDROID)
#include <android/log.h>
#elif defined(_WIN32)
#include <windows.h>
#endif
using namespace std;
struct CCodeProfItem
{
	long t;
	string tag;	
	int d;
};
vector<CCodeProfItem*> CCodeProResultVector;
vector<string> CCodeProStackVector;

CCodeProf::CCodeProf(const char * s)
{
	t = GameApp::getInstance()->getSysTime();
	strcpy(b, s);	
	CCodeProStackVector.push_back(s);
}
CCodeProf::~CCodeProf()
{
	CCodeProfItem*a = new CCodeProfItem();
	a->d = CCodeProStackVector.size() - 1;
	a->t = GameApp::getInstance()->getSysTime() - t;
	a->tag = b;
	CCodeProStackVector.pop_back();
	CCodeProResultVector.push_back(a);
}

void printCCodeProfAllStack()
{
	char b[1023];
	int sz = CCodeProResultVector.size();
	for (int i = 0; i < sz; i++){
		CCodeProfItem * tttttt = CCodeProResultVector[i];
		sprintf(b, "tag: %s, depth: %d, time: %d \n", tttttt->tag.c_str(), tttttt->d, tttttt->t);
		delete tttttt;
		ENG_DBG::DOut(ENG_DBG::CPLUS_L, b);		
	}
	CCodeProResultVector.clear();
}