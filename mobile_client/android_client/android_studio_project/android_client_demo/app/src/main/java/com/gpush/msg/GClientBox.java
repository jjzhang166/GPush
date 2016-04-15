package com.gpush.msg;

import com.gim.client;
import com.gpush.config.Config;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

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
			try {
				path = Environment.getExternalStorageDirectory().getAbsolutePath();
				copy(context,Config.PubKeyFile,path,Config.PubKeyFile);
				path=path+"/"+Config.PubKeyFile;
//				path = context.getDatabasePath(Config.PubKeyFile).getAbsolutePath();



			}
			catch (Exception ex)
			{

			}
		}
		mClient = new client();
		mClient.init(mRouter, path);
		mClient.loglevel(2);
	}
	private byte[] InputStreamToByte(InputStream is) throws IOException {
		ByteArrayOutputStream bytestream = new ByteArrayOutputStream();
		int ch;
		while ((ch = is.read()) != -1) {
			bytestream.write(ch);
		}
		byte imgdata[] = bytestream.toByteArray();
		bytestream.close();
		return imgdata;
	}
	public void copy(Context myContext, String ASSETS_NAME,
							String savePath, String saveName) {
		String filename = savePath + "/" + saveName;

		File dir = new File(savePath);
		// 如果目录不中存在，创建这个目录
		if (!dir.exists())
			dir.mkdir();
		try {
			File f=new File(filename);
			if (f.exists()) {
				f.delete();

			}
			InputStream is = myContext.getResources().getAssets()
					.open(ASSETS_NAME);
			FileOutputStream fos = new FileOutputStream(filename);
			byte[] buffer = new byte[7168];
			int count = 0;
			while ((count = is.read(buffer)) > 0) {
				fos.write(buffer, 0, count);
			}
			fos.close();
			is.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
