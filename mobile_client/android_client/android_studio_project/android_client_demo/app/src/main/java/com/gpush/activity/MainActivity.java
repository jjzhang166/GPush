package com.gpush.activity;

import java.util.LinkedList;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import com.gim.GMsg;
import com.gpush.msg.GClientBox;
import com.gpush.config.Config;
import com.gpush.service.Bootstrap;

import android.app.Activity;
import android.content.Intent;
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
	private Intent intent = null;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Bootstrap.startAlwaysOnService(this, "Main");

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

}
