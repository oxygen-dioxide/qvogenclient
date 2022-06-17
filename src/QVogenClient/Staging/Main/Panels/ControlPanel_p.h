#ifndef CONTROLPANELPRIVATE_H
#define CONTROLPANELPRIVATE_H

#include "CentralPanel_p.h"

#include "ControlPanel.h"

class ControlPanelPrivate : public CentralPanelPrivate {
    Q_DECLARE_PUBLIC(ControlPanel)
public:
    ControlPanelPrivate();
    ~ControlPanelPrivate();

    void init();
};

#endif // CONTROLPANELPRIVATE_H