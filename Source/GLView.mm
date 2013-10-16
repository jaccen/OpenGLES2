#import "GLView.h"
#include "TouchInput.h"

#define GL_RENDERBUFFER 0x8d41

const static int kFps = 60;

@implementation GLView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id) initWithFrame: (CGRect) frame
{
    if (self = [super initWithFrame:frame])
    {
		// Enable retina support if available
		if ( [[UIScreen mainScreen] respondsToSelector:@selector(scale)] ) {
			self.contentScaleFactor = [[UIScreen mainScreen] scale];
		}
		
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) self.layer;
        eaglLayer.opaque = YES;

        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        mContext = [[EAGLContext alloc] initWithAPI:api];
        
        if (!mContext || ![EAGLContext setCurrentContext:mContext]) {
            [self release];
            return nil;
        }
        
		mRenderingEngine		= RenderingEngine::get();
		mGame					= Game::get();

        [mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable: eaglLayer];
                
        int width = CGRectGetWidth( self.bounds );
        int height = CGRectGetHeight( self.bounds );
		mRenderingEngine->setup( width, height, self.contentScaleFactor );
        mGame->setup( width, height );
        
        [self drawView: nil];
        mTimestamp = CACurrentMediaTime();
		
		self.multipleTouchEnabled = YES;
        
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		hasUpdatedOnce = false;
    }
    return self;
}

-(void) dealloc
{
	[super dealloc];
}

- (void) drawView: (CADisplayLink*) displayLink
{
	const float frameRate = 1.0 / (float) kFps;
	
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - mTimestamp;
		//if ( elapsedSeconds >= frameRate ) {
			TouchInput::get()->update( elapsedSeconds );
			mGame->update( elapsedSeconds );
			mRenderingEngine->update( elapsedSeconds );
			mTimestamp = displayLink.timestamp;
			hasUpdatedOnce = true;
		//}
    }
    
	if ( hasUpdatedOnce ) {
		mRenderingEngine->draw();
		mGame->debugDraw();
		[mContext presentRenderbuffer:GL_RENDERBUFFER];
	}
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	mTouches.clear();
	for( UITouch* touch in touches ) {
		CGPoint location  = [touch locationInView:self];
		mTouches.push_back( ci::Vec2i( location.x, location.y ) );
	}
	TouchInput::get()->touchesBegan( mTouches );
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	mTouches.clear();
	for( UITouch* touch in touches ) {
		CGPoint location  = [touch locationInView:self];
		mTouches.push_back( ci::Vec2i( location.x, location.y ) );
	}
	TouchInput::get()->touchesEnded( mTouches );
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	mTouches.clear();
	for( UITouch* touch in touches ) {
		CGPoint location  = [touch locationInView:self];
		mTouches.push_back( ci::Vec2i( location.x, location.y ) );
	}
	TouchInput::get()->touchesMoved( mTouches );
}

@end