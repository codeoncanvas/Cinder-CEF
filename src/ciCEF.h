#pragma once

#include <vector>
#include <memory>       // unique_ptr
#include <algorithm>    // if_any
#include <iso646.h>     // alternative logical operator tokens

//#include "include/cef_application_mac.h"
#include "include/wrapper/cef_helpers.h"

#include "cinder/app/App.h"
#include "cinder/app/Event.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//#include "cef_helpers.h"
#include "ciCEFClientApp.h"
#include "ciCEFBrowserClient.h"
#include "ciCEFRenderHandler.h"

namespace coc {
    
    void initCiCEF(int argc, char **argv);

	class ciCEFJSMessageArgs {

		public:
			CefRefPtr<CefListValue> args;
	};

    
    class ciCEF {
        
    public:
        ~ciCEF();
        void setup(std::string url, ci::ivec2 size);
        void update();
        void draw(ci::vec2 pos = ci::vec2(0));
        void reshape( ci::ivec2 size );
   
        ci::gl::TextureRef getTexture();
        void registerEvents();
        void enableResize();
        void windowResized();
        void onLoadStart();
        void onLoadEnd(int httpStatusCode);
        
		void executeJS(const std::string& command);
		ci::signals::Signal<void(ciCEFJSMessageArgs&)> signalJS;

		template <typename ArgumentsType, class ListenerClass>
		void bind(ListenerClass * listener, std::string functionName, void(ListenerClass::*listenerMethod)(ArgumentsType&) );

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

	template <typename ArgumentsType, class ListenerClass>
	void ciCEF::bind(ListenerClass * listener, std::string functionName, void(ListenerClass::* listenerMethod)(ArgumentsType&)){

		// Tell renderer process that we want
		CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("js-make-bind");

		// Retrieve the argument list object. Set the function name
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		args->SetSize(1);
		args->SetString(0, functionName);

		// Send the message to the render process
		if (browser()) {
			browser()->SendProcessMessage(PID_RENDERER, message);
		}

        signalJS.connect(cinder::signals::slot(listener, listenerMethod));

	}
}
