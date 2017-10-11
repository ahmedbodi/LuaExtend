
#include <string>
#include <jni.h>
#include <android/log.h>
#include <AndroidCommon.h>
#include <AndroidUtils.h>
#include "Common.h"
#include "lua_extensions.h"
#include "LuaInterface.h"
#include "TextInput.h"

static jclass jclassTextInput = NULL;
static jmethodID jmethodConstructor = NULL;
static jmethodID jmethodShow = NULL;
static jmethodID jmethodHide = NULL;
static jmethodID jmethodSetPos = NULL;
static jmethodID jmethodSetMaxLength = NULL;
static jmethodID jmethodGetText = NULL;
static jmethodID jmethodAddText = NULL;
static jmethodID jmethodSetText = NULL;
static jmethodID jmethodRelease = NULL;

static jmethodID jmethodGetTextLines = NULL;
static jmethodID jmethodGetTextLineHeight = NULL;

static bool initJNI(JNIEnv* env, jclass cls) {
    if (jmethodRelease != NULL) {
        return true;
    }

    if (env == NULL) {        
        return false;
    }

    jclassTextInput = (jclass) env->NewGlobalRef(cls);
    if (jclassTextInput == NULL) { 
        return false;
    }
    jmethodConstructor = env->GetMethodID(jclassTextInput, "<init>",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIIIIIZZ)V");
    if (jmethodConstructor == NULL) {
        LOGE("Get jmethodConstructor failed");
        return false;
    }

    jmethodSetPos = env->GetMethodID(jclassTextInput, "setPos", "(IIII)V");
    if (jmethodSetPos == NULL) {
        LOGE("Get jmethodSetPos failed");
        return false;
    }
    jmethodGetText = env->GetMethodID(jclassTextInput, "getText", "()Ljava/lang/String;");
    if (jmethodGetText == NULL) {
        LOGE("Get jmethodGetText failed");
        return false;
    }

    jmethodSetMaxLength = env->GetMethodID(jclassTextInput, "setMaxLength", "(I)V");
    if (jmethodSetMaxLength == NULL) {
        LOGE("Get jmethodSetMaxLength failed");
        return false;
    }


    jmethodAddText = env->GetMethodID(jclassTextInput, "addText", "(Ljava/lang/String;)V");
    if (jmethodAddText == NULL) {
        LOGE("Get jmethodAddText failed");
        return false;
    }

 
    jmethodShow = env->GetMethodID(jclassTextInput, "show", "()V");
    if (jmethodShow == NULL) {
        LOGE("Get jmethodShow failed");
        return false;
    }
    jmethodSetText = env->GetMethodID(jclassTextInput, "setText", "(Ljava/lang/String;)V");
    if (jmethodSetText == NULL) {
        LOGE("Get jmethodSetText failed");
        return false;
    }
    jmethodHide = env->GetMethodID(jclassTextInput, "hide", "()V");
    if (jmethodHide == NULL) {
        LOGE("Get jmethodHide failed");
        return false;
    }
    jmethodGetTextLines = env->GetMethodID(jclassTextInput, "getTextLines", "()I");
    if(jmethodGetTextLines == NULL)
    {
        LOGE("Get jmethodGetTextLines failed");
        return false;   
    }
    jmethodGetTextLineHeight = env->GetMethodID(jclassTextInput, "getTextLineHeight", "()I");
    if(jmethodGetTextLineHeight == NULL)
    {
        LOGE("Get jmethodGetTextLineHeight failed");
        return false;   
    }
    jmethodRelease = env->GetMethodID(jclassTextInput, "release", "()V");
    if (jmethodRelease == NULL) {
        LOGE("Get jmethodRelease failed");
        return false;
    }
    return true;
}



extern TextInput *g_inputtext;
extern "C" void jniCall_InputBox_BeginEditing()
{
    g_inputtext->BeginEditing();
}
extern "C" void jniCall_InputBox_DidChange()
{
    g_inputtext->DidChange(g_inputtext->GetText());
}
extern "C" void jniCall_InputBox_EndEditing()
{
    g_inputtext->EndEditing(g_inputtext->GetText());
}

#define INPUT_FUNCTION(function) Java_com_harry_engine_##function

extern "C" {
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_initJNI)(JNIEnv * env, jclass cls);
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniBeginEditing) (JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniDidChange) (JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniEndEditing) (JNIEnv * env, jobject obj);    
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniNotifyKeyboardShown)(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniNotifyKeyboardHidden)(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_releaseJNI)(JNIEnv * env, jobject obj);
}

JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_initJNI)(JNIEnv * env, jclass cls) {
    if (!initJNI(env, cls)) {
        LOGE("initJNI failed");
    }
}

JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniBeginEditing)(JNIEnv * env, jobject obj) {
    jniCall_InputBox_BeginEditing();
}
 

JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniDidChange)(JNIEnv * env, jobject obj) {
    jniCall_InputBox_DidChange();
}

JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniEndEditing)(JNIEnv * env, jobject obj) {  
    jniCall_InputBox_EndEditing();
}


extern void setKeyboardSize(int width, int height);
extern void notificationKeyboardHiden();
extern bool notifyKeyboardReturnPress();

JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniNotifyKeyboardShown)(JNIEnv * env, jobject obj, jint width,
        jint height) {
    LOGD("TextInput_jniNotifyKeyboardShown");
    RECORD_GET_LUA_SDK(_L);
    setKeyboardSize(width,height);
    RECOVER_SVD_LUA_SDK(_L, 0);
}

JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_jniNotifyKeyboardHidden)(JNIEnv * env, jobject obj) {
    LOGD("TextInput_jniNotifyKeyboardHidden");
    RECORD_GET_LUA_SDK(_L);
    notificationKeyboardHiden();
    RECOVER_SVD_LUA_SDK(_L, 0);
}
 
JNIEXPORT void JNICALL INPUT_FUNCTION(TextInput_releaseJNI)(JNIEnv * env, jobject obj) {
    env->DeleteGlobalRef(jclassTextInput);
    jclassTextInput = NULL;
}








extern int g_returnpressfunction;
extern InputCSharpCallBack g_callbackfunction;

TextInput::TextInput():m_InputBox(NULL)
{
    m_id = -1;
}

TextInput::~TextInput()
{
    m_id = -1;
	this->Destroy();
    if(m_InputBox != NULL)
    {
        JNIEnv*env = AndroidGetJavaEnv();
        env->CallVoidMethod(m_InputBox,jmethodRelease);
        env->DeleteGlobalRef(m_InputBox);
        m_InputBox = NULL;
    }
}
int g_enableUseCompatibleModeNoMove = 0;
int g_enableEditTopFirst = 1;


void TextInput::Init(const char * content,int multline,int x,int y,int w,int h,int alpha,int fontsize,int fontcolor,const char * inputboxtype)
{
    size_t len;
    const char* typeStr = "TextField";
    if(multline == 1)
        typeStr = "TextView";
    
    if ( content == NULL )
        content = "";
    int enable = 1;
    const char* keyboardType = NULL;
    if ( inputboxtype != NULL )
        keyboardType = inputboxtype;

    g_returnpressfunction = 0;
  
    JNIEnv* env = AndroidGetJavaEnv();
    jstring jsContent = env->NewStringUTF(content);
    jstring jsKbType = env->NewStringUTF(keyboardType);
    jstring jsType = env->NewStringUTF(typeStr);
     
    jobject joinput = env->NewObject(jclassTextInput, jmethodConstructor, jsType, jsKbType, jsContent, x, y,
            w, h, enable, alpha, fontsize, fontcolor, g_enableUseCompatibleModeNoMove != 0, g_enableEditTopFirst != 0);
    m_InputBox = env->NewGlobalRef(joinput);
    env->DeleteLocalRef(jsContent);
    env->DeleteLocalRef(jsKbType);
    env->DeleteLocalRef(jsType);
    env->DeleteLocalRef(joinput);    
}


void TextInput::SetPos(int x,int y,int w,int h)
{     
    LOGD("TextInput::SetPos\n");    
    JNIEnv* env = AndroidGetJavaEnv();
    env->CallVoidMethod(m_InputBox, jmethodSetPos, x, y, w, h);
}

void TextInput::SetMaxLength(int len)
{    
    LOGD("TextInput::SetMaxLength\n");
    if(m_id == -1) return;
    JNIEnv* env = AndroidGetJavaEnv();
    env->CallVoidMethod(m_InputBox, jmethodSetMaxLength, len);
}



extern char* getJniRetBuff(int size);
const char * TextInput::GetText()
{
    if(m_id == -1) return "";
    JNIEnv* env = AndroidGetJavaEnv();
    jstring jsText = (jstring) env->CallObjectMethod(m_InputBox, jmethodGetText);

    const char* text = env->GetStringUTFChars(jsText, NULL);
    char * buf = NULL;    
    buf = getJniRetBuff(strlen(text) + 2);
    strcpy(buf,text);    
    env->ReleaseStringUTFChars(jsText, text);
    env->DeleteLocalRef(jsText);
    return buf;
}

int TextInput::GetTextLines()
{
    if(m_id == -1) return 1;
    JNIEnv* env = AndroidGetJavaEnv();
    jint lineNum = env->CallIntMethod(m_InputBox, jmethodGetTextLines);
    //jint lineHeight = env->CallIntMethod(mInput, jmethodGetTextLineHeight);
    return lineNum;
}

void TextInput::AddText(const char * txt)
{   
    if(m_id == -1) return;
    const char* text = txt;
    JNIEnv* env = AndroidGetJavaEnv();
    jstring jsText = env->NewStringUTF(text);
    env->CallVoidMethod(m_InputBox, jmethodAddText, jsText);
    env->DeleteLocalRef(jsText);
}

void TextInput::SetText(const char * txt)
{
    LOGD("TextInput::SetText\n");
    if(m_id == -1) return;
    const char* text = txt;
    JNIEnv* env = AndroidGetJavaEnv();
    jstring jsText = env->NewStringUTF(text);
    env->CallVoidMethod(m_InputBox, jmethodSetText, jsText);
    env->DeleteLocalRef(jsText);
}
int TextInput::GetTextLineHeight()
{
    if(m_id == -1) return 22;
    JNIEnv* env = AndroidGetJavaEnv();
    //jint lineNum = env->CallIntMethod(mInput, jmethodGetTextLines);
    jint lineHeight = env->CallIntMethod(m_InputBox, jmethodGetTextLineHeight);
    return lineHeight;
}

void TextInput::SetAlign(const char * txt)
{
    if(m_id == -1) return;
}
void TextInput::BeginEditing()
{
    //TODO do something whild begin editing
    LOGD("TextInput::BeginEditing()\n");
    char tmp[512];
    sprintf(tmp,"%d",m_id);
    std::string t = tmp;
    t+= "+++++BeginEditing";
    t+= "+++++";
    g_callbackfunction(t.c_str());
}

void TextInput::DidChange(const char * txt)
{
   //TODO do something whild begin editing
    LOGD("TextInput::DidChange()\n");
    char tmp[512];
    sprintf(tmp, "%d", m_id);
    std::string t = tmp;
    t += "+++++DidChange";
    t += "+++++";
    t += txt;
    g_callbackfunction(t.c_str());
}

void TextInput::EndEditing(const char *txt)
{
    //TODO do something whild begin editing
    LOGD("TextInput::EndEditing()\n");
    char tmp[512];
    sprintf(tmp, "%d", m_id);
    std::string t = tmp;
    t += "+++++EndEditing";
    t += "+++++";
    t += txt;
    g_callbackfunction(t.c_str());

    // do command for keyboard hiden
    // Hide();
}
void TextInput::Show()
{
    LOGD("TextInput::Show\n");    
    JNIEnv* env = AndroidGetJavaEnv();
    env->CallVoidMethod(m_InputBox, jmethodShow);
}

void TextInput::Hide()
{
    LOGD("TextInput::Hide\n");  
    JNIEnv* env = AndroidGetJavaEnv();
    env->CallVoidMethod(m_InputBox, jmethodHide);
}
void TextInput::SetKeyboardReturnShow(int type)
{

}
void TextInput::SetKeyboardReturnPressFunction(int hideornone)
{
    g_returnpressfunction = hideornone;
}
void TextInput::Destroy()
{
    LOGD("TextInput::Destroy()\n");
    if (m_id != -1)        
    {
        EndEditing(GetText());
    //    Hide();
        notificationKeyboardHiden();
    }
    m_id = -1;
    if(m_InputBox != NULL)
    {
        JNIEnv*env = AndroidGetJavaEnv();
        env->CallVoidMethod(m_InputBox,jmethodRelease);
        env->DeleteGlobalRef(m_InputBox);
        m_InputBox = NULL;
    }
}


extern "C" void EXPORT_API SetKeyboardUseCompatibleMode(int enable)
{
    g_enableUseCompatibleModeNoMove = enable;
}
extern "C" void EXPORT_API SetKeyboardEditTopFirst(int enable)
{
    g_enableEditTopFirst = enable;
}
extern "C" void HideKeyboard(){}
#include "TextInputCommon.hpp"