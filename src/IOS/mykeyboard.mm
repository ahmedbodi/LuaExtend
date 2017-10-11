#include "mykeyboard.h"
extern "C" {
#include "lua.hpp"
}
#include "TextInput.h"
extern UIView* GetGLView();
int sGlobalViewCount = 0;
Keyboard* g_keyboard = nil;


extern UIView * sGlobalView;



extern "C" void SetGlobalView(UIView * gView){ sGlobalView = gView; }
extern UIView * getGlobalView();
extern void removeGlobalView();
extern int getExtKeyboardTable(lua_State *L);
extern "C" void GlobalViewRotate();
@implementation Keyboard

- (id)initWithType:(TextInputType)type Frame:(CGRect)frame  Parent:(TextInput*)callback
{
    self = [super init];
    m_type = type;
    m_callback = callback;
    UIView * tgView = getGlobalView();
    if (m_type == InputField || m_type == InputKeyboard)
    {
        m_tField = [[UITextField alloc] initWithFrame: frame];
        m_tField.delegate = self;
        m_tField.backgroundColor = [UIColor whiteColor];
        m_tField.keyboardType = UIKeyboardTypeDefault;
        m_tField.autocorrectionType = UITextAutocorrectionTypeYes;
        m_tField.autocapitalizationType = UITextAutocapitalizationTypeNone;
        [tgView addSubview:m_tField];
        [m_tField addTarget:self action:@selector(textFieldEditChanged:) forControlEvents:UIControlEventEditingChanged];
        m_tView = nil;
    }
    else
    {
        m_tView = [[UITextView alloc] initWithFrame: frame];
        m_tView.delegate = self;
        m_tView.backgroundColor = [UIColor whiteColor];
        m_tView.keyboardType = UIKeyboardTypeDefault;
        m_tView.autocorrectionType = UITextAutocorrectionTypeYes;
        m_tView.autocapitalizationType = UITextAutocapitalizationTypeNone;
        [tgView addSubview:m_tView];
        m_tField = nil;
    }
    return self;
}
- (int)getInputLineHeight
{
    if(m_type == InputKeyboard || m_type == InputField)
    {
        CGSize size = [m_tField.text sizeWithFont:[m_tField font]];
        return size.height;
    }
    else if ( m_type == InputView )
    {
        CGSize size = [m_tView.text sizeWithFont:[m_tView font]];
        return size.height;
    }
    return 1;
}

- (void)dealloc
{
    if ( m_tField != nil )
    {
        [m_tField removeFromSuperview];
        [m_tField setDelegate:nil];
        [m_tField release];
        m_tField = nil;
    }
    if ( m_tView != nil )
    {
        [m_tView removeFromSuperview];
        [m_tView setDelegate:nil];
        [m_tView release];
        m_tView = nil;
    }
    removeGlobalView();
    [super dealloc];
}
- (unsigned int)strLengthByUTF8:(NSString *)string
{
    unsigned int len = 0;
    char *p = (char *)[string cStringUsingEncoding:NSUTF8StringEncoding];
    for (unsigned int i=0 ; i<[string lengthOfBytesUsingEncoding:NSUTF8StringEncoding] ;i++)
    {
        if (*p)
        {
            len++;
        }
        p++;
    }
    return len;
}


- (void)setAlpha:(float)alpha
{
    if(m_tField != nil) m_tField.backgroundColor = [UIColor colorWithWhite:1 alpha:alpha];
    if(m_tView != nil) m_tView.backgroundColor = [UIColor colorWithWhite:1 alpha:alpha];
}

- (void)setFSize:(int)size
{
    if(m_tField != nil)
    {
        if(m_tField.font)
        {
            m_tField.font = [m_tField.font fontWithSize:size];
        }
        else
        {
            m_tField.font = [UIFont systemFontOfSize:size];
        }
    }
    if(m_tView != nil)
    {
        if(m_tView.font)
        {
            m_tView.font = [m_tView.font fontWithSize:size];
        }
        else
        {
            m_tView.font = [UIFont systemFontOfSize:size];
        }
    }
}
- (void)setAlign:(const char*)align
{
    if (m_type != InputField)
    {
        NSLog(@"not support asdf asdf align");
        return;
    }
    if (0 == strcmp("left", align))
        m_tField.textAlignment = NSTextAlignmentLeft;
    else if (0 == strcmp("center", align))
        m_tField.textAlignment = NSTextAlignmentCenter;
    else if (0 == strcmp("right", align))
        m_tField.textAlignment = NSTextAlignmentRight;
    else
    {
    }
}

- (void)setFColor:(int)color
{
    float b = (color & 0xff) / 255.0f;
    float g = ((color >> 8) & 0xff) / 255.0f;
    float r = ((color >> 16) & 0xff) / 255.0f;
    if(m_tField != nil) m_tField.textColor = [UIColor colorWithRed:r green:g blue:b alpha:1.0];
    if(m_tView != nil) m_tView.textColor = [UIColor colorWithRed:r green:g blue:b alpha:1.0];
}

- (void)setPos:(CGRect)frame
{
    if(m_tField != nil) m_tField.frame = frame;
    if(m_tView != nil) m_tView.frame = frame;
}

- (void)setType:(const char*)keyboardType
{
    UIKeyboardType kbtype = UIKeyboardTypeDefault;
    if ( keyboardType != NULL )
    {
        if ( 0 == strcmp(keyboardType, "ASCIICapable" )){
            kbtype = UIKeyboardTypeASCIICapable;
        }else if ( 0 == strcmp(keyboardType, "EmailAddress" )){
            kbtype = UIKeyboardTypeEmailAddress;
        }else if ( 0 == strcmp(keyboardType, "NamePhonePad" )){
            kbtype = UIKeyboardTypeNamePhonePad;
        }else if ( 0 == strcmp(keyboardType, "PhonePad" )){
            kbtype = UIKeyboardTypePhonePad;
        }else if ( 0 == strcmp(keyboardType, "NumberPad" )){
            kbtype = UIKeyboardTypeNumberPad;
        }else if ( 0 == strcmp(keyboardType, "URL" )){
            kbtype = UIKeyboardTypeURL;
        }else if ( 0 == strcmp(keyboardType, "NumbersAndPunctuation" )){
            kbtype = UIKeyboardTypeNumbersAndPunctuation;
        }
        
        if(m_tField != nil) m_tField.keyboardType = kbtype;
        
        if(m_tView != nil) m_tView.keyboardType = kbtype;
        
        if ( 0 == strcmp(keyboardType, "Password" ) )
        {
            if(m_tField != nil) m_tField.secureTextEntry = YES;
            if(m_tView != nil) m_tView.secureTextEntry = YES;
        }
        else
        {
            if(m_tField != nil) m_tField.secureTextEntry = NO;
            if(m_tView != nil) m_tView.secureTextEntry = NO;
        }
    }
}
- (int)getInputLines
{
    if(m_type == InputKeyboard || m_type == InputField)
    {
        return 1;
    }
    else if ( m_type == InputView )
    {
        CGSize size = [m_tView.text sizeWithFont:[m_tView font]];
        int length = size.height;
        int width = m_tView.contentSize.width;
        CGSize size1 = [m_tView sizeThatFits:CGSizeMake(width,FLT_MAX)];
        int testHeight = size1.height;
        int colomNumber = m_tView.contentSize.height/length;
        colomNumber = testHeight / length;
        return colomNumber;
    }
    return 1;
}
- (void)setMaxLength:(int)maxLength
{
    m_maxLength = maxLength;
}



- (void)showKeyboard
{
    if(m_tField != nil) [m_tField becomeFirstResponder];
    if(m_tView != nil) [m_tView becomeFirstResponder];
}

- (void)hideKeyboard
{
    if(m_tField != nil) [m_tField resignFirstResponder];
    if(m_tView != nil)[m_tView resignFirstResponder];
}
- (void)setEnable:(bool)enable
{
    if(m_tField != nil) m_tField.enabled = enable;
    
    if(m_tView != nil) m_tView.editable = enable;
    
}

- (NSString*)getInput
{
    if(m_tField != nil) return m_tField.text;
    if(m_tView != nil)return m_tView.text;
    return @"";
}
- (void)setInput:(NSString*)text
{
    if(m_tField != nil) m_tField.text = text;
    if(m_tView != nil) m_tView.text = text;
}

- (void)setReturnKeyType:(int)type
{
    UIReturnKeyType rtype = UIReturnKeyDefault;
    if(type == 0)
        rtype = UIReturnKeyDefault;
    else if(type == 1)
        rtype = UIReturnKeyGo;
    else if(type == 2)
        rtype = UIReturnKeyGoogle;
    else if(type == 3)
        rtype = UIReturnKeyJoin;
    else if(type == 4)
        rtype = UIReturnKeyNext;
    else if(type == 5)
        rtype = UIReturnKeyRoute;
    else if(type == 6)
        rtype = UIReturnKeySearch;
    else if(type == 7)
        rtype = UIReturnKeySend;
    else if(type == 8)
        rtype = UIReturnKeyDone;
    
    if(m_tField != nil)
        m_tField.returnKeyType = rtype;
    if(m_tView != nil)
        m_tView.returnKeyType = rtype;
}

- (void)addInput:(NSString*)text
{
    if (m_tField != nil )m_tField.text = [m_tField.text stringByAppendingString:text];
    if (m_tView != nil )m_tView.text = [m_tView.text stringByAppendingString:text];
}
#pragma mark ------------------------------

- (void)textViewDidBeginEditing:(UITextView *)textView
{
    if ( m_callback )
        
        m_callback->BeginEditing();
}

- (void)textViewDidEndEditing:(UITextView *)textView
{
    NSString * txt = [self getInput];
    
    if ( m_callback )
        m_callback->EndEditing([txt UTF8String]);
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)string
{
    return YES;
}

- (void)textViewDidChange:(UITextView *)textView
{
     NSString * txt = [self getInput];
    if ( m_callback )
    {
        m_callback->DidChange([txt UTF8String]);
    }
}
#pragma mark ------------------------------
extern bool notifyKeyboardReturnPress();

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    if ( m_callback )
    {
        m_callback->BeginEditing();
    }
}

- (void)textFieldEditChanged:(UITextField *)textField
{
    if ( m_callback )
    {
        NSString * txt = [self getInput];
        m_callback->DidChange([txt UTF8String]);
    }
}

- (BOOL)textField:(UITextField*)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString*)string
{
	return YES;
}

- (void)textFieldDidEndEditing:(UITextField*)textField
{
    NSString * txt = [self getInput];
    
    if ( m_callback )
        m_callback->EndEditing([txt UTF8String]);
}

- (BOOL)textFieldShouldReturn:(UITextField*)textField
{
	notifyKeyboardReturnPress();
    [m_tField resignFirstResponder];
	return YES;
}





@end

extern "C" void ShowKeyboard(const char* keyboardType)
{
	if (g_keyboard == nil)
	{
		g_keyboard = [[Keyboard alloc] initWithType:InputKeyboard Frame:CGRectMake(0, 0, 0, 0) Parent:NULL];
	}
    [g_keyboard setType:keyboardType];
	[g_keyboard showKeyboard];
}

extern "C" void HideKeyboard()
{
	[g_keyboard hideKeyboard];
    [g_keyboard release];
    g_keyboard = nil;
}

extern "C" void CopyText(const char * text){
    [UIPasteboard generalPasteboard].string = [NSString stringWithUTF8String:text];
}
