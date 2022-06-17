#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include "CentralPanel.h"

class ControlPanelPrivate;

class ControlPanel : public CentralPanel {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ControlPanel)
public:
    ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel();

protected:
    ControlPanel(ControlPanelPrivate &d, QWidget *parent = nullptr);
};

#endif // CONTROLPANEL_H