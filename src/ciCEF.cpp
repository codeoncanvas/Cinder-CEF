#define TARGET_OSX CINDER_COCOA
#define TARGET_WIN32 CINDER_MSW

//#include "cinder/Log.h"
#include <stdexcept>

#if defined(TARGET_OSX)
#include <Cocoa/Cocoa.h>
#endif

#include <cef_app.h>
#include "cinder/Log.h"
#include "cinderCEFRenderHandler.h"
#include "ciCEF.h"

namespace coc {
    
    using namespace ci;
    using namespace ci::app;
    using namespace std;

    void initCiCEF(int argc, char **argv) {
    
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
    
    std::cout << "Args: " << command_line->GetCommandLineString().ToString() << '\n';
    
    // Create a ClientApp of the correct type.
    
    CefRefPtr<CefApp> app;
    
    // The command-line flag won't be specified for the browser process.
    if (command_line->HasSwitch(kProcessType)) {
        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        CI_LOG_I("Process type: " << process_type);
        
        if (process_type == kRendererProcess) {
            app = new CinderCEFClientApp();
            
            //#if defined(OS_LINUX)
            //        } else if (process_type == kZygoteProcess) {
            //            return ZygoteProcess;
            //
            //#endif // defined(OS_LINUX)
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
    
#if defined(TARGET_OSX)
    cefSettings.remote_debugging_port = 8088;
    // On Windows this leads to:
    // tcp_socket_win.cc bind() retunred an error: an attempt was made to access a socket in a way forbidden by its access permissions
#endif
    
    // This could be used on windows, could improve performance
    // If you enable this, 'CefDoMessageLoopWork()' should not be called
    //cefSettings.multi_threaded_message_loop = true;
    
    // Implement external message pump?! see 'main_message_loop_external_pump' in 'ceftest/shared/browser'
    //cefSettings.external_message_pump = true;
    
    // Default is LOGSEVERITY_INFO
    //cefSettings.log_severity = LOGSEVERITY_VERBOSE;
    
    
    // Initialize CEF
    const auto didInitialize = CefInitialize(mainArgs, cefSettings, nullptr, nullptr);
    if (not didInitialize) { throw std::runtime_error{"CEF process execution failed"}; }
    
    }
    
    ciCEF::~ciCEF() {
        
        //TODO: clean up
        
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
        HWND hWnd = ofGetWin32Window();
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
        CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(), url, settings, NULL);
        //mBrowser = CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(),
        //        url, settings, nullptr);
        
        if(!mBrowserClient) { CI_LOG_E( "client pointer is NULL" ); }
    }
    
    void ciCEF::update() {
        // Single iteration of message loop, does not block
        CefDoMessageLoopWork();
    }
    
    void ciCEF::onLoadStart() {
        
    }
    
    void ciCEF::onLoadEnd(int httpStatusCode) {
        
    }
    
    void ciCEF::registerEvents() {
//        getWindow()->getSignalKeyDown().connect( signals::slot( this, &CinderCEF::keyDown) );
//        getWindow()->getSignalKeyUp().connect( signals::slot( this, &CinderCEF::keyUp) );
//        getWindow()->getSignalMouseDown().connect( signals::slot( this, &CinderCEF::mouseDown) );
//        getWindow()->getSignalMouseUp().connect( signals::slot( this, &CinderCEF::mouseUp) );
//        getWindow()->getSignalMouseWheel().connect( signals::slot( this, &CinderCEF::mouseWheel) );
//        getWindow()->getSignalMouseMove().connect( signals::slot( this, &CinderCEF::mouseMove) );
//        getWindow()->getSignalMouseDrag().connect( signals::slot( this, &CinderCEF::mouseDrag) );
    }
    
    void ciCEF::bindCallFromJS(CefRefPtr<CefListValue> args) {
        //mMessageFromJS = args;
    
        //TODO ofNotifyEvent(messageFromJS, msg, this);
    }
    
    void ciCEF::draw( ci::vec2  pos ) {
    
//        if (!isReady()) { return; }
//
//        gl::TextureRef tex = getTexture();
//        if (tex) gl::draw( tex );
//
//        // TODO implement cursor changes, see CefRenderHandler::OnCursorChange
    }
    
    
}
