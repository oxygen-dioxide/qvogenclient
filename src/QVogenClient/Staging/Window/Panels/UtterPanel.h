#ifndef UTTERPANEL_H
#define UTTERPANEL_H

#include "CentralPanel.h"

class UtterPanelPrivate;

class UtterPanel : public CentralPanel {
    Q_OBJECT
    Q_DECLARE_PRIVATE(UtterPanel)
public:
    UtterPanel(QWidget *parent = nullptr);
    ~UtterPanel();

protected:
    UtterPanel(UtterPanelPrivate &d, QWidget *parent = nullptr);
};

#endif // UTTERPANEL_H