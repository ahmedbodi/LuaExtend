
#include <string>
#include "TextInput.h"
extern InputCSharpCallBack g_callbackfunction;
TextInput::TextInput()
{
    m_id = -1;
}

TextInput::~TextInput()
{
	this->Destroy();
}

void TextInput::Init(const char * content,int multline,int x,int y,int w,int h,int alpha,int fontsize,int fontcolor,const char * inputboxtype)
{
    const char* typeStr = "TextField";
    if(multline == 1)
        typeStr = "TextView";
    
    if ( content == NULL )
        content = "";
    int enable = 1;
    const char* keyboardType = NULL;
    if ( inputboxtype != NULL )
        keyboardType = inputboxtype;

    Show();
	BeginEditing();
	DidChange("");
}

void TextInput::Destroy()
{
	if (m_id != -1)
		EndEditing("");
	m_id = -1;
}

void TextInput::SetPos(int x,int y,int w,int h)
{ 
}

void TextInput::SetMaxLength(int len)
{
}

void TextInput::Show()
{
 
}

void TextInput::Hide()
{
 
}

const char * TextInput::GetText()
{
    return "";
}

int TextInput::GetTextLines()
{
    return 1;
}
int TextInput::GetTextLineHeight()
{
    return 20;
}

void TextInput::AddText(const char * txt)
{   
}

void TextInput::SetText(const char * txt)
{
}

void TextInput::SetAlign(const char * txt)
{   
}
void TextInput::BeginEditing()
{
    //TODO do something whild begin editing
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
    char tmp[512];
    sprintf(tmp, "%d", m_id);
    std::string t = tmp;
    t += "+++++DidChange";
    t += "+++++inputingsomething";
    g_callbackfunction(t.c_str());
}

void TextInput::EndEditing(const char *txt)
{
    //TODO do something whild begin editing
    char tmp[512];
    sprintf(tmp, "%d", m_id);
    std::string t = tmp;
    t += "+++++EndEditing";
    t += "+++++EndEditingsomething";
    g_callbackfunction(t.c_str());
}
 
void TextInput::SetKeyboardReturnShow(int type){}
void TextInput::SetKeyboardReturnPressFunction(int hideornone){}
extern "C" void HideKeyboard(){}
#include "TextInputCommon.hpp"