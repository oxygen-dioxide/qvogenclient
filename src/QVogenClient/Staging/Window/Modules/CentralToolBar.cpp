#include "CentralToolBar.h"
#include "CentralToolBar_p.h"

#include "MainWindow.h"
#include "Managers/TabManager.h"

CentralToolBar::CentralToolBar(QWidget *parent)
    : CentralToolBar(*new CentralToolBarPrivate(), parent) {
}

CentralToolBar::~CentralToolBar() {
}

CentralToolBar::CursorModes CentralToolBar::cursorMode() const {
    Q_D(const CentralToolBar);
    return static_cast<CursorModes>(d->m_cursorGroup->checkedId());
}

void CentralToolBar::setCursorMode(CursorModes mode) {
    Q_D(CentralToolBar);
    d->m_cursorGroup->button(mode)->setChecked(true);
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
        emit cursorModeChanged(CursorModes::Select);
    } else if (button == d->m_noteButton) {
        emit cursorModeChanged(CursorModes::Sketch);
    } else if (button == d->m_freehandButton) {
        emit cursorModeChanged(CursorModes::Free);
    }
}
