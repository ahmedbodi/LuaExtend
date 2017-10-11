package com.harry.engine;

import android.app.Application;
import android.util.Log;

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
    }
}
