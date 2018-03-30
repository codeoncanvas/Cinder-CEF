#ifndef simpleBrowserClient_hpp
#define simpleBrowserClient_hpp

#include <cef_client.h>
#include <cef_render_handler.h>

// for manual render handler
class BrowserClient : public CefClient {
public:
    BrowserClient(CefRenderHandler *renderHandler)
            : mRenderHandler(renderHandler) {}

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override {
        return mRenderHandler;
    }

    CefRefPtr<CefRenderHandler> mRenderHandler;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif /* simpleBrowserClient_hpp */
