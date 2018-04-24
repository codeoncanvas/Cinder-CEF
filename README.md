# Cinder-CEF

A (currently windows-only) block for integrating CEF in Cinder with support for Cinder <-> CEF communication via Javascript.

OS X Support is a Work-in-progress

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
Release/libcef.lib -> Cinder-CEF/lib/vs/x64/Release/
libcef_dll_wrapper/Release/libcef_dll_wrapper.lib -> Cinder-CEF/lib/vs/x64/Release/

/Release/libcef.lib into Cinder-CEF/lib/vs/x64/Debug/
libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib -> Cinder-CEF/lib/vs/x64/Debug/

Release/*  -> Cinder-CEF/lib/export/vs/x64/
Resources/* -> /export/vs/x64/

Removing the following files from Cinder-CEF/lib/export/vs/x64/    
- libcef.lib
- cef_sandbox.lib
- widevinecdmadapter.dll
```

## Multi-threaded Message Loop

Cinder-CEF is currently setup to run in a single-threaded mode. While some performance benefit may be possible using CEF's multi-threaded-message-loop option, there appears to be some clash with Cinder's message handling that causes CEF to hang until mouse events are received.

## OS X Support

Support for Mac OS X is a work in progress. Unlike on Windows, Cinder-CEF relies on a secondary browser process to run on a Mac which is automatically started. This setup exhibits a similar issue to the Multithreaded Message Loop issue described above. More details on setting up Cinder-CEF for Mac are [here](osx_notes.md)
