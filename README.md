# Cinder-CEF

A (currently windows-only) block for integrating CEF in Cinder with support for Cinder <-> CEF communication via Javascript.
This effort owes much to [this branch of ofxCEF](https://github.com/michaelbaisch/ofxCef/tree/make_it_work) for OpenFrameworks

OS X Support is a Work-in-progress. More details on setting up Cinder-CEF for Mac are [here](osx_notes.md)

## Building CEF

Cinder-CEF depends on a pre-built version of libCEF version which can be created using Git Bash on Windows using the following steps.
Depends on [cmake](https://cmake.org/download/) and VS 2015.

For proprietary video codecs it is necessary to build CEF from source with specific compiler flags set. Further details are [here](proprietary_codec_support.md)

1. Download version 3.3282.1734.g8f26fe0 of CEF

```
curl -O "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1734.g8f26fe0_windows64_minimal.tar.bz2"
```

2. Unpack and clean up

```
tar -xf cef_binary_3.3282.1734.g8f26fe0_windows64_minimal.tar.bz2
mv cef_binary_3.3282.1734.g8f26fe0_windows64_minimal cef
rm cef_binary_3.3282.1734.g8f26fe0_windows64_minimal.tar.bz2
```

3. Build

```
cd cef
cmake -G "Visual Studio 14 2015 Win64" -DUSE_SANDBOX=Off
"/c/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe" -target:libcef_dll_wrapper -p:"Configuration=Release;Platform=x64" cef.sln
"/c/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe" -target:libcef_dll_wrapper -p:"Configuration=Debug;Platform=x64" cef.sln
```

4. Copy various files to blocks 'lib' folder (creating folders as needed)

```
include/* -> Cinder-CEF/libs/cef/include/
Release/libcef.lib -> Cinder-CEF/libs/cef/lib/vs/x64/Release/
libcef_dll_wrapper/Release/libcef_dll_wrapper.lib -> Cinder-CEF/lib/vs/x64/Release/

/Release/libcef.lib into Cinder-CEF/libs/cef/lib/vs/x64/Debug/
libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib -> Cinder-CEF/libs/cef/lib/vs/x64/Debug/

Release/*  -> Cinder-CEF/libs/cef/export/vs/x64/
Resources/* -> Cinder-CEF/libs/cef/export/vs/x64/

Removing the following files from Cinder-CEF/lib/export/vs/x64/    
- libcef.lib
- cef_sandbox.lib
- widevinecdmadapter.dll
```

## Multi-threaded Message Loop

Cinder-CEF is currently setup to run in a single-threaded mode. While some performance benefit may be possible using CEF's multi-threaded-message-loop option, there appears to be some clash with Cinder's message handling that causes CEF to hang until mouse events are received. Efforts continue on a (currently broken) branch [here](https://github.com/codeoncanvas/Cinder-CEF/tree/multithreaded)

## Command-line arguments

Mileage varies but there are a number of CEF command-line arguments which may improve performance in some cases. Some of these are included but commented out inside `ciCEFClient.cpp`. These allow things like gpu-vsync and begin-frame-scheduling to be enabled/disabled.
