//
//  IntroSequence.cpp
//
//  Created by Soso Limited on 6/9/15.
//
//

#include "IntroSequence.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace soso;
using namespace cinder;

void IntroSequence::setup( const ci::fs::path &iImageBasePath )
{
	timeline->clear();
	timeline->reset();
	timeline->stepTo( 0.0f );
	items.clear();
	endTime = 0.0f;

	overlayColor = ColorA::hex( 0xffF8ED31 );
	backgroundColor = Color::gray( 0.12f );
	backgroundAlpha = 1.0f;

	if( ! iImageBasePath.empty() ) {
		CI_LOG_I( "Loading intro images from: " << iImageBasePath );
		showItem( iImageBasePath / "soso-logo.png", 2.0f );
		showItem( iImageBasePath / "selfie-logo.png", 2.0f );
		showBlank( 0.33f );
		auto cd = 0.9f;
		showItem( iImageBasePath / "countdown-3.png", cd );
		showItem( iImageBasePath / "countdown-2.png", cd );
		showItem( iImageBasePath / "countdown-1.png", cd );
	}
	else {
		CI_LOG_W( "Provided image directory is empty." );
	}

	showFlash();

	timeline->add( [this] { handleFinish(); }, endTime );
	timer.start();
}

void IntroSequence::showItem( const ci::fs::path &iPath, float duration )
{
	auto start = endTime;
	endTime += duration + 0.2f;

	auto surf = Surface( loadImage( app::loadAsset( iPath ) ) );
	if( ! surf.isPremultiplied() ) {
		ip::premultiply( &surf );
	}
	auto item = Image( gl::Texture::create( surf ) );
	auto size = ivec2( item.getSize() );
	auto position = vec2( app::getWindowSize() - size ) / 2.0f;
	item.setPosition( position );
	item.setAlpha( 0.0f );
	item.setTint( overlayColor );

	timeline->apply( item.getAlphaAnim(), 1.0f, 0.2f ).easeFn( EaseOutQuad() ).startTime( start );
	timeline->appendTo( item.getAlphaAnim(), 0.0f, 0.2f ).delay( duration );

	items.push_back( item ); // copy brings the anim with it (move makes this clearer in Choreograph)
}

void IntroSequence::showBlank( float duration )
{
	auto start = endTime;
	endTime += duration;
	timeline->appendTo( &backgroundAlpha, 0.0f, 0.2f ).startTime( start );
}

void IntroSequence::showFlash()
{
	auto start = endTime;
	endTime += 0.25f;

	timeline->appendTo( &backgroundColor, Color::gray( 1.0f ), 0.1f ).easeFn( EaseInBack() ).startTime( start );
	timeline->appendTo( &backgroundAlpha, 1.0f, 0.0f, 1.5f ).easeFn( EaseInOutSine() ).startTime( start + 0.075f );
}

void IntroSequence::handleFinish()
{
	if( finishFn ) {
		finishFn();
	}

	items.clear();
}

void IntroSequence::update()
{
	timeline->step( timer.getSeconds() );
	timer.start();
}

void IntroSequence::draw()
{
	gl::ScopedAlphaBlend blend( true );

	if( backgroundAlpha > 0.0f ) {
		gl::ScopedColor color( ColorA( backgroundColor() ) * backgroundAlpha );
		gl::drawSolidRect( app::getWindowBounds() );
	}

	for( auto &item : items ) {
		item.draw();
	}
}