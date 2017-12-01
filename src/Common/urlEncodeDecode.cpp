#include <stdio.h>
#include <string>
#include <assert.h>
#include <vector>
#include "urlEncodeDecode.h"
#include "GameApp.h"
unsigned char ToHex(unsigned char x)
{
	return x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if(x >='A'&&x <='Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp	+= '%';
			strTemp	+= ToHex((unsigned char)str[i] >> 4);
			strTemp	+= ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i  + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

bool StringSplit(std::string src, std::string pattern, std::vector<std::string>& strVec)
{
	std::string::size_type pos;
	src += pattern;
	int size = src.size();
 
	for (int i = 0; i<size; i++)
	{
		pos = src.find(pattern, i);
		if (pos<size)
		{
			std::string s = src.substr(i, pos - i);
			strVec.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return true;
}
static char urlContentBuf[1024] = {0};
extern "C" void OpenByUrl(const char * uri)
{
	if (uri != NULL && strlen(uri) < 1024)
	{
		memset(urlContentBuf, 0, sizeof(urlContentBuf));
		strcpy(urlContentBuf, uri);
	}
}

extern "C" void DoCommandFromOpenUrl_impl(const char * uri)
{
	std::string tmp = uri;
	int index = tmp.find_last_of("?");
	if (index < 0)
	{
		return;
	}
	std::string params = tmp.substr(index+1);
	std::vector<std::string> paramsList;
	StringSplit(params, "&", paramsList);
	for (int i = 0; i < paramsList.size(); i++)
	{
		std::vector<std::string> kv;
		StringSplit(paramsList[i], "=", kv);
		if (kv.size() == 2)
		{
			if (kv[0].compare("cleanConfigFile") == 0)
			{
				std::string fpn = "";
				fpn = fpn + (GameApp::GetInstance()->getSavePath()) + "config.txt";
				remove(fpn.c_str());
			}
			else if (kv[0].compare("removeSaveFile") == 0)
			{
				std::string fpn = "";
				std::string filename = UrlDecode(kv[1]);
				fpn = fpn + (GameApp::GetInstance()->getSavePath()) + filename;
				remove(fpn.c_str());
			}
			else if (kv[0].compare("removeCacheFile") == 0)
			{
				std::string fpn = "";
				std::string filename = UrlDecode(kv[1]);
				fpn = fpn + (GameApp::GetInstance()->getCachePath()) + filename;
				remove(fpn.c_str());
			}
			else if (kv[0].compare("configfilecontent") == 0)
			{
				std::string fpn = "";
				fpn = fpn + (GameApp::GetInstance()->getSavePath()) + "config.txt";
				std::string filecontent = UrlDecode(kv[1]);
				FILE* fptr = fopen(fpn.c_str(),"w");
				if (fptr != NULL)
				{
					fwrite(filecontent.c_str(), filecontent.size(), 1, fptr);
					fclose(fptr);
				}
			}
		}
	}
}
extern "C" void DoCommandFromOpenUrl()
{
	DoCommandFromOpenUrl_impl(urlContentBuf);
	memset(urlContentBuf, 0, sizeof(urlContentBuf));
}