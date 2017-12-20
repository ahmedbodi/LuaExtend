package com.harry.engine;

/**
 * Created by jq on 2017/12/18.
 */

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.media.MediaPlayer;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Locale;

public class SrtParser {

    public static ArrayList<SRT>srtList = null;
    public static int lastEndTime;

    /**
     * 解析SRT字幕文件
     * 字幕路径
     */
    public static void parseSrt(AssetManager am) {
        InputStream inputStream = null;
        try {
            String lan = Locale.getDefault().getLanguage();
            String country = Locale.getDefault().getCountry();
            String res_id = "video_en.srt";

            if (lan.equals("zh")) {
                if (country.equals("CN"))
                    res_id = "video_cn.srt";
                else
                    res_id = "video_zh.srt";
            }
            //else if (lan.equals("ja"))
            //  res_id = "video_ja";
            //else if (lan.equals("ko"))
            //  res_id = "video_kr";

            //inputStream = res.openRawResource(res_id);
            inputStream = am.open(res_id);

            BufferedReader br = new BufferedReader(new InputStreamReader(
                    inputStream,"UTF-8"));

            if (srtList == null)
                srtList = new ArrayList<SRT>();
            else
                srtList.clear();

            String line = null;
            StringBuffer sb = new StringBuffer();

            while ((line = br.readLine()) != null) {

                if (!line.equals("")) {
                    sb.append(line).append("@");
                    continue;
                }

                String[] parseStrs = sb.toString().split("@");
                // 该if为了适应一开始就有空行以及其他不符格式的空行情况
                if (parseStrs.length < 3) {
                    sb.delete(0, sb.length());// 清空，否则影响下一个字幕元素的解析</i>
                    continue;
                }

                SRT srt = new SRT();

                // 解析开始和结束时间
                String timeTotime = parseStrs[1];
                int begin_hour = Integer.parseInt(timeTotime.substring(0, 2));
                int begin_minute = Integer.parseInt(timeTotime.substring(3, 5));
                int begin_second = Integer.parseInt(timeTotime.substring(6, 8));
                int begin_milli = Integer.parseInt(timeTotime.substring(9, 12));
                int beginTime = (begin_hour * 3600 + begin_minute * 60 + begin_second)
                        * 1000 + begin_milli;
                int end_hour = Integer.parseInt(timeTotime.substring(17, 19));
                int end_minute = Integer.parseInt(timeTotime.substring(20, 22));
                int end_second = Integer.parseInt(timeTotime.substring(23, 25));
                int end_milli = Integer.parseInt(timeTotime.substring(26, 29));
                int endTime = (end_hour * 3600 + end_minute * 60 + end_second)
                        * 1000 + end_milli;

                /*
                System.out.println("开始:" + begin_hour + ":" + begin_minute +
                        ":"
                        + begin_second + ":" + begin_milli + "=" + beginTime
                        + "ms");
                System.out.println("结束:" + end_hour + ":" + end_minute + ":"
                        + end_second + ":" + end_milli + "=" + endTime + "ms");
                */

                //     解析字幕文字
                String srtBody = "";
                // 可能1句字幕，也可能2句及以上。
                for (int i = 2; i < parseStrs.length; i++) {
                    srtBody += parseStrs[i]+ "\n";
                }
                // 删除最后一个"\n"
                srtBody = srtBody.substring(0, srtBody.length() - 1);
                // 设置SRT
                srt.setBeginTime(beginTime);
                srt.setEndTime(endTime);
                srt.setSrtBody(new String(srtBody.getBytes(), "UTF-8"));

                // 插入队列
                srtList.add(srt);
                sb.delete(0, sb.length());// 清空，否则影响下一个字幕元素的解析
            }

            lastEndTime = srtList.get(srtList.size()-1).getEndTime();
            br.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        finally {
            try {
                inputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void showSRT(MediaPlayer mediaPlayer, TextView tvSrt) {

        int currentPosition = mediaPlayer.getCurrentPosition();//vv是VideoView播放器

        if(currentPosition>lastEndTime){
            tvSrt.setVisibility(View.GONE);
            return;
        }

        for(int i=0;i<srtList.size();i++){
            SRT srt_bean = srtList.get(i);
            if (currentPosition > srt_bean.getBeginTime()
                    && currentPosition < srt_bean.getEndTime()) {

                String subtitle = srt_bean.getSrtBody();
                subtitle = subtitle.replace("\\n", "\n");
                tvSrt.setText(subtitle);

                //显示过的就删掉，提高查询效率（删除后，如果切出再切回来，字幕会有问题）
                //srtList.remove(i);
                break;
            }
        }
    }
}
