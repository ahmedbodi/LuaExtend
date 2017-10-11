#ifndef AsdfasdfasdfasdfasdfLE_sdioeieioiowiofjieijfi
#define AsdfasdfasdfasdfasdfLE_sdioeieioiowiofjieijfi

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "ZipData.h"
using namespace std;
class AndroidFile : public ReadFileInt
{
public:
    AndroidFile();
    virtual ~AndroidFile();
    
public:
    virtual bool open(string f);
    virtual int length();
    virtual int read(void* b, int sz);
    virtual void seek(int o, int m);
    virtual void close();
    AAsset* getAsset(){return m_file;}
    size_t getastsize(){return m_exist;}
    bool exist1(){return m_exist;}
    AAsset* m_file;
    size_t m_size;
    bool m_exist;
};
#endif