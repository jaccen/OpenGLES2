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
        m_context = [[EAGLContext alloc] initWithAPI:api];
        
        if (!m_context) {
            api = kEAGLRenderingAPIOpenGLES1;
            m_context = [[EAGLContext alloc] initWithAPI:api];
        }
        
        if (!m_context || ![EAGLContext setCurrentContext:m_context]) {
            [self release];
            return nil;
        }
        
        m_resourceManager		= new ResourceManager();
		m_renderingEngine		= new RenderingEngine();
		m_Game		= new Game( m_renderingEngine, m_resourceManager );

        [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable: eaglLayer];
                
        int width = CGRectGetWidth( frame );
        int height = CGRectGetHeight( frame );
        m_Game->setup(width * self.contentScaleFactor, height * self.contentScaleFactor );
        
        [self drawView: nil];
        m_timestamp = CACurrentMediaTime();
        
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

-(void) dealloc
{
	delete m_Game;
	delete m_renderingEngine;
	delete m_resourceManager;
	
	[super dealloc];
}

- (void) drawView: (CADisplayLink*) displayLink
{
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - m_timestamp;
        m_timestamp = displayLink.timestamp;
        m_Game->update( elapsedSeconds );
    }
    
    m_Game->draw();
    [m_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
    UITouch* touch = [touches anyObject];
    CGPoint location  = [touch locationInView: self];
    m_Game->touchBegan(ci::Vec2i(location.x, location.y));
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
    UITouch* touch = [touches anyObject];
    CGPoint location  = [touch locationInView: self];
    m_Game->touchEnded(ci::Vec2i(location.x, location.y));
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
    UITouch* touch = [touches anyObject];
    CGPoint previous  = [touch previousLocationInView: self];
    CGPoint current = [touch locationInView: self];
    m_Game->touchMoved(ci::Vec2i(previous.x, previous.y),
                                      ci::Vec2i(current.x, current.y));
}

@end