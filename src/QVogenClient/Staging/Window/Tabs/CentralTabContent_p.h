#ifndef CENTRALTABCONTENTPRIVATE_H
#define CENTRALTABCONTENTPRIVATE_H

#include "CentralTabContent.h"

class CentralTabContentPrivate {
    Q_DECLARE_PUBLIC(CentralTabContent)
public:
    CentralTabContentPrivate();
    virtual ~CentralTabContentPrivate();

    void init();

    CentralTabContent *q_ptr;
};

#endif // CENTRALTABCONTENTPRIVATE_H