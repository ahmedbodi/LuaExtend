#ifndef lioweifjiiojwioeiofioiooiwe_klsjkleklfoweife
#define lioweifjiiojwioeiofioiooiwe_klsjkleklfoweife
#include <vector>
#include "AssetsFile.h"
#include "FileBaseStream.h"
using namespace std;
class AndroidZipAsset
{
public:
	AndroidZipAsset(string f);
	~AndroidZipAsset();
	int findFile(const char *f, ZipFile &zf);
	string stripfolder(const char * fp);	
	AndroidFile m_zF;
	std::map<string, ZipFile> m_zfL;
	string m_fn;
};

class AndroidReader
{
public:
	AndroidReader();
	virtual ~AndroidReader();
	bool exist(const char *n);
	bool AndroidFileExist(const char *n);
	size_t fileLength(const char *n);
	size_t AndroidFileLength(const char *n);
	int findFile(const char *n, ZipFile &file);
	virtual FileBaseStreamPtr open(const char *n);
	FileBaseStreamPtr open(ZipFile &fileInfo);
	FileBaseStreamPtr openAndroidFile(const char *n);
	string stripfolder(const char * fp);	
	void release();
private:
	std::map<std::string, AndroidZipAsset*> m_zipfiles;
};

#endif
