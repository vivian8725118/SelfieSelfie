//
//  IntroSequence.h
//
//  Created by Soso Limited on 6/9/15.
//
//

#pragma once

#include "Image.h"
#include "cinder/Filesystem.h"
#include "cinder/Signals.h"
#include "cinder/Timer.h"
#include "choreograph/Choreograph.h"

namespace soso {

///
/// IntroSequence is a series of images that play out on the timeline.
///
class IntroSequence
{
public:
	/// Set up the sequence to load images from the given path.
	void setup( const ci::fs::path &iImageBasePath );

	/// Set a function to be called when the intro animations are complete.
	void setFinishFn( const std::function<void ()> &iFunction ) { finishFn = iFunction; }

	void start();
	void stop();

	void draw();

private:
	void update();
	std::function<void ()>		finishFn;
	ch::Timeline							timeline;
	std::vector<Image>				items;
	ci::Timer									timer;

	double										endTime = 0.0;
	void showItem( const ci::fs::path &iPath, float duration );
	void showBlank( float duration );
	void showFlash();
	void handleFinish();
	ci::ColorA								overlayColor = ci::ColorA::hex( 0xffF8ED31 );
	ch::Output<ci::Color>				backgroundColor = ci::Color::gray( 0.12f );
	ch::Output<float>						backgroundAlpha = 1.0f;
	ci::signals::ScopedConnection updateConnection;
};

} // namespace soso
