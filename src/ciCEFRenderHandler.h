#ifndef RenderHandler_h
#define RenderHandler_h

#include <memory>       // unique_ptr
#include <cstdint>      // uint8_t

#include <cef_app.h>
#include <cef_client.h> // IMPLEMENT_REFCOUNTING
#include <cef_render_handler.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"


class ciCEFRenderHandler : public CefRenderHandler {
public:
	ciCEFRenderHandler(int width, int height) : mWidth{ width }, mHeight{ height } {
		const size_t bufferSize = mWidth * mHeight * 4;
		mBuffer = std::unique_ptr<uint8_t>{ new uint8_t[bufferSize] };
		memset(mBuffer.get(), 0, bufferSize);
		mTex = ci::gl::Texture::create(mBuffer.get(), GL_BGRA, mWidth, mHeight);
		mTex->setTopDown();  // specified to flip pixels vertically
	}

	// Retrieve the view rectangle which is relative to screen coordinates
	// returns `true` if the rectangle was provided.
	// See `GetScreenInfo`
	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override {
		rect = CefRect{ 0, 0, mWidth, mHeight };
		return true;
	}

	// Called when an element should be painted. Pixel values passed to this
	// method are scaled relative to the view coordinates based on the value of
	// `CefScreenInfo.device_scale_factor` returned from `GetScreenInfo`.
	// type indicates whether the element is the view or the popup widget.
	// @buffer contains the pixel data for the whole image.
	// @dirtyRects contains the set of rectangles in pixel coordinates that need
	// to be repainted.
	// buffer will be @width * @height * 4 bytes in size and represent a BGRA
	// image with an upper-left origin
	void OnPaint(CefRefPtr<CefBrowser> browser,
		PaintElementType type, const RectList &dirtyRects, const void *buffer,
		int width, int height) override {

		std::cout << "onPaint \t" << mWidth << ", " << mHeight << std::endl;
		mTex->update(buffer, GL_BGRA, GL_UNSIGNED_BYTE, 0, width, height);
	}

	ci::gl::TextureRef getTexture() { return mTex; }

	void reshape(int width, int height) {
		mWidth = width;
		mHeight = height;
	}

private:
	int mWidth;
	int mHeight;
	ci::gl::TextureRef mTex;
	std::unique_ptr<uint8_t> mBuffer;


	IMPLEMENT_REFCOUNTING(ciCEFRenderHandler);
};

#endif /* RenderHandler_h */
