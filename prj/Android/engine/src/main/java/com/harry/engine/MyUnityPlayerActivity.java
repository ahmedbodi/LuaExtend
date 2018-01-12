package com.harry.engine;

import com.harry.sdk.LiLithSDKUtils;
import com.unity3d.player.*;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.drawable.Drawable;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;

import sh.lilith.lilithchat.open.LilithChat;

/**
 * Created by harry on 2017/3/23.
 */

public class MyUnityPlayerActivity extends Activity implements
        MediaPlayer.OnCompletionListener,
        MediaPlayer.OnPreparedListener,
        MediaPlayer.OnErrorListener
{

    //视频播放相关--->
    private Button bt_skip;
    private ImageButton bt_imageSkip;
    private MediaPlayer mediaPlayer;//多媒体播放器
    private SurfaceView sv;//****************SurfaceView是一个在其他线程中显示、更新画面的组件，专门用来完成在单位时间内大量画面变化的需求
    private SurfaceHolder holder;//****************SurfaceHolder接口为一个显示界面内容的容器
    private static int savedPosition;//===============记录当前播放文件播放的进度
    private TextView tv_srt;
    private String playVideoFinishedJsonString = "";
    Handler mHandler=new Handler(){
        public void handleMessage(Message msg){
            long pos;
            switch (msg.what) {
                case 0:
                    SrtParser.showSRT(mediaPlayer, tv_srt) ;
                    //每隔500ms执行一次showSRT()
                    mHandler.sendEmptyMessageDelayed(0, 500);
                    break;
                case 1:
                    ShowVideoUI();
                    PlayVideo();
            }

        }
    };
    private RelativeLayout rootLayout;
    private String mObserver_VideoFinish = "";
    //<---视频播放相关


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

        addVideoView();
        HideVideoUI();

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
       // LiLithSDKUtils.SDKUILess().reportStart();reportResume(this);
    }
    @Override
    protected void onStop()
    {
        super.onStop();
        mUnityPlayer.pause();
        mUnityPlayer.windowFocusChanged(false);
       // LiLithSDKUtils.SDKUILess().reportPause(this);
    }

   // Pause Unity
     @Override
    protected void onPause() {
    //    Log.d("MyUnityPlayerActivity","onPause");
         mUnityPlayer.windowFocusChanged(false);
         LiLithSDKUtils.SDKUILess().reportPause(this);
        super.onPause();
    //    mUnityPlayer.pause();
    }

    // Resume Unity
    @Override
    protected void onResume() {
    //    Log.d("MyUnityPlayerActivity","onResume");
        mUnityPlayer.windowFocusChanged(true);
        LiLithSDKUtils.SDKUILess().reportResume(this);
        super.onResume();
    //    mUnityPlayer.resume();
     }

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











    public void PlayVideoFromCSharp(String lang){
        Log.e("DDDDDDDDDDDDDDDDD----->","play video in " + lang);

        if (rootLayout != null)
        {
            mHandler.sendEmptyMessageDelayed(1, 0);

            //Intent intent = new Intent(this, VideoActivity.class);
            ////this.startActivityForResult(intent, 99);
            //this.startActivity(intent);
        }
    }

    private void addVideoView()
    {
        //布局 --->开始
        rootLayout = new RelativeLayout(this);
        RelativeLayout.LayoutParams rootLayoutParams = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        );
        rootLayout.setLayoutParams(rootLayoutParams);

        sv = new SurfaceView(this);
        RelativeLayout.LayoutParams lpSV = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT);
        //sv.setBackgroundColor(Color.RED);
        //lpSV.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        //lpSV.addRule(RelativeLayout.ALIGN_PARENT_START);
        //lpSV.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        rootLayout.addView(sv, lpSV);

        bt_skip = new Button(this);
        bt_skip.setText("SKIP");
        bt_skip.setGravity(Gravity.LEFT | Gravity.CENTER_VERTICAL);
        bt_skip.setShadowLayer(3,0,0,Color.BLACK);
        bt_skip.setBackgroundColor(Color.TRANSPARENT);
        bt_skip.setTextColor(Color.parseColor("#ffc851"));
        RelativeLayout.LayoutParams lpBtn = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT);
        lpBtn.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
        lpBtn.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        rootLayout.addView(bt_skip, lpBtn);

        bt_imageSkip = new ImageButton(this);
        bt_imageSkip.setBackgroundColor(Color.TRANSPARENT);
        try {
            InputStream ims = getAssets().open("skip.png");
            Drawable d = Drawable.createFromStream(ims, null);
            bt_imageSkip.setImageDrawable(d);

            ims.close();
        }
        catch(IOException ex)
        {
            return;
        }
        RelativeLayout.LayoutParams lpImageBtn = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT);
        lpImageBtn.addRule(RelativeLayout.LEFT_OF, bt_skip.getId());
        rootLayout.addView(bt_imageSkip, lpImageBtn);

        tv_srt = new TextView(this);
        tv_srt.setGravity(Gravity.CENTER);
        tv_srt.setTextColor(Color.rgb(255, 255, 255));
        tv_srt.setTextSize(20f);
        tv_srt.setShadowLayer(3,0,0,Color.BLACK);
        RelativeLayout.LayoutParams lpText = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.WRAP_CONTENT);
        lpText.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        lpText.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        lpText.addRule(RelativeLayout.ALIGN_PARENT_START);
        lpText.setMargins(0,0,0,17);
        rootLayout.addView(tv_srt, lpText);

        ViewGroup vg = (ViewGroup) UnityPlayer.currentActivity.getWindow().getDecorView();
        vg.addView(rootLayout, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        sv.setZOrderMediaOverlay(true);
        //布局 --->结束

        bt_skip.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                skip();
                Log.e("ddd", "skip click");
            }
        });
        bt_imageSkip.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                skip();
                Log.e("ddd on image button", "skip click");
            }
        });

        mediaPlayer = new MediaPlayer();
        mediaPlayer.setOnCompletionListener(this);
        mediaPlayer.setOnPreparedListener(this);
        mediaPlayer.setOnErrorListener(this);
        //sv = (SurfaceView) findViewById(R.id.sv);//****************
        holder = sv.getHolder();//****************得到显示界面内容的容器
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS); //设置surfaceView自己不管理缓存区。虽然提示过时，但最好还是设置下

        //在界面【最小化】时暂停播放，并记录holder播放的位置——————————————————————————————
        holder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {//holder被销毁时回调。最小化时都会回调
                if (mediaPlayer != null) {
                    Log.e("bqt", "----> surfaceDestroyed" + "--" + mediaPlayer.getCurrentPosition());
                    savedPosition = mediaPlayer.getCurrentPosition();//当前播放位置

                    mediaPlayer.stop();
                }
            }
            @Override
            public void surfaceCreated(SurfaceHolder holder) {//holder被创建时回调
                //Log.e("bqt", "创建了--" + savedPosition + "--" + savedFilepath);
                if (savedPosition > 0) {//如果记录的数据有播放进度。
                    try {
                        mediaPlayer.reset();
                        //mediaPlayer.setDataSource(target, Uri.parse("android.resource://com.test.video/"+R.raw.video));
                        AssetFileDescriptor afd = getAssets().openFd("video.mp4");
                        mediaPlayer.setDataSource(
                                afd.getFileDescriptor(),
                                afd.getStartOffset(),
                                afd.getLength());
                        mediaPlayer.setDisplay(holder);
                        mediaPlayer.prepare();

                        mHandler.sendEmptyMessageDelayed(0, 500);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }


            }

            //holder宽高发生变化（横竖屏切换）时回调
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            }
        });
    }

    public void PlayVideo() {
        //Intent intent = new Intent(this, VideoActivity.class);
        //intent.setFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        //this.startActivityForResult(intent, 99);

        SrtParser.parseSrt(getAssets());
        tv_srt.setText("");

        play();
    }

    /**
     * 播放本地多媒体
     */
    public void play() {
        if (mediaPlayer != null) {
            try {
                //mediaPlayer.setDataSource(this, Uri.parse("android.resource://com.test.video/"+R.raw.video));
                AssetFileDescriptor afd = getAssets().openFd("video.mp4");
                mediaPlayer.setDataSource(
                        afd.getFileDescriptor(),
                        afd.getStartOffset(),
                        afd.getLength());
                mediaPlayer.setDisplay(holder);
                mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
                mediaPlayer.prepare();

                //mHandler.sendEmptyMessageDelayed(0, 500);
            } catch (Exception e) {
                e.printStackTrace();
                //Toast.makeText(this, "请检查是否有写SD卡权限", 0).show();

                endVideo();
            }
        } else {
            //Toast.makeText(this, "文件不存在", 0).show();
        }
    }
    /**
     * 跳过
     */
    public void skip() {
        endVideo();
    }

    public void endVideo() {
        if (mediaPlayer != null && mediaPlayer.isPlaying()) {
            mediaPlayer.stop();
        }

        mediaPlayer.reset();
        savedPosition = 0;

        HideVideoUI();

        // 通知lua
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.put("MSG_ID", "VIDEO_PlayFinished");
        }catch(Exception e){}
        mObserver_VideoFinish = jsonObject.toString();
        Runnable tmp = new Runnable() {
            @Override
            public void run() {
                AndroidUtils.sendMessageToLua(mObserver_VideoFinish);
            }
        };
        AndroidUtils.AddUnityThread(tmp);
    }
    //********************************************************************************************************************
    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        //Toast.makeText(MainActivity.this, "报错了--" + what + "--" + extra, 0).show();
        Log.e("video", "video error--" + what + "--" + extra);
        return false;
    }
    @Override
    public void onPrepared(MediaPlayer mp) {//只有准备好以后才能处理很多逻辑

        MediaPlayer.TrackInfo[] info = mediaPlayer.getTrackInfo();
        int count = info.length;

        mediaPlayer.start();
        mediaPlayer.seekTo(savedPosition);//开始时是从0开始播放，恢复时是从指定位置开始播放

        mHandler.sendEmptyMessageDelayed(0, 500);
        //Toast.makeText(MainActivity.this, "准备好了！", 0).show();


    }
    @Override
    public void onCompletion(MediaPlayer mp) {
        //Toast.makeText(MainActivity.this, "播放完毕！", 0).show();
        //mediaPlayer.reset();
        endVideo();
    }
    /*
    @Override
    public boolean onKeyDown(int keyCode,KeyEvent event){
        if(keyCode==KeyEvent.KEYCODE_BACK)
            return true;//不执行父类点击事件

        return super.onKeyDown(keyCode, event);//继续执行父类其他点击事件
    }
    */

    private void HideVideoUI() {
        if (bt_skip != null) {
            bt_skip.setVisibility(View.INVISIBLE);
        }

        if (bt_imageSkip != null) {
            bt_imageSkip.setVisibility(View.INVISIBLE);
        }

        if (sv != null) {
            sv.setVisibility(View.INVISIBLE);
        }

        if (tv_srt != null) {
            tv_srt.setVisibility(View.INVISIBLE);
        }

        rootLayout.setClickable(false);
    }

    private void ShowVideoUI() {
        if (bt_skip != null) {
            bt_skip.setVisibility(View.VISIBLE);
        }

        if (bt_imageSkip != null) {
            bt_imageSkip.setVisibility(View.VISIBLE);
        }

        if (sv != null) {
            sv.setVisibility(View.VISIBLE);
        }

        if (tv_srt != null) {
            tv_srt.setVisibility(View.VISIBLE);
        }

        rootLayout.setClickable(true);
    }











}
