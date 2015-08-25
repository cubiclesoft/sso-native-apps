************************** SSO Server/Client for iOS *************************

The iOS version of the native apps was developed by my friend Jeff Turman who
volunteered his time to develop it.  While I'm quite thankful I didn't have to
develop it, there are a couple of issues with the code you should be aware of.

The main issue is that all of the code seems to be contained in such a way
that seamless integration in a "multiple ViewController app" is probably a lot
more difficult than it needs to be.  In the other native apps, API calls are
isolated from the Activity/wxFrame/etc. and rely on callbacks to make it easy
to expand the API.  This issue is probably my fault for not mentioning that
users will probably be developing apps with multiple ViewControllers in them
and might reuse the same API in two different ViewControllers.

Overall, this is a decent starting point for an iOS native app.

If you fix any issues, please consider submitting a patch and contributing to
the project.  I don't have access to iOS hardware nor do I have any interest
in spending the gobs of money required to get the hardware to develop/maintain
this one example app.
