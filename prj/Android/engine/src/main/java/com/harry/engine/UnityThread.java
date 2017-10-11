package com.harry.engine;

import java.util.ArrayList;

public class UnityThread {
    public UnityThread() {
    }

    public void run() {
        try {
            guardedRun();
        } catch (InterruptedException e) {
        } finally {
        }
    }

    private void guardedRun() throws InterruptedException {
        synchronized (this) {
            Runnable r;
            while ((r = getEvent()) != null) {
                r.run();
            }
        }
    }

    public void queueEvent(Runnable r) {
        synchronized (this) {
            mEventQueue.add(r);
        }
    }

    private Runnable getEvent() {
        synchronized (this) {
            if (mEventQueue.size() > 0) {
                return mEventQueue.remove(0);
            }
        }
        return null;
    }

    private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();
}

