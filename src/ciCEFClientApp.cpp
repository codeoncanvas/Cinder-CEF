#include <string>
#include <iostream>

//#include "cinder/Log.h"
#include "ciCEFClientApp.h"
#include "cef_build.h"

void ciCEFClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
    mV8Context = context;
    // Retrieve the context's window object.
    // CefRefPtr<CefV8Value> globalObject = context->GetGlobal();

    // printf("context pointer: '%n' \n", context.get());

    // Create an instance of my CefV8Handler object.
    // CefRefPtr<CefV8Handler> mV8Handler = new MyV8Handler();

    // Create the "myfunc" function.
    // CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc",
    // mV8Handler);

    // Add the "myfunc" function to the "window" object.
    // globalObject->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);

    //CI_LOG_I("OnContextCreated\n");
    std::cout << "OnContextCreated\n";

    // Tell browser that V8 Context was created
    CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(CefString("OnV8ContextCreated"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

void ciCEFClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {

    mV8Context = NULL;
    // Tell browser that V8 Context was released
    CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(CefString("OnV8ContextReleased"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

void  ciCEFClientApp::OnBeforeCommandLineProcessing(const CefString &process_type,
	CefRefPtr<CefCommandLine> command_line){


CefString osr(L"-off-screen-rendering-enabled");
command_line->AppendSwitch(osr);

CefString d3d11(L"-disable-d3d11");
command_line->AppendSwitch(d3d11);

//CefString frameScheduling(L"-enable-begin-frame-scheduling");
//command_line->AppendSwitch(frameScheduling);

//CefString vsync(L"-disable-gpu-vsync");
//command_line->AppendSwitch(vsync);

// CefString singleProcess(L"-single-process");
// command_line->AppendSwitch(singleProcess);

// CefString igProxy(L"-no-proxy-server");
// command_line->AppendSwitch(igProxy);

// CefString addFile(L"-allow-file-access-from-files");
// command_line->AppendSwitch(addFile);

// CefString touchEventsDisabled(L"-touch-events=disabled");
// command_line->AppendSwitch(touchEventsDisabled);

// CefString screenCap(L"-allow-http-screen-capture");
// command_line->AppendSwitch(screenCap);

// CefString optimizedUI(L"-touch-optimized-ui=disabled");
// command_line->AppendSwitch(optimizedUI);

// CefString touchSimulated(L"-simulate-touch-screen-with-mouse");
// command_line->AppendSwitch(touchSimulated);
	}

bool ciCEFClientApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Retrieve message name
    std::string name = message->GetName().ToString();

    //CI_LOG_I("Renderer received message: " << name);
    std::cout << "Renderer received message: " << name << '\n';

    if (name == "js-make-bind") {

        // Check if context pointer is null. if nil store jsfunction name to
        // bind them when the context was created.
        // CefRefPtr<CefV8Context> mV8Context =
        // CefV8Context::GetCurrentContext();

        //CI_LOG_I("Context is valid: " << mV8Context.get());
        std::cout << "Context is valid: " << mV8Context.get() << '\n';
        if (mV8Context.get()) {

            //CI_LOG_I("arg size: " << args->GetSize());
            std::cout << "arg size: " << args->GetSize() << '\n';
            std::string jsFunctionName = args->GetString(0).ToString();

            //CI_LOG_I("Context is valid: " << mV8Context->IsValid());
            std::cout << "Context is valid: " << mV8Context->IsValid() << '\n';

            //CI_LOG_I("Context enter: " << mV8Context->Enter());
            std::cout << "Context enter: " << mV8Context->Enter() << '\n';

            CefRefPtr<CefV8Value> globalObject = mV8Context->GetGlobal();

            // Create the "myfunc" function.
            CefRefPtr<CefV8Value> func =
                    CefV8Value::CreateFunction(jsFunctionName, mV8Handler);

            // Add the "myfunc" function to the "window" object.
            globalObject->SetValue(
                    jsFunctionName, func, V8_PROPERTY_ATTRIBUTE_NONE);

            mV8Context->Exit();

            mV8Handler->bindJSFunctionNames.insert(jsFunctionName);

            //CI_LOG_I("Created js function " << jsFunctionName);
            std::cout << "Created js function " << jsFunctionName << '\n';

            return true;
        }
    }

    return false;
}
