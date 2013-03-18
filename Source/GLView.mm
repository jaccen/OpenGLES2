#import "GLView.h"

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
        
        if (!mContext) {
            api = kEAGLRenderingAPIOpenGLES1;
            mContext = [[EAGLContext alloc] initWithAPI:api];
        }
        
        if (!mContext || ![EAGLContext setCurrentContext:mContext]) {
            [self release];
            return nil;
        }
        
		mRenderingEngine		= new RenderingEngine();
		mGame					= new Game( mRenderingEngine );

        [mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable: eaglLayer];
                
        int width = CGRectGetWidth( frame );
        int height = CGRectGetHeight( frame );
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
	delete mGame;
	delete mRenderingEngine;
	
	[super dealloc];
}

- (void) drawView: (CADisplayLink*) displayLink
{
	const float frameRate = 1.0 / (float) kFps;
	
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - mTimestamp;
		if ( elapsedSeconds >= frameRate ) {
			mGame->update( elapsedSeconds );
			mRenderingEngine->update( elapsedSeconds );
			mTimestamp = displayLink.timestamp;
			hasUpdatedOnce = true;
		}
    }
    
	if ( hasUpdatedOnce ) {
		mRenderingEngine->draw();
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
	mGame->touchBegan( mTouches );
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	mTouches.clear();
	for( UITouch* touch in touches ) {
		CGPoint location  = [touch locationInView:self];
		mTouches.push_back( ci::Vec2i( location.x, location.y ) );
	}
	mGame->touchEnded( mTouches );
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	mTouches.clear();
	for( UITouch* touch in touches ) {
		CGPoint location  = [touch locationInView:self];
		mTouches.push_back( ci::Vec2i( location.x, location.y ) );
	}
	mGame->touchMoved( mTouches );
}

@end