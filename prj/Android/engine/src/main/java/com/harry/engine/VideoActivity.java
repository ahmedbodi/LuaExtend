package com.harry.engine;

import java.io.IOException;

import android.content.res.AssetFileDescriptor;
import android.graphics.Color;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.net.Uri;
import android.os.Bundle;
import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

// 在音乐播放器基础上加***部分的内容即可播放视频；加上===部分即可控制拖动栏；加上————部分可解决MediaPlayer与SurfaceHolder同步的问题
public class VideoActivity extends Activity implements View.OnClickListener,
        OnCompletionListener,
        MediaPlayer.OnPreparedListener,
        MediaPlayer.OnErrorListener
{
    private Button bt_skip;
    private MediaPlayer mediaPlayer;//多媒体播放器
    private SurfaceView sv;//****************SurfaceView是一个在其他线程中显示、更新画面的组件，专门用来完成在单位时间内大量画面变化的需求
    private SurfaceHolder holder;//****************SurfaceHolder接口为一个显示界面内容的容器
    private static int savedPosition;//===============记录当前播放文件播放的进度
    private static String savedFilepath;//===============记录当前播放文件的位置
    private TextView tv_srt;
    private VideoActivity target;

    Handler mHandler=new Handler(){
        public void handleMessage(Message msg){
            long pos;
            switch (msg.what) {
                case 0:
                    SrtParser.showSRT(mediaPlayer, tv_srt) ;
                    //每隔500ms执行一次showSRT()
                    mHandler.sendEmptyMessageDelayed(0, 500);
                    break;
            }

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        target = this;

        //setContentView(R.layout.activity_video);
        //取消标题栏
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        //取消状态栏
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //布局 --->开始
        RelativeLayout rootLayout = new RelativeLayout(this);
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

        setContentView(rootLayout);
        //布局 --->结束

        //tv_srt = (TextView)findViewById(R.id.tv_srt);
        //bt_skip = (Button) findViewById(R.id.bt_skip);
        //bt_skip.setOnClickListener(this);
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
                else
                {
                    SrtParser.parseSrt(getAssets());

                    play();

                    SrtParser.showSRT(mediaPlayer, tv_srt) ;

                }

                mHandler.sendEmptyMessageDelayed(0, 500);
            }

            //holder宽高发生变化（横竖屏切换）时回调
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            }
        });
    }
    @Override
    public void onClick(View v) {


        /*
        switch (v.getId()) {
            case R.id.bt_play:
                play();
                break;
            case R.id.bt_playUrl:
                playUrl();
                break;
            case R.id.bt_pause:
                pause();
                break;
            case R.id.bt_stop:
                stop();
                break;
            case R.id.bt_replay:
                replay();
                break;
            case R.id.bt_skip:
                skip();
                break;
            default:
                break;
        }
        */
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
     * 暂停
     */
    /*
    public void pause() {
        if (mediaPlayer != null) {
            if (mediaPlayer.isPlaying()) {
                mediaPlayer.pause();
                bt_pause.setText(STATE_CONTINUE);
            } else {
                mediaPlayer.start();
                bt_pause.setText(STATE_PAUSE);
                return;
            }
        }
    }
    */
    /**
     * 停止
     */
    /*
    public void stop() {
        if (mediaPlayer != null && mediaPlayer.isPlaying()) {
            mediaPlayer.stop();
        }
        mediaPlayer.reset();
        bt_play.setEnabled(true);
        bt_playUrl.setEnabled(true);
        bt_pause.setText("暂停");
    }
    */
    /**
     * 重播
     */
    /*
    public void replay() {
        if (mediaPlayer != null) {
            mediaPlayer.start();
            mediaPlayer.seekTo(0);
        }
        bt_pause.setText("暂停");
    }
    */
    /**
     * 跳过
     */
    public void skip() {
        endVideo();
    }

    public void endVideo() {
        //mediaPlayer.seekTo(mediaPlayer.getDuration());
        if (mediaPlayer != null && mediaPlayer.isPlaying()) {
            mediaPlayer.stop();
        }
        mediaPlayer.reset();

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
    @Override
    public boolean onKeyDown(int keyCode,KeyEvent event){
        if(keyCode==KeyEvent.KEYCODE_BACK)
            return true;//不执行父类点击事件

        return super.onKeyDown(keyCode, event);//继续执行父类其他点击事件
    }
}