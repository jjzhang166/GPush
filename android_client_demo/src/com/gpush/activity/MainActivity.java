package com.gpush.activity;

import java.util.LinkedList;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import com.gim.GMsg;
import com.gim.msg.GClientBox;
import com.gpush.config.Config;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class MainActivity extends Activity {
	private ListView listView;
	private ArrayAdapter<String> adpter;
	private List<String> list;
	
	private Handler mHandler;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

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
		listView = new ListView(this);
		list = new LinkedList<String>();
		list.add("GPush Demo");
		adpter = new ArrayAdapter<String>(this,
				android.R.layout.simple_expandable_list_item_1, list);
		listView.setAdapter(adpter);
		setContentView(listView);
		
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		GClientBox.instance().getClient().logout(Config.UID);
	}

	public int onNotify(Message msg) {
		if (msg.what == GMsg.GIM_EVTYPE_PUSH) {

			try {
				JSONObject j = (JSONObject) msg.obj;

				String m = "msg [sn:" + j.getString("sn") + "] [payload:"
						+ j.getString("payload") + "]";
				if (list.size() > 6) {
					list.clear();
					list.add("GPush Demo ");
				}
				list.add(m);
				adpter.notifyDataSetChanged();
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
