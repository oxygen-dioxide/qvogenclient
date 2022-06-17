#include "CentralNavBar_p.h"

CentralNavBarPrivate::CentralNavBarPrivate() {
}

CentralNavBarPrivate::~CentralNavBarPrivate() {
}

void CentralNavBarPrivate::init() {
    Q_Q(CentralNavBar);
    q->setAttribute(Qt::WA_StyledBackground);
}
