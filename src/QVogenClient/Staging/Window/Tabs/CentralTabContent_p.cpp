#include "CentralTabContent_p.h"

CentralTabContentPrivate::CentralTabContentPrivate() {
}

CentralTabContentPrivate::~CentralTabContentPrivate() {
}

void CentralTabContentPrivate::init() {
    Q_Q(CentralTabContent);
    q->setAttribute(Qt::WA_StyledBackground);
}
