// Single Sign-On integration for Android
// (C) 2013 CubicleSoft.  All Rights Reserved.

package com.cubiclesoft.sso;

import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.List;
import java.util.Scanner;

import org.json.JSONObject;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.os.AsyncTask;
import android.widget.Toast;

public class API {
	private Context context;
	private String lang;
	private static boolean inprogress;

	public interface Callback
	{
		public void result(JSONObject json);
	}

	public interface InternalCallback extends Callback
	{
		public String modifyConnection(HttpURLConnection conn) throws Exception;
	}

	public API(Context context)
	{
		this.context = context;
		this.lang = this.context.getResources().getConfiguration().locale.getLanguage() + "-" + this.context.getResources().getConfiguration().locale.getCountry();
		inprogress = false;
	}

	// A test API call.  This is where you would expose your own API calls in a similar fashion.
	public void test(final Callback callback) {
		execute("action=test", new InternalCallback() {
			@Override
			public void result(JSONObject json) {
				callback.result(json);
			}

			@Override
			public String modifyConnection(HttpURLConnection conn) throws Exception {
				String postparams = "opt1=" + URLEncoder.encode("test", "UTF-8");
				postparams += "&opt2=" + URLEncoder.encode("test2", "UTF-8");

				return postparams;
			}
		});
	}

	public void logout(final Callback callback) {
		execute("action=logout", false, new InternalCallback() {
			@Override
			public void result(JSONObject json) {
				callback.result(json);
			}

			@Override
			public String modifyConnection(HttpURLConnection conn) throws Exception {
				return null;
			}
		});
	}

	private class API_Wrapper {
		String url;
		InternalCallback callback;
		boolean handlenologin;

		boolean success;
		String result;
	}

	public void execute(String apiopts, InternalCallback callback) {
		execute(apiopts, true, callback);
	}

	public void execute(String apiopts, boolean handlenologin, InternalCallback callback) {
		if (inprogress)
		{
			Toast.makeText(API.this.context, R.string.sso_api_call_in_progress, Toast.LENGTH_SHORT).show();
			return;
		}

		try {
			API_Wrapper wrapper = new API_Wrapper();
			wrapper.url = SSO.SSO_API_URL + (SSO.SSO_API_URL.indexOf("?") == -1 ? "?" : "&") + "lang=" + URLEncoder.encode(this.lang, "UTF-8") + "&" + apiopts;
			wrapper.callback = callback;
			wrapper.handlenologin = handlenologin;
			wrapper.success = false;
			wrapper.result = "";

			inprogress = true;

			InternalExecuteAPI task = new InternalExecuteAPI();
			task.execute(wrapper);
		} catch (Exception e) {
			Toast.makeText(this.context, String.format(this.context.getResources().getString(R.string.sso_api_call_init_error_toast), (e.getMessage() == null ? "" : e.getMessage())), Toast.LENGTH_SHORT).show();
		}
	}

	private class InternalExecuteAPI extends AsyncTask<API_Wrapper, API_Wrapper, String> {
		@Override
		protected String doInBackground(API_Wrapper... params) {
			if (params.length > 0)
			{
				try {
					URL url = new URL(params[0].url);
					HttpURLConnection conn = (HttpURLConnection)url.openConnection();
					conn.setConnectTimeout(10000);
					conn.setRequestProperty("Connection", "close");

					// Process POST request options.
					String postparams = params[0].callback.modifyConnection(conn);
					if (postparams != null)
					{
						conn.setDoOutput(true);
						conn.setRequestMethod("POST");
						conn.setFixedLengthStreamingMode(postparams.getBytes().length);
						conn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
					}

					// Set cookies in requests.
					android.webkit.CookieManager webkitCookieManager = android.webkit.CookieManager.getInstance();
					String cookie = webkitCookieManager.getCookie(conn.getURL().toString());
					if (cookie != null)  conn.setRequestProperty("Cookie", cookie);

					// Write POST request data.
					if (postparams != null)
					{
						PrintWriter out = new PrintWriter(conn.getOutputStream());
						out.print(postparams);
						out.close();
					}

					String response = "";
					Scanner inStream = new Scanner(conn.getInputStream());
					while (inStream.hasNextLine())  response += inStream.nextLine();
					params[0].result = response;

					// Get cookies from responses and save into the cookie manager.
					List<String> cookieList = conn.getHeaderFields().get("Set-Cookie");
					if (cookieList == null)  cookieList = conn.getHeaderFields().get("set-cookie");
					if (cookieList != null)
					{
						for (String tempcookie : cookieList)  webkitCookieManager.setCookie(conn.getURL().toString(), tempcookie);
					}
					android.webkit.CookieSyncManager.getInstance().sync();

					params[0].success = true;
				} catch (Exception e) {
					if (e.getMessage() != null)  params[0].result = e.getMessage();
				}

				publishProgress(params[0]);
			}

			return "";
		}

		@Override
		protected void onProgressUpdate(API_Wrapper... params) {
			super.onProgressUpdate(params);

			if (params.length > 0)
			{
				if (params[0].success)
				{
					try {
						JSONObject json = new JSONObject(params[0].result);
						if (json.getBoolean("success"))  params[0].callback.result(json);
						else if (params[0].handlenologin && json.getString("errorcode").equals("no_login"))
						{
							AlertDialog.Builder builder = new AlertDialog.Builder(API.this.context);
							builder.setTitle(R.string.sso_api_no_login_title);
							builder.setMessage(R.string.sso_api_no_login_message);
							builder.setPositiveButton(R.string.sso_api_no_login_yes, new OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									API.this.context.startActivity(new Intent(API.this.context, SSO.class));
								}
							});
							builder.setNegativeButton(R.string.sso_api_no_login_no, new OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
								}
							});

							builder.create().show();
						}
						else
						{
							Toast.makeText(API.this.context, String.format(API.this.context.getResources().getString(R.string.sso_api_server_error_message_toast), json.getString("error"), json.getString("errorcode")), Toast.LENGTH_LONG).show();
						}
					} catch (Exception e) {
						Toast.makeText(API.this.context, R.string.sso_api_malformed_json_error_toast, Toast.LENGTH_SHORT).show();
					}
				}
				else
				{
					Toast.makeText(API.this.context, String.format(API.this.context.getResources().getString(R.string.sso_api_read_error_toast), params[0].result), Toast.LENGTH_SHORT).show();
				}
			}
		}

		@Override
		protected void onPostExecute(String result) {
			super.onPostExecute(result);

			inprogress = false;
		}
	}
}
