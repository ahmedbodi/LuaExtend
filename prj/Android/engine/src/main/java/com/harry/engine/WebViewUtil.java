package com.harry.engine;

import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.RelativeLayout;

/**
 * Created by harry on 2017/3/23.
 */

public class WebViewUtil {
    private static WebView m_webview = null;

    public static void OpenWebView(String url, int pos_x, int pos_y, int width, int height) {
        MainActivity mactivity = (MainActivity) AndroidUtils.gameActivity;
        if (m_webview != null) {
            mactivity.getGameLayout().removeView(m_webview);
            m_webview = null;
        }
        m_webview = new WebView(AndroidUtils.gameActivity);
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(width, height);
        lp.leftMargin = pos_x;
        lp.topMargin = pos_y;
        m_webview.setBackgroundColor(Color.TRANSPARENT);
        m_webview.setHorizontalScrollBarEnabled(false);
        m_webview.setWebViewClient(new WebViewClient() {
            public void OnReceivedError(WebView view, int ecode, String desc, String urlerror) {
                Log.e("WebViewUtil", String.format("webview error: code %d,desc: %s, url : %s", ecode, desc, urlerror));
            }
        });
        mactivity.getGameLayout().addView(m_webview, lp);
        m_webview.getSettings().setJavaScriptEnabled(true);
        m_webview.getSettings().setLoadWithOverviewMode(true);
        m_webview.loadUrl(url);
    }

    public static void SetVisibleWebView(boolean visible) {
        if (m_webview == null)
            return;
        if (visible)
            m_webview.setVisibility(View.VISIBLE);
        else
            m_webview.setVisibility(View.INVISIBLE);
    }

    public static void CloseWebView() {
        if (m_webview != null) {
            MainActivity mactivity = (MainActivity) AndroidUtils.gameActivity;
            mactivity.getGameLayout().removeView(m_webview);
            m_webview = null;
        }
    }

    public static void MoveWebView(int x, int y) {
        if (m_webview != null) {
            RelativeLayout.LayoutParams lp = (RelativeLayout.LayoutParams) m_webview.getLayoutParams();
            lp.leftMargin = x;
            lp.topMargin = y;
            m_webview.setLayoutParams(lp);
        }
    }

}
