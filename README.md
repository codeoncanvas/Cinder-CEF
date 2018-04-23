# Cinder-CEF

A (currently windows-only) block for integrating CEF in Cinder with support for Cinder <-> CEF communication via Javascript.

Relies on a pre-built version of libCEF version which can be created using Git Bash on Windows using the following steps.
Depends on cmake https://cmake.org/download/ and VS 2015

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




