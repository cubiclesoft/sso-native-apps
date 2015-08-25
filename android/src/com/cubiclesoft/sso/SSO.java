// Single Sign-On integration for Android
// (C) 2013 CubicleSoft.  All Rights Reserved.

package com.cubiclesoft.sso;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.webkit.CookieSyncManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class SSO extends Activity {
	public static String SSO_API_URL = "https://barebonescms.com/sso/native_app/api/";
	public static String SSO_LOGIN_URL = "https://barebonescms.com/sso/native_app/api/login.php";

    @SuppressLint("SetJavaScriptEnabled")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.sso);

		String url = SSO_LOGIN_URL;
		String lang = getResources().getConfiguration().locale.getLanguage() + "-" + getResources().getConfiguration().locale.getCountry();
		try {
			url += (url.indexOf("?") == -1 ? "?" : "&") + "lang=" + URLEncoder.encode(lang, "UTF-8");
		} catch (Exception e) {
		}

		WebView wv = (WebView)findViewById(R.id.web);
		WebSettings wvSettings = wv.getSettings();
		wvSettings.setJavaScriptEnabled(true);
		wv.setWebViewClient(new SSOWebViewClient());
		wv.loadUrl(url);
	}

	@Override
	protected void onResume() {
		super.onResume();

		CookieSyncManager.getInstance().startSync();
	}

	@Override
	protected void onPause() {
		super.onPause();

		CookieSyncManager.getInstance().stopSync();
	}

	private class SSOWebViewClient extends WebViewClient {
		@Override
		public void onPageFinished(WebView view, String url) {
			super.onPageFinished(view, url);

			CookieSyncManager.getInstance().sync();

			Map<String, List<String>> params = SSO.GetURLParameters(url);
			if (params.get("").get(0).equals(SSO_LOGIN_URL) && params.containsKey("action") && params.get("action").get(0).equals("sso_done"))  finish();
		}

		@Override
		public boolean shouldOverrideUrlLoading(WebView view, String url) {
			return false;
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK)
		{
			WebView wv = (WebView)findViewById(R.id.web);
			if (wv != null && wv.canGoBack())
			{
				wv.goBack();

				return true;
			}
		}

		return super.onKeyDown(keyCode, event);
	}

	// Swiped from:  http://stackoverflow.com/questions/1667278/parsing-query-strings-in-java/
	public static Map<String, List<String>> GetURLParameters(String url)
	{
		Map<String, List<String>> params = new HashMap<String, List<String>>();

		try {
			String[] urlParts = url.split("\\?");

			// Technically not a parameter, but no actual parameter should be an empty string.
			List<String> values = new ArrayList<String>();
			params.put("", values);
			values.add(urlParts[0]);

			if (urlParts.length > 1)
			{
				String query = urlParts[1];
				for (String param : query.split("&"))
				{
					String pair[] = param.split("=");
					String key = URLDecoder.decode(pair[0], "UTF-8");
					String value = "";
					if (pair.length > 1)  value = URLDecoder.decode(pair[1], "UTF-8");

					values = params.get(key);
					if (values == null)
					{
						values = new ArrayList<String>();
						params.put(key, values);
					}

					values.add(value);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return params;
    }
}
