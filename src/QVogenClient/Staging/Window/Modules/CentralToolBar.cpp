#include "CentralToolBar.h"
#include "CentralToolBar_p.h"

#include "MainWindow.h"
#include "Managers/TabManager.h"

#include "Types/Actions.h"

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
    if (mode == cursorMode()) {
        return;
    }

    if (mode == NoCursor) {
        auto button = d->m_cursorGroup->checkedButton();
        if (button) {
            d->m_cursorGroup->setExclusive(false);
            button->setChecked(false);
            d->m_cursorGroup->setExclusive(true);
        }
    } else {
        d->m_cursorGroup->button(mode)->setChecked(true);
    }
}

void CentralToolBar::setPlayFlags(int flags) {
    Q_D(CentralToolBar);
    d->m_playButton->setEnabled(flags & ActionImpl::PlayFlag);
    d->m_stopButton->setEnabled(flags & ActionImpl::StopFlag);
    d->m_synthButton->setEnabled(flags & ActionImpl::RenderFlag);
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
    } else if (button == d->m_playButton) {
        emit playButtonClicked(PlayButton::Play);
    } else if (button == d->m_stopButton) {
        emit playButtonClicked(PlayButton::Stop);
    } else if (button == d->m_synthButton) {
        emit playButtonClicked(PlayButton::Synth);
    }
}
