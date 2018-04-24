# OS X Support

## Current issues with OS X

Support for Mac OS X is a work in progress. Unlike on Windows, Cinder-CEF on Mac relies on a secondary browser process to run which is automatically started by the main process. This setup currently exhibits a similar issue to the Multithreaded Message Loop issue described in the main [README](README.md).

## Building CEF for OS X

1. Download version 3.3282.1734.g8f26fe0 of CEF

```
curl -O "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1734.g8f26fe0_macosx64_minimal.tar.bz2"
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
cmake -G "Xcode"
xcodebuild -target libcef_dll_wrapper -configuration Release -project cef.xcodeproj/
```

4. Copy various files to blocks 'lib' folder (creating folders as needed)
```
Release/Chromium Embedded Framework.framework -> Cinder-CEF/lib/osx
libcef_dll_wrapper/Release/libcef_dll_wrapper.a ->  Cinder-CEF/lib/osx
```

5. Build cef_helper_mac
```
xcodebuild -target cef_helper_mac -configuration Release -project ../../cef_helper_mac/cef_helper_mac.xcodeproj/
```

6. In the project settings go to Build Phases. There we need another `Run Script` build phase. Press the `+` then `New Run Script Phase`. In there goes:

	```
	install_name_tool -change "@rpath/Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework" "@executable_path/../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework" "${BUILT_PRODUCTS_DIR}/${EXECUTABLE_PATH}"
	rsync -aved "$OF_PATH/addons/ofxCef/libs/cef/lib/osx/cef_helper_mac.app" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/"
	rm -rf "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app"
	mv "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/cef_helper_mac.app" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app"
	mv "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app/Contents/MacOS/cef_helper_mac" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app/Contents/MacOS/$PRODUCT_NAME Helper"
	```

4. On the left in the source tree select now each `.cpp` file in the block and set the type in the file inspector to `Objective-C++ Source`.


**Note on the `cef_helper_mac`**

If you change any of the following sources you have to rebuild the `cef_helper_mac`:

- `process_helper_mac.cpp`
- `ofxCEFClientApp.cpp/.h`
- `ofxCEFV8ExtensionHandler.cpp/.h`
- `cefListV8Converter.cpp/.hpp`
