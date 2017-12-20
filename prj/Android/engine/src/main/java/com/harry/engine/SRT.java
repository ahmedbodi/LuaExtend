package com.harry.engine;

/**
 * Created by jq on 2017/12/18.
 */

public class SRT {
    private int beginTime;
    private int endTime;
    private String srtBody;

    public int getBeginTime() {
        return beginTime;
    }

    public void setBeginTime(int beginTime) {
        this.beginTime = beginTime;
    }

    public int getEndTime() {
        return endTime;
    }

    public void setEndTime(int endTime) {
        this.endTime = endTime;
    }

    public String getSrtBody() {
        return srtBody;
    }

    public void setSrtBody(String srtBody) {
        this.srtBody = srtBody;
    }

    @Override
    public String toString() {
        return "" + beginTime + ":" + endTime + ":" + srtBody;
    }
}
