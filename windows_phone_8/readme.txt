******************** SSO Server/Client for Windows Phone 8 *******************

Building the SSO Server/Client for Windows Phone 8 requires a LOT of patience.
The next couple of paragraphs assume that you don't have the Windows Phone 8
SDK installed like I did when I started.  Hopefully these tips help save you
the headache and hair pulling I went through.

First off, you need at least Windows 8 Professional or better to install the
Windows Phone 8 SDK.  If you want to run the SDK emulator, you must have
Second Level Address Translation (SLAT) capable hardware.  If you want to run
test apps on a real device, you need to spend $99/year or join BizSpark,
DreamSpark, or have an active/current MSDN Subscription that will give you a
promo code to use during the signup process.  After that, you can unlock up to
three devices for developer mode and each device can run up to three developer
apps.  Microsoft doesn't tell potential developers any of this up front.

Note:  The Windows Phone 8 SDK emulator requires SLAT-capable hardware and
can't be run in a Hyper-V environment such as VMWare Workstation (although
there are some workarounds, they won't necessarily work).  Just because you
can run Windows 8 doesn't mean the emulator will work.  Software such as
VMWare Workstation won't emulate SLAT in software for performance reasons.
Ultimately, you need to run Windows 8 Pro on real SLAT-capable hardware to run
the emulator, and the emulator is buggy to begin with.  However, the natural
reaction to go get a real device costs money as well and unlocking the device
is a huge hassle if you want to avoid the $99/year fee.  I spent over a month
figuring all of this out.

Assuming you've met all the prerequisites for the emulator or have a real
device on hand, install the Windows Phone 8 SDK and run all the updates.
Register Visual Studio Express with Microsoft to get the free license code so
it won't expire after 30 days.


Now you are ready to use this little project:

Open the 'SSOClient.sln' solution file in Visual Studio.
Install the NuGet package Json.NET:
  Project -> Manage NuGet Packages -> Locate and Install "Json.NET".
  Close the window.
'Build' the solution.
'Debug' the solution to send it to the emulator/device and run it.


Bugs:

There is a bug when signing in that I'm not sure how to fix.  The
barebonescms.com website detects screen width and sets a cookie and reloads
the page if the screen isn't wide enough to select an alternate view.  Only
Windows Phone 8 gets the display size wrong on the reload and so the sign in
selection screen ends up being really tiny in the upper left corner.
Double-tapping the display makes the page jump to the correct size.  This only
happens on first run of the app. If you figure out how fix this bug, let me
know in the forums.
