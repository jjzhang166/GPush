package com.gpush.application;
import com.gpush.msg.*;
import com.gpush.R;
import com.gpush.config.Config;

import android.app.Application;
import android.net.Uri;
import android.util.Log;

public class GPushApplication extends Application {
	@Override
	public void onCreate() {
		super.onCreate();
		GClientBox.instance().init(getApplicationContext());
	}
	
    @Override
    public void onTerminate() {
        super.onTerminate();
    }
}
