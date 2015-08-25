// Single Sign-On integration for Android
// (C) 2013 CubicleSoft.  All Rights Reserved.

package com.cubiclesoft.sso;

import android.app.Application;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;

// Derive your custom Application class from SSOApplication or copy the code to your Application's onCreate().
public class SSOApplication extends Application {
	public void onCreate() {
		super.onCreate();
 
		// Setup Cookie Manager and Persistence to disk.
		CookieSyncManager.createInstance(this);
		CookieManager.getInstance().setAcceptCookie(true);
	}
}