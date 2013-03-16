#pragma once

#include "Game.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface GLView : UIView {
@private
    Game* m_Game;
    RenderingEngine* m_renderingEngine;
    ResourceManager* m_resourceManager;
    EAGLContext* m_context;
    float m_timestamp;
}

- (void) drawView: (CADisplayLink*) displayLink;

@end
