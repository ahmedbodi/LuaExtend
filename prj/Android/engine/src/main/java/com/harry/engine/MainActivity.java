package com.harry.engine;

import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.flurry.android.FlurryAgent;
import com.flurry.android.FlurryAgentListener;


public class MainActivity extends MyUnityPlayerActivity {

    static {
        System.loadLibrary("EngineDll");
    }
    FlurryAgent.Builder m_flurry;
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d("OverrideActivity", "onCreate called!");
        AndroidUtils.gameActivity = this;
        AndroidUtils.SetGameName(AndroidUtils.INIT_GAME_NAME);
        AndroidUtils.SetAppPath(AndroidUtils.INIT_APP_PATH_VALUE);
        AndroidUtils.SetDocPath(AndroidUtils.INIT_DOC_PATH_VALUE);
        AndroidUtils.init();
        TextInput.init();
        initSDK();
        AndroidUtils.EventLog("onCreate","","","");
    }

    @Override
    protected void onStart() {
        super.onStart();
        FlurryAgent.onStartSession(this);
    }

    @Override
    protected void onStop() {
        super.onStop();
        FlurryAgent.onEndSession(this);
    }

    public void onBackPressed() {
        // instead of calling UnityPlayerActivity.onBackPressed() we just ignore the back button event
        // super.onBackPressed();
    }

    public void initSDK()
    {
        initFlurrySDK();
    }

    class FlurryListener implements FlurryAgentListener {
        @Override
        public void onSessionStarted() {
            AndroidUtils.EventSetEnabled();
        }
    }

    private void initFlurrySDK()
    {
        m_flurry = new FlurryAgent.Builder();
        try {
            ActivityInfo info = this.getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
            String msg = info.metaData.getString("flurry_key");
            Log.i("debug_info","flurry_key:" + msg);
            FlurryListener listeren = new FlurryListener();
            m_flurry.withListener(listeren);
            m_flurry.build(this, msg);

        }catch(Exception e)
        {
            Log.i("debug_info","flurry_key: error");
        }
    }
}
