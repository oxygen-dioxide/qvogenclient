#include "CentralNavBar.h"
#include "CentralNavBar_p.h"

CentralNavBar::CentralNavBar(QWidget *parent) : CentralNavBar(*new CentralNavBarPrivate(), parent) {
}

CentralNavBar::~CentralNavBar() {
}

CentralNavBar::CentralNavBar(CentralNavBarPrivate &d, QWidget *parent) : QWidget(parent), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}