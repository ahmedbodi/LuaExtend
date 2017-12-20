package com.harry.engine;

import com.unity3d.player.*;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import sh.lilith.lilithchat.open.LilithChat;

import static android.content.Intent.FLAG_ACTIVITY_CLEAR_TOP;
import static android.content.Intent.FLAG_ACTIVITY_NEW_TASK;

/**
 * Created by harry on 2017/3/23.
 */

public class MyUnityPlayerActivity extends Activity {
    protected UnityPlayer mUnityPlayer; // don't change the name of this variable; referenced from native code

    private static MyRelativeLayout m_gamelayout = null;

    public static MyRelativeLayout getGameLayout() {
        return m_gamelayout;
    }
    private ViewSizeObserver m_ViewSizeObserver = null;
    // Setup activity layout
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);

        getWindow().setFormat(PixelFormat.RGBX_8888);

        mUnityPlayer = new UnityPlayer(this);

        m_gamelayout = new MyRelativeLayout(UnityPlayer.currentActivity);
        setContentView(mUnityPlayer.getView());
        mUnityPlayer.requestFocus();

        ViewGroup vg = (ViewGroup) UnityPlayer.currentActivity.getWindow().getDecorView();
        vg.addView(m_gamelayout, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        m_ViewSizeObserver = ViewSizeObserver.getInstance(this,m_gamelayout);
        m_ViewSizeObserver.start();

        LilithChat.init();
        LilithChat.preInit(this,5);
    }

    // Quit Unity
    @Override
    protected void onDestroy() {
        mUnityPlayer.quit();
        super.onDestroy();
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        mUnityPlayer.resume();
        mUnityPlayer.windowFocusChanged(true);
    }
    @Override
    protected void onStop()
    {
        super.onStop();
        mUnityPlayer.pause();
        mUnityPlayer.windowFocusChanged(false);
    }

   // Pause Unity
   /*  @Override
    protected void onPause() {
        Log.d("MyUnityPlayerActivity","onPause");
        super.onPause();
        mUnityPlayer.pause();
    }

    // Resume Unity
    @Override
    protected void onResume() {
        Log.d("MyUnityPlayerActivity","onResume");
        super.onResume();
        mUnityPlayer.resume();
     }
    */
    // This ensures the layout will be correct.
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mUnityPlayer.configurationChanged(newConfig);
    }

    // Notify Unity of the focus change.
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if(hasFocus != false)
        mUnityPlayer.windowFocusChanged(hasFocus);
    }

    // For some reason the multiple keyevent type is not supported by the ndk.
    // Force event injection by overriding dispatchKeyEvent().
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_MULTIPLE)
            return mUnityPlayer.injectEvent(event);
        return super.dispatchKeyEvent(event);
    }

    // Pass any events not handled by (unfocused) views straight to UnityPlayer
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        return mUnityPlayer.injectEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return mUnityPlayer.injectEvent(event);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return mUnityPlayer.injectEvent(event);
    }

    /*API12*/
    public boolean onGenericMotionEvent(MotionEvent event) {
        return mUnityPlayer.injectEvent(event);
    }

    public static void CallThreadOnUnity() {
       AndroidUtils.CallThreadOnUnity();
    }

    public void LogD(String msg)
    {
        CSharpHelper.LogD(msg);
    }

    public void LogE(String msg)
    {
        CSharpHelper.LogE(msg);
    }

    public void LogW(String msg)
    {
        CSharpHelper.LogW(msg);
    }

    public void LogI(String msg)
    {
        CSharpHelper.LogI(msg);
    }

    public void CopyToClipboard(String msg){
        CSharpHelper.CopyToClipboard(this, msg);
    }

    public void PlayVideo() {
        Intent intent = new Intent(this, VideoActivity.class);
        intent.addFlags(FLAG_ACTIVITY_NEW_TASK);
        intent.addFlags(FLAG_ACTIVITY_CLEAR_TOP);
        this.startActivityForResult(intent, 99);
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == 99) {
            if (resultCode == RESULT_OK) {
                UnityPlayer.UnitySendMessage("Main Camera","CbOfVideoPlayFinished", "");
            }
        }
    }
}
