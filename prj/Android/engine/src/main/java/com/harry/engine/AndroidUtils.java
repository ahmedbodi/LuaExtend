package com.harry.engine;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.app.usage.UsageEvents;
import android.content.ClipData;
import android.content.ClipDescription;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Build;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.util.Base64;
import android.provider.Settings;
import android.util.Log;
import android.view.Display;
import android.widget.RelativeLayout;

import com.flurry.android.FlurryAgent;

import org.json.JSONObject;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;


public class AndroidUtils {

    public static String INIT_APP_PATH_VALUE = "InvalidPath";
    public static String INIT_DOC_PATH_VALUE = "InvalidPath";
    public static String INIT_GAME_NAME = "Engine";
    public static final String ANDROID_OBB_DIR = "/Android/obb/";

    public static Activity gameActivity = null;

    private static ActivityManager m_activityManager = null;
    private static ConnectivityManager m_connectManager = null;


    private static native void initJNI();

    private static native void releaseJNI();

    private static native void setPath(String apppath, String docPath, String obbPath);

    private static native void setAssetManager(AssetManager asm);

    public static void init() {
        initJNI();
        setAssetManager(gameActivity.getAssets());
        m_activityManager = (ActivityManager) gameActivity.getSystemService(Context.ACTIVITY_SERVICE);
        m_connectManager = (ConnectivityManager) gameActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
        m_OSVersion = Build.VERSION.RELEASE;
        m_GPUName = android.opengl.GLES20.glGetString(android.opengl.GLES20.GL_RENDERER);
        m_DeviceName = String.format("Android_%s_%s_%s", Build.BRAND, Build.MODEL, Build.PRODUCT);
        m_MacAddr = getMacAddr_impl();
        m_TotalMemSize = getTotalMemSize_impl();
        m_FreeMemSize = getFreeMemSize_impl();
        m_Language = Locale.getDefault().getLanguage();
        m_Country = Locale.getDefault().getCountry();
        m_TimeZone = TimeZone.getDefault().getID();
        m_BundleIdentifier = gameActivity.getPackageName();
        m_AndroidID = Settings.Secure.getString(gameActivity.getContentResolver(), Settings.Secure.ANDROID_ID);
        try {
            PackageInfo pinfo = gameActivity.getPackageManager().getPackageInfo(gameActivity.getPackageName(), 0);
            m_GameVersionCode = "" + pinfo.versionCode;
            m_GameVersionName = pinfo.versionName;
        } catch (Exception e) {
        }
        m_OpenUDID = GetOpenUDID_impl();
        initPaths();
        unityThread = new UnityThread();
    }

    private static String m_homePath = "";
    private static String m_cachePath = "";

    public static String getExternalStoragePath() {
        String path = Environment.getExternalStorageDirectory().getAbsolutePath();
        String packagename = gameActivity.getPackageName();
        return path + "/GLGData/" + packagename.replace('.', '_');
    }

    private static void initPaths() {
        m_homePath = gameActivity.getFilesDir().getAbsolutePath();
        m_cachePath = gameActivity.getCacheDir().getAbsolutePath();
        MakePathExists(m_homePath);
        MakePathExists(m_cachePath);

        File saveFile = new File(m_homePath + "/save.txt");
        Log.d("InitPaths", saveFile.getAbsolutePath());
        if (!saveFile.exists()) {
            try {
                Log.d("InitPaths", "Del:" + GetAppPath());
                DeleteAllFilesAndFolder(GetAppPath());
                MakePathExists(GetAppPath());
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                Log.d("InitPaths", "Del:" + GetDocPath());
                DeleteAllFilesAndFolder(GetDocPath());
                MakePathExists(GetDocPath());
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                saveFile.createNewFile();
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            Log.d("InitPaths", "found save files");
        }
        String fileName = "main." + GetGameVersionCode() + "." + gameActivity.getPackageName() + ".obb";
        String packName = gameActivity.getPackageName();
        File sdCardDir = Environment.getExternalStorageDirectory();
        String sdcardpath = "";
        try {
            sdcardpath = sdCardDir.getCanonicalPath();
        } catch (Exception e) {
        }
        String expansiobFileDir = sdcardpath + ANDROID_OBB_DIR + packName + "/" + fileName;
        Log.d("InitPaths", "expansiobFileDir=" + expansiobFileDir);
        File obbFile = new File(expansiobFileDir);
        if (!obbFile.exists()) {
            setPath(GetAppPath() + "/", GetDocPath() + "/", "");
        } else {
            setPath(GetAppPath() + "/", GetDocPath() + "/", expansiobFileDir/*+ "!"/com_test_test/app/*/);
        }
    }

    public static String CallStaticAndroidJavaFunction(String functionAndParams) {
        try {
            JSONObject jsonObject = new JSONObject(functionAndParams);
            String functionName = jsonObject.getString("function");
            if (functionName.compareTo("GetDeviceName") == 0) {
                return GetDeviceName();
            } else if (functionName.compareTo("GetLanguage") == 0) {
                return GetLanguage();
            } else if (functionName.compareTo("GetCountry") == 0) {
                return GetCountry();
            } else if (functionName.compareTo("GetCPUName") == 0) {
                return GetCPUName();
            } else if (functionName.compareTo("GetAndroidID") == 0) {
                return GetAndroidID();
            } else if (functionName.compareTo("GetMacAddr") == 0) {
                return GetMacAddr();
            } else if (functionName.compareTo("GetOpenUDID") == 0) {
                return GetOpenUDID();
            } else if (functionName.compareTo("GetAdvertisementID") == 0) {
                return GetAdvertisementID();
            } else if (functionName.compareTo("GetGameVersionCode") == 0) {
                return GetGameVersionCode();
            } else if (functionName.compareTo("GetGameVersionName") == 0) {
                return GetGameVersionName();
            } else if (functionName.compareTo("GetOSVersion") == 0) {
                return GetOSVersion();
            } else if (functionName.compareTo("GetTimeZone") == 0) {
                return GetTimeZone();
            } else if (functionName.compareTo("GetTotalMemSize") == 0) {
                return "" + GetTotalMemSize();
            } else if (functionName.compareTo("GetFreeMemSize") == 0) {
                return "" + GetFreeMemSize();
            } else if (functionName.compareTo("OpenUrl") == 0) {
                String p1 = jsonObject.getString("param1");
                OpenUrl(p1);
                return null;
            } else if (functionName.compareTo("GetGameName") == 0) {
                return GetGameVersionName();
            } else if (functionName.compareTo("ExitGame") == 0) {
                ExitGame();
                return null;
            } else if (functionName.compareTo("GetBundleIdentifier") == 0) {
                return GetBundleIdentifier();
            } else if (functionName.compareTo("GetNetWorkType") == 0) {
                return GetNetWorkType();
            } else if (functionName.compareTo("isWifiReached") == 0) {
                return "" + isWifiReached();
            } else if (functionName.compareTo("isNetConnected") == 0) {
                return "" + isNetConnected();
            } else if (functionName.compareTo("killSelf") == 0) {
                killSelf();
                return null;
            } else if (functionName.compareTo("CopyToPastBoard") == 0) {
                String p1 = jsonObject.getString("param1");
                CopyToPastBoard(p1);
                return null;
            } else if (functionName.compareTo("GetFromPastBoard") == 0) {
                return GetFromPastBoard();
            } else if (functionName.compareTo("GetDocPath") == 0) {
                return GetDocPath();
            } else if (functionName.compareTo("GetAppPath") == 0) {
                return GetAppPath();
            } else if (functionName.compareTo("GetGameName") == 0) {
                return GetGameName();
            } else if (functionName.compareTo("OpenWebView") == 0) {
                String url = jsonObject.getString("param1");
                int px = jsonObject.getInt("param2");
                int py = jsonObject.getInt("param3");
                int w = jsonObject.getInt("param4");
                int h = jsonObject.getInt("param5");
                OpenWebView(url, px, py, w, h);
                return null;
            } else if (functionName.compareTo("EventLog") == 0) {
                String key = jsonObject.getString("key");
                String p1 = jsonObject.getString("param1");
                String p2 = jsonObject.getString("param2");
                String p3 = jsonObject.getString("param3");
                AndroidUtils.EventLog(key,p1,p2,p3);
                return null;
            } else if (functionName.compareTo("CloseWebView") == 0) {
                CloseWebView();
                return null;
            } else if (functionName.compareTo("SetVisibleWebView") == 0) {
                int px = jsonObject.getInt("param1");
                SetVisibleWebView(px != 0);
                return null;
            } else if (functionName.compareTo("MoveWebView") == 0) {
                int px = jsonObject.getInt("param1");
                int py = jsonObject.getInt("param2");
                MoveWebView(px, py);
                return null;
            } else {
                Log.e("CallJavaFunction", "Error:Can not found function :" + functionName);
            }
        } catch (Exception e) {
            return null;
        }
        return null;
    }


    private static String ForLockUIThead = "";
    private static volatile boolean isUIThreadEnd = false;

    private static void OpenWebView(final String html, final int x, final int y, final int w, final int h) {
        isUIThreadEnd = false;
        gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                synchronized (ForLockUIThead) {
                    WebViewUtil.OpenWebView(html, x, y, w, h);
                    isUIThreadEnd = true;
                    ForLockUIThead.notify();
                }
            }
        });
        synchronized (ForLockUIThead) {
            //wait for thread end
            while (!isUIThreadEnd) {
                try {
                    ForLockUIThead.wait();
                } catch (Exception ee) {
                }
            }
        }
    }

    private static void CloseWebView() {
        isUIThreadEnd = false;
        gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                synchronized (ForLockUIThead) {
                    WebViewUtil.CloseWebView();
                    ;
                    isUIThreadEnd = true;
                    ForLockUIThead.notify();
                }
            }
        });
        synchronized (ForLockUIThead) {
            //wait for thread end
            while (!isUIThreadEnd) {
                try {
                    ForLockUIThead.wait();
                } catch (Exception ee) {
                }
            }
        }
    }

    private static void SetVisibleWebView(final boolean visible) {
        isUIThreadEnd = false;
        gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                synchronized (ForLockUIThead) {
                    WebViewUtil.SetVisibleWebView(visible);
                    isUIThreadEnd = true;
                    ForLockUIThead.notify();
                }
            }
        });
        synchronized (ForLockUIThead) {
            //wait for thread end
            while (!isUIThreadEnd) {
                try {
                    ForLockUIThead.wait();
                } catch (Exception ee) {
                }
            }
        }
    }

    private static void MoveWebView(final int posx, final int posy) {
        isUIThreadEnd = false;
        gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                synchronized (ForLockUIThead) {
                    WebViewUtil.MoveWebView(posx, posy);
                    isUIThreadEnd = true;
                    ForLockUIThead.notify();
                }
            }
        });
        synchronized (ForLockUIThead) {
            //wait for thread end
            while (!isUIThreadEnd) {
                try {
                    ForLockUIThead.wait();
                } catch (Exception ee) {
                }
            }
        }
    }

    private static String m_DeviceName = "";

    public static String GetDeviceName() {
        return m_DeviceName;
    }

    private static String m_Language = "";

    public static String GetLanguage() {
        return m_Language;
    }

    private static String m_Country = "";

    public static String GetCountry() {
        return m_Country;
    }

    private static String m_GPUName = "";

    public static String GetCPUName() {
        return m_GPUName;
    }

    private static String m_AndroidID = "";

    public static String GetAndroidID() {
        return m_AndroidID;
    }

    private static String m_MacAddr = "";

    public static String GetMacAddr() {
        return m_MacAddr;
    }

    private static String m_OpenUDID = "";

    public static String GetOpenUDID() {
        return m_OpenUDID;
    }


    private static String m_AdvertisementID = "";

    public static String GetAdvertisementID() {
        return "needTodo";
    }


    private static String m_GameVersionCode = "";

    public static String GetGameVersionCode() {
        return m_GameVersionCode;
    }

    private static String m_GameVersionName = "";

    public static String GetGameVersionName() {
        return m_GameVersionName;
    }

    private static String m_OSVersion = "";

    public static String GetOSVersion() {
        return m_OSVersion;
    }

    private static String m_TimeZone = "";

    public static String GetTimeZone() {
        return m_TimeZone;
    }

    private static int m_TotalMemSize = 0;

    public static int GetTotalMemSize() {
        return m_TotalMemSize;/*M*/
    }

    private static int m_FreeMemSize = 0;

    public static int GetFreeMemSize() {
        return m_FreeMemSize;/*M*/
    }

    public static void OpenUrl(String url) {
        if (!url.startsWith("http://") && !url.startsWith("https://") && !url.startsWith("market://")) {
            url = String.format("https://%s", url);
        }
        final String furl = url;
        gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    Intent itent = new Intent(Intent.ACTION_VIEW, Uri.parse(furl));
                    if (furl.indexOf("market://") == -1) {
                        gameActivity.startActivity(Intent.createChooser(itent, gameActivity.getString(ResUtil.getStringId(MyApplication.instance, "tips_openurl"))));
                    } else {
                        gameActivity.startActivity(itent);
                    }
                } catch (Exception e) {
                    if (furl.indexOf("market://") != -1) {
                        String Url = "http://play.google.com/store/apps/details?" + furl.substring("market://details?".length());
                        Intent itent = new Intent(Intent.ACTION_VIEW, Uri.parse(Url));
                        gameActivity.startActivity(Intent.createChooser(itent, gameActivity.getString(ResUtil.getStringId(MyApplication.instance, "tips_openurl"))));
                    } else {
                        Log.e("OPENURL", e.getMessage());
                    }
                }
            }
        });
    }

    private static String m_GameName = INIT_GAME_NAME;

    public static String GetGameName() {
        return m_GameName;
    }

    public static void SetGameName(String gamename) {
        m_GameName = gamename;
    }


    public static void ExitGame() {
        gameActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gameActivity.finish();
            }
        });
    }

    private static String m_BundleIdentifier = "";

    public static String GetBundleIdentifier() {
        return m_BundleIdentifier;
    }

    private static String m_NetWorkType = "";

    public static String GetNetWorkType() {
        return GetNetWorkType_impl();
    }

    public static int isWifiReached() {
        int result = 0;
        try {
            if (GetNetWorkType_impl().compareTo("wifi") == 0)
                return 1;
        } catch (Exception e) {
        }
        return 0;
    }

    public static int isNetConnected() {
        if (GetNetWorkType_impl().compareTo("none") != 0)
            return 1;
        return 0;
    }

    public static void killSelf() {
        try {
            int pid = android.os.Process.myPid();
            android.os.Process.killProcess(pid);
        } catch (Exception e) {
        }
    }

    static Handler PastBoardHandle = new Handler() {
        @Override
        public void handleMessage(Message message) {
            try {
                ClipboardManager m = (ClipboardManager) gameActivity.getSystemService(Context.CLIPBOARD_SERVICE);
                m.setPrimaryClip(ClipData.newPlainText(ClipDescription.MIMETYPE_TEXT_PLAIN, message.getData().getString("copy")));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };

    public static void CopyToPastBoard(String param) {
        try {
            Bundle b = new Bundle();
            Message message = new Message();
            b.putString("copy", param);
            message.setData(b);
            PastBoardHandle.sendMessage(message);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static String GetFromPastBoard() {
        ClipboardManager cbm = (ClipboardManager) gameActivity.getSystemService(Context.CLIPBOARD_SERVICE);
        if (!cbm.hasPrimaryClip())
            return "";
        return (String) cbm.getPrimaryClip().getItemAt(0).getText();
    }

    private static String getMacAddr_impl() {
        WifiManager m = (WifiManager) gameActivity.getSystemService(Context.WIFI_SERVICE);
        WifiInfo winfo = m.getConnectionInfo();
        if (winfo == null || winfo.getMacAddress() == null) {
            return "";
        } else {
            String mac = winfo.getMacAddress();
            return mac.replace(":", "-");
        }
    }

    private static int getTotalMemSize_impl() {
        //m_TotalMemSize
        String tMemInfo;
        long initial_memory = 0;
        try {
            BufferedReader BReader = new BufferedReader(new FileReader("/proc/meminfo"), 2048);
            tMemInfo = BReader.readLine();
            int start = tMemInfo.indexOf(":");
            tMemInfo = tMemInfo.substring(start + 1);
            tMemInfo = tMemInfo.trim();
            int end = tMemInfo.indexOf(" kB");
            initial_memory = Long.valueOf(tMemInfo.substring(0, end));
            initial_memory = initial_memory / 1024;
            BReader.close();
        } catch (IOException e) {
        }
        return (int) initial_memory;
    }

    private static int getFreeMemSize_impl() {
        MemoryInfo memInfo = new MemoryInfo();
        m_activityManager.getMemoryInfo(memInfo);
        return (int) (memInfo.availMem >> 20);
    }

    private static String m_DocPath = INIT_DOC_PATH_VALUE;

    public static String GetDocPath() {
        return m_DocPath;
    }

    public static void SetDocPath(String docpath) {
        m_DocPath = docpath;
        if (m_DocPath.compareTo(INIT_DOC_PATH_VALUE) == 0) {
            m_DocPath = getExternalStoragePath() + "/doc";
        }
        MakePathExists(m_DocPath);
    }

    private static String m_AppPath = INIT_APP_PATH_VALUE;

    public static String GetAppPath() {
        return m_AppPath;
    }

    public static void SetAppPath(String apppath) {
        m_AppPath = apppath;
        if (m_AppPath.compareTo(INIT_APP_PATH_VALUE) == 0) {
            m_AppPath = getExternalStoragePath() + "/app";
        }
        MakePathExists(m_AppPath);
    }

    private static void MakePathExists(String path) {
        try {
            new File(path).mkdirs();
        } catch (Exception e) {
        }
    }

    private static String GetOpenUDID_impl() {
        String serial = "";
        if (Build.VERSION.SDK_INT > 8) {
            try {
                Field serialField = Build.class.getField("SERIAL");
                serial = serialField.get(null).toString();
            } catch (Exception e) {
            }
        }
        return md5Encode(GetAndroidID() + serial + Build.SERIAL);
    }

    private static String GetNetWorkType_impl() {
        ConnectivityManager cm = (ConnectivityManager) gameActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = cm.getActiveNetworkInfo();
        String nettype = "none";
        if (info != null && info.isConnected()) {
            int type = info.getType();
            if (type == ConnectivityManager.TYPE_WIFI) {
                nettype = "wifi";
            } else if (type == ConnectivityManager.TYPE_MOBILE) {
                nettype = "cell";
            }
            return nettype;
        }
        return "none";
    }

    /***********************************
     * <For common encode>
     **********************************/
    public static String urlEncode(String source) {
        String result = source;
        try {
            result = URLEncoder.encode(source, "utf-8");
        } catch (Exception e) {
        }
        return result;
    }

    public static String urlDecode(String source) {

        String result = source;
        try {
            result = URLDecoder.decode(source, "utf-8");
        } catch (Exception e) {

        }
        return result;
    }

    public static String EncodeBase64(String source) {
        String result = Base64.encodeToString(source.getBytes(), Base64.NO_WRAP);
        return result;
    }

    public static String md5Encode(String str) {
        java.security.MessageDigest md5 = null;
        try {
            md5 = java.security.MessageDigest.getInstance("MD5");
        } catch (Exception e) {
            return "";
        }

        char[] charArray = str.toCharArray();
        byte[] byteArray = new byte[charArray.length];

        for (int i = 0; i < charArray.length; i++) {
            byteArray[i] = (byte) charArray[i];
        }

        byte[] md5Bytes = md5.digest(byteArray);

        StringBuffer hexValue = new StringBuffer();

        for (int i = 0; i < md5Bytes.length; i++) {
            int val = ((int) md5Bytes[i]) & 0xff;
            if (val < 16)
                hexValue.append("0");
            hexValue.append(Integer.toHexString(val));
        }
        return hexValue.toString().toUpperCase();
    }


    private static void DeleteAllFilesAndFolder(String path) throws IOException {
        File f = new File(path);
        if (f.exists()) {
            if (f.isDirectory()) {
                File[] childFiles = f.listFiles();
                if (childFiles.length == 0) {
                    f.delete();
                } else {
                    for (int i = 0; i < childFiles.length; i++) {
                        DeleteAllFilesAndFolder(childFiles[i].getAbsolutePath());
                    }
                    f.delete();
                }
            } else {
                f.delete();
            }
        } else {
        }
    }

    private static int m_screenWidth = 0;
    private static int m_screenHeight = 0;

    private static void InitScreenSize() {
        Display display = gameActivity.getWindowManager().getDefaultDisplay();
        m_screenWidth = display.getWidth();
        m_screenHeight = display.getHeight();
        reInitScreenSize(m_screenWidth,m_screenHeight);
    }

    public static void reInitScreenSize(int w, int h) {
        m_screenWidth = w;
        m_screenHeight = h;
        int tmpw = m_screenWidth;
        int tmph = m_screenHeight;
        if(gameActivity.getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE)
        {
            m_screenWidth = Math.max(tmpw,tmph);
            m_screenHeight = Math.min(tmpw,tmph);
        }else
        {
            m_screenWidth = Math.min(tmpw,tmph);
            m_screenHeight = Math.max(tmpw,tmph);
        }
    }

    public static int GetScreenWidth() {
        return m_screenWidth;
    }

    public static int GetScreenHeight() {
        return m_screenHeight;
    }


    ///------------- Thread of call on unity3d
    public static UnityThread unityThread = null;

    public static void CallThreadOnUnity() {
        if (unityThread != null) {
            unityThread.run();
        }
        if(m_screenWidth == 0 || m_screenHeight == 0)
        {
            InitScreenSize();
        }
    }

    public static void AddUnityThread(Runnable r) {
        if (unityThread != null)
            unityThread.queueEvent(r);
    }

    public static Handler gameHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                default:
                    super.handleMessage(msg);
            }
        }
    };


    private static ArrayList EventRecordList = new ArrayList();
    private static boolean m_eventSdkInited = false;
    public static void EventSetEnabled(){m_eventSdkInited = true;}
    public static void EventLog(String eventkey, String param1,String param2,String param3)
    {
        if (!m_eventSdkInited)
        {
            EventItem t = new EventItem(eventkey,param1,param2,param3);
            EventRecordList.add(t);
            return;
        }
        if (EventRecordList.size() != 0)
        {
            for (int i = 0; i < EventRecordList.size(); i ++)
            {
                EventItem t = (EventItem) EventRecordList.get(i);
                t.Sent();
            }
            EventRecordList.clear();
        }
        EventItem.Sent(eventkey,param1,param2,param3);
    }
}
class EventItem
{
    public EventItem(String eventkey,String param1,String param2,String param3)
    {
        m_eventkey = eventkey;
        m_param1 = param1;
        m_param2 = param2;
        m_param3 = param3;
    }
    private String m_eventkey;
    private String m_param1;
    private String m_param2;
    private String m_param3;
    public void Sent()
    {
        Map<String,String> params = new HashMap<String,String>();
        if (m_param1 != null && !m_param1.isEmpty())
            params.put("cparam1",m_param1);
        if (m_param2 != null && !m_param2.isEmpty())
            params.put("cparam2",m_param2);
        if (m_param3 != null && !m_param3.isEmpty())
            params.put("cparam3",m_param3);
        params.put("duid",AndroidUtils.GetOpenUDID());
        params.put("time_zone",AndroidUtils.GetTimeZone());
        params.put("language",AndroidUtils.GetLanguage());
        params.put("country",AndroidUtils.GetCountry());
        params.put("devicename",AndroidUtils.GetDeviceName());
        FlurryAgent.logEvent(m_eventkey,params);
    }
    public static void Sent(String key,String p1,String p2,String p3)
    {
        Map<String,String> params = new HashMap<String,String>();
        if(p1 != null && !p1.isEmpty())
            params.put("cparam1",p1);
        if(p2 != null && !p2.isEmpty())
            params.put("cparam2",p2);
        if(p3 != null && !p3.isEmpty())
            params.put("cparam3",p3);
        params.put("duid",AndroidUtils.GetOpenUDID());
        params.put("time_zone",AndroidUtils.GetTimeZone());
        params.put("language",AndroidUtils.GetLanguage());
        params.put("country",AndroidUtils.GetCountry());
        params.put("devicename",AndroidUtils.GetDeviceName());
        FlurryAgent.logEvent(key,params);
    }
}
