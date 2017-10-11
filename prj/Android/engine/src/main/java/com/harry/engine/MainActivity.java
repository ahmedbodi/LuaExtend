package com.harry.engine;

import android.os.Bundle;
import android.util.Log;


public class MainActivity extends MyUnityPlayerActivity {

    static {
        System.loadLibrary("EngineDll");
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d("OverrideActivity", "onCreate called!");
        AndroidUtils.gameActivity = this;
        AndroidUtils.SetGameName(AndroidUtils.INIT_GAME_NAME);
        AndroidUtils.SetAppPath(AndroidUtils.INIT_APP_PATH_VALUE);
        AndroidUtils.SetDocPath(AndroidUtils.INIT_DOC_PATH_VALUE);
        AndroidUtils.init();
        TextInput.init();
    }

    public void onBackPressed() {
        // instead of calling UnityPlayerActivity.onBackPressed() we just ignore the back button event
        // super.onBackPressed();
    }
}
