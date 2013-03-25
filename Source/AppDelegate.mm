#import "AppDelegate.h"

@implementation RootViewController

-(BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return UIInterfaceOrientationIsLandscape( toInterfaceOrientation );
}

@end

@implementation AppDelegate

- (void) applicationDidFinishLaunching: (UIApplication*) application
{
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    
    m_window = [[UIWindow alloc] initWithFrame: screenBounds ];
	RootViewController* root = [[RootViewController alloc] init];
	root.wantsFullScreenLayout = YES;
	m_window.rootViewController = root;
    [m_window makeKeyAndVisible];
	
	UIViewController* controller = [[UIViewController alloc] init];
    m_view = [[GLView alloc] initWithFrame: CGRectMake( 0, 0, 1024, 768 )];
	[controller.view addSubview:m_view];
	[m_window.rootViewController presentModalViewController:controller animated:NO];
}

- (void) dealloc
{
    [m_view release];
    [m_window release];
    [super dealloc];
}

@end
