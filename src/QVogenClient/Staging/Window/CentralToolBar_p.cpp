#include "CentralToolBar_p.h"

CentralToolBarPrivate::CentralToolBarPrivate() {
}

CentralToolBarPrivate::~CentralToolBarPrivate() {
}

void CentralToolBarPrivate::init() {
    Q_Q(CentralToolBar);
    q->setAttribute(Qt::WA_StyledBackground);
}
