package com.harry.engine;

import android.app.Application;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;

import com.harry.sdk.LiLithSDKUtils;
import com.lilith.sdk.LilithSDK;
import com.lilith.sdk.special.uiless.LilithUILess;
import com.umeng.message.IUmengRegisterCallback;
import com.umeng.message.MessageSharedPrefs;
import com.umeng.message.PushAgent;

import java.util.Locale;

/**
 * Created by harry on 2017/3/21.
 */

public class MyApplication extends Application {
    static MyApplication instance = null;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d("MyApplication", "onCreate .......");
        instance = this;
        PushAgent mPushAgent = PushAgent.getInstance(this);
        mPushAgent.register(new IUmengRegisterCallback() {
            @Override
            public void onSuccess(String deviceToken) {
                Log.d("PushOnSuccess",deviceToken);
            }

            @Override
            public void onFailure(String s, String s1) {
                Log.d("PushOnFailure","register failed");
            }
        });
        Log.d("PushDeviceToken",MessageSharedPrefs.getInstance(this).getDeviceToken());
        LiLithSDKUtils.SDKUILess().init(this);
        LiLithSDKUtils.SDKUILess().setLocale(Locale.getDefault());
      //Bundle bundle = new Bundle();
      //bundle.putInt("PlayPhoneGravity", Gravity.LEFT|Gravity.BOTTOM);
      //LiLithSDKUtils.SDKUILess().setInitConfig(bundle);
      //LiLithSDKUtils.SDKUILess().setOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    @Override
    public void onTerminate()
    {
        super.onTerminate();
        LiLithSDKUtils.SDKUILess().unInit();
    }
}
