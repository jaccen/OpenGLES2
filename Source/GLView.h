#pragma once

#include "ApplicationEngine.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface GLView : UIView {
@private
    ApplicationEngine* m_applicationEngine;
    RenderingEngine* m_renderingEngine;
    ResourceManager* m_resourceManager;
    EAGLContext* m_context;
    float m_timestamp;
}

- (void) drawView: (CADisplayLink*) displayLink;

@end
