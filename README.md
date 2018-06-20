A block for integrating CEF in Cinder with support for Cinder <-> CEF communication via Javascript.
This effort owes much to [this branch of ofxCEF](https://github.com/michaelbaisch/ofxCef/tree/make_it_work) for OpenFrameworks

## Building CEF

Cinder-CEF depends on a pre-built version of libCEF version which can be created using [Git Bash](https://gitforwindows.org) on Windows.
Depends on [cmake](https://cmake.org/download/) and VS 2015.

For proprietary video codecs (Windows) it is necessary to build CEF from source with specific compiler flags set. Further details are [here](proprietary_codec_support.md)

1. Clone this repository into the `Cinder/blocks/` directory.
    ```
    git clone https://github.com/codeoncanvas/Cinder-CEF/
    ```
2. Open the Cinder-CEF folder in a terminal (Git Bash on Windows)
3. Run 
    ```
    sh build_cef.sh
    ```

This script will handle downloading and building CEF, and placing items in the correct folders. One it has successfully completed, the provided sample project should work.
For building manually, see the script source, or check the instructions in this repository for [Mac](osx_notes.md) and [Windows](win_notes.md).

## Adding CEF to a new Project

To create a new project with Cinder-CEF, simply use TinderBox.
On macOS there is an additional manual step needed.

1. In the project settings go to Build Phases. There we need another `Run Script` build phase. Press the `+` then `New Run Script Phase`. In there goes:

	```
	install_name_tool -change "@rpath/Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework" "@executable_path/../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework" "${BUILT_PRODUCTS_DIR}/${EXECUTABLE_PATH}"
	rsync -aved "$CINDER_PATH/blocks/Cinder-CEF/libs/cef/lib/osx/cef_helper_mac.app" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/"
	rm -rf "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app"
	mv "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/cef_helper_mac.app" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app"
	mv "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app/Contents/MacOS/cef_helper_mac" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/$PRODUCT_NAME Helper.app/Contents/MacOS/$PRODUCT_NAME Helper"
	```

## Multi-threaded Message Loop

Cinder-CEF is currently setup to run in a single-threaded mode. While some performance benefit may be possible using CEF's multi-threaded-message-loop option, there appears to be some clash with Cinder's message handling that causes CEF to hang until mouse events are received. Efforts continue on a (currently broken) branch [here](https://github.com/codeoncanvas/Cinder-CEF/tree/multithreaded)

## Command-line arguments

Mileage varies but there are a number of CEF command-line arguments which may improve performance in some cases. Some of these are included but commented out inside `ciCEFClient.cpp`. These allow things like gpu-vsync and begin-frame-scheduling to be enabled/disabled.
