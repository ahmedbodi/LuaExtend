package com.harry.engine;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.widget.RelativeLayout;

/**
 * Created by harry on 2017/3/21.
 */

public class MyRelativeLayout extends RelativeLayout implements ViewSizeObserver.OnResizeListener {
    static int WIDTH = 0;
    static int HEIGHT = 0;
    private Context mContext;

    public MyRelativeLayout(Context context) {
        super(context);
        mContext = context;
    }

    public MyRelativeLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
    }

    public MyRelativeLayout(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }


    public static int getStringId(Context application, String resname) {
        return application.getResources().getIdentifier(resname, "string", application.getPackageName());
    }

    public static int getDrawableId(Context application, String resname) {
        return application.getResources().getIdentifier(resname, "drawable", application.getPackageName());
    }

    @Override
    public void onWindowResize(int width, int height) {
        if (AndroidUtils.gameActivity == null) {
            return;
        }

        Display display = AndroidUtils.gameActivity.getWindowManager().getDefaultDisplay();
        if (mOnWindowResizeListener != null)
            mOnWindowResizeListener.onWindowResize(width, height);
    }

    public interface OnWindowResizeListener {
        public int onWindowResize(int screenHeight, int curScreenHeight);
    }

    private OnWindowResizeListener mOnWindowResizeListener;

    public void setOnWindowResizeListener(OnWindowResizeListener listener) {
        mOnWindowResizeListener = listener;
    }


    @Override
    public boolean dispatchKeyEventPreIme(KeyEvent event) {
        if (mBackKeyListener != null) {
            if (event.getKeyCode() == KeyEvent.KEYCODE_BACK) {
                KeyEvent.DispatcherState state = getKeyDispatcherState();
                if (state != null) {
                    if (event.getAction() == KeyEvent.ACTION_DOWN && event.getRepeatCount() == 0) {
                        state.startTracking(event, this);
                        return true;

                    } else if (event.getAction() == KeyEvent.ACTION_UP && !event.isCanceled() && state.isTracking(event)) {
                        mBackKeyListener.OnBackKeyPressed();
                        return true;
                    }
                }
            }
        }
        return super.dispatchKeyEventPreIme(event);
    }

    public interface BackKeyListener {
        void OnBackKeyPressed();
    }

    private BackKeyListener mBackKeyListener = null;

    public void setBackKeyListener(BackKeyListener backKeyListener) {
        this.mBackKeyListener = backKeyListener;
    }

    @Override
    protected void onMeasure(int width, int height) {
        int w = MeasureSpec.getSize(width);
        int h = MeasureSpec.getSize(height);
        WIDTH = w;
        HEIGHT = h;
        if(AndroidUtils.gameActivity != null) {
            if (AndroidUtils.gameActivity.getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE) {
                WIDTH = Math.max(w, h);
                HEIGHT = Math.min(w, h);
            } else {
                WIDTH = Math.min(w, h);
                HEIGHT = Math.max(w, h);
            }
        }
        super.onMeasure(width, height);
    }
}
