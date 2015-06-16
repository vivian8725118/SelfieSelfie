//
//  TouchArea.cpp
//
//  Created by Soso Limited on 6/10/15.
//
//

#include "TouchArea.h"
#include "cinder/app/Window.h"
#include "cinder/app/App.h"

using namespace soso;

std::unique_ptr<TouchArea> TouchArea::create( const ci::Rectf &iBounds, const std::function<void ()> &iCallback )
{
	return std::unique_ptr<TouchArea>( new TouchArea( iBounds, iCallback ) );
}

void TouchArea::connect()
{
	auto window = ci::app::getWindow();
	touchBeginConnection = window->getSignalTouchesBegan().connect( std::bind( &TouchArea::touchBegin, this, std::placeholders::_1 ) );
	touchEndConnection = window->getSignalTouchesEnded().connect( std::bind( &TouchArea::touchEnd, this, std::placeholders::_1 ) );
}

void TouchArea::disconnect()
{
	touchBeginConnection.disconnect();
	touchEndConnection.disconnect();
}

void TouchArea::touchBegin( const ci::app::TouchEvent &iEvent )
{
	if( ! wasInside ) {
		for( auto &touch : iEvent.getTouches() ) {
			if( bounds.contains( touch.getPos() ) ) {
				wasInside = true && isEnabled();
				trackedTouch = touch.getId();
				break;
			}
		}
	}
}

void TouchArea::touchEnd( const ci::app::TouchEvent &iEvent )
{
	bool should_call = false;
	if( wasInside ) {
		for( auto &touch : iEvent.getTouches() ) {
			if( touch.getId() == trackedTouch ) {
				wasInside = false;
				trackedTouch = 0;
				should_call = bounds.contains( touch.getPos() );
				break;
			}
		}
	}

	should_call = should_call && isEnabled() && callback;
	// call last, in case callback destroys this object.
	if( should_call ) {
		callback();
	}
}
