#include "ControlPanel.h"
#include "ControlPanel_p.h"

ControlPanel::ControlPanel(QWidget *parent) : ControlPanel(*new ControlPanelPrivate(), parent) {
}

ControlPanel::~ControlPanel() {
}

ControlPanel::ControlPanel(ControlPanelPrivate &d, QWidget *parent) : CentralPanel(d, parent) {
    d.init();
}