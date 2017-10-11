
#include <string>
#include "TextInput.h"
extern int g_returnpressfunction;
UIView* GetGLView();
extern InputCSharpCallBack g_callbackfunction;
TextInput::TextInput():
m_keyboard(nil)
{
    m_id = -1;
}

TextInput::~TextInput()
{
    if ( m_keyboard != nil )
    {
        [m_keyboard release];
        m_keyboard = nil;
    }
}

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

    TextInputType type = InputField;
    if (!strcmp(typeStr, "TextView")) {
        type = InputView;
    }
    CGFloat scale = [GetGLView() contentScaleFactor];
    g_returnpressfunction = 0;
    m_keyboard = [[Keyboard alloc] initWithType:type Frame:CGRectMake((CGFloat)x/scale, (CGFloat)y/scale, (CGFloat)w/scale, (CGFloat)h/scale) Parent:this];
    [m_keyboard setInput:[NSString stringWithCString:content encoding:NSUTF8StringEncoding]];
    [m_keyboard setEnable:enable];
    if ( alpha != -1 )
        [m_keyboard setAlpha:(alpha/255.0f)];
    if ( fontsize != -1 )
        [m_keyboard setFSize:fontsize];
    if ( fontcolor != -1 )
        [m_keyboard setFColor:fontcolor];
    if ( inputboxtype != NULL )
        [m_keyboard setType:inputboxtype];
    Show();
}

void TextInput::Destroy()
{
    [m_keyboard release];
    m_keyboard = nil;
}

void TextInput::SetPos(int x,int y,int w,int h)
{
    CGFloat scale = [GetGLView() contentScaleFactor];
    
    [m_keyboard setPos:CGRectMake((CGFloat)x/scale, (CGFloat)y/scale, (CGFloat)w/scale, (CGFloat)h/scale)];
}

void TextInput::SetMaxLength(int len)
{
    [m_keyboard SetMaxLength:len];
}

void TextInput::Show()
{
    [m_keyboard showKeyboard];
}

void TextInput::Hide()
{
    [m_keyboard hideKeyboard];
}

const char * TextInput::GetText()
{
    return [[m_keyboard getText] UTF8String];
}

int TextInput::GetTextLines()
{
    return [m_keyboard getInputLines];
}
int TextInput::GetTextLineHeight()
{
    return [m_keyboard getInputLineHeight];
}

void TextInput::AddText(const char * txt)
{
    [m_keyboard addInput:[NSString stringWithCString:txt encoding:NSUTF8StringEncoding]];
}

void TextInput::SetText(const char * txt)
{
    [m_keyboard setInput:[NSString stringWithCString:txt encoding:NSUTF8StringEncoding]];
}

void TextInput::SetAlign(const char * txt)
{
    [m_keyboard setAlign:txt];
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
    t += "+++++";
    t += txt;
    g_callbackfunction(t.c_str());
}

void TextInput::EndEditing(const char * txt)
{
    //TODO do something whild begin editing
    char tmp[512];
    sprintf(tmp, "%d", m_id);
    std::string t = tmp;
    t += "+++++EndEditing";
    t += "+++++";
    t += txt;
    g_callbackfunction(t.c_str());
    this->Destroy();
}
 
void TextInput::SetKeyboardReturnShow(int type)
{
    [m_keyboard setReturnKeyType:type];
}
void TextInput::SetKeyboardReturnPressFunction(int hideornone)
{
    //if(hideornone == 1)
        g_returnpressfunction = hideornone;
}
#include "TextInputCommon.hpp"
