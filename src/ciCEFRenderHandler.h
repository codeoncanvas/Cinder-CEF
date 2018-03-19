#ifndef RENDER_HANDLER_H
#define RENDER_HANDLER_H

#include "cef_render_handler.h"

#include <memory>       // unique_ptr
#include <cstdint>      // uint8_t

#include <cef_app.h>
#include <cef_client.h> // IMPLEMENT_REFCOUNTING
#include <cef_render_handler.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//--------------------------------------------------------------
class ciCEFRenderHandler : public CefRenderHandler
{
  
public:
    
    ciCEFRenderHandler(int width, int height) : mWidth{width}, mHeight{height} {
    }

    bool initialized;
    bool bIsRetinaDisplay;
    
    bool bIsShuttingDown;
    

    void init(void);
    void draw(void);
    void reshape(int, int);
    void render();
    

    bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) OVERRIDE;
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;
    void OnPaint(CefRefPtr<CefBrowser> browser,
                 PaintElementType type,
                 const RectList &dirtyRects,
                 const void* buffer,
                 int width,
                 int height) OVERRIDE;
    const CefRect& popup_rect() const { return popup_rect_; }
    const CefRect& original_popup_rect() const { return original_popup_rect_; }
    CefRect GetPopupRectInWebView(const CefRect& original_rect);
    void ClearPopupRects();
    void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
    void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;
    
    
  //private:
    
    CefRect popup_rect_;
    CefRect original_popup_rect_;
    
    int w, h;
    bool transparent_;
    bool show_update_rect_;
    bool initialized_;
    unsigned int texture_id_;
    int view_width_;
    int view_height_;
    CefRect update_rect_;
    

  
private:
    
    int mWidth;
    int mHeight;
    ci::gl::TextureRef mTex;
    std::unique_ptr<uint8_t> mBuffer;
    
    IMPLEMENT_REFCOUNTING(ciCEFRenderHandler);

};

#endif
