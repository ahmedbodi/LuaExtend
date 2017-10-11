#include "lua.hpp"
#include "Common/Common.h"
#include "LuaInterface.h"
#ifdef WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif
TextInput *g_inputtext = NULL;
int inputbox_createtimes = 0;
InputCSharpCallBack g_callbackfunction = NULL;
TextInput * CreateNewTextInput()
{
    if(g_inputtext != NULL)
        delete g_inputtext;
    g_inputtext = NULL;
    g_inputtext = new TextInput();
    inputbox_createtimes ++;
    g_inputtext->SetUID(inputbox_createtimes);
    return g_inputtext;
}
extern "C" EXPORT_API void InputBox_CreateNew()
{
    CreateNewTextInput();
}
extern "C" EXPORT_API void InputBox_SetUid(int id)
{
    g_inputtext->SetUID(id);
}

extern "C" EXPORT_API void InputBox_Init(const char * content,int multline,int x,int y,int w,int h,int alpha,int fontsize,int fontcolor,const char * inputboxtype)
{
    g_inputtext->Init(content, multline, x, y, w, h, alpha, fontsize, fontcolor, inputboxtype);
}
extern "C" EXPORT_API void InputBox_SetPos(int x, int y,int w,int h)
{
    g_inputtext->SetPos(x, y, w, h);
}
extern "C" EXPORT_API void InputBox_Destroy()
{
    g_inputtext->Destroy();
}
extern "C" void DestroyKeyboard()
{
	InputBox_Destroy();
}
extern "C" EXPORT_API void InputBox_SetMaxLength(int len)
{
    g_inputtext->SetMaxLength(len);
}
extern "C" EXPORT_API void InputBox_Show()
{
    g_inputtext->Show();
}
extern "C" EXPORT_API void InputBox_Hide()
{
    g_inputtext->Hide();
}
extern "C" EXPORT_API const char * InputBox_GetText()
{
    return g_inputtext->GetText();
}
extern "C" EXPORT_API int InputBox_GetTextLines()
{
    return g_inputtext->GetTextLines();
}
extern "C" EXPORT_API int InputBox_GetTextLineHeight()
{
    return g_inputtext->GetTextLineHeight();
}
extern "C" EXPORT_API void InputBox_SetText(const char * txt)
{
    return g_inputtext->SetText(txt);
}
extern "C" void SetKeyboardContent(const char * txt)
{
    InputBox_SetText(txt);
}
extern "C" EXPORT_API void InputBox_SetAlign(const char * txt)
{
    return g_inputtext->SetAlign(txt);
}

extern "C" EXPORT_API void InputBox_SetKeyboardReturnShow(int type)
{
    g_inputtext->SetKeyboardReturnShow(type);
}
extern "C" EXPORT_API void InputBox_SetKeyboardReturnPressFunction(int hideornone)
{
    g_inputtext->SetKeyboardReturnPressFunction(hideornone);
}

//typedef void(__stdcall *InputCSharpCallBack)(const char * str);
extern "C" void EXPORT_API SetInputBoxCallBackFromNativeCode(InputCSharpCallBack callback)
{
    g_callbackfunction = callback;
}

void setKeyboardSize(int width, int height)
{
    char tmp[512];
    sprintf(tmp, "%d", -1);
    std::string t = tmp;
    t += "+++++setKeyboardSize";
    
    t += "+++++";
    char param[512];
    sprintf(param,"%d_%d",width,height);
    t += param;
    if(g_callbackfunction)
        g_callbackfunction(t.c_str());
    
    // add protection for _L is null when receiving the notification UIKeyboardDidShowNotification message
    if (_L == NULL)
    {
        return;
    }
	RECORD_GET_LUA_SDK(_L);
    
	lua_getglobal(_L, "OnKeyBoardSize");
    
    if(lua_isnil(_L, -1))
    {
        lua_pop(_L, 1);
    }
    else
    {
        lua_pushnumber(_L, width);
        lua_pushnumber(_L, height);
        LUA_CALL(_L, 2, 0);
    }
    
	RECOVER_SVD_LUA_SDK(_L, 0);
}
int g_returnpressfunction = 0; //0 none, 1 hide while return key pressed
extern "C" void HideKeyboard();
bool notifyKeyboardReturnPress()
{
    char tmp[512];
    sprintf(tmp, "%d", -1);
    std::string t = tmp;
    t += "+++++KeyboardReturnPress";
    t += "+++++";
    if(g_callbackfunction)
        g_callbackfunction(t.c_str());
  //  if(g_returnpressfunction == 1)
  //  {
  //      HideKeyboard();
  //  }
    if (_L == NULL)
    {
        return false;
    }
    RECORD_GET_LUA_SDK(_L);
    
    lua_getglobal(_L, "OnKeyBoardReturnPress");
    
    if(lua_isnil(_L, -1))
    {
        lua_pop(_L, 1);
    }
    else
    {
        LUA_CALL(_L, 0, 0);
    }
    
    RECOVER_SVD_LUA_SDK(_L, 0);
	return true;
}

void notificationKeyboardHiden()
{
    char tmp[512];
    sprintf(tmp, "%d", -1);
    std::string t = tmp;
    t += "+++++KeyboardHiden";
    t += "+++++";
    if(g_callbackfunction)
        g_callbackfunction(t.c_str());
    // add protection for _L is null when receiving the notification UIKeyboardDidHideNotification message
    if (_L == NULL)
    {
        return;
    }
	RECORD_GET_LUA_SDK(_L);
    
	lua_getglobal(_L, "OnKeyBoardHiden");
    
    if(lua_isnil(_L, -1))
    {
        lua_pop(_L, 1);
    }
    else
    {
        LUA_CALL(_L, 0, 0);
    }
    
	RECOVER_SVD_LUA_SDK(_L, 0);
}

