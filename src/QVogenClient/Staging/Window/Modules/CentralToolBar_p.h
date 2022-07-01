#ifndef CENTRALTOOLBARPRIVATE_H
#define CENTRALTOOLBARPRIVATE_H

#include "CentralToolBar.h"

#include "CToolButton.h"

#include <QButtonGroup>
#include <QHBoxLayout>

class CentralToolBarPrivate {
    Q_DECLARE_PUBLIC(CentralToolBar)
public:
    CentralToolBarPrivate();
    virtual ~CentralToolBarPrivate();

    void init();

    CentralToolBar *q_ptr;

    QHBoxLayout *layout;

    CToolButton *m_cursorButton, *m_noteButton, *m_freehandButton;
    CToolButton *m_playButton, *m_stopButton, *m_synthButton;

    QButtonGroup *m_cursorGroup;
};

#endif // CENTRALTOOLBARPRIVATE_H
