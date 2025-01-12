#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <objc/runtime.h>
#include <objc/message.h>

// Cocoa headers stub
#define cls objc_getClass
#define sel sel_registerName
#define msg ((id (*)(id, SEL))objc_msgSend)
#define msg_id ((id (*)(id, SEL, id))objc_msgSend)
#define msg_bool ((id (*)(id, SEL, BOOL))objc_msgSend)
#define msg_int ((id (*)(id, SEL, int))objc_msgSend)
#define msg_rect ((id (*)(id, SEL, NSRect))objc_msgSend)
#define msg_size ((id (*)(id, SEL, NSSize))objc_msgSend)
#define msg_rect_bool ((id (*)(id, SEL, NSRect, BOOL))objc_msgSend)
#define msg_id_sel_id ((id (*)(id, SEL, id, SEL, id))objc_msgSend)
#define msg_rect_int_int_int ((id (*)(id, SEL, NSRect, int, int, int))objc_msgSend)
#define msg_cls ((id (*)(Class, SEL))objc_msgSend)
#define msg_cls_id ((id (*)(Class, SEL, id))objc_msgSend)
#define msg_cls_str ((id (*)(Class, SEL, char *))objc_msgSend)
#define msg_cls_float ((id (*)(Class, SEL, CGFloat))objc_msgSend)
#define msg_cls_float_float_float_float ((id (*)(Class, SEL, CGFloat, CGFloat, CGFloat, CGFloat))objc_msgSend)
#define msg_ret_rect ((NSRect (*)(id, SEL))objc_msgSend)

typedef double CGFloat;

typedef struct CGSize {
    CGFloat width;
    CGFloat height;
} CGSize;
typedef CGSize NSSize;

typedef struct CGRect {
    CGFloat x;
    CGFloat y;
    CGFloat width;
    CGFloat height;
} CGRect;
typedef CGRect NSRect;

typedef enum NSWindowStyleMask {
    NSWindowStyleMaskTitled = 1,
    NSWindowStyleMaskClosable = 2,
    NSWindowStyleMaskMiniaturizable = 4,
    NSWindowStyleMaskResizable = 8
} NSWindowStyleMask;

typedef enum NSBackingStoreType {
    NSBackingStoreBuffered = 2
} NSBackingStoreType;

typedef enum NSTextAlignment {
    NSTextAlignmentCenter = 1
} NSTextAlignment;

id NSString(char *string) {
    return msg_cls_str(cls("NSString"), sel("stringWithUTF8String:"), string);
}

// Application code
#define LABEL_SIZE 48
char *app_ivar = "app";
typedef struct App {
    id application;
    id appDelegate;
    id window;
    id label;
} App;

void windowDidResize(id self, SEL cmd) {
    (void)cmd;
    App *app;
    object_getInstanceVariable(self, app_ivar, (void **)&app);
    NSRect windowFrame = msg_ret_rect(app->window, sel("frame"));
    msg_rect(app->label, sel("setFrame:"), (NSRect){0, (windowFrame.height - LABEL_SIZE) / 2.f, windowFrame.width, LABEL_SIZE});
}

void applicationDidFinishLaunching(id self, SEL cmd) {
    (void)cmd;
    App *app;
    object_getInstanceVariable(self, app_ivar, (void **)&app);

    // Create menu
    id menubar = msg_cls(cls("NSMenu"), sel("new"));
    msg_id(app->application, sel("setMainMenu:"), menubar);

    id menuBarItem = msg_cls(cls("NSMenuItem"), sel("new"));
    msg_id(menubar, sel("addItem:"), menuBarItem);

    id appMenu = msg_cls(cls("NSMenu"), sel("new"));
    msg_id(menuBarItem, sel("setSubmenu:"), appMenu);

    id quitMenuItem = msg_id_sel_id(msg_cls(cls("NSMenuItem"), sel("alloc")),
        sel("initWithTitle:action:keyEquivalent:"), NSString("Quit BassieTest"), sel("terminate:"), NSString("q"));
    msg_id(appMenu, sel("addItem:"), quitMenuItem);

    // Create window
    app->window = msg_rect_int_int_int(
        msg_cls(cls("NSWindow"), sel("alloc")),
        sel("initWithContentRect:styleMask:backing:defer:"),
        (NSRect){0, 0, 1024, 768},
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable,
        NSBackingStoreBuffered,
        NO
    );
    msg_id(app->window, sel("setTitle:"), NSString("BassieTest"));
    msg_bool(app->window, sel("setTitlebarAppearsTransparent:"), YES);
    id screen = msg(app->window, sel("screen"));
    NSRect screenFrame = msg_ret_rect(screen, sel("frame"));
    NSRect windowFrame = msg_ret_rect(app->window, sel("frame"));
    CGFloat windowX = (screenFrame.width - windowFrame.width) / 2;
    CGFloat windowY = (screenFrame.height - windowFrame.height) / 2;
    msg_rect_bool(app->window, sel("setFrame:display:"), (NSRect){windowX, windowY, windowFrame.width, windowFrame.height}, YES);
    msg_size(app->window, sel("setMinSize:"), (NSSize){320, 240});
    msg_id(app->window, sel("setBackgroundColor:"), msg_cls_float_float_float_float(
        cls("NSColor"), sel("colorWithRed:green:blue:alpha:"), 0x05 / 255.f, 0x44 / 255.f, 0x5e / 255.f, 1));
    msg_id(app->window, sel("setFrameAutosaveName:"), NSString("window"));
    msg_id(app->window, sel("setDelegate:"), app->appDelegate);

    // Create label
    app->label = msg_rect(msg_cls(cls("NSText"), sel("alloc")), sel("initWithFrame:"),
        (NSRect){0, (windowFrame.height - LABEL_SIZE) / 2.f, windowFrame.width, LABEL_SIZE});
    msg_id(app->label, sel("setString:"), NSString("Hello macOS!"));
    msg_id(app->label, sel("setFont:"), msg_cls_float(cls("NSFont"), sel("systemFontOfSize:"), LABEL_SIZE));
    msg_int(app->label, sel("setAlignment:"), NSTextAlignmentCenter);
    msg_bool(app->label, sel("setEditable:"), NO);
    msg_bool(app->label, sel("setSelectable:"), NO);
    msg_bool(app->label, sel("setDrawsBackground:"), NO);
    msg_id(msg(app->window, sel("contentView")), sel("addSubview:"), app->label);

    msg_id(app->window, sel("makeKeyAndOrderFront:"), nil);
}

BOOL applicationShouldTerminateAfterLastWindowClosed(id self, SEL cmd) {
    (void)self;
    (void)cmd;
    return YES;
}

int main(void) {
    // Register classes
    Class AppDelegate = objc_allocateClassPair(cls("NSObject"), "AppDelegate", 0);
    class_addIvar(AppDelegate, app_ivar, sizeof(void *), log2(sizeof(void *)), "^v");
    class_addMethod(AppDelegate, sel("windowDidResize:"), (IMP)windowDidResize, "v@:");
    class_addMethod(AppDelegate, sel("applicationDidFinishLaunching:"), (IMP)applicationDidFinishLaunching, "v@:");
    class_addMethod(AppDelegate, sel("applicationShouldTerminateAfterLastWindowClosed:"), (IMP)applicationShouldTerminateAfterLastWindowClosed, "B@:");
    objc_registerClassPair(AppDelegate);

    // Start application
    App app;
    app.application = msg_cls(cls("NSApplication"), sel("sharedApplication"));
    app.appDelegate = msg_cls(AppDelegate, sel("new"));
    object_setInstanceVariable(app.appDelegate, app_ivar, &app);
    msg_id(app.application, sel("setDelegate:"), app.appDelegate);
    msg(app.application, sel("run"));
    return EXIT_SUCCESS;
}
