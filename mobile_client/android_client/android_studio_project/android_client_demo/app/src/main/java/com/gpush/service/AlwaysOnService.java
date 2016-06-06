package com.gpush.service;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.widget.Toast;

import com.gim.GMsg;
import com.gpush.config.Config;
import com.gpush.msg.GClientBox;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class AlwaysOnService extends BaseService {
	private static String LOG_TAG = AlwaysOnService.class.getSimpleName();
	public static boolean isRunning = false;
	private ScheduledExecutorService backgroundService;
	private Handler mHandler;
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.e(LOG_TAG, "onStartCommand--:");
		if (isRunning == false) {
			// run something
			backgroundService = Executors.newSingleThreadScheduledExecutor();
			backgroundService.scheduleAtFixedRate(new TimerIncreasedRunnable(
					this), 0, 1000, TimeUnit.MILLISECONDS);
			isRunning = true;
			init();
		}
		// the following will return START_STICKY
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		// stop running
		isRunning = false;
		backgroundService.shutdownNow();
		super.onDestroy();
	}

	public class TimerIncreasedRunnable implements Runnable {
		private SharedPreferences currentSharedPreferences;

		public TimerIncreasedRunnable(Context context) {
			this.currentSharedPreferences = context.getSharedPreferences(
					Constants.SHAREDPREF_APP_STRING, MODE_PRIVATE);
		}

		@Override
		public void run() {
			int timeCount = this.readTimeCount() + 1;
			this.writeTimeCount(timeCount);
			int currentEpochTimeInSeconds = (int) (System.currentTimeMillis() / 1000L);
			Log.v(LOG_TAG, "Count:" + timeCount + " at time:" + currentEpochTimeInSeconds);
		}

		private int readTimeCount() {
			return this.currentSharedPreferences.getInt(
					Constants.SHAREDPREF_RUNNINGTIMECOUNT_STRING, 0);
		}

		private void writeTimeCount(int timeCount) {
			this.currentSharedPreferences.edit().putInt(
					Constants.SHAREDPREF_RUNNINGTIMECOUNT_STRING,
					timeCount).commit();
		}
	}


	private void init()
	{
		mHandler = new Handler(){
			public void handleMessage(Message msg) {
				onNotify(msg);
				super.handleMessage(msg);
			}
		};
		GClientBox.instance().getRouter().setHandler(mHandler);

		GClientBox
				.instance()
				.getClient()
				.login(Config.SRVIP, Config.SRVPORT, Config.CLIVERSION,
						Config.UID, "ss");
	}

	public int onNotify(Message msg) {
		if (msg.what == GMsg.GIM_EVTYPE_PUSH) {

			try {
				JSONObject j = (JSONObject) msg.obj;

				String m = "msg [sn:" + j.getString("sn") + "] [payload:"
						+ j.getString("payload") + "]";
				Toast toast = Toast.makeText(getApplicationContext(),
						m, Toast.LENGTH_LONG);
				toast.setGravity(Gravity.CENTER, 0, 0);
				toast.show();
			} catch (JSONException e) {
				Log.e("msg ex:", e.toString());
			}
		} else if (msg.what == GMsg.GIM_EVTYPE_LOGIN_OK) {
			Toast toast = Toast.makeText(getApplicationContext(),
					"login success", Toast.LENGTH_LONG);
			toast.setGravity(Gravity.CENTER, 0, 0);
			toast.show();
		} else if (msg.what == GMsg.GIM_EVTYPE_LOGIN_FAIL) {
			Toast toast = Toast.makeText(getApplicationContext(), "login fail",
					Toast.LENGTH_LONG);
			toast.setGravity(Gravity.CENTER, 0, 0);
			toast.show();
		} else if (msg.what == GMsg.GIM_EVTYPE_LOGIN_FAIL) {
			Toast toast = Toast.makeText(getApplicationContext(), "logout",
					Toast.LENGTH_LONG);
			toast.setGravity(Gravity.CENTER, 0, 0);
			toast.show();
		}
		return 0;
	}
}
