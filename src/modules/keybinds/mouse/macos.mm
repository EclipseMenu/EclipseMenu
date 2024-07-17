#ifdef __APPLE__
#define CommentType CommentTypeDummy
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#undef CommentType
#endif

// copied from MouseAPI for the most part

#include <Geode/Geode.hpp>

#ifdef GEODE_IS_MACOS

#include <Geode/cocos/platform/mac/CCEventDispatcher.h>
#import <Geode/cocos/platform/mac/EAGLView.h>
#import <objc/runtime.h>
#include <modules/keybinds/manager.hpp>

using namespace geode::prelude;

using EventType = void(*)(id, SEL, NSEvent*);

namespace eclipse::keybinds {
    Keys convertMouseKey(int button) {
        switch(button) {
            case 0:
                return Keys::MouseLeft;
            case 1:
                return Keys::MouseRight;
            case 2:
                return Keys::MouseMiddle;
            case 3:
                return Keys::MouseButton4;
            case 4:
                return Keys::MouseButton5;
            default:
                return Keys::None;
        }
    }
}

@interface MacMouseEvent : NSObject

    +(MacMouseEvent*) sharedEvent;

    -(void) down:(NSEvent*)event type:(int)type;
    -(void) up:(NSEvent*)event type:(int)type;

@end

static MacMouseEvent* s_sharedEvent = nil;

@implementation MacMouseEvent

#define OBJC_SWIZZLE_METHODS(mouseType, prefix, originalPrefix)               \
static EventType originalPrefix##Down;                                                  \
- (void)prefix##DownHook:(NSEvent*)event {                                              \
	originalPrefix##Down(self, @selector(prefix##Down:), event);                        \
	[[MacMouseEvent sharedEvent] down:event type:mouseType];               \
}                                                                                       \
static EventType originalPrefix##Up;                                                    \
- (void)prefix##UpHook:(NSEvent*)event {                                                \
	originalPrefix##Up(self, @selector(prefix##Up:), event);                            \
	[[MacMouseEvent sharedEvent] up:event type:mouseType];                 \
}

OBJC_SWIZZLE_METHODS(0, mouse, s_originalMouse)
OBJC_SWIZZLE_METHODS(1, rightMouse, s_originalRightMouse)
OBJC_SWIZZLE_METHODS(2, otherMouse, s_originalOtherMouse)

#define OBJC_SWIZZLE(method, swizzle, original)                               \
Method method##Method = class_getInstanceMethod(class_, @selector(method:));            \
Method swizzle##Method = class_getInstanceMethod([self class], @selector(swizzle:));    \
original = (decltype(original))method_getImplementation(method##Method);                \
method_exchangeImplementations(method##Method, swizzle##Method);


+ (void)load {
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		Class class_ = NSClassFromString(@"EAGLView");
		OBJC_SWIZZLE(mouseDown, mouseDownHook, s_originalMouseDown)
		OBJC_SWIZZLE(mouseUp, mouseUpHook, s_originalMouseUp)

		OBJC_SWIZZLE(rightMouseDown, rightMouseDownHook, s_originalRightMouseDown)
		OBJC_SWIZZLE(rightMouseUp, rightMouseUpHook, s_originalRightMouseUp)

		OBJC_SWIZZLE(otherMouseDown, otherMouseDownHook, s_originalOtherMouseDown)
		OBJC_SWIZZLE(otherMouseUp, otherMouseUpHook, s_originalOtherMouseUp)
	});
}

+(MacMouseEvent*) sharedEvent {
	@synchronized(self) {
		if (s_sharedEvent == nil) {
			s_sharedEvent = [[self alloc] init];
		}
	}
	
	return s_sharedEvent;
}

-(void) down:(NSEvent*)event type:(int)type {
	Loader::get()->queueInMainThread([=]() {
		eclipse::keybinds::Manager::get()->registerKeyPress(eclipse::keybinds::convertMouseKey(type));
	});
}

-(void) up:(NSEvent*)event type:(int)type {
	Loader::get()->queueInMainThread([=]() {
		eclipse::keybinds::Manager::get()->registerKeyRelease(eclipse::keybinds::convertMouseKey(type));
	});
}

@end

#endif