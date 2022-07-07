#include "MacMainWindow.h"

#include "MacWindowCtl.h"

#include <QDebug>

MacMainWindow::MacMainWindow(QWidget *parent)
    : QMainWindow(parent), m_macCtl(new MacWindowCtl(this)) {
    if (!m_macCtl->setup()) {
        qDebug() << "Failed to setup mac native window.";
    }
}

MacMainWindow::~MacMainWindow() {
}

void MacMainWindow::setTitleBarHeight(int height) {
    m_macCtl->setTitleBarHeight(height);
}

int MacMainWindow::titleBarHeight() const {
    return m_macCtl->titleBarHeight();
}
