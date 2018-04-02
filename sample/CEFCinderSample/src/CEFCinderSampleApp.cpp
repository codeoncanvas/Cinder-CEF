#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
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
	//void gotMessageFromJS(const ofxCEFJSMessageArgs& msg);
};

void CEFCinderSampleApp::setup() {

    mFont = Font("Arial", 18.0f);

   mWebViewWrapper.setup("http://youtube.com", getWindowSize());
   mWebViewWrapper.registerEvents();
    
}

void CEFCinderSampleApp::mouseDown(MouseEvent event) {

}

void CEFCinderSampleApp::update() {

	//if (!jsFunctionBinded && cef.isReady()) {

	//	// Bind js function 'dataToOf' to C++ method 'ofApp::gotMessageFromJS'
	//	cef.bind("dataToCinder", this, &App::gotMessageFromJS);
	//	jsFunctionBinded = true;
	//}

    mWebViewWrapper.update();
}

/*
void CEFCinderSampleApp::gotMessageFromJS(const ofxCEFJSMessageArgs& msg) {

	cout << "gotMessageFromJS()" << endl;

	for (int i = 0; i < msg.args->GetSize(); i++) {
		CefValueType type = msg.args->GetType(i);
		ofLogNotice() << "  Message index " + ofToString(i) + " of type " + ofToString(type);

		switch (type) {
		case VTYPE_BOOL:
			ofLogNotice() << "  Bool content: " << ofToString(msg.args->GetBool(i));
			break;
		case VTYPE_INT:
			ofLogNotice() << "  Int content: " << ofToString(msg.args->GetInt(i));
			break;
		case VTYPE_DOUBLE:
			ofLogNotice() << "  Double content: " << ofToString(msg.args->GetDouble(i));
			break;
		case VTYPE_STRING:
			ofLogNotice() << "  String content: " << msg.args->GetString(i).ToString();
			break;

		default:
			ofLogNotice() << "  Might be a VTYPE_BINARY, VTYPE_DICTIONARY or VTYPE_LIST";
			break;
		}
	}
}
*/

void CEFCinderSampleApp::draw() {
    gl::clear(Color{0, 0, 0});
    mWebViewWrapper.draw();
	gl::drawString("FPS: " + std::to_string(getAverageFps()), ci::vec2(10.0f, 10.0f), Color::white(), mFont);
}

void prepareSettings(App::Settings *settings) {
	settings->setWindowSize(1920, 1080);
    char *argv[5] = {};
    coc::initCiCEF(0, argv);
}

CINDER_APP(CEFCinderSampleApp, RendererGl, &prepareSettings)
