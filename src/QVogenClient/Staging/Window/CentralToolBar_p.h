#ifndef CENTRALTOOLBARPRIVATE_H
#define CENTRALTOOLBARPRIVATE_H

#include "CentralToolBar.h"

class CentralToolBarPrivate {
    Q_DECLARE_PUBLIC(CentralToolBar)
public:
    CentralToolBarPrivate();
    virtual ~CentralToolBarPrivate();

    void init();

    CentralToolBar *q_ptr;
};

#endif // CENTRALTOOLBARPRIVATE_H