package com.harry.engine;

import android.content.Context;

/**
 * Created by harry on 2017/3/21.
 */

public class ResUtil {
    public static int getStringId(Context application, String resname) {
        return application.getResources().getIdentifier(resname, "string", application.getPackageName());
    }

    public static int getDrawableId(Context application, String resname) {
        return application.getResources().getIdentifier(resname, "drawable", application.getPackageName());
    }
}
