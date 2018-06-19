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
For building manually, see the script source, or check the instructions in this repository for [mac](osx_notes.md) and [Windows](win_notes.md).

## Multi-threaded Message Loop

Cinder-CEF is currently setup to run in a single-threaded mode. While some performance benefit may be possible using CEF's multi-threaded-message-loop option, there appears to be some clash with Cinder's message handling that causes CEF to hang until mouse events are received. Efforts continue on a (currently broken) branch [here](https://github.com/codeoncanvas/Cinder-CEF/tree/multithreaded)

## Command-line arguments

Mileage varies but there are a number of CEF command-line arguments which may improve performance in some cases. Some of these are included but commented out inside `ciCEFClient.cpp`. These allow things like gpu-vsync and begin-frame-scheduling to be enabled/disabled.
