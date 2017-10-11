#ifndef MAUKEYJIOASIODFJIOJIE_MYKEYBOARD_H_KLSDJKIO3IOJI
#define MAUKEYJIOASIODFJIOJIE_MYKEYBOARD_H_KLSDJKIO3IOJI
enum TextInputType {
    InputKeyboard,
    InputField,
    InputView
};
#if defined(TARGET_OS_IPHONE) ||  defined(IPHONEOS)
#import <UIKit/UIKit.h>
class TextInput;
@interface Keyboard : NSObject <UITextFieldDelegate, UITextViewDelegate>
{
    int m_maxLength;
    TextInputType m_type;
    TextInput * m_callback;
@public
    UITextView* m_tView;
    UITextField* m_tField;
}
- (id)initWithType:(TextInputType)type Frame:(CGRect)frame Parent:(TextInput*)callback;
- (void)setPos:(CGRect)pos;
- (void)setMaxLength:(int)maxLength;
- (void)showKeyboard;
- (void)hideKeyboard;
- (NSString*)getInput;
- (int)getInputLines;
- (int)getInputLineHeight;
- (void)setType:(const char*)keyboardType;
- (void)setInput:(NSString*)text;
- (void)addInput:(NSString*)text;
- (void)setAlpha:(float)alpha;
- (void)setFSize:(int)size;
- (void)setFColor:(int)color;
- (void)setEnable:(bool)enable;
- (void)setReturnKeyType:(int)type;
- (unsigned int)strLengthByUTF8:(NSString *)text;
- (void)setAlign:(const char*)aligh;

@end
#else
class Keyboard
{
};

#endif
#endif
