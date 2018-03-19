#ifndef BROWSER_CLIENT_H
#define BROWSER_CLIENT_H

#include "cef_client.h"
//#include "ciCEF.h"

#include "ciCEFRenderHandler.h"

namespace coc {
    class ciCEF;
}

//--------------------------------------------------------------
class ciCEFBrowserClient : public CefClient, public CefLoadHandler, public CefLifeSpanHandler
{
public:
    ciCEFBrowserClient(coc::ciCEF* parent, ciCEFRenderHandler* renderHandler);

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;
    
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
    
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                              CefProcessId source_process,
                              CefRefPtr<CefProcessMessage> message) OVERRIDE;
    
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame);
    

    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           int httpStatusCode) OVERRIDE;
    
    // CefLifeSpanHandler methods.
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    CefRefPtr<CefBrowser> GetBrowser() const { return m_Browser; }
    
private:
    CefRefPtr<CefRenderHandler> handler;
    coc::ciCEF* _parent;
    
    CefRefPtr<CefBrowser> m_Browser;
    int m_BrowserId;
    int m_BrowserCount;
    
    
    IMPLEMENT_REFCOUNTING(ciCEFBrowserClient);

};

#endif
