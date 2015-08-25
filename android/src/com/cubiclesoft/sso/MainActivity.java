// Single Sign-On integration example for Android
// (C) 2013 CubicleSoft.  All Rights Reserved.

package com.cubiclesoft.sso;

import org.json.JSONObject;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		final API api = new API(this);

		Button bv = (Button)findViewById(R.id.test_api);
		bv.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				api.test(msgcallback);
			}
		});

		bv = (Button)findViewById(R.id.logout);
		bv.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				api.logout(msgcallback);
			}
		});
	}

	private API.Callback msgcallback = new API.Callback() {
		@Override
		public void result(JSONObject json) {
			try {
				Toast.makeText(getApplicationContext(), json.getString("msg"), Toast.LENGTH_SHORT).show();
			} catch (Exception e) {
			}
		}
	};
}
