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
};

void CEFCinderSampleApp::setup() {
    mWebViewWrapper.setup("http://codeoncanvas.cc", getWindowSize());
    mWebViewWrapper.registerEvents();
}

void CEFCinderSampleApp::mouseDown(MouseEvent event) {}

void CEFCinderSampleApp::update() {
    mWebViewWrapper.update();
}

void CEFCinderSampleApp::draw() {
    gl::clear(Color{0, 0, 0});
    mWebViewWrapper.draw();
}

void prepareSettings(App::Settings *settings) {
    char *argv[5] = {};
    coc::initCiCEF(0, argv);
}

CINDER_APP(CEFCinderSampleApp, RendererGl, &prepareSettings)
