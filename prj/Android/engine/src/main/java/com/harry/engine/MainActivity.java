package com.harry.engine;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.flurry.android.FlurryAgent;
import com.flurry.android.FlurryAgentListener;
import com.harry.sdk.LiLithSDKUtils;
import com.lilith.sdk.LilithSDK;
import com.lilith.sdk.SDKObserver;
import com.lilith.sdk.common.constant.LoginType;
import com.lilith.sdk.common.constant.PayType;
import com.umeng.message.PushAgent;

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
        DealWithScheme();
        TextInput.init();
        initSDK();
        AndroidUtils.EventLog("onCreate","","","");
    }

    @Override
    protected void onStart() {
        super.onStart();
        LocalPushNotify.m_onfront = true;
        FlurryAgent.onStartSession(this);
    }

    @Override
    protected void onStop() {
        super.onStop();
        LocalPushNotify.m_onfront = false;
        FlurryAgent.onEndSession(this);
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        LocalPushNotify.m_onfront = false;
        LiLithSDKUtils.SDKUILess().removeSDKObserver(LiLithSDKUtils.getInstance().getSDKObserver());
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
    {
        if (requestCode == AndroidUtils.REQUEST_READWRITE_STORAGE)
        {
            if(grantResults.length>0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
            {
           //     AndroidUtils.initPaths();
            }
        }
    }

    public void onBackPressed() {
        // instead of calling UnityPlayerActivity.onBackPressed() we just ignore the back button event
        // super.onBackPressed();
    }

    public void initSDK()
    {
        initFlurrySDK();
        LiLithSDKUtils.SDKUILess().addSDKObserver(LiLithSDKUtils.getInstance().getSDKObserver());
        LiLithSDKUtils.getInstance().Init();
        PushAgent.getInstance(this.getApplicationContext()).onAppStart();
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
    private void DealWithScheme()
    {
        Intent intent = getIntent();
        Uri uri = intent.getData();
        if (uri != null)
        {
            String dataString = intent.getDataString();
            AndroidUtils.OpenByUrl(dataString);
        }
    }
}
