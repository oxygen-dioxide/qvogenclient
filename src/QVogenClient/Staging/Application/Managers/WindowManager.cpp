#include "WindowManager.h"
#include "WindowManager_p.h"

#include "Logs/CRecordHolder.h"
#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QMessageBox>
#include <QScreen>

Q_SINGLETON_DECLARE(WindowManager)

WindowManager::WindowManager(QObject *parent) : WindowManager(*new WindowManagerPrivate(), parent) {
}

WindowManager::~WindowManager() {
}

MainWindow *WindowManager::newWindow() {
    Q_D(WindowManager);

    auto w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
	w->installEventFilter(this);

    // Recover Window State
    QRect rect = qRecordCData.windowRect;
    bool max = qRecordCData.windowMaximized;
    if (!rect.isNull() && d->windows.isEmpty()) {
        w->setGeometry(rect);
    } else {
        w->resizeByDesktop(2.0 / 3.0, true);
    }
    if (max && d->windows.isEmpty()) {
        w->showMaximized();
    }
    w->show();

    // Save Window
    d->windows.insert(w);

    return w;
}

QList<MainWindow *> WindowManager::windows() const {
    Q_D(const WindowManager);
    return d->windows.values();
}

WindowManager::WindowManager(WindowManagerPrivate &d, QObject *parent) : BaseManager(d, parent) {
    construct();

    d.init();
}

bool WindowManager::eventFilter(QObject *obj, QEvent *event) {
    Q_D(WindowManager);
    if (qstrcmp(obj->metaObject()->className(), "MainWindow") == 0) {
        auto w = qobject_cast<MainWindow *>(obj);
        if (event->type() == QEvent::DeferredDelete) {
            // Save Window State
            qRecordData.windowRect = w->geometry();
            qRecordData.windowMaximized = w->isMaximized();

            // Detach Window
            d->windows.remove(w);
        }
    }
    return BaseManager::eventFilter(obj, event);
}
