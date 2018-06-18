#!/bin/bash

CEF_VERSION="cef_binary_3.3282.1734.g8f26fe0_macosx64_minimal"
BIN_SOURCE="http://opensource.spotify.com/cefbuilds/"

echo "Downloading CEF binary release..."
echo $BIN_SOURCE$CEF_VERSION".tar.bz2"
curl -O $BIN_SOURCE$CEF_VERSION".tar.bz2"
if [ $? != 0 ]; then echo "Error downloading CEF binary release."; exit 1; fi

echo "Unpacking..."
tar -xf $CEF_VERSION.tar.bz2
if [ $? != 0 ]; then echo "Error unpacking CEF binary release."; exit 1; fi
mv $CEF_VERSION cef
echo "Cleaning up..."
rm $CEF_VERSION.tar.bz2

echo "Building CEF..."
cd cef
cmake -G "Xcode"
xcodebuild -target libcef_dll_wrapper -configuration Release -project cef.xcodeproj/
cd ..

echo "Copying files to block lib folder..."
mkdir -p "libs/cef/include"
mkdir -p "libs/cef/lib/osx"
mv "cef/include" "libs/cef/"
mv "cef/Release/Chromium Embedded Framework.framework" "libs/cef/lib/osx/"
mv "cef/libcef_dll_wrapper/Release/libcef_dll_wrapper.a" "libs/cef/lib/osx/"

# Build cef_helper_mac
cd libs/cef/
xcodebuild -target cef_helper_mac -configuration Release -project ../../cef_helper_mac/cef_helper_mac.xcodeproj/
if [ $? != 0 ]; then echo "Error unpacking CEF binary release."; exit 1; fi
echo "Done!"
