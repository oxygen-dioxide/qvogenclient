#ifndef UTTERPANELPRIVATE_H
#define UTTERPANELPRIVATE_H

#include "CentralPanel_p.h"

#include "UtterPanel.h"

class UtterPanelPrivate : public CentralPanelPrivate {
    Q_DECLARE_PUBLIC(UtterPanel)
public:
    UtterPanelPrivate();
    ~UtterPanelPrivate();

    void init();
};

#endif // UTTERPANELPRIVATE_H