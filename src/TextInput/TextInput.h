#ifndef kllaslkjiweoofjjoasdfjio_TextInput_h
#define kllaslkjiweoofjjoasdfjio_TextInput_h
#ifdef TARGET_OS_IPHONE
#include "mykeyboard.h"
#endif
#ifdef OS_ANDROID
#include "jni.h"
#endif
#ifdef WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define	EXPORT_API
#endif
class TextInput
{
public:
    TextInput();
    virtual ~TextInput();
	
private:
#ifdef TARGET_OS_IPHONE
    Keyboard * m_keyboard;
#endif
#ifdef OS_ANDROID
	jobject m_InputBox;
#endif
	int m_id;
public:
	void SetUID(int id){ m_id = id; }
	void SetKeyboardReturnShow(int type);
	void SetKeyboardReturnPressFunction(int hideornone);
    void Init(const char * content,int multline,int x,int y,int w,int h,int alpha,int fontsize,int fontcolor,const char * inputboxtype);
    void Destroy();
    void SetPos(int x,int y,int w,int h);
    void SetMaxLength(int len);
    void Show();
    void Hide();
    const char * GetText();
    void AddText(const char * txt);
    void SetText(const char * txt);
    void SetAlign(const char * txt);
	int GetTextLines();
    int GetTextLineHeight();
    void BeginEditing();
    void DidChange(const char * txt);
    void EndEditing(const char * txt);
};

TextInput * CreateNewTextInput();
#ifndef OS_ANDROID
typedef void(__stdcall *InputCSharpCallBack)(const char * str);
#else
typedef void(*InputCSharpCallBack)(const char * str);
#endif
extern "C" void EXPORT_API SetInputBoxCallBackFromNativeCode(InputCSharpCallBack callback);
#endif
