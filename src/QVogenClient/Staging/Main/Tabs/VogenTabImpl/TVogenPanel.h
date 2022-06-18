#ifndef TVOGENPANEL_H
#define TVOGENPANEL_H

#include "../../CentralPanel.h"

class VogenTab;

class TVogenPanel : public CentralPanel {
    Q_OBJECT
public:
    explicit TVogenPanel(VogenTab *tab, QWidget *parent = nullptr);
    ~TVogenPanel();

protected:
    VogenTab *m_tab;

signals:
};

#endif // TVOGENPANEL_H
