#include "CentralTabContent.h"
#include "CentralTabContent_p.h"

CentralTabContent::CentralTabContent(QWidget *parent) : CentralTabContent(*new CentralTabContentPrivate(), parent) {
}

CentralTabContent::~CentralTabContent() {
}

CentralTabContent::CentralTabContent(CentralTabContentPrivate &d, QWidget *parent) : QWidget(parent), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}