#include "VogenPanel.h"
#include "../VogenTab.h"

VogenPanel::VogenPanel(VogenTab *tab, QWidget *parent) : CentralTabContent(parent), m_tab(tab) {
}

VogenPanel::~VogenPanel() {
}
