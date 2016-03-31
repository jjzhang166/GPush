package com.gpush.msg;

import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import com.gim.GMsg;
import com.gim.listener;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class GMsgRouter implements listener{
	private Context mContext;

	public GMsgRouter(Context context) {
		mContext = context;
	}

	public void handleMessage(String msg) {
		
		try {
			JSONObject j = new JSONObject(msg);
			int type = j.getInt("evtype");
			
			Bundle b  = new Bundle();
			b.putString("j", msg);
			if(mHandler != null){
				Message m = Message.obtain(mHandler, type, j);
				m.sendToTarget();
			}
			
		} catch (JSONException e) {
			Log.e("gpushdemo", e.toString());
		}
	}
	
	public void setHandler(Handler h){
		mHandler = h;
	}
	private Handler mHandler = null;
}