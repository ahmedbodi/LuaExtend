package com.harry.engine;

import android.util.Log;
import android.content.ClipboardManager;
import android.content.ClipData;
import android.content.Context;

/**
 * Created by jq on 2017/12/6.
 */

public class CSharpHelper {
    public static void LogV(String msg)
    {
        Log.v("DDDDDDDDDDDDDDDDD----->", msg);
    }

    public static void LogD(String msg)
    {
        Log.d("DDDDDDDDDDDDDDDDD----->", msg);
    }

    public static void LogI(String msg)
    {
        Log.i("DDDDDDDDDDDDDDDDD----->", msg);
    }

    public static void LogW(String msg)
    {
        Log.w("DDDDDDDDDDDDDDDDD----->", msg);
    }

    public static void LogE(String msg) { Log.e("DDDDDDDDDDDDDDDDD----->", msg); }

    public static void CopyToClipboard(Context context, String msg)
    {
        ClipboardManager clipboard = (ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData clip = ClipData.newPlainText("mass_copy", msg);
        clipboard.setPrimaryClip(clip);
    }
}
