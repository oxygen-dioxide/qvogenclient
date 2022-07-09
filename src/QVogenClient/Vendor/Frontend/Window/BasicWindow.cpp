#include "BasicWindow.h"
#include "CMenuBar.h"

#include <QApplication>
#include <QDesktopWidget>

#ifndef Q_OS_MAC
#include <FramelessWidgetsHelper>

FRAMELESSHELPER_USE_NAMESPACE

using namespace Global;

BasicWindow::BasicWindow(QWidget *parent) : FramelessMainWindow(parent) {
    auto helper = FramelessWidgetsHelper::get(this);

    // Insert Menu Bar To Title Bar
    auto bar = new CMenuBar();

    // New Window Bar
    m_titleBar = new CWindowBarV2(bar);
    m_titleBar->installEventFilter(this);

    // setMenuWidget(): make the menu widget become the first row of the window.
    this->setMenuWidget(m_titleBar);
    m_titleBar->setWidget(this);

    helper->setTitleBarWidget(m_titleBar);
    helper->setSystemButton(m_titleBar->minButton(), SystemButtonType::Minimize);
    helper->setSystemButton(m_titleBar->maxButton(), SystemButtonType::Maximize);
    helper->setSystemButton(m_titleBar->closeButton(), SystemButtonType::Close);
    helper->setHitTestVisible(bar); // IMPORTANT!
}
#else
#include <QResizeEvent>

BasicWindow::BasicWindow(QWidget *parent) {
    // Insert Menu Bar To Title Bar
    auto bar = new CMenuBar();

    // New Window Bar
    m_titleBar = new CWindowBarV2(bar);
    m_titleBar->installEventFilter(this);

    // setMenuWidget(): make the menu widget become the first row of the window.
    this->setMenuWidget(m_titleBar);
    m_titleBar->setWidget(this);
}

#endif

BasicWindow::~BasicWindow() {
}

void BasicWindow::setMenuBar(QMenuBar *menuBar) {
#ifndef Q_OS_MAC
    auto helper = FramelessWidgetsHelper::get(this);
    auto orgBar = this->menuBar();
    if (orgBar) {
        helper->setHitTestVisible(orgBar, false);
    }
    helper->setHitTestVisible(menuBar);
#endif
    m_titleBar->setMenuBar(menuBar);
}

QMenuBar *BasicWindow::menuBar() const {
    return m_titleBar->menuBar();
}

void BasicWindow::resizeByDesktop(double r, bool centralize) {
    QWidget *desktop = qApp->desktop();
    if (parentWidget()) {
        desktop = parentWidget();
    }
    int dw = desktop->width();
    int dh = desktop->height();
    QSize size = this->size();
    if (r > 0 && r <= 1) {
        size = QSize(dw * r, dh * r);
    }
    if (centralize) {
        setGeometry(desktop->x() + (dw - size.width()) / 2, desktop->y() + (dh - size.height()) / 2,
                    size.width(), size.height());
    } else {
        setGeometry(x(), y(), size.width(), size.height());
    }
}

bool BasicWindow::eventFilter(QObject *obj, QEvent *event) {
#ifdef Q_OS_MAC
//    if (obj == m_titleBar) {
//        switch (event->type()) {
//        case QEvent::Resize: {
//            auto e = static_cast<QResizeEvent *>(event);
//            if (e->oldSize().height() != e->size().height()) {
//                setTitleBarHeight(m_titleBar->height());
//            }
//            break;
//        }
//        default:
//            break;
//        }
//    }
#endif
    return Super::eventFilter(obj, event);
}
