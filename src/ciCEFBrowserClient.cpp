#include "include/wrapper/cef_helpers.h"
#include "ciCEF.h"
#include "ciCEFBrowserClient.h"
#include "cinder/Log.h"

ciCEFBrowserClient::ciCEFBrowserClient(
	coc::ciCEF *parent, ciCEFRenderHandler *renderHandler) {
	mParent = parent;
	mHandler = renderHandler;
}

CefRefPtr<CefRenderHandler> ciCEFBrowserClient::GetRenderHandler() {
	return mHandler;
}

CefRefPtr<CefLoadHandler> ciCEFBrowserClient::GetLoadHandler() { return this; }

CefRefPtr<CefLifeSpanHandler> ciCEFBrowserClient::GetLifeSpanHandler() {
	return this;
}

void ciCEFBrowserClient::OnLoadStart(
	CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
	mParent->onLoadStart();
}

void ciCEFBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame, int httpStatusCode) {
	mParent->onLoadEnd(httpStatusCode);
}

bool ciCEFBrowserClient::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser, CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> args = message->GetArgumentList();

	// Retrieve the argument values.
	std::string name = message->GetName().ToString();

	CI_LOG_I("Message: " << name << " ; " << args);
	
	std::string jsBindPrefix = "js-bind-";
	if (name.compare(0, jsBindPrefix.size(), jsBindPrefix) == 0) {
		mParent->bindCallFromJS(args);
		return true;
	}
	else if (name == "OnV8ContextCreated") {
		mParent->mV8ContextCreated = true;
	}
	else if (name == "OnV8ContextReleased") {
		mParent->mV8ContextCreated = false;
	}

	return true;
}

void ciCEFBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	// Must be executed on the UI thread.
	CEF_REQUIRE_UI_THREAD();

	if (!mBrowser.get()) {
		// Keep a reference to the main browser.
		mBrowser = browser;
		mBrowserId = browser->GetIdentifier();
	}

	// Keep track of how many browsers currently exist.
	mBrowserCount++;
}

void ciCEFBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	// Must be executed on the UI thread.
	CEF_REQUIRE_UI_THREAD();

	if (mBrowserId == browser->GetIdentifier()) {
		// Free the browser pointer so that the browser can be destroyed.
		mBrowser = NULL;
	}

	if (--mBrowserCount == 0) {}
}
