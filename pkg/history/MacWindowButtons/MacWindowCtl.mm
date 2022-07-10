#include "MacWindowCtl.h"

#include <Cocoa/Cocoa.h>

#include <QDebug>
#include <QMouseEvent>
#include <QWindow>

// this Objective-c class is used to override the action of sysytem close button and zoom button
// https://stackoverflow.com/questions/27643659/setting-c-function-as-selector-for-nsbutton-produces-no-results
@interface ButtonPasser : NSObject {
}
@property (readwrite) QWidget *window;
- (void)zoomAction:(id)sender;
@end

@implementation ButtonPasser {
}
- (void)zoomAction:(id)sender {
    Q_UNUSED(sender);
    if (0 == self.window) {
        return;
    }
    if (self.window->isMaximized()) {
        self.window->showNormal();
    } else {
        self.window->showMaximized();
    }
}
@end

MacWindowCtl::MacWindowCtl(QWidget *w) : QObject(w), w(w) {
    Q_ASSERT(!w->parentWidget());

    innerPointer = nullptr;

    m_titleBarHeight = 0;
    m_bWinMoving = false;
    m_bMousePressed = false;
    m_bIsCloseBtnEnabled = true;
    m_bIsMinBtnEnabled = true;
    m_bIsZoomBtnEnabled = true;
    m_bTitleBarVisible = false;
}

MacWindowCtl::~MacWindowCtl() {
    if (innerPointer) {
        ButtonPasser *passer = (ButtonPasser *) innerPointer;
        [passer release];
    }
}

bool MacWindowCtl::setup() {
    NSView *view = (NSView *) w->winId();
    if (!view) {
        return false;
    }

    NSWindow *window = view.window;
    if (!window) {
        return false;
    }

    // Set title words and icons invisible
    window.titleVisibility = NSWindowTitleHidden; // MAC_10_10及以上版本支持

    // Set title bar transparent
    window.titlebarAppearsTransparent = YES; // MAC_10_10及以上版本支持

    // Set title bar undraggable
    [window setMovable:NO]; // MAC_10_6及以上版本支持

    // Extend view to title bar
    window.styleMask |= NSWindowStyleMaskFullSizeContentView; // MAC_10_10及以上版本支持

    // Alloc button parser
    ButtonPasser *passer = [[ButtonPasser alloc] init];
    passer.window = w;

    // Save pointer
    innerPointer = passer;

    // Override the action of fullscreen button
    NSButton *zoomButton = [window standardWindowButton:NSWindowZoomButton];
    [zoomButton setTarget:passer];
    [zoomButton setAction:@selector(zoomAction:)];

    w->installEventFilter(this);

    return true;
}

QWidget *MacWindowCtl::widget() const {
    return w;
}

void MacWindowCtl::setCloseBtnEnabled(bool enabled) {
    NSView *view = (NSView *) w->winId();
    NSWindow *window = view.window;

    m_bIsCloseBtnEnabled = enabled;
    if (enabled) {
        [[window standardWindowButton:NSWindowCloseButton] setEnabled:YES];
    } else {
        [[window standardWindowButton:NSWindowCloseButton] setEnabled:NO];
    }
}

void MacWindowCtl::setMinBtnEnabled(bool enabled) {
    NSView *view = (NSView *) w->winId();
    NSWindow *window = view.window;

    m_bIsMinBtnEnabled = enabled;
    if (enabled) {
        [[window standardWindowButton:NSWindowMiniaturizeButton] setEnabled:YES];
    } else {
        [[window standardWindowButton:NSWindowMiniaturizeButton] setEnabled:NO];
    }
}

void MacWindowCtl::setZoomBtnEnabled(bool enabled) {
    NSView *view = (NSView *) w->winId();
    NSWindow *window = view.window;

    m_bIsZoomBtnEnabled = enabled;
    if (enabled) {
        [[window standardWindowButton:NSWindowZoomButton] setEnabled:YES];
    } else {
        [[window standardWindowButton:NSWindowZoomButton] setEnabled:NO];
    }
}

void MacWindowCtl::setTitlebarVisible(bool visible) {
    NSView *view = (NSView *) w->winId();
    NSWindow *window = view.window;

    m_bTitleBarVisible = visible;
    if (visible) {
        window.styleMask ^= NSWindowStyleMaskFullSizeContentView; // MAC_10_10
    } else {
        window.styleMask |= NSWindowStyleMaskFullSizeContentView; // MAC_10_10
    }
}

void MacWindowCtl::setTitleBarHeight(int height) {
    m_titleBarHeight = height;
}

bool MacWindowCtl::eventFilter(QObject *obj, QEvent *event) {
    auto deci = [](QWidget *w, int h, const QPoint &pos) {
        int height = w->height();
        if (h > 0) {
            height = h;
        }
        QRect rect;
        rect.setRect(0, 0, w->width(), height);

        if (rect.contains(pos)) {
            return true;
        }
        return false;
    };

    if (obj == w) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            auto e = static_cast<QMouseEvent *>(event);
            if ((e->button() == Qt::LeftButton) && !w->isMaximized()) {
                if (deci(w, m_titleBarHeight, e->pos())) {
                    w->windowHandle()->startSystemMove();
                    return true;
                }
            }
            break;
        }
        case QEvent::MouseButtonDblClick: {
            auto e = static_cast<QMouseEvent *>(event);
            if (deci(w, m_titleBarHeight, e->pos())) {
                if (w->isMaximized()) {
                    w->showNormal();
                } else {
                    w->showMaximized();
                }
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return QObject::eventFilter(obj, event);
}
