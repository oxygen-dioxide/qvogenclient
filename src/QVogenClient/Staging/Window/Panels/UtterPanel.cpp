#include "UtterPanel.h"
#include "UtterPanel_p.h"

UtterPanel::UtterPanel(QWidget *parent) : UtterPanel(*new UtterPanelPrivate(), parent) {
}

UtterPanel::~UtterPanel() {
}

UtterPanel::UtterPanel(UtterPanelPrivate &d, QWidget *parent) : CentralPanel(d, parent) {
    d.init();
}