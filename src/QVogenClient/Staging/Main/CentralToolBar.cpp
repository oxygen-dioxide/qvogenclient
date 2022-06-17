#include "CentralToolBar.h"
#include "CentralToolBar_p.h"

CentralToolBar::CentralToolBar(QWidget *parent) : CentralToolBar(*new CentralToolBarPrivate(), parent) {
}

CentralToolBar::~CentralToolBar() {
}

CentralToolBar::CentralToolBar(CentralToolBarPrivate &d, QWidget *parent) : QWidget(parent), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}