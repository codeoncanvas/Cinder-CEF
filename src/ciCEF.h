#pragma once

#include <vector>
#include <memory>       // unique_ptr
#include <algorithm>    // if_any
#include <iso646.h>     // alternative logical operator tokens

#if defined(TARGET_OSX)
#include "include/cef_application_mac.h"
#include "ciCEFMacUpdate.h"
#endif

#include "include/wrapper/cef_helpers.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//#include "cef_helpers.h"
#include "ciCEFClientApp.h"
#include "ciCEFBrowserClient.h"
#include "ciCEFRenderHandler.h"

namespace coc {

    void initCiCEF(int argc, char **argv);
    void updateCEF();

    class ciCEF {

    public:
        ~ciCEF();
        void setup(std::string url, ci::ivec2 size);
        void macHack();
        void update();
        void draw(ci::vec2 pos = ci::vec2(0));
        void reshape( ci::ivec2 size );
        void cleanup();

        ci::gl::TextureRef getTexture();
        void registerEvents();
        void unregisterEvents();
        void enableResize();
        void windowResized();
        void onLoadStart();
        void onLoadEnd(int httpStatusCode);

//		void executeJS(const string& command);
       void bindCallFromJS(CefRefPtr<CefListValue> args);

        bool mV8ContextCreated = false; // Don't set this
        bool isReady() const { return mV8ContextCreated; }


        void keyDown( ci::app::KeyEvent event );
        void keyUp( ci::app::KeyEvent event );
        void mouseDown( ci::app::MouseEvent event );
        void mouseUp( ci::app::MouseEvent event );
        void mouseWheel( ci::app::MouseEvent event );
        void mouseMove( ci::app::MouseEvent event );
        void mouseDrag( ci::app::MouseEvent event );

        bool fixedSize;
        float width_, height_;
        
        #if defined(TARGET_OSX)
        ciCEFMacUpdate *macUpdate;
        #endif

        CefRefPtr<CefBrowser> browser() const { return mBrowserClient->GetBrowser(); }

        CefRefPtr<CefBrowser> mBrowser;
        CefRefPtr<ciCEFBrowserClient> mBrowserClient;
        ciCEFRenderHandler* mRenderHandler;

        CefRefPtr<CefListValue> mMessageFromJS;

        static constexpr auto mScrollSensitivity = 50;  // set arbitrarily
        const std::vector<int> mNonCharKeys = std::vector<int>{
            ci::app::KeyEvent::KEY_UP, ci::app::KeyEvent::KEY_DOWN,
            ci::app::KeyEvent::KEY_LEFT, ci::app::KeyEvent::KEY_RIGHT,
            ci::app::KeyEvent::KEY_HOME, ci::app::KeyEvent::KEY_END,
            ci::app::KeyEvent::KEY_PAGEUP, ci::app::KeyEvent::KEY_PAGEDOWN,
            ci::app::KeyEvent::KEY_DELETE, ci::app::KeyEvent::KEY_BACKSPACE};

    };
}
