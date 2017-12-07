package com.harry.engine;

import android.app.Application;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;

import com.lilith.sdk.LilithSDK;
import com.lilith.sdk.special.uiless.LilithUILess;

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
        LilithSDK.getInstance(LilithUILess.class).init(this);
        LilithSDK.getInstance(LilithUILess.class).setLocale(Locale.getDefault());
      //  Bundle bundle = new Bundle();
        //bundle.putInt("PlayPhoneGravity", Gravity.LEFT|Gravity.BOTTOM);
       // LilithSDK.getInstance(LilithUILess.class).setInitConfig(bundle);
        //LilithSDK.getInstance(LilithUILess.class).setOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    @Override
    public void onTerminate()
    {
        super.onTerminate();
        LilithSDK.getInstance(LilithUILess.class).unInit();
    }
}
