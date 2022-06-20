#ifndef CENTRALNAVBARPRIVATE_H
#define CENTRALNAVBARPRIVATE_H

#include "CentralNavBar.h"

class CentralNavBarPrivate {
    Q_DECLARE_PUBLIC(CentralNavBar)
public:
    CentralNavBarPrivate();
    virtual ~CentralNavBarPrivate();

    void init();

    CentralNavBar *q_ptr;
};

#endif // CENTRALNAVBARPRIVATE_H