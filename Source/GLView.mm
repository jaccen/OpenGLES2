#import "GLView.h"

#define GL_RENDERBUFFER 0x8d41

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
                
        int width = CGRectGetWidth( frame ) * self.contentScaleFactor;
        int height = CGRectGetHeight( frame ) * self.contentScaleFactor;
		mRenderingEngine->setup( width, height, self.contentScaleFactor );
        mGame->setup(width, height );
        
        [self drawView: nil];
        mTimestamp = CACurrentMediaTime();
		
		self.multipleTouchEnabled = YES;
        
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
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
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - mTimestamp;
        mTimestamp = displayLink.timestamp;
        mGame->update( elapsedSeconds );
    }
    
    mRenderingEngine->draw();
    [mContext presentRenderbuffer:GL_RENDERBUFFER];
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