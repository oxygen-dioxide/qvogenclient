#ifndef CENTRALPANELPRIVATE_H
#define CENTRALPANELPRIVATE_H

#include "CentralPanel.h"

class CentralPanelPrivate {
    Q_DECLARE_PUBLIC(CentralPanel)
public:
    CentralPanelPrivate();
    virtual ~CentralPanelPrivate();

    void init();

    CentralPanel *q_ptr;
};

#endif // CENTRALPANELPRIVATE_H