package com.harry.sdk;

import com.lilith.sdk.SDKObserver;
import com.lilith.sdk.common.constant.LoginType;
import com.lilith.sdk.common.constant.PayType;

import org.json.JSONObject;

/**
 * Created by 55555 on 2017/12/6.
 */

public class LiLithSDKUtils {

    public static LiLithSDKUtils getInstance()
    {
        if (minstance == null)
            minstance = new LiLithSDKUtils();
        return minstance;
    }
    public void CallSDKFunction(String jsoncmd)
    {

    }
    public SDKObserver getSDKObserver()
    {
        return mObserver;
    }
    private static LiLithSDKUtils minstance = null;

    private SDKObserver mObserver = new SDKObserver() {
        @Override
        public void onUpdate(int i, Object[] objects) {
            super.onUpdate(i, objects);
        }

        @Override
        protected void onLoginFinish(long l, String s, LoginType loginType) {
            super.onLoginFinish(l, s, loginType);
        }

        @Override
        protected void onLoginFailed(LoginType loginType, int i) {
            super.onLoginFailed(loginType, i);
        }

        @Override
        protected void onBindFinish(boolean b, long l, String s, LoginType loginType) {
            super.onBindFinish(b, l, s, loginType);
        }

        @Override
        protected void onPayFinish(boolean b, int i, String s, PayType payType) {
            super.onPayFinish(b, i, s, payType);
        }

        @Override
        protected void onRegionNotified(String s) {
            super.onRegionNotified(s);
        }

        @Override
        protected void onIdentified() {
            super.onIdentified();
        }

        @Override
        protected void onUserForgetAccount() {
            super.onUserForgetAccount();
        }

        @Override
        protected void onEarningHalfed(long l, long l1) {
            super.onEarningHalfed(l, l1);
        }

        @Override
        protected void onEarningCleared(long l, long l1) {
            super.onEarningCleared(l, l1);
        }

        @Override
        protected void onEarningReset(long l, long l1) {
            super.onEarningReset(l, l1);
        }

        @Override
        protected void onWebPayDBFinish(boolean b, int i, int i1) {
            super.onWebPayDBFinish(b, i, i1);
        }

        @Override
        protected void onGoogleGamesConnectFailed(boolean b, int i, String s) {
            super.onGoogleGamesConnectFailed(b, i, s);
        }

        @Override
        protected void onGoogleGameConnectSucceed(boolean b) {
            super.onGoogleGameConnectSucceed(b);
        }
    };
}
