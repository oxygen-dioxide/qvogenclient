#include "CentralToolBar.h"
#include "CentralToolBar_p.h"

#include "MainWindow.h"
#include "Managers/TabManager.h"

CentralToolBar::CentralToolBar(QWidget *parent)
    : CentralToolBar(*new CentralToolBarPrivate(), parent) {
}

CentralToolBar::~CentralToolBar() {
}

MainWindow *CentralToolBar::mainWindow() const {
    return qobject_cast<MainWindow *>(window());
}

CentralToolBar::CentralToolBar(CentralToolBarPrivate &d, QWidget *parent)
    : QWidget(parent), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}

void CentralToolBar::_q_buttonClicked() {
    Q_D(CentralToolBar);
    auto button = qobject_cast<QAbstractButton *>(sender());
    if (button == d->m_cursorButton) {
        mainWindow()->tabMgr()->triggerCurrent(ActionImpl::View_Cursor_Select);
    } else if (button == d->m_noteButton) {
        mainWindow()->tabMgr()->triggerCurrent(ActionImpl::View_Cursor_Sketch);
    } else if (button == d->m_freehandButton) {
        mainWindow()->tabMgr()->triggerCurrent(ActionImpl::View_Cursor_Freehand);
    }
}
