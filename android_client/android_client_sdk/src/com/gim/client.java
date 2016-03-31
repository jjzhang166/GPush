package com.gim;

public class client {
	public native int init(listener lstr, String pubkey);

	public native int stop();

	public native int login(String srvip, int srvport, String cliver, String cid, String token);

	public native int logout(String cid);

	// 0:nothing, 1:error, 2:all
	public native int loglevel(int lvl);

	static {
		System.loadLibrary("clientsdk");
	}
}