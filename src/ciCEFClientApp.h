#ifndef CINDERCEFCLIENTAPP_H
#define CINDERCEFCLIENTAPP_H

#include <string>
#include <iostream>

#include "ciCEFV8ExtensionHandler.h"
#include "include/cef_app.h"
#include "include/cef_client.h"

class ciCEFClientApp : public CefApp, public CefRenderProcessHandler {
public:
    ciCEFClientApp() : mV8Handler(new ciCEFV8ExtensionHandler(this)){};

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
        return this;
    }

    //void OnWebKitInitialized() OVERRIDE;

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context) OVERRIDE;

    void OnContextReleased(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context) OVERRIDE;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) OVERRIDE;

    virtual void OnBeforeCommandLineProcessing(const CefString &process_type,
            CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    CefRefPtr<ciCEFV8ExtensionHandler> mV8Handler;
    CefRefPtr<CefV8Context> mV8Context;

    IMPLEMENT_REFCOUNTING(ciCEFClientApp);
};

#endif
