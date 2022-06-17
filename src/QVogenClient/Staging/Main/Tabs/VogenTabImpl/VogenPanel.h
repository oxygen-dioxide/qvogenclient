#ifndef VOGENPANEL_H
#define VOGENPANEL_H

#include "../../CentralTabContent.h"

class VogenTab;

class VogenPanel : public CentralTabContent {
    Q_OBJECT
public:
    explicit VogenPanel(VogenTab *tab, QWidget *parent = nullptr);
    ~VogenPanel();

protected:
    VogenTab *m_tab;

signals:
};

#endif // VOGENPANEL_H
