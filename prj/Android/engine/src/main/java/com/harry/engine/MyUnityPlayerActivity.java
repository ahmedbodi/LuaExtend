package com.harry.engine;

import com.unity3d.player.*;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.PixelFormat;
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
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.json.JSONObject;

import java.io.IOException;

import sh.lilith.lilithchat.open.LilithChat;

/**
 * Created by harry on 2017/3/23.
 */

public class MyUnityPlayerActivity extends Activity implements
        MediaPlayer.OnCompletionListener,
        MediaPlayer.OnPreparedListener,
        MediaPlayer.OnErrorListener
{
    protected UnityPlayer mUnityPlayer; // don't change the name of this variable; referenced from native code

    private static MyRelativeLayout m_gamelayout = null;

    public static MyRelativeLayout getGameLayout() {
        return m_gamelayout;
    }
    private ViewSizeObserver m_ViewSizeObserver = null;

    //视频播放相关--->
    private Button bt_skip;
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
                    //SrtParser.showSRT(mediaPlayer, tv_srt) ;
                    //每隔500ms执行一次showSRT()
                    mHandler.sendEmptyMessageDelayed(0, 500);
                    break;
            }

        }
    };
    private RelativeLayout rootLayout;
    //<---视频播放相关

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
        lpSV.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        lpSV.addRule(RelativeLayout.ALIGN_PARENT_START);
        lpSV.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        rootLayout.addView(sv, lpSV);

        bt_skip = new Button(this);
        bt_skip.setText("SKIP");
        RelativeLayout.LayoutParams lpBtn = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT);
        lpBtn.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
        lpBtn.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        rootLayout.addView(bt_skip, lpBtn);

        tv_srt = new TextView(this);
        tv_srt.setGravity(Gravity.CENTER);
        tv_srt.setTextColor(Color.rgb(255, 255, 255));
        tv_srt.setTextSize(20f);
        RelativeLayout.LayoutParams lpText = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.WRAP_CONTENT);
        lpText.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        lpText.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        lpText.addRule(RelativeLayout.ALIGN_PARENT_START);
        lpText.setMargins(0,0,0,17);
        rootLayout.addView(tv_srt, lpText);

        vg.addView(rootLayout, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        //布局 --->结束

        bt_skip.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                skip();
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
                    Log.i("bqt", "----> surfaceDestroyed" + "--" + mediaPlayer.getCurrentPosition());
                    savedPosition = mediaPlayer.getCurrentPosition();//当前播放位置

                    mediaPlayer.stop();
                }
            }
            @Override
            public void surfaceCreated(SurfaceHolder holder) {//holder被创建时回调
                //Log.i("bqt", "创建了--" + savedPosition + "--" + savedFilepath);
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
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }


                mHandler.sendEmptyMessageDelayed(0, 500);
            }

            //holder宽高发生变化（横竖屏切换）时回调
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            }
        });

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
        //Intent intent = new Intent(this, VideoActivity.class);
        //intent.setFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        //this.startActivityForResult(intent, 99);

        //SrtParser.parseSrt(getAssets());

        play();

        //SrtParser.showSRT(mediaPlayer, tv_srt) ;
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == 99) {
            if (resultCode == RESULT_OK) {
                //Log.e("video", "video close");
                //UnityPlayer.UnitySendMessage("Main Camera","CbOfVideoPlayFinished", "");
                JSONObject jsonObject = new JSONObject();
                try {
                    jsonObject.put("MSG_ID", "VIDEO_PlayFinished");
                }catch(Exception e){}
                playVideoFinishedJsonString = jsonObject.toString();
                Runnable tmp = new Runnable() {
                    @Override
                    public void run() {
                        AndroidUtils.sendMessageToLua(playVideoFinishedJsonString);
                    }
                };
                AndroidUtils.AddUnityThread(tmp);
            }
        }
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
            } catch (Exception e) {
                e.printStackTrace();
                //Toast.makeText(this, "请检查是否有写SD卡权限", 0).show();
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

        setResult(Activity.RESULT_OK);
        finish();
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
        mediaPlayer.start();

        mediaPlayer.seekTo(savedPosition);//开始时是从0开始播放，恢复时是从指定位置开始播放

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

}
