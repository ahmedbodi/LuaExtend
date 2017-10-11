package com.harry.engine;

import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Build;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.util.TypedValue;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnKeyListener;
import android.view.ViewTreeObserver;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by harry on 2017/3/21.
 */

public class TextInput implements OnEditorActionListener, TextWatcher, OnKeyListener, OnFocusChangeListener, OnTouchListener, MyRelativeLayout.OnWindowResizeListener, MyRelativeLayout.BackKeyListener {
    private static final float ONELINE_HEIGHT_RATIO = 1 / 20f;
    private static final float MULTILINE_LINE_HEIGHT_RATIO = 4 / 20f;
    private static final int DEFAULT_FONT_SIZE = 22;
    private static final float FONTSIZE_SCALE_FROM_UNITY = 1 / 2f;
    private static boolean m_isCompatibleMode = false; /*Show inputbox on the top*/
    private static boolean m_ReCreateEditorInput = false;
    public static InputMethodManager m_keyboardManager = null;
    public static EditText m_EditText = null;
    private volatile boolean isInited = false;
    public static boolean m_isShowing = false;
    public static boolean m_isKeyboardShowing = false;
    private volatile boolean m_isUIComplete = false;
    private volatile boolean m_isEndEditing = false;
    private static boolean m_frameChanged = false;
    private int m_maxLength = 0;
    private boolean m_isMultiLine = false;
    private String m_originStr = "";
    private boolean m_isFromSelfReverse = false;
    private int savedBefore = 0;
    int m_lcount = 1;
    int m_lheight = 24;
    private static native void initJNI();
    private static native void releaseJNI();
    public static void init() {
        m_keyboardManager = (InputMethodManager) AndroidUtils.gameActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
        initJNI();
    }

    public TextInput(final String type, final String keyboardType, final String content, final int x, final int y, final int w, final int h, final int alpha, final int fontSize, final int fontColor, final boolean CompatibleMode, final boolean EditTopFirst) throws InterruptedException {
        isInited = false;
        m_ReCreateEditorInput = m_isCompatibleMode != CompatibleMode;
        m_isCompatibleMode = CompatibleMode;
        if (m_isCompatibleMode && m_EditText != null && m_EditText.getVisibility() != View.INVISIBLE) {
            AndroidUtils.gameHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    synchronized (TextInput.this) {
                        m_isShowing = false;
                        initInput_impl(type, keyboardType, content, x, y, w, h, alpha, fontSize, fontColor, EditTopFirst);
                        isInited = true;
                        TextInput.this.notify();
                        try {
                            show_in_isCompatibleMode();
                        } catch (Exception e) {
                        }
                    }
                }
            }, 1000);

        } else {
            AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                synchronized (TextInput.this) {
                    m_isShowing = false;
                    m_frameChanged = false;
                    initInput_impl(type, keyboardType, content, AndroidUtils.GetScreenWidth() - 1, 0, 1, 1, alpha, fontSize, fontColor, EditTopFirst);
                    m_EditText.setCursorVisible(false);
                    isInited = true;
                    TextInput.this.notify();
                    try {
                        show();
                    } catch (Exception e) {
                    }
                    checkFrameIsChanged(x,y,w,h,true);
                }
                }
            });
        }
    }
    private Runnable m_checkframeischanged = null;
    private void checkFrameIsChanged(final int x, final int y,final  int w ,final  int h,final boolean cleansign)
    {
        if(m_checkframeischanged == null)
        {
            m_checkframeischanged = new Runnable() {
                @Override
                public void run() {
                    if(m_frameChanged == true)
                    {
                        if(m_EditText.getWidth() <= 2) {
                            setPos(x, y, w <= 1 ? 2 : w, h);
                        }
                    }else
                    {
                        final int height = (int) (AndroidUtils.GetScreenHeight() * (m_isMultiLine ? MULTILINE_LINE_HEIGHT_RATIO : ONELINE_HEIGHT_RATIO));
                        setPos(AndroidUtils.GetScreenWidth() - 2,1,2,2);
                        AndroidUtils.gameHandler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                if(m_EditText.getWidth() <= 2) {
                                    setPos(1, 1, AndroidUtils.GetScreenWidth(), height);
                                }
                            }
                        }, 200);
                    }
                    AndroidUtils.gameHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            m_EditText.setCursorVisible(true);
                            m_EditText.setTextColor(Color.BLACK);

                        }
                    }, 1000);
                }
            };
        }
        AndroidUtils.gameHandler.removeCallbacks(m_checkframeischanged);
        AndroidUtils.gameHandler.postDelayed(m_checkframeischanged,500);
    }
    private void initInput_impl(String type, String keyboardType, String content, int x, int y, int w, int h, int alpha, int fontSize, int fontColor, boolean EditTopFirst) {
        if (type.equals("TextView")) {
            m_isMultiLine = true;
        }
        if (m_isMultiLine && h < 80) {
            m_isMultiLine = false;
        }
        if (m_ReCreateEditorInput) {
            if (m_EditText != null) {
                MainActivity.getGameLayout().removeView(m_EditText);
                m_EditText = null;
            }
        }
        if (m_isCompatibleMode) {
            int height = (int) (AndroidUtils.GetScreenHeight() * (m_isMultiLine ? MULTILINE_LINE_HEIGHT_RATIO : ONELINE_HEIGHT_RATIO));
            if (m_EditText == null) {
                m_lcount = 1;
                m_lheight = 24;
                m_EditText = new EditText(AndroidUtils.gameActivity);
                m_EditText.setVisibility(View.INVISIBLE);
                m_EditText.setPadding(2, 0, 2, 0);
                m_EditText.setGravity(Gravity.LEFT | Gravity.TOP);
                MyRelativeLayout.LayoutParams lp = new MyRelativeLayout.LayoutParams(AndroidUtils.GetScreenWidth(), height);
                lp.leftMargin = 0;
                lp.topMargin = 0;
                MainActivity.getGameLayout().addView(m_EditText, lp);
                m_EditText.setBackgroundColor(Color.WHITE);
                m_EditText.setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                m_EditText.setTextSize(TypedValue.COMPLEX_UNIT_SP, DEFAULT_FONT_SIZE);
            } else {
                MyRelativeLayout.LayoutParams lp = (MyRelativeLayout.LayoutParams) m_EditText.getLayoutParams();
                lp.height = height;
                m_EditText.setLayoutParams(lp);
            }
        } else {
            int width = MainActivity.getGameLayout().WIDTH;
            int height = MainActivity.getGameLayout().HEIGHT;
            if (width > AndroidUtils.GetScreenWidth() || height > AndroidUtils.GetScreenHeight()) {
                AndroidUtils.reInitScreenSize(width, height);
            }
            w = w <= 0 ? AndroidUtils.GetScreenWidth() : w;
            h = h <= 0 ? 80 : h;
            if (m_EditText == null) {
                m_lcount = 1;
                m_lheight = 24;
                m_EditText = new EditText(AndroidUtils.gameActivity);

                m_EditText.setPadding(2, 0, 2, 0);
                if (m_isMultiLine) {
                    m_EditText.setGravity(Gravity.LEFT | Gravity.TOP);
                } else {
                    m_EditText.setGravity(Gravity.LEFT);
                }
                m_EditText.setBackgroundColor(Color.WHITE);
                MyRelativeLayout.LayoutParams lp = new MyRelativeLayout.LayoutParams(MyRelativeLayout.LayoutParams.WRAP_CONTENT, MyRelativeLayout.LayoutParams.WRAP_CONTENT);
                MainActivity.getGameLayout().addView(m_EditText, lp);
                MainActivity.getGameLayout().setOnWindowResizeListener(this);
                m_EditText.setMaxLines(1);
                m_EditText.setFocusable(true);
                m_EditText.setFocusableInTouchMode(true);
                m_EditText.setVisibility(View.INVISIBLE);

                m_EditText.setWidth(w);
                m_EditText.setHeight(h);
                m_EditText.setX(x);
                m_EditText.setY(EditTopFirst ? 0 : y);
                if (EditTopFirst) {
                    m_EditText.setCursorVisible(false);
                    m_EditText.setTextColor(Color.BLACK);
                } else {
                }
                m_EditText.requestLayout();
                m_EditText.setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                ColorDrawable dw = new ColorDrawable(Color.TRANSPARENT);
            } else {
                if (m_isMultiLine) {
                    m_EditText.setGravity(Gravity.LEFT | Gravity.TOP);
                } else {
                    m_EditText.setGravity(Gravity.LEFT | Gravity.CENTER_VERTICAL);
                }
                m_EditText.setFocusable(true);
                m_EditText.setVisibility(View.INVISIBLE);
                m_EditText.setWidth(w);
                m_EditText.setHeight(h);
                m_EditText.setX(x);
                m_EditText.setY(EditTopFirst ? 0 : y);
                if (EditTopFirst) {
                    m_EditText.setCursorVisible(false);
                    m_EditText.setTextColor(Color.BLACK);
                } else {
                }
                m_EditText.requestLayout();
            }
        }
        m_EditText.setText(content);
        m_EditText.setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
        setKeyboardType(keyboardType);
        m_EditText.setTextColor(Color.BLACK);
        m_EditText.setBackgroundResource(ResUtil.getDrawableId(MyApplication.instance,"edit_box"));
    }

    private void setKeyboardType(String keyboardType) {
        int type = InputType.TYPE_CLASS_TEXT;
        if (keyboardType == null || keyboardType.length() < 1) {
            type = InputType.TYPE_CLASS_TEXT;
        } else {
            if (keyboardType.equals("ASCIICapable")) {
                type = InputType.TYPE_CLASS_TEXT;
            } else if (keyboardType.equals("EmailAddress")) {
                type = InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS | InputType.TYPE_CLASS_TEXT ;
            } else if (keyboardType.equals("Password")) {
                type = InputType.TYPE_TEXT_VARIATION_PASSWORD | InputType.TYPE_CLASS_TEXT ;
            } else if (keyboardType.equals("NamePhonePad")) {
                type = InputType.TYPE_CLASS_TEXT;
            } else if (keyboardType.equals("NumbersAndPunctuation")) {
                type = InputType.TYPE_CLASS_NUMBER;
            } else if (keyboardType.equals("PhonePad")) {
                type = InputType.TYPE_CLASS_PHONE;
            } else if (keyboardType.equals("NumberPad")) {
                type = InputType.TYPE_CLASS_NUMBER;
            } else if (keyboardType.equals("URL")) {
                type = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI;
            } else {
                type = InputType.TYPE_CLASS_TEXT;
            }
        }
        if (m_isMultiLine) {
            type |= InputType.TYPE_TEXT_FLAG_MULTI_LINE;
        }
        m_EditText.setInputType(type);
    }
    public static void purge() {
        Runnable tmp = new Runnable() {
            @Override
            public void run() {
                releaseJNI();
            }
        };
        AndroidUtils.AddUnityThread(tmp);

        if (m_EditText != null) {
            MainActivity.getGameLayout().removeView(m_EditText);
            m_EditText = null;
        }
        m_keyboardManager = null;
    }
    private void releaseSelf() {
        m_isShowing = false;
        m_EditText.setVisibility(View.INVISIBLE);
        m_EditText.setOnFocusChangeListener(null);
        m_EditText.setOnKeyListener(null);
        m_EditText.setOnEditorActionListener(null);
        m_EditText.removeTextChangedListener(TextInput.this);
    }

    private void closeInput() {
        Runnable tmp = new Runnable() {
            @Override
            public void run() {
                try {
                    hide_impl();
                } catch (Exception ee) {
                }
            }
        };
        AndroidUtils.AddUnityThread(tmp);
    }

    private void setEditListener() {
        m_EditText.setOnFocusChangeListener(null);
        m_EditText.setOnKeyListener(null);
        m_EditText.setOnEditorActionListener(null);
        m_EditText.removeTextChangedListener(TextInput.this);

        m_EditText.setOnFocusChangeListener(TextInput.this);
        m_EditText.setOnKeyListener(TextInput.this);
        m_EditText.setOnEditorActionListener(TextInput.this);
        m_EditText.addTextChangedListener(TextInput.this);
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        if (!m_isShowing || m_isFromSelfReverse) {
            return;
        }
        m_originStr = s.toString();
    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
        savedBefore = before;
        if (!m_isShowing || m_isFromSelfReverse) {
            return;
        }
    }

    @Override
    public void afterTextChanged(Editable s) {
        if (!m_isShowing || m_isFromSelfReverse) {
            return;
        }
        try {
            if (m_maxLength > 0 && getStringLengthInUTF8(s.toString()) > m_maxLength) {
                m_isFromSelfReverse = true;
                s.clear();
                s.append(m_originStr);
                m_isFromSelfReverse = false;
            }else {

                Runnable tmp = new Runnable() {
                    @Override
                    public void run() {
                        jniDidChange();
                    }
                };
                AndroidUtils.AddUnityThread(tmp);

                AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                    m_lcount = m_EditText.getLineCount();
                    m_lheight = m_EditText.getLineCount();
                    }
                });
            }
        } catch (Exception e) {
        }
    }

    private void stopEventLisening() {
        MainActivity.getGameLayout().setBackKeyListener(null);
    }

    private int getStringLengthInUTF8(String s) {
        return s.getBytes().length;
    }


    public native void jniBeginEditing();
    public native void jniDidChange();
    public native void jniEndEditing();
    public native void jniNotifyKeyboardShown(int width, int height);
    public native void jniNotifyKeyboardHidden();
    public void CallNotifyKeyboardHidden() {
        jniNotifyKeyboardHidden();
        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (Build.VERSION.SDK_INT >= 19) {
                    AndroidUtils.gameActivity.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
                }
            }
        });
    }
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (this.m_isKeyboardShowing == false) {
            m_isKeyboardShowing = true;
            setEditListener();
        }
        return false;
    }

    public void show_in_isCompatibleMode() {
        m_isEndEditing = false;
        m_isUIComplete = false;
        m_EditText.requestFocus();
        m_EditText.setVisibility(View.VISIBLE);
        m_EditText.setSelection(m_EditText.getText().toString().length());
        MainActivity.getGameLayout().invalidate();
        MainActivity.getGameLayout().requestLayout();
        m_isUIComplete = true;
        m_EditText.setOnTouchListener(this);
        m_isShowing = true;
        MainActivity.getGameLayout().setBackKeyListener(this);
    }

    public void addText(final String text) {
        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_EditText.setText(m_EditText.getText().toString() + text);
                m_lcount = m_EditText.getLineCount();
                m_lheight = m_EditText.getLineCount();
            }
        });
    }

    public void hide() throws InterruptedException {
        hide_impl();
    }

    private void hide_impl() throws InterruptedException {
        stopEventLisening();
        m_isEndEditing = true;
        m_isUIComplete = false;
        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
            m_EditText.setVisibility(View.INVISIBLE);
            m_isKeyboardShowing = false;
            m_isUIComplete = true;
            }
        });
        Runnable tmp = new Runnable() {
            @Override
            public void run() {
            jniEndEditing();
                CallNotifyKeyboardHidden();
            }
        };
        AndroidUtils.AddUnityThread(tmp);
    }

    public void show() {
        try {
            show_impl();
        } catch (Exception e) {
        }
    }

    public void show_impl() throws InterruptedException {
        synchronized (this) {
            while (!isInited) {
                wait();
            }
        }
        jniBeginEditing();

        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                setEditListener();
                MainActivity.getGameLayout().setBackKeyListener(TextInput.this);
                m_EditText.setVisibility(View.VISIBLE);
                m_EditText.setSelection(m_EditText.getText().toString().length());
                m_EditText.setFocusable(true);
                m_EditText.setFocusableInTouchMode(true);
                m_EditText.requestFocus();
                m_EditText.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        m_keyboardManager.showSoftInput(m_EditText, InputMethodManager.SHOW_FORCED);
                        m_isKeyboardShowing = true;
                        m_isShowing = true;
                    }
                }, 50);
            }
        });
    }
    public void setSize(int w, int h) {
        if (m_EditText == null)
            return;

        final int W = (int) (w);
        final int H = (int) (h);
        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_EditText.setHeight(H);
                m_EditText.setWidth(W);
                m_EditText.requestLayout();
            }
        });
    }

    private String setPosBlock = "";

    public void setPos(final int x, final int y,final int w,final int h) {
        if (m_isCompatibleMode)
            return;

        m_isUIComplete = false;
        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
            synchronized (setPosBlock) {
                if (m_isCompatibleMode) {
                    return;
                }
                m_EditText.setX(x);
                m_EditText.setY(y);
                m_EditText.setWidth(w);
                m_EditText.setHeight(h);
                m_isUIComplete = true;
                setPosBlock.notify();
            }
            }
        });

        synchronized (setPosBlock) {
            while (!m_isUIComplete) {
                try {
                    setPosBlock.wait();
                } catch (InterruptedException e) {
                }
            }
        }
    }







    public String getText() {
        final Editable sp = m_EditText.getText();
        if (m_EditText != null) {
            if(sp == null)
                return "";
            return sp.toString();
        }else {
            return m_originStr;
        }
    }



    public void release() {
        try {
            stopEventLisening();
            AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                m_keyboardManager.hideSoftInputFromWindow(m_EditText.getWindowToken(), 0);
                releaseSelf();
                }
            });
        } catch (Exception ee) {
        }
    }

    public int getTextLineHeight() {
        return m_lheight;
    }

    public int getTextLines() {
        return m_lcount;
    }


    @Override
    public int onWindowResize(final int screenWidth, final int screenHeight) {
        if (m_isCompatibleMode) {
            return screenHeight;
        }
        if (!isInited) {
            return screenHeight;
        }

        m_frameChanged = true;

        int height = screenHeight;
        int reCurHeight =height;

        final int dheight = AndroidUtils.GetScreenHeight() - screenHeight;

        Runnable tmp = new Runnable() {
            @Override
            public void run() {
                if (dheight > 0) {

                    jniNotifyKeyboardShown(AndroidUtils.GetScreenWidth(), dheight);
                } else {
                    //onIMEBackKeyPressed();
                }
            }
        };
        AndroidUtils.AddUnityThread(tmp);
        return reCurHeight;

    }



    @Override
    public void onFocusChange(View v, boolean hasFocus) {
    }
    public void setMaxLength(int len) {
        m_maxLength = len;
    }
    @Override
    public void OnBackKeyPressed() {
        /*closeInput();*/
    }
    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        if (!m_isShowing) {
            return false;
        }
        return false;
    }
    @Override
    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        if (!m_isShowing) {
            return false;
        }
        if (actionId == EditorInfo.IME_ACTION_DONE) {
            closeInput();
        } else if (actionId == EditorInfo.IME_ACTION_SEND) {
            //   do action send
            return true;
        }
        return false;
    }
    public void setText(final String text) {
        AndroidUtils.gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_EditText.setText(text);
                m_lcount = m_EditText.getLineCount();
                m_lheight = m_EditText.getLineCount();
            }
        });
    }

}
