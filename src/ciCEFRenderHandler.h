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

		std::cout << "onPaint \t" << mWidth << ", " << mHeight << std::endl;

		memcpy(mSurface->getData(), buffer, 640 * 480 * 4);
		// create a surface
		ci::Surface newSurface(320, 240, true, ci::SurfaceChannelOrder::BGRA);
		newSurface.copyFrom(*mSurface, ci::Area(100, 100, 420, 340), ci::vec2(-100, -100));
		mTex->update(newSurface, 0, ci::vec2(0, 0));
		//mTex->update(newSurface, GL_BGRA, GL_UNSIGNED_BYTE, 0, 639, 480, ci::vec2(1,0));

		////--------------------------------------------------------------

		//	int old_width = view_width_;
		//	int old_height = view_height_;

		//	view_width_ = width;
		//	view_height_ = height;


		//	if (show_update_rect_)
		//		update_rect_ = dirtyRects[0];

		//	glPixelStorei(GL_UNPACK_ROW_LENGTH, view_width_); VERIFY_NO_ERROR;

		//	if (old_width != view_width_ || old_height != view_height_ ||
		//		(dirtyRects.size() == 1 &&
		//			dirtyRects[0] == CefRect(0, 0, view_width_, view_height_))) {
		//		// Update/resize the whole texture.
		//		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0); VERIFY_NO_ERROR;
		//		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0); VERIFY_NO_ERROR;
		//		glTexImage2D(
		//			GL_TEXTURE_2D, 0, GL_RGBA, view_width_, view_height_, 0,
		//			GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer); VERIFY_NO_ERROR;
		//	}
		//	else {
		//		// Update just the dirty rectangles.
		//		CefRenderHandler::RectList::const_iterator i = dirtyRects.begin();
		//		for (; i != dirtyRects.end(); ++i) {

		//			const CefRect& rect = *i;
		//			///printf("%i %i %i %i \n", rect.x, rect.y, rect.width,
		//			//    rect.height);

		//			glPixelStorei(GL_UNPACK_SKIP_PIXELS, rect.x); VERIFY_NO_ERROR;
		//			glPixelStorei(GL_UNPACK_SKIP_ROWS, rect.y); VERIFY_NO_ERROR;
		//			glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width,
		//				rect.height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
		//				buffer); VERIFY_NO_ERROR;
		//		}
		//	}
		//}
		//else if (type == PET_POPUP && popup_rect_.width > 0 &&
		//	popup_rect_.height > 0) {
		//	int skip_pixels = 0, x = popup_rect_.x;
		//	int skip_rows = 0, y = popup_rect_.y;
		//	int w = width;
		//	int h = height;

		//	// Adjust the popup to fit inside the view.
		//	if (x < 0) {
		//		skip_pixels = -x;
		//		x = 0;
		//	}
		//	if (y < 0) {
		//		skip_rows = -y;
		//		y = 0;
		//	}
		//	if (x + w > view_width_)
		//		w -= x + w - view_width_;
		//	if (y + h > view_height_)
		//		h -= y + h - view_height_;

		//	// Update the popup rectangle.
		//	glPixelStorei(GL_UNPACK_ROW_LENGTH, width); VERIFY_NO_ERROR;
		//	glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixels); VERIFY_NO_ERROR;
		//	glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows); VERIFY_NO_ERROR;
		//	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_BGRA,
		//		GL_UNSIGNED_INT_8_8_8_8_REV, buffer); VERIFY_NO_ERROR;
		//}

		//// Disable 2D textures.
		//glDisable(GL_TEXTURE_2D); VERIFY_NO_ERROR;

		//if (transparent_) {
		//	// Disable alpha blending.
		//	glDisable(GL_BLEND); VERIFY_NO_ERROR;
		//}

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
