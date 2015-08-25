************************ SSO Server/Client Native Apps ***********************

Native applications are pretty sweet.  The SSO server/client comes with
source code that form a basic framework for native apps for a variety of
platforms:

  * Android
  * iOS
  * Windows Phone 8
  * wxWidgets (Windows, Mac, and Linux)

Each native application looks and feels the same and is meant to be used as a
starting point for developing your own application against the SSO server.

Most of the frameworks include a 'readme.txt' file specific to the framework.
It is highly recommended to read the information contained within as each
implementation has its strengths and weaknesses.  The Android implementation
is the baseline and contains the full feature set as described in this file.

There are two buttons in each application:  'Test API' and 'Logout'.  Each
button talks to an API that relies on the SSO client.  'Test API' makes a
standard POST request while 'Logout' makes a standard GET request behind the
scenes.

There is an example API located in the 'api' subdirectory.  This is the code
that the demo apps connect to on the Barebones CMS website.  'login.php' is
probably only going to need a little modification while 'index.php' will be
modified heavily.  It helps to have set up the SSO server and a couple of
clients before working with native apps to understand the architecture.

Within the native apps, all API requests happen on a separate thread from the
UI thread so that the application remains responsive during the API call.
Only one API call can be made at a time per API class instance to avoid
potential issues server-side.  It is up to each individual application as to
how to handle API requests but one-at-a-time may solve certain synchronization
problems.  Feel free to alter the API calls to suit your needs.

When a user isn't signed in, the demo apps display a dialog to let the user
know that they aren't signed in and then give the user the option to sign in.
If the user presses Yes/OK, the app fires up a WebView (or equivalent) and
connects to the SSO client sign in page.  The app ignores all activity in the
WebView until the URL changes to a specific URL.  At that point, it knows that
the user is signed in and proceeds to extract the session cookies using either
native means (the ideal approach) or, in the case of wxWidgets, a hack that
reads the content of the page after the server redirects the browser after
switching the WebView to an alternative URL.

Once the user is signed in, the 'Test API' button successfully runs the test
API.  And, of course, the 'Logout' button successfully signs the user out.

The most common question I run into with the native apps is, "Does the sign
in really have to be a WebView?"  There is no way around the WebView.  That's
the simplest answer.  I hate WebViews in native apps because they are slow and
unresponsive to touch gestures.  The reasons why only a WebView will work are
many.  For example, dealing with multiple sign in providers - each one has
their own unique user interface and provider owners constantly change them.
Another reason is that the Generic Login module's form fields are randomly
named based on the session ID.  I would end up re-implementing the WebView in
a native app - it is easier, faster, and better to just throw a WebView at the
problem.  Once the user is signed in, the WebView goes away and the rest of
the application can be a pure native app.
