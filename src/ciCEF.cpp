//#define TARGET_OSX CINDER_COCOA
#define TARGET_WIN32 CINDER_MSW

//#include "cinder/Log.h"
#include <stdexcept>

#if defined(TARGET_OSX)
#include <Cocoa/Cocoa.h>
#endif

#include <cef_app.h>
#include "cinder/Log.h"
#include "ciCEFRenderHandler.h"
#include "ciCEF.h"

namespace coc {
    
    using namespace ci;
    using namespace ci::app;
    using namespace std;

    void initCiCEF(int argc, char **argv) {

		std::cout << "initCiCEF" << endl;
		
    
    #if defined(TARGET_OSX)
        CefMainArgs mainArgs(argc, argv);
    
    #elif defined(TARGET_WIN32)
        CefMainArgs mainArgs(::GetModuleHandle(NULL));
    
    // These flags must match the Chromium values.
    const char kProcessType[] = "type";
    const char kRendererProcess[] = "renderer";
    
    //#if defined(OS_LINUX)
    //    const char kZygoteProcess[] = "zygote";
    //
    //#endif // defined(OS_LINUX)
    
    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());
    
	string args = command_line->GetCommandLineString().ToString();
    std::cout << "Args: " << command_line->GetCommandLineString().ToString() << '\n';

    
    // Create a ClientApp of the correct type.
    
    CefRefPtr<CefApp> app;
    
    // The command-line flag won't be specified for the browser process.
    if (command_line->HasSwitch(kProcessType)) {
        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        CI_LOG_I("Process type: " << process_type);
        
        if (process_type == kRendererProcess) {
            app = new ciCEFClientApp();
        } else {
            //app = new ClientAppOther();
        }
        
    } else {
        //app = new ClientAppBrowser();
    }
    
    // Execute the secondary process, if any.
    int exitCode = CefExecuteProcess(mainArgs, app, NULL);
	
    if (exitCode >= 0) { throw std::runtime_error{"CEF process execution failed"}; }
    
#endif // defined(TARGET_WIN32)
    
    //CefRefPtr<ofxCEFClientApp> app(new ofxCEFClientApp);
    
    CefSettings cefSettings;
    cefSettings.background_color = 0xFFFF00FF;
    cefSettings.single_process = false;
    cefSettings.windowless_rendering_enabled = true;
    cefSettings.command_line_args_disabled = true;
	cefSettings.remote_debugging_port = 8080;
    
#if defined(TARGET_OSX)
    cefSettings.remote_debugging_port = 8080;
    // On Windows this leads to:
    // tcp_socket_win.cc bind() retunred an error: an attempt was made to access a socket in a way forbidden by its access permissions
#endif
    
    // This could be used on windows, could improve performance
    // If you enable this, 'CefDoMessageLoopWork()' should not be called
    cefSettings.multi_threaded_message_loop = true;
    
    // Implement external message pump?! see 'main_message_loop_external_pump' in 'ceftest/shared/browser'
    //cefSettings.external_message_pump = true;
    
    // Default is LOGSEVERITY_INFO
    cefSettings.log_severity = LOGSEVERITY_DISABLE;
    
	
    // Initialize CEF
    const auto didInitialize = CefInitialize(mainArgs, cefSettings, nullptr, nullptr);
	
    if (not didInitialize) { throw std::runtime_error{"CEF process execution failed"}; }
    
#if defined(TARGET_WIN32)

	// Take back std:cout from CEF for CI_LOG
	AllocConsole();
	freopen("conout$", "w", stdout);
	auto str = new std::ofstream("conout$");
	std::cout.rdbuf(str->rdbuf());

#endif

    }
    
    
    
    ciCEF::~ciCEF() {
        
        //TODO: clean up
		unregisterEvents();

		// Shut down CEF.
		if (browser()) {
			browser()->GetHost()->CloseBrowser(true);
		}

		// The following call to CefShutdown make the app crash on OS X. Still not working on Windows neither.
		// http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11441&p=24037&hilit=AutoreleasePoolPage#p24037
		CefShutdown();
        
    }
    
    void ciCEF::setup(string url, ci::ivec2 size) {
        
        CefWindowInfo windowInfo;
        mRenderHandler = new ciCEFRenderHandler{ size.x, size.y };
        
#if defined(TARGET_OSX)
        
        NSView * view =  (NSView *) getWindow()->getNative();
        NSWindow * cocoaWindow = [ view window ];
        [cocoaWindow setReleasedWhenClosed:NO];
        windowInfo.SetAsWindowless(view);
        
#elif defined(TARGET_WIN32)
		HWND hWnd = (HWND)ci::app::getWindow()->getNative();
        windowInfo.SetAsWindowless(hWnd);
#endif
        
        
        if (size.x <= 0 && size.y <= 0) {
            width_ = size.x;
            height_ = size.y;
            
#if defined(TARGET_OSX)
            //        if (mRenderHandler->bIsRetinaDisplay) { //todo: add
            //            width_ = size.x*2;
            //            height_ = size.y*2;
            //        }
#endif
            //        enableResize(); //todo: register resize event
        }
        else {
            width_ = size.x;
            height_ = size.y;
        }
        
        // Tell the mRenderHandler about the size
        // Do it before the using it in the browser client
        mRenderHandler->reshape(width_, height_); //todo: equivalent needed?
        
        
        CefBrowserSettings settings;
        settings.webgl = STATE_ENABLED;
        settings.windowless_frame_rate = 60;
        settings.background_color = 0x00FFFFFF;
        settings.web_security = STATE_DISABLED;
        
        mBrowserClient = new ciCEFBrowserClient{this, mRenderHandler};
        CefBrowserHost::CreateBrowser(windowInfo, mBrowserClient.get(), url, settings, NULL);
        //mBrowser = CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(),
        //        url, settings, nullptr);
        
        if(!mBrowserClient) { CI_LOG_E( "client pointer is NULL" ); }

    }
    
    void ciCEF::update() {
        // Single iteration of message loop, does not block
        //CefDoMessageLoopWork();
		MSG msg;

		// Run the application message loop.
		if (GetMessage(&msg, NULL, 0, 0)) {
			//CI_LOG_I("received a message");
			//TranslateMessage(&msg);
			//DispatchMessage(&msg);
			/*if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}*/
		}

     
    }
    
	//--------------------------------------------------------------
	void ciCEF::executeJS(const std::string& command) {

		if (!browser()) { return; }

		CefRefPtr<CefFrame> frame = browser()->GetMainFrame();
		frame->ExecuteJavaScript(command, frame->GetURL(), 0);

	}

    void ciCEF::onLoadStart() {
        
    }
    
    void ciCEF::onLoadEnd(int httpStatusCode) {
        
    }
    
    void ciCEF::registerEvents() {
        getWindow()->getSignalKeyDown().connect( signals::slot( this, &ciCEF::keyDown) );
        getWindow()->getSignalKeyUp().connect( signals::slot( this, &ciCEF::keyUp) );
        getWindow()->getSignalMouseDown().connect( signals::slot( this, &ciCEF::mouseDown) );
        getWindow()->getSignalMouseUp().connect( signals::slot( this, &ciCEF::mouseUp) );
        getWindow()->getSignalMouseWheel().connect( signals::slot( this, &ciCEF::mouseWheel) );
        getWindow()->getSignalMouseMove().connect( signals::slot( this, &ciCEF::mouseMove) );
        getWindow()->getSignalMouseDrag().connect( signals::slot( this, &ciCEF::mouseDrag) );
    }

	void ciCEF::unregisterEvents() {
		//TODO disconnect signals
		//getWindow()->getSignalKeyDown().disconnect( signals::slot( this, &CinderCEF::keyDown) );
		//getWindow()->getSignalKeyUp().disconnect( signals::slot( this, &CinderCEF::keyUp) );
		//getWindow()->getSignalMouseDown().disconnect( signals::slot( this, &CinderCEF::mouseDown) );
		//getWindow()->getSignalMouseUp().disconnect( signals::slot( this, &CinderCEF::mouseUp) );
		//getWindow()->getSignalMouseWheel().disconnect( signals::slot( this, &CinderCEF::mouseWheel) );
		//getWindow()->getSignalMouseMove().disconnect( signals::slot( this, &CinderCEF::mouseMove) );
		//getWindow()->getSignalMouseDrag().disconnect( signals::slot( this, &CinderCEF::mouseDrag) );
	}
    
    void ciCEF::enableResize(){
        //ofAddListener(ofEvents().windowResized, this, &ofxCEF::windowResized);
        getWindow()->getSignalResize().connect( signals::slot( this, &ciCEF::windowResized) );
    }
    
    void ciCEF::reshape( ci::ivec2 size ) {
        //TODO this doesn't work fully
        
        //mRenderHandler->reshape(size.x,size.y);
        
        // Check host is available
        if (browser() == nullptr) { return; }
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
        browserHost->WasResized();
    }
    
    void ciCEF::keyDown( KeyEvent event ) {
        
        // Check host is available
        if (browser() == nullptr) { return; }
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
        CefKeyEvent cefKeyEvent;
        
        // hack inherited from ofxCEF
        // https://github.com/ofZach/ofxCef/blame/master/src/ofxCEF.cpp#L425
        if (std::any_of(mNonCharKeys.cbegin(), mNonCharKeys.cend(),
                        [&event](int k){ return k == event.getCode(); })) {
            cefKeyEvent.type = KEYEVENT_KEYDOWN;
            
        } else {
            cefKeyEvent.type = KEYEVENT_CHAR;
            cefKeyEvent.character = event.getChar();
        }
		cefKeyEvent.windows_key_code = event.getChar();
        cefKeyEvent.native_key_code = event.getNativeKeyCode();
		browser()->GetHost()->SendKeyEvent(cefKeyEvent);
    }
    
    void ciCEF::keyUp( KeyEvent event ) {
        
        // Check host is available
        if (browser() == nullptr) { return; }
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
        CefKeyEvent cefKeyEvent;
        
        if (std::any_of(mNonCharKeys.cbegin(), mNonCharKeys.cend(),
                        [&event](int key){ return key == event.getCode(); })) {
            cefKeyEvent.type = KEYEVENT_CHAR;
            cefKeyEvent.character = event.getChar();
            
        } else {
            cefKeyEvent.type = KEYEVENT_KEYUP;
        }
		cefKeyEvent.windows_key_code = event.getChar();
        cefKeyEvent.native_key_code = event.getNativeKeyCode();
		browser()->GetHost()->SendKeyEvent(cefKeyEvent);
    }
    
    void ciCEF::mouseDown( MouseEvent event ) {
        
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
		const auto mouseButtonType = MBT_LEFT;
        //event.isLeft() ? MBT_LEFT :
        //event.isRight() ? MBT_RIGHT :  // causes crash, so treat as middle
        //TODO can use EVENTFLAG_LEFT_MOUSE_BUTTON
        //MBT_MIDDLE;
        
        CefMouseEvent cefMouseEvent;
        cefMouseEvent.x = event.getX();
        cefMouseEvent.y = event.getY();
        cefMouseEvent.modifiers = event.getNativeModifiers();
        
        browser()->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, false, 1);
    }
    
    void ciCEF::windowResized(){
        if (!fixedSize) {
            width_ = getWindowWidth();
            height_ = getWindowHeight();
            reshape(vec2{width_, height_});
            //mRenderHandler->init();  // not implemented
        }
        //cefgui->browser->Reload();
    }
    
    void ciCEF::mouseUp( MouseEvent event ) {
        
        // Check host is available
        if (browser() == nullptr) { return; }
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
		const auto mouseButtonType = MBT_LEFT;
        //event.isLeft() ? MBT_LEFT :
        //event.isRight() ? MBT_RIGHT :  // causes crash, so treat as middle
        //MBT_MIDDLE;
        
        CefMouseEvent cefMouseEvent;
        cefMouseEvent.x = event.getX();
        cefMouseEvent.y = event.getY();
        cefMouseEvent.modifiers = event.getNativeModifiers();
        
        browser()->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, true, 1);
    }
    
    void ciCEF::mouseWheel( MouseEvent event ) {
        
        // Check host is available
        if (browser() == nullptr) { return; }
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
        const int deltaY = std::round(event.getWheelIncrement() * mScrollSensitivity);
        
        CefMouseEvent cefMouseEvent;
        cefMouseEvent.x = event.getX();
        cefMouseEvent.y = event.getY();
        cefMouseEvent.modifiers = event.getNativeModifiers();
        
        browser()->GetHost()->SendMouseWheelEvent(cefMouseEvent, 0, deltaY);
    }
    
    void ciCEF::mouseMove( MouseEvent event ) {
        
        // Check host is available
        if (browser() == nullptr) { return; }
        const auto browserHost = browser()->GetHost();
        if (browserHost == nullptr) { return; }
        
        CefMouseEvent cefMouseEvent;
        cefMouseEvent.x = event.getX();
        cefMouseEvent.y = event.getY();
        cefMouseEvent.modifiers = event.getNativeModifiers();
        
        browserHost->SendMouseMoveEvent(cefMouseEvent, false);
    }
    
    void ciCEF::mouseDrag( MouseEvent event ) {

		if (browser() == nullptr) { return; }
		const auto browserHost = browser()->GetHost();
		if (browserHost == nullptr) { return; }

		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = event.getX();
		cefMouseEvent.y = event.getY();
		cefMouseEvent.modifiers = event.getNativeModifiers();

		browserHost->SendMouseMoveEvent(cefMouseEvent, false);
    }
    
    void ciCEF::bindCallFromJS(CefRefPtr<CefListValue> args) {
        
		ciCEFJSMessageArgs msg;
		msg.args = args;
		//msg.args = args;
		CI_LOG_I("Received JS call in ciCEF");

		signalJS.emit(msg);
    }
    
    void ciCEF::draw( ci::vec2  pos ) {
    
        if (!mV8ContextCreated) return; 

        gl::TextureRef tex = getTexture();
        if (tex) gl::draw( tex );

        // TODO implement cursor changes, see CefRenderHandler::OnCursorChange
    }
    
    ci::gl::TextureRef ciCEF::getTexture()
    {
        return mRenderHandler->getTexture();
    }
    
    
    
    
}
