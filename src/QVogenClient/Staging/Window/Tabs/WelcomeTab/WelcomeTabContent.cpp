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

    d->titleLabel->setText(qData->appName());
    d->subtitleLabel->setText(tr("Tuning Evoved"));

    d->startLabel->setText(tr("Start"));
    d->recentLabel->setText(tr("Recent"));
    d->circumLabel->setText(tr("More"));
}

WelcomeTabContent::WelcomeTabContent(WelcomeTabContentPrivate &d, QWidget *parent)
    : CentralTabContent(d, parent) {
    d.init();

    Q_TR_NOTIFY(WelcomeTabContent);
}
