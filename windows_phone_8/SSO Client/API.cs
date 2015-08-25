// Single Sign-On integration for Windows Phone 8
// (C) 2013 CubicleSoft.  All Rights Reserved.

using Microsoft.Phone.Controls;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using SSO_Client.Resources;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO.IsolatedStorage;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace SSO_Client
{
    class API
    {
        private bool InProgress;
        private string Lang;

        public API()
        {
            Lang = Thread.CurrentThread.CurrentCulture.Name;
            if (Lang.IndexOf("/") != -1)  Lang = Lang.Substring(0, Lang.IndexOf("/"));
            InProgress = false;
        }

        public Task<JObject> Test()
        {
            string Params = "opt1=" + Uri.EscapeDataString("test");
            Params += "&opt2=" + Uri.EscapeDataString("test2");

            return Execute("action=test", Params);
        }

        public Task<JObject> Logout()
        {
            return Execute("action=logout", null, false);
        }

        async private Task<JObject> Execute(string APIOpts, string POSTParams = null, bool HandleNoLogin = true)
        {
            if (InProgress)
            {
                MessageBox.Show(AppResources.SSOAPI_CallInProgress);

                return null;
            }

            InProgress = true;
            JObject Result = null;
            try
            {
                string URL = SSO.SSO_API_URL + (SSO.SSO_API_URL.IndexOf("?") == -1 ? "?" : "&") + "lang=" + Uri.EscapeDataString(Lang) + "&" + APIOpts;

                var Client = new WebClientEx();
                string Response;
                if (POSTParams != null)  Response = await Client.UploadStringTaskAsyncEx(new System.Uri(URL), "POST", POSTParams);
                else  Response = await Client.DownloadStringTaskAsyncEx(new System.Uri(URL + "&_ts=" + DateTime.UtcNow.Ticks));

                Result = JObject.Parse(Response);
                if (!(bool)Result["success"])
                {
                    if (HandleNoLogin && (string)Result["errorcode"] == "no_login")
                    {
                        if (MessageBox.Show(AppResources.SSOAPI_NoLogin, AppResources.SSOAPI_NoLogin_Title, MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                        {
                            (Application.Current.RootVisual as PhoneApplicationFrame).Navigate(new Uri("/SSO.xaml", UriKind.Relative));
                        }
                    }
                    else
                    {
                        MessageBox.Show((string)Result["error"] + " (" + (string)Result["errorcode"] + ")", AppResources.SSOAPI_ServerError_Title, MessageBoxButton.OK);
                    }

                    Result = null;
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(AppResources.SSOAPI_Exception + "  " + e.Message, AppResources.SSOAPI_Exception_Title, MessageBoxButton.OK);

                Result = null;
            }

            InProgress = false;

            return Result;
        }
    }

    public static class Extensions
    {
        public static Task<string> DownloadStringTaskAsyncEx(this WebClient webClient, Uri uri)
        {
            var tcs = new TaskCompletionSource<string>();

            webClient.Encoding = System.Text.UTF8Encoding.UTF8;

            webClient.DownloadStringCompleted += (s, e) =>
            {
                if (e.Error != null)  tcs.SetException(e.Error);
                else  tcs.SetResult(e.Result);
            };

            webClient.DownloadStringAsync(uri);

            return tcs.Task;
        }

        public static Task<string> UploadStringTaskAsyncEx(this WebClient webClient, Uri uri, string Method, string Data)
        {
            var tcs = new TaskCompletionSource<string>();

            webClient.Encoding = System.Text.UTF8Encoding.UTF8;

            webClient.UploadStringCompleted += (s, e) =>
            {
                if (e.Error != null) tcs.SetException(e.Error);
                else tcs.SetResult(e.Result);
            };

            webClient.UploadStringAsync(uri, Method, Data);

            return tcs.Task;
        }
    }

    public class WebClientEx : WebClient
    {
        private readonly CookieContainer container = new CookieContainer();

        public WebClientEx()
        {
        }

        public WebClientEx(CookieContainer container)
        {
            this.container = container;
        }

        protected override WebRequest GetWebRequest(Uri address)
        {
            WebRequest r = base.GetWebRequest(address);
            var request = r as HttpWebRequest;
            if (request != null)
            {
                request.CookieContainer = container;

                if (IsolatedStorageSettings.ApplicationSettings.Contains("sso_cookies"))
                {
                    Dictionary<string, string> cookies = JsonConvert.DeserializeObject<Dictionary<string, string>>((string)IsolatedStorageSettings.ApplicationSettings["sso_cookies"]);
                    foreach (KeyValuePair<string, string> cookie in cookies)
                    {
                        container.Add(address, new Cookie(cookie.Key, cookie.Value));
                    }
                }
            }

            return r;
        }

        protected override WebResponse GetWebResponse(WebRequest request, IAsyncResult result)
        {
            WebResponse response = base.GetWebResponse(request, result);
            ReadCookies(response);

            return response;
        }

        private void ReadCookies(WebResponse r)
        {
            var response = r as HttpWebResponse;
            if (response != null)
            {
                Dictionary<string, string> cookies;
                if (!IsolatedStorageSettings.ApplicationSettings.Contains("sso_cookies"))  cookies = new Dictionary<string, string>();
                else  cookies = JsonConvert.DeserializeObject<Dictionary<string, string>>((string)IsolatedStorageSettings.ApplicationSettings["sso_cookies"]);

                // Hacky workaround to sync cookies with semi-permanent storage.
                if (response.Headers.AllKeys.Contains("Set-Cookie"))
                {
                    var cookiestr = response.Headers["Set-Cookie"];
                    string[] cookies2 = cookiestr.Split(new string[] {",  "}, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string cookie in cookies2)
                    {
                        string cookie2 = cookie;
                        string value = "";

                        int pos = cookie2.IndexOf(';');
                        if (pos != -1)  cookie2 = cookie2.Substring(0, pos);
                        pos = cookie2.IndexOf('=');
                        if (pos != -1)
                        {
                            value = cookie2.Substring(pos + 1);
                            cookie2 = cookie2.Substring(0, pos);
                        }

                        if (value == "")  cookies.Remove(cookie2);
                        else  cookies[cookie2] = value;
                    }
                }

                IsolatedStorageSettings.ApplicationSettings["sso_cookies"] = JsonConvert.SerializeObject(cookies);
            }
        }
    }
}
