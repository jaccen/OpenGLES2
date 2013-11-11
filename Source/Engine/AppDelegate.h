#include "GLView.h"

@interface RootViewController : UIViewController
@end

@interface AppDelegate : NSObject <UIApplicationDelegate> {
@private
    UIWindow* m_window;
    GLView* m_view;
}

@end

