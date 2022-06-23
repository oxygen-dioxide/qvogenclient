#include "WelcomeTab.h"
#include "WelcomeTab_p.h"

WelcomeTab::WelcomeTab(QWidget *parent) : WelcomeTab(*new WelcomeTabPrivate(), parent) {
}

WelcomeTab::~WelcomeTab() {
}

void WelcomeTab::reloadStrings() {
}

CentralTab::Type WelcomeTab::type() const {
    return Welcome;
}

WelcomeTab::WelcomeTab(WelcomeTabPrivate &d, QWidget *parent) : CentralTab(d, parent) {
    d.init();

    updateTabName();
}
