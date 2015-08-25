// Single Sign-On integration for Windows Phone 8
// (C) 2013 CubicleSoft.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using System.IO.IsolatedStorage;
using Newtonsoft.Json;

namespace SSO_Client
{
    public partial class SSO : PhoneApplicationPage
    {
        public static string SSO_API_URL = "https://barebonescms.com/sso/native_app/api/";
        public static string SSO_LOGIN_URL = "https://barebonescms.com/sso/native_app/api/login.php";

        public SSO()
        {
            InitializeComponent();

            Web.IsScriptEnabled = true;
            Web.Navigate(new Uri(SSO_LOGIN_URL));
        }

        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            if (Web.CanGoBack)
            {
                Web.GoBack();
                e.Cancel = true;
            }

            base.OnBackKeyPress(e);
        }

        private void Web_LoadCompleted(object sender, NavigationEventArgs e)
        {
            string URL = e.Uri.AbsoluteUri;
            Dictionary<string, List<string>> Params = GetURLParameters(URL);
            List<string> Action;

            if (Params[""][0] == SSO_LOGIN_URL && Params.TryGetValue("action", out Action) && Action[0] == "sso_done")
            {
                Dictionary<string, string> cookies = new Dictionary<string, string>();
                foreach (Cookie cookie in Web.GetCookies())
                {
                    cookies[cookie.Name] = cookie.Value;
                }

                IsolatedStorageSettings.ApplicationSettings["sso_cookies"] = JsonConvert.SerializeObject(cookies);

                NavigationService.GoBack();
            }
        }

        public static Dictionary<string, List<string>> GetURLParameters(string URL)
        {
            Dictionary<string, List<string>> Params = new Dictionary<string, List<string>>();

            try
            {
                string[] URLParts = URL.Split('?');

    			// Technically not a parameter, but no actual parameter should be an empty string.
                List<string> Values = new List<string>();
                Params[""] = Values;
                Values.Add(URLParts[0]);

                if (URLParts.Length > 1)
                {
                    string Query = URLParts[1];
                    foreach (string Param in Query.Split('&'))
                    {
                        string[] Pair = Param.Split('=');
                        string Key = Uri.UnescapeDataString(Pair[0]);
                        string Value = "";
                        if (Pair.Length > 1)  Value = Uri.UnescapeDataString(Pair[1]);

                        if (!Params.TryGetValue(Key, out Values))
                        {
                            Values = new List<string>();
                            Params[Key] = Values;
                        }

                        Values.Add(Value);
                    }
                }
            }
            catch (Exception)
            {
            }

            return Params;
        }
    }
}