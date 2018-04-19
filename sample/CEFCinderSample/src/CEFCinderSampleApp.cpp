#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "ciCEF.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CEFCinderSampleApp : public App {
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;

    coc::ciCEF mWebViewWrapper;
	ci::Font mFont;

private:
	void gotMessageFromJS(const coc::ciCEFJSMessageArgs& msg);
	bool jsFunctionBinded = false;
};

void CEFCinderSampleApp::setup() {

    mFont = Font("Arial", 18.0f);
	CI_LOG_I("Info log test");
   mWebViewWrapper.setup("https://bl.ocks.org/mbostock/2647924", getWindowSize());
   mWebViewWrapper.registerEvents();
}

void CEFCinderSampleApp::mouseDown(MouseEvent event) {
	mWebViewWrapper.executeJS("callFromCinder()");
}

void CEFCinderSampleApp::update() {

	
	/*if (!jsFunctionBinded && mWebViewWrapper.isReady()) {

		CI_LOG_I("Ready to bind to JS");
		mWebViewWrapper.bind(this, "DataToCinder", &CEFCinderSampleApp::gotMessageFromJS);
		jsFunctionBinded = true;
	
	}*/

    mWebViewWrapper.update();
}


void CEFCinderSampleApp::gotMessageFromJS(const coc::ciCEFJSMessageArgs& msg) {

	CI_LOG_I("gotMessageFromJS()");

	for (int i = 0; i < msg.args->GetSize(); i++) {
		CefValueType type = msg.args->GetType(i);
		CI_LOG_I( "  Message index " + std::to_string(i) + " of type " + std::to_string(type));

		switch (type) {
		case VTYPE_BOOL:
			CI_LOG_I("  Bool content: " << std::to_string(msg.args->GetBool(i)));
			break;
		case VTYPE_INT:
			CI_LOG_I("  Int content: " << std::to_string(msg.args->GetInt(i)));
			break;
		case VTYPE_DOUBLE:
			CI_LOG_I("  Double content: " << std::to_string(msg.args->GetDouble(i)));
			break;
		case VTYPE_STRING:
			CI_LOG_I("  String content: " << msg.args->GetString(i).ToString());
			break;

		default:
			CI_LOG_I("  Might be a VTYPE_BINARY, VTYPE_DICTIONARY or VTYPE_LIST");
			break;
		}
	}
}


void CEFCinderSampleApp::draw() {
    gl::clear(Color{1, 1, 1});
    mWebViewWrapper.draw();
	gl::drawString("FPS: " + std::to_string(getAverageFps()), ci::vec2(10.0f, 10.0f), Color::white(), mFont);
}

void prepareSettings(App::Settings *settings) {
	settings->setWindowSize(1920, 1080);
	settings->setConsoleWindowEnabled();
	//settings->setFrameRate(120);
    char *argv[5] = {};
    coc::initCiCEF(0, argv);
}

CINDER_APP(CEFCinderSampleApp, RendererGl, &prepareSettings)
