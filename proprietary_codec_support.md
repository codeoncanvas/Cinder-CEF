# Proprietary Codec Support

Support for proprietary video codecs (e.g. H264) relies on flags being set during the compilation process. As a result it is not possible to use the binary distribution of CEF and you need to build from source yourself.

These instructions are currently Windows-only. OS X version of Cinder-CEF is a work-in-progress and you can reac more about that [here](osx_notes.md)

## Building CEF from source

The process for building CEF from source is very close to the instructions provided under Windows Setup [here](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md)

The key differences:

* We are using VS2015 instead of VS2017
* In addition Visual Studio and the Windows 10 SDK also we also needed to install Ninja from [here](https://github.com/ninja-build/ninja/releases)
* At Step 2: don't download depot_tools.zip, just create a directory for it at `c:\code\depot_tools` - It will be downloaded automatically by the automate-git.py at step 6.
* Skip Step 3
* At Step 4 make add an additional Environment Variable `CEF_VCVARS` with the following value (because we are using VS2015 instead of VS2017)
```
C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat
```
* Make sure the following are included in your PATH Variable
```
c:\code\depot_tools
C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\
C:\Program Files (x86)\Windows Kits\10\bin\10.0.15063.0\x64
```
* At Step 6 - use the following in place of the contents provided
```
set CEF_USE_GN=1
set GYP_MSVS_VERSION=2015
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GN_DEFINES=is_official_build=true proprietary_codecs=true ffmpeg_branding=Chrome
set GN_ARGUMENTS=--ide=vs2015 --sln=cef --filters=//cef/*
python ..\automate\automate-git.py --branch=3282 --download-dir=C:\code\chromium_git --depot-tools-dir=C:\code\depot_tools --no-distrib --no-build
```
* At Step 7 - use thefollowing in place of the contents provided
```
set CEF_USE_GN=1
set GYP_MSVS_VERSION=2015
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GN_DEFINES=is_win_fastlink=true proprietary_codecs=true ffmpeg_branding=Chrome
set GN_ARGUMENTS=--ide=vs2015 --sln=cef --filters=//cef/*
call cef_create_projects.bat
```
* At step 8 - Use the following to create a Debug x64 builds
```
cd c:\code\chromium_git\chromium\src
ninja -C out\Debug_GN_x64 cef
```
* Then Use the following to create a Release x64 build
```
cd c:\code\chromium_git\chromium\src
ninja -C out\Release_GN_x64 cef
```
* Finally we package up a binary version which is interchangeable with the binary described in the [README](README.md)
```
cd c:code/chromium/src/cef/tools
make_distrib.bat --ninja-build --x64-build
```
