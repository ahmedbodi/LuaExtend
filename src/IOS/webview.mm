#import <UIKit/UIKit.h>
#include "Common/ENG_DBG.h"

extern "C" UIView* UnityGetGLView();
UIView* g_sharedInstanceUIView = NULL;
UIView* GetGLView()
{
    if(g_sharedInstanceUIView == NULL)
        g_sharedInstanceUIView = UnityGetGLView();
    return g_sharedInstanceUIView;
}

UIView * sGlobalView = nil;
UIWebView *g_webview = nil;
int sGlobalViewCount = 0;

extern "C" void GlobalViewRotate()
{
}
UIView * getGlobalView()
{
    if ( sGlobalView == nil )
    {
        
        int x = 0;
        int y = 0;
        int w = [GetGLView() frame].size.width;
        int h = [GetGLView() frame].size.height;
        sGlobalView = [[UIView alloc] initWithFrame: CGRectMake(x, y, w, h)];
        GlobalViewRotate();
        
        [GetGLView() addSubview:sGlobalView];
        sGlobalView.multipleTouchEnabled = YES;
    }
    sGlobalViewCount++;
    return sGlobalView;
}

void removeGlobalView()
{
    sGlobalViewCount--;
    if ( sGlobalViewCount == 0 )
    {
        [sGlobalView removeFromSuperview];
        [sGlobalView release];
        sGlobalView = nil;
    }
}


bool CreateWebview(const char *html, int x, int y, int w, int h)
{
    CGFloat scale = [GetGLView() contentScaleFactor];
    CGRect rect = {{(CGFloat)x/scale, (CGFloat)y/scale}, {(CGFloat)w/scale, (CGFloat)h/scale}};
    g_webview = [[UIWebView alloc] initWithFrame:rect];
    
    g_webview.backgroundColor = [UIColor clearColor];
    g_webview.opaque = NO;
    g_webview.scalesPageToFit = YES;
    if([g_webview respondsToSelector:@selector(scrollView)])
    {
        for(UIView* subview in g_webview.scrollView.subviews)
        {
            if([subview isKindOfClass:[UIImageView class]])
                subview.hidden = YES;
        }
    }
    NSURLRequest *request =[NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:html]]];
    [g_webview loadRequest:request];
    [getGlobalView() addSubview:g_webview];
    return true;
}

void HideWebview(bool hide)
{
    [g_webview stopLoading];
    [g_webview removeFromSuperview];
    [g_webview release];
    g_webview = nil;
    removeGlobalView();
}

void DestroyWebview()
{
    [g_webview stopLoading];
    [g_webview removeFromSuperview];
    [g_webview release];
    g_webview = nil;
    removeGlobalView();
}
extern "C" bool DoCreateAndShowWebView(const char *html, int x, int y, int w, int h)
{
    return CreateWebview(html, x, y, w, h);
}

extern "C" void DoHideWebView(bool b)
{
    HideWebview(b);
}
void MoveWebView(int x,int y)
{
    CGRect rect = {{(CGFloat)x,(CGFloat)y}, CGSizeZero};
    if(rect.origin.x!=g_webview.frame.origin.x || rect.origin.y != g_webview.frame.origin.y)
    {
        g_webview.frame = (CGRect){rect.origin,g_webview.frame.size};
    }
}

extern "C" void CopyTextToPasteboard(const char * text)
{
    [UIPasteboard generalPasteboard].string = [NSString stringWithUTF8String:text];
}
extern "C" const char* GetPasteboardText()
{
    return [[UIPasteboard generalPasteboard].string UTF8String];
}
