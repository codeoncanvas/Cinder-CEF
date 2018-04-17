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
#include "cinder/Rand.h"


class ciCEFRenderHandler : public CefRenderHandler {
public:
	ciCEFRenderHandler(int width, int height) : mWidth{ width }, mHeight{ height } {
		const size_t bufferSize = mWidth * mHeight * 4;
		mBuffer = std::unique_ptr<uint8_t>{ new uint8_t[bufferSize] };
		memset(mBuffer.get(), 0, bufferSize);
		mTex = ci::gl::Texture::create(mBuffer.get(), GL_BGRA, mWidth, mHeight);
		mTex->setTopDown();  // specified to flip pixels vertically
		mSurface = ci::Surface::create( mWidth, mHeight, ci::SurfaceChannelOrder::BGRA);
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

		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

		CefRenderHandler::RectList::const_iterator i = dirtyRects.begin();
		
		for (; i != dirtyRects.end(); ++i) {
			const CefRect& rect = *i;

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, rect.x);
			glPixelStorei(GL_UNPACK_SKIP_ROWS, rect.y);

			//uint8_t *img = (uint8_t*)buffer;
			//uint8_t *src = NULL;
			//uint8_t *newimage = new uint8_t[rect.width * rect.height * 4];
			//uint8_t *dst = newimage;
			//short lineWidth = rect.width * 4; //Width of line to be copied

			//for (int y = rect.y; y < rect.y + rect.height; y++)
			//{
			//	src = img + (y * width * 4 + rect.x * 4);
			//	memcpy(dst, src, lineWidth);
			//	dst += lineWidth;

			//}

			mTex->update(buffer, GL_BGRA, GL_UNSIGNED_BYTE, 0, rect.width, rect.height, ci::vec2(rect.x, rect.y));
			//delete newimage;
		}

		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

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
	ci::SurfaceRef mSurface;


	IMPLEMENT_REFCOUNTING(ciCEFRenderHandler);
};

#endif /* RenderHandler_h */
