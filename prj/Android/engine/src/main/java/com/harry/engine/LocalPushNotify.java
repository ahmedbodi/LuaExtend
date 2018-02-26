package com.harry.engine;

import java.util.Calendar;
import java.util.List;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlarmManager;
import android.app.Application;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Parcel;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import com.unity3d.player.UnityPlayer;

public class LocalPushNotify extends BroadcastReceiver{

    public static boolean m_onfront = false;
    private static int m_nLastID = 0;

    public static void ShowNotification(String pTitle,String pContent,int pDelaySecond,boolean pIsDailyLoop){
        if (pDelaySecond < 0)
            return ;
        String pAppName = AndroidUtils.GetGameName();
        Activity curActivity = AndroidUtils.gameActivity;
        Intent itent = new Intent("UNITY_NOTIFICATOR");
        itent.putExtra("appname",pAppName);
        itent.putExtra("title",pTitle);
        itent.putExtra("content",pContent);
        PendingIntent pi = PendingIntent.getBroadcast(curActivity,0,itent,0);
        AlarmManager am = (AlarmManager)curActivity.getSystemService(Context.ALARM_SERVICE);
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.SECOND,pDelaySecond);
        long alarmTime = calendar.getTimeInMillis();
        if(pIsDailyLoop)
        {
            am.setRepeating(AlarmManager.RTC_WAKEUP,alarmTime,86400,pi);
        }else
        {
            am.set(AlarmManager.RTC_WAKEUP,alarmTime,pi);
        }
    }
    public static void ClearNotification()
    {
        Activity act = AndroidUtils.gameActivity;
        NotificationManager nManager = (NotificationManager)act.getSystemService(Context.NOTIFICATION_SERVICE);
        for(int i = m_nLastID; i >= 0; i --)
        {
            nManager.cancel(i);
        }
        m_nLastID = 0;
    }
    @Override
    public void onReceive(Context context, Intent intent) {
        if(m_onfront)
            return;
        Class<?> mainActivity = null;
        try {
            mainActivity = context.getClassLoader().loadClass("com.harry.engine.MainActivity");
        }catch(Exception ex)
        {
            ex.printStackTrace();
            return;
        }
        ApplicationInfo applicationInfo = null;
        PackageManager pm = context.getPackageManager();
        try{
            applicationInfo = pm.getApplicationInfo(context.getPackageName(),PackageManager.GET_META_DATA);
        }catch(Exception e){
            e.printStackTrace();
            return;
        }
        Bundle bundle = intent.getExtras();



        PendingIntent contentIntent = PendingIntent.getActivity(context,m_nLastID,new Intent(context,mainActivity),0);
        NotificationManager nm = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationChannel notificationChannel = new NotificationChannel(AndroidUtils.GetBundleIdentifier(),"Notify",NotificationManager.IMPORTANCE_HIGH);
            notificationChannel.setDescription("channel description");
            notificationChannel.enableLights(true);
            notificationChannel.setLightColor(Color.RED);
            nm.createNotificationChannel(notificationChannel);
        }
        NotificationCompat.Builder builder = new NotificationCompat.Builder(MyApplication.instance.getApplicationContext(),AndroidUtils.GetBundleIdentifier());

        Notification notification = builder.setSmallIcon(applicationInfo.icon)
                .setContentTitle((String)bundle.get("title"))
                .setContentText((String)bundle.get("content"))
                .setContentIntent(contentIntent)
                .setLargeIcon(BitmapFactory.decodeResource(context.getResources(), applicationInfo.icon))
                .build();
        nm.notify(m_nLastID,notification);
        m_nLastID++;
    }
}
