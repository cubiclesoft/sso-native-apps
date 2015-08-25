**************** SSO Server/Client for Windows, Mac, and Linux ***************

So you want to build a native Windows, Mac, or Linux application against the
SSO server/client infrastructure?  That's very cool.  Alright, here's the
skinny on how you go about doing that.  This is wxWidgets compatible code, so
you will need some stuff to make this work:

1)  wxWidgets 2.9.x or later (I use the SVN trunk).  Build it.

2)  libcurl (whatever the latest version is should work fine).
    Build it with SSL support and optionally with zlib support.  This can be
    tricky, so pre-built libcurl may work better for you depending on your
    platform and compiler.  Be sure to run the mk-ca-bundle.sh or
    mk-ca-bundle.vbs file in the 'lib' directory.  Rename it to 'cacert.pem'.

3)  wxJSON 1.2 or later.  This library is so small that building it inline
    with your project is better than trying to link against it (just add the
    relevant .cpp files directly).

4)  The wxWidgets C++ code located in this directory.

The above aims to be a cross-platform solution to the problem of creating a
native desktop app, which means dragging in a lot of cruft (libraries) to make
it work.

To use OpenSSL (most platforms), you may need to uncomment the lines that
initialize the OpenSSL library in 'main.cpp' and the relevant #include line
located in 'main.h'.

Figuring out the dependencies and getting them playing nice together in a
build can take a couple of days to work it all out.  I can't really provide
specific steps except to use Google searches whenever you get stuck.  My build
platform of choice is Windows and I guarantee that the demo code compiles,
runs, and works great on my computer.  Unfortunately, your mileage will vary.
