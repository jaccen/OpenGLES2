#pragma once

#include "cinder/Vector.h"

#include "Game.h"
#include "RenderingEngine.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#include <vector>

@interface GLView : UIView {
@private
    Game* mGame;
    RenderingEngine* mRenderingEngine;
    EAGLContext* mContext;
    float mTimestamp;
	std::vector<ci::Vec2i> mTouches;
	bool hasUpdatedOnce;
}

- (void) drawView: (CADisplayLink*) displayLink;

@end
