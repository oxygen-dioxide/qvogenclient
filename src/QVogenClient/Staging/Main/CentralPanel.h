#ifndef CENTRALPANEL_H
#define CENTRALPANEL_H

#include "CentralTabContent.h"

#include <QtGlobal>

class CentralPanelPrivate;

class CentralPanel : public CentralTabContent {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralPanel)
public:
    CentralPanel(QWidget *parent = nullptr);
    ~CentralPanel();

protected:
    CentralPanel(CentralPanelPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CentralPanelPrivate> d_ptr;
};

#endif // CENTRALPANEL_H