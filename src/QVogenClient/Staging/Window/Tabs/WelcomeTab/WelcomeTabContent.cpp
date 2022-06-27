#include "WelcomeTabContent.h"
#include "WelcomeTabContent_p.h"

#include "DataManager.h"

WelcomeTabContent::WelcomeTabContent(QWidget *parent)
    : WelcomeTabContent(*new WelcomeTabContentPrivate(), parent) {
}

WelcomeTabContent::~WelcomeTabContent() {
}

void WelcomeTabContent::reloadStrings() {
    Q_D(WelcomeTabContent);

    d->reloadStrings_helper();
}

WelcomeTabContent::WelcomeTabContent(WelcomeTabContentPrivate &d, QWidget *parent)
    : CentralTabContent(d, parent) {
    d.init();

    Q_TR_NOTIFY(WelcomeTabContent);
}
