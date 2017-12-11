package com.harry.sdk;

import android.app.Application;
import android.os.Debug;
import android.util.Log;

import com.harry.engine.AndroidUtils;
import com.harry.engine.MainActivity;
import com.lilith.sdk.LilithSDK;
import com.lilith.sdk.SDKObserver;
import com.lilith.sdk.base.model.User;
import com.lilith.sdk.base.model.UserInfo;
import com.lilith.sdk.common.constant.LoginType;
import com.lilith.sdk.common.constant.PayType;

import org.json.JSONObject;

import java.util.Set;

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
        try {
            JSONObject jsonObject = new JSONObject(jsoncmd);
            String functionName = jsonObject.getString("Function");
            if (functionName.compareTo("Login") == 0) {
                LilithSDK.getInstance().startLogin(AndroidUtils.gameActivity);
            } else if (functionName.compareTo("SwitchAccount") == 0) {
                LilithSDK.getInstance().startSwitchAccount(AndroidUtils.gameActivity);
            } else if (functionName.compareTo("clearAutoLogin") == 0){
                LilithSDK.getInstance().clearAutoLogin();
            } else if (functionName.compareTo("bindLogin") == 0)
            {
                int type = Integer.parseInt((String)jsonObject.get("type"));
                LilithSDK.getInstance().bindLogin(AndroidUtils.gameActivity,LoginType.parseValue(type,-1));
            } else if (functionName.compareTo("startPay") == 0)
            {
                String payItemID = (String)jsonObject.get("payItemID");
                String payContext = (String)jsonObject.get("payContext");
                LilithSDK.getInstance().startPay(AndroidUtils.gameActivity,payItemID,payContext);
            } else if (functionName.compareTo("queryCurrentUser") == 0)
            {
                User cuser = LilithSDK.getInstance().queryCurrentUser();
                UserInfo cuserinfo = LilithSDK.getInstance().queryCurrentUserInfo();
                JSONObject jsonO = new JSONObject();
                try {
                    jsonO.put("MSG_ID", "SDK_onQueryCurrentUser");
                    jsonO.put("appUid", "" + cuser.getAppUid());
                    jsonO.put("appToken", cuser.getAppToken());
                    jsonO.put("loginType", "" + cuser.getLoginType().getLoginType());
                    String assocaitep = cuserinfo.getAssociatePhone();
                    String assocaiteemail = cuserinfo.getAssociateEmail();
                    boolean isnewreg = cuserinfo.isNewReg();
                    Set t = cuserinfo.getBoundLoginTypes();
                    Object[] lt =t.toArray();
                    String loginTypeStr="";
                    for(int i = 0; i < lt.length; i ++)
                    {
                        LoginType lty = (LoginType) lt[i];
                        loginTypeStr += "" + lty.getLoginType();
                        if(i != lt.length - 1)
                            loginTypeStr += "_";
                    }
                    String userName = cuser.getName();
                    if(assocaitep != null)
                        jsonO.put("PhoneNumber", "" + assocaitep);
                    if(assocaiteemail != null)
                        jsonO.put("Email", "" + assocaiteemail);
                    jsonO.put("isNewReg", "" + isnewreg);
                    if (lt.length > 0)
                        jsonO.put("LoginTypes", "" + loginTypeStr);
                }catch(Exception e){}
                AndroidUtils.sendMessageToLua(jsonO.toString());
            }
            else {
                int i = 0;
                i = 3;
                Log.d("dsdfsf","dddsdf" + i);
            }
        }catch(Exception e){ }
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
            JSONObject jsonObject = new JSONObject();
            try {
                jsonObject.put("MSG_ID", "SDK_onLoginFinish");
                jsonObject.put("appUid", "" + l);
                jsonObject.put("appToken", s);
                jsonObject.put("loginType", "" + loginType.getLoginType());
            }catch(Exception e){}
            AndroidUtils.sendMessageToLua(jsonObject.toString());
            CallSDKFunction("{\"Function\":\"queryCurrentUser\"}");
        }

        @Override
        protected void onLoginFailed(LoginType loginType, int i) {
            super.onLoginFailed(loginType, i);
            JSONObject jsonObject = new JSONObject();
            try {
                jsonObject.put("MSG_ID", "SDK_onLoginFailed");
                jsonObject.put("errorCode", "" + i);
                jsonObject.put("loginType", "" + loginType.getLoginType());
            }catch(Exception e){}
            AndroidUtils.sendMessageToLua(jsonObject.toString());
        }

        @Override
        protected void onBindFinish(boolean b, long l, String s, LoginType loginType) {
            super.onBindFinish(b, l, s, loginType);
            JSONObject jsonObject = new JSONObject();
            try {
                jsonObject.put("MSG_ID", "SDK_onBindFinish");
                jsonObject.put("bSuccess", "" + b);
                jsonObject.put("appUid", "" + l);
                jsonObject.put("appToken", s);
                jsonObject.put("loginType", "" + loginType.getLoginType());
            }catch(Exception e){}
            AndroidUtils.sendMessageToLua(jsonObject.toString());
        }

        @Override
        protected void onPayFinish(boolean b, int i, String s, PayType payType) {
            super.onPayFinish(b, i, s, payType);
            JSONObject jsonObject = new JSONObject();
            try {
                jsonObject.put("MSG_ID", "SDK_onPayFinish");
                jsonObject.put("bSuccess", "" + b);
                jsonObject.put("price", "" + i);
                jsonObject.put("itemID", s);
                jsonObject.put("payType", "" + payType.getPayType());
            }catch(Exception e){}
            AndroidUtils.sendMessageToLua(jsonObject.toString());
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
