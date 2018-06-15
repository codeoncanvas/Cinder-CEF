//#include "cinder/Log.h"
#include <stdexcept>

#include <cef_app.h>
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "ciCEFRenderHandler.h"
#include "ciCEF.h"

namespace coc {
    
    using namespace ci;
    using namespace ci::app;
    using namespace std;

    void initCiCEF(int argc, char **argv) {

		std::cout << "initCiCEF" << endl;
		
    
    #if defined(CINDER_MAC)
        CefMainArgs mainArgs(argc, argv);
    
    #elif defined(CINDER_MSW)
        CefMainArgs mainArgs(::GetModuleHandle(NULL));
    
    // These flags must match the Chromium values.
    const char kProcessType[] = "type";
    const char kRendererProcess[] = "renderer";
    
    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());
    
    std::cout << "Args: " << command_line->GetCommandLineString().ToString() << '\n';

    
    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    
    // The command-line flag won't be specified for the browser process.
    if (command_line->HasSwitch(kProcessType)) {
        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        CI_LOG_I("Process type: " << process_type);
        
        if (process_type == kRendererProcess) {
            app = new ciCEFClientApp();
			app->OnBeforeCommandLineProcessing(process_type, command_line);
        }
    }
        
    // Execute the secondary process, if any.
    int exitCode = CefExecuteProcess(mainArgs, app, NULL);
	
    if (exitCode >= 0) { throw std::runtime_error{"CEF process execution failed"}; }
    
#endif // defined(TARGET_WIN32)
    
    CefSettings cefSettings;
    cefSettings.background_color = 0xFFFF00FF;
    cefSettings.single_process = false;
    cefSettings.windowless_rendering_enabled = true;
    cefSettings.command_line_args_disabled = true;
	cefSettings.remote_debugging_port = 8080;
    
#if defined(CINDER_MAC)
    cefSettings.remote_debugging_port = 8080;
    // On Windows this leads to:
    // tcp_socket_win.cc bind() retunred an error: an attempt was made to access a socket in a way forbidden by its access permissions
#endif
    
    // This could be used on windows, could improve performance
    // If you enable this, 'CefDoMessageLoopWork()' should not be called
    // cefSettings.multi_threaded_message_loop = true;
    
    // Implement external message pump?! see 'main_message_loop_external_pump' in 'ceftest/shared/browser'
    //cefSettings.external_message_pump = true;
    
    // Default is LOGSEVERITY_INFO
    cefSettings.log_severity = LOGSEVERITY_INFO;
	
    // Initialize CEF
    const auto didInitialize = CefInitialize(mainArgs, cefSettings, nullptr, nullptr);
	
    if (not didInitialize) { throw std::runtime_error{"CEF process execution failed"}; }
    
#if defined(CINDER_MSW)

	// Take back std:cout from CEF for CI_LOG
	AllocConsole();
	freopen("conout$", "w", stdout);
	auto str = new std::ofstream("conout$");
	std::cout.rdbuf(str->rdbuf());

#endif

    }
    
    ciCEF::~ciCEF() {
        
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
        
#if defined(CINDER_MAC)
        
        windowSetup(windowInfo);
        
#elif defined(CINDER_MSW)
        
		HWND hWnd = (HWND)ci::app::getWindow()->getNative();
        windowInfo.SetAsWindowless(hWnd);

#endif
        
        width_ = size.x;
        height_ = size.y;
        
        // Tell the mRenderHandler about the size
        // Do it before the using it in the browser client
        mRenderHandler->reshape(width_, height_); //todo: equivalent needed?
        
        CefBrowserSettings settings;
        settings.webgl = STATE_ENABLED;
        settings.windowless_frame_rate = 30;
        settings.background_color = 0x00FFFFFF;
        settings.web_security = STATE_DISABLED;
        
        mBrowserClient = new ciCEFBrowserClient{this, mRenderHandler};
        CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(), url, settings, NULL);
        
        if(!mBrowserClient) { CI_LOG_E( "client pointer is NULL" ); }

    }
   

    void ciCEF::update() {
        
        // Single iteration of message loop, does not block
		CefDoMessageLoopWork();

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
    
    void ciCEF::enableResize(){
        
		getWindow()->getSignalResize().connect( signals::slot( this, &ciCEF::windowResized) );
    
	}
    
    void ciCEF::reshape( ci::ivec2 size ) {
    
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
    
        // This is a hack to get something drawing on mac
        #if defined(CINDER_MAC)
            CefDoMessageLoopWork();
        #endif
    
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
		CI_LOG_I("Received JS call in ciCEF");

		signalJS.emit(msg);

    }
    
    void ciCEF::draw( ci::vec2  pos ) {
    
        if (!mV8ContextCreated) return; 

        gl::TextureRef tex = getTexture();
        if (tex) gl::draw( tex );

        // TODO implement cursor changes, see CefRenderHandler::OnCursorChange
    }
    
    ci::gl::TextureRef ciCEF::getTexture() {
        
        return mRenderHandler->getTexture();
    
    }
    
}
