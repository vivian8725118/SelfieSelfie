#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Shader.h"

#include "IntroSequence.h"
#include "SelfieExperience.h"

#include "cinder/MotionManager.h"
#include "cinder/Timeline.h"
#include "asio/asio.hpp"

#ifdef CINDER_ANDROID
	#include "cinder/android/CinderAndroid.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace soso;

class SelfieSelfieApp : public App {
public:
	SelfieSelfieApp();

	void setup() override;
	void playIntroAndGetOrientation();
	void determineSizeIndicator();
	void update() override;
	void draw() override;

  void focusGained();
  void focusLost();

	void updateCamera();
	void updateOrientationOffset();
	void showLandscape();

private:
	IntroSequence											introduction;
	std::unique_ptr<SelfieExperience> selfieExperience;
	std::string												sizeIndicator = "xhdpi";
};

SelfieSelfieApp::SelfieSelfieApp()
{
	#ifdef CINDER_ANDROID
		ci::android::setActivityGainedFocusCallback( [this] { focusGained(); } );
		ci::android::setActivityLostFocusCallback( [this] { focusLost(); } );
	#endif
}

void SelfieSelfieApp::setup()
{
  CI_LOG_I("Setting up selfie_x_selfie");

	determineSizeIndicator();
	auto image_path = fs::path("img") / sizeIndicator;
	introduction.setup( image_path );
	introduction.setFinishFn( [this] { showLandscape(); } );
}

void SelfieSelfieApp::focusGained()
{
	CI_LOG_I("Focus Gained");
	if( selfieExperience ) {
		selfieExperience->resume();
	}
}

void SelfieSelfieApp::focusLost()
{
	CI_LOG_I("Focus Lost");
	if( selfieExperience ) {
		selfieExperience->pause();
	}
}

void SelfieSelfieApp::determineSizeIndicator()
{
	auto large_side = toPixels( max( getWindowWidth(), getWindowHeight() ) );

	if( large_side <= 1280 ) {
		sizeIndicator = "xhdpi";
	}
	else if( large_side <= 1920 ) {
		sizeIndicator = "xxhdpi";
	}
	else {
		sizeIndicator = "xxxhdpi";
	}

	CI_LOG_I( "Device size: " << large_side << " using images for: " << sizeIndicator );
}

void SelfieSelfieApp::update()
{
	if( selfieExperience )
	{
		selfieExperience->update();
	}
	else
	{
		io_service().post( [this] {
			auto image_path = fs::path("img") / sizeIndicator;
			selfieExperience = unique_ptr<SelfieExperience>( new SelfieExperience( image_path ) );
			introduction.start();
		} );
	}
}

void SelfieSelfieApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	if( selfieExperience )
	{
		selfieExperience->draw();
	}

	introduction.draw();

	#if DEBUG
		auto err = gl::getError();
		if( err ) {
			CI_LOG_E( "Draw gl error: " << gl::getErrorString(err) );
		}
	#endif
}

void SelfieSelfieApp::showLandscape()
{
	if( ! selfieExperience ) {
		auto image_path = fs::path("img") / sizeIndicator;
		selfieExperience = unique_ptr<SelfieExperience>( new SelfieExperience( image_path ) );
	}
	selfieExperience->showLandscape();
}

inline void prepareSettings( app::App::Settings *iSettings )
{
  iSettings->setMultiTouchEnabled();
	iSettings->setHighDensityDisplayEnabled();
	#ifdef CINDER_ANDROID
		iSettings->setKeepScreenOn(true);
	#endif
}

CINDER_APP( SelfieSelfieApp, RendererGl, &prepareSettings )
