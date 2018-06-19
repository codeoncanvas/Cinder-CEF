#!/bin/bash

# Enable bash strict mode
set -uo pipefail
IFS=$'\n\t'

# Detect OS
platform='unknown'
unamestr=`uname`
if [[ "$unamestr" == 'Darwin' ]]; then platform='mac'
elif [[ "$unamestr" =~ ^MINGW ]]; then platform='win'
elif [[ "$unamestr" == 'Linux' ]]; then platform='linux'
fi

if [[ "$platform" == 'linux' ]]; then
    echo "Linux platform not supported"
    exit 1

elif [[ "$platform" == 'unknown' ]]; then
    echo "Please run this script with Terminal on Mac, or Git Bash/MINGW terminal on Windows"
    exit 1
fi

# Set CEF Version
BIN_SOURCE="http://opensource.spotify.com/cefbuilds/"

if [[ "$platform" == 'mac' ]]; then
  CEF_VERSION="cef_binary_3.3282.1734.g8f26fe0_macosx64_minimal"

elif [[ "$platform" == 'win' ]]; then
  CEF_VERSION="cef_binary_3.3282.1734.g8f26fe0_windows64_minimal"
fi

# Get CEF binary release
DOWNLOAD_CEF=1

if [[ -e "$CEF_VERSION.tar.bz2" ]]; then
    echo "Found $CEF_VERSION.tar.bz2 in the current directory"
    read -p "Use this file? [y]/n: " -n 1 -r
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then DOWNLOAD_CEF=0; fi
fi

if [ $DOWNLOAD_CEF == 1 ]; then
    echo "Downloading CEF binary release..."
    echo "$BIN_SOURCE$CEF_VERSION.tar.bz2"
    curl -O "$BIN_SOURCE$CEF_VERSION.tar.bz2"
    if [ $? != 0 ]; then echo "Error downloading CEF binary release."; exit 1; fi
fi

echo "Unpacking..."
tar -xf "$CEF_VERSION.tar.bz2"
if [ $? != 0 ]; then echo "Error unpacking CEF binary release."; exit 1; fi
rm -rf cef
mv "$CEF_VERSION" cef

# Run cmake
echo "Building CEF...(cmake)"
cd cef

if [[ "$platform" == 'mac' ]]; then
    cmake -G "Xcode"

elif [[ "$platform" == 'win' ]]; then
    cmake -G "Visual Studio 14 2015 Win64" -DUSE_SANDBOX=Off
fi
if [ $? != 0 ]; then echo "Error preparing build files with cmake."; exit 1; fi

# Compile
echo "Building CEF...(compiling)"
if [[ "$platform" == 'mac' ]]; then
    xcodebuild -quiet -target libcef_dll_wrapper -configuration Release -project cef.xcodeproj/

elif [[ "$platform" == 'win' ]]; then
    'C:/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe' -m -nologo -verbosity:quiet -target:libcef_dll_wrapper -p:"Configuration=Release;Platform=x64" cef.sln
    'C:/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe' -m -nologo -verbosity:quiet -target:libcef_dll_wrapper -p:"Configuration=Debug;Platform=x64" cef.sln
fi
cd ..
if [ $? != 0 ]; then echo "Error building CEF."; exit 1; fi

echo "Copying files to block lib folder..."
mkdir -p "libs/cef/include"

if [[ "$platform" == 'mac' ]]; then
    mkdir -p "libs/cef/lib/osx"
    mv "cef/include" "libs/cef/"
    mv "cef/Release/Chromium Embedded Framework.framework" "libs/cef/lib/osx/"
    mv "cef/libcef_dll_wrapper/Release/libcef_dll_wrapper.a" "libs/cef/lib/osx/"
    echo "Building cef_helper_mac..."
    cd libs/cef/
    xcodebuild -quiet -target cef_helper_mac -configuration Release -project ../../cef_helper_mac/cef_helper_mac.xcodeproj/
    if [ $? != 0 ]; then echo "Error building cef_helper_mac."; exit 1; fi

elif [[ "$platform" == 'win' ]]; then
    rm -rf libs/cef/include/*
    cp -r cef/include/* "libs/cef/include/"

    mkdir -p "libs/cef/lib/vs/x64/Release"
    cp "cef/Release/libcef.lib" "libs/cef/lib/vs/x64/Release"
    cp "cef/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib" "libs/cef/lib/vs/x64/Release/"

    mkdir -p "libs/cef/lib/vs/x64/Debug"
    cp "cef/Release/libcef.lib" "libs/cef/lib/vs/x64/Debug"
    cp "cef/libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib" "libs/cef/lib/vs/x64/Debug/"

    cp -r cef/Release/* "libs/cef/export/vs/x64/"
    cp -r cef/Resources/* "libs/cef/export/vs/x64/"

    rm "libs/cef/export/vs/x64/libcef.lib"
    rm "libs/cef/export/vs/x64/cef_sandbox.lib"
    rm "libs/cef/export/vs/x64/widevinecdmadapter.dll"
fi

#echo "Cleaning up..."
#rm $CEF_VERSION.tar.bz2

echo "Done!"

