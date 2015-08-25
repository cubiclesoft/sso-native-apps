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
using SSO_Client.Resources;
using Newtonsoft.Json.Linq;

namespace SSO_Client
{
    public partial class MainPage : PhoneApplicationPage
    {
        private API api = new API();

        // Constructor
        public MainPage()
        {
            InitializeComponent();

            // Sample code to localize the ApplicationBar
            //BuildLocalizedApplicationBar();
        }

        private async void TestAPI_Click(object sender, RoutedEventArgs e)
        {
            var Result = await api.Test();
            DisplayMessage(Result);
        }

        private async void Logout_Click(object sender, RoutedEventArgs e)
        {
            var Result = await api.Logout();
            DisplayMessage(Result);
        }

        private void DisplayMessage(JObject Result)
        {
            if (Result != null)  MessageBox.Show((string)Result["msg"]);
        }

        // Sample code for building a localized ApplicationBar
        //private void BuildLocalizedApplicationBar()
        //{
        //    // Set the page's ApplicationBar to a new instance of ApplicationBar.
        //    ApplicationBar = new ApplicationBar();

        //    // Create a new button and set the text value to the localized string from AppResources.
        //    ApplicationBarIconButton appBarButton = new ApplicationBarIconButton(new Uri("/Assets/AppBar/appbar.add.rest.png", UriKind.Relative));
        //    appBarButton.Text = AppResources.AppBarButtonText;
        //    ApplicationBar.Buttons.Add(appBarButton);

        //    // Create a new menu item with the localized string from AppResources.
        //    ApplicationBarMenuItem appBarMenuItem = new ApplicationBarMenuItem(AppResources.AppBarMenuItemText);
        //    ApplicationBar.MenuItems.Add(appBarMenuItem);
        //}
    }
}