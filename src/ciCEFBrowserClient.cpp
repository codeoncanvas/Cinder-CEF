#include "ciCEFBrowserClient.h"
#include "ciCEF.h"
#include "include/wrapper/cef_helpers.h"

//--------------------------------------------------------------
ciCEFBrowserClient::ciCEFBrowserClient(coc::ciCEF* parent, ciCEFRenderHandler* renderHandler){
    _parent = parent;
    handler = renderHandler;
}

//--------------------------------------------------------------
CefRefPtr<CefRenderHandler> ciCEFBrowserClient::GetRenderHandler(){
    return handler;
}

//--------------------------------------------------------------
CefRefPtr<CefLoadHandler> ciCEFBrowserClient::GetLoadHandler(){
    return this;
}

CefRefPtr<CefLifeSpanHandler> ciCEFBrowserClient::GetLifeSpanHandler(){
    return this;
}

//--------------------------------------------------------------
void ciCEFBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame){
    _parent->onLoadStart();
}


//--------------------------------------------------------------
void ciCEFBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode){
    _parent->onLoadEnd(httpStatusCode);
}

//--------------------------------------------------------------
bool ciCEFBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                             CefProcessId source_process,
                                             CefRefPtr<CefProcessMessage> message){
    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Retrieve the argument values.
    std::string name = message->GetName().ToString();
    
    std::cout << "Message: " << name << " ; " << args;
    
    
    std::string jsBindPrefix = "js-bind-";
    if (name.compare(0, jsBindPrefix.size(), jsBindPrefix) == 0) {
        _parent->bindCallFromJS(args);
        return true;
    }
    else if (name == "OnV8ContextCreated") {
        _parent->V8ContextCreated = true;
    }
    else if (name == "OnV8ContextReleased") {
        _parent->V8ContextCreated = false;
    }

    return true;
}


void ciCEFBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();
    
    if (!m_Browser.get())   {
        // Keep a reference to the main browser.
        m_Browser = browser;
        m_BrowserId = browser->GetIdentifier();
    }
    
    // Keep track of how many browsers currently exist.
    m_BrowserCount++;
}

void ciCEFBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();
    
    if (m_BrowserId == browser->GetIdentifier()) {
        // Free the browser pointer so that the browser can be destroyed.
        m_Browser = NULL;
    }
    
    if (--m_BrowserCount == 0) {

    }
}
