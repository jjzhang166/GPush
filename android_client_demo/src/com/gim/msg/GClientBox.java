package com.gim.msg;

import com.gim.client;
import com.gpush.config.Config;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

public class GClientBox {
	private static GClientBox ins = null;

	public static GClientBox instance() {
		if (ins == null) {
			ins = new GClientBox();
		}
		return ins;
	}

	public static final String TAG = "GClientBox";
	private client mClient;
	private GMsgRouter mRouter;

	private GClientBox() {
	}

	public client getClient() {
		return mClient;
	}
	
	public GMsgRouter getRouter() {
		return mRouter;
	}
	
	public void init(Context context) {
		Log.d(TAG, "init");
		mRouter = new GMsgRouter(context);

		String path = "" ;
		if(!Config.PubKeyFile.isEmpty()){
			path = Environment.getExternalStorageDirectory().getAbsolutePath() + Config.PubKeyFile;
		}
		mClient = new client();
		mClient.init(mRouter, path);
		mClient.loglevel(1);
	}
}
