#include "CentralPanel.h"
#include "CentralPanel_p.h"

CentralPanel::CentralPanel(QWidget *parent) : CentralPanel(*new CentralPanelPrivate(), parent) {
}

CentralPanel::~CentralPanel() {
}

CentralPanel::CentralPanel(CentralPanelPrivate &d, QWidget *parent) : CentralTabContent(parent), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}