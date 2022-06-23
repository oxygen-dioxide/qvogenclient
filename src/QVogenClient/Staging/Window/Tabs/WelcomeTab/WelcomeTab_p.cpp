#include "WelcomeTab_p.h"

WelcomeTabPrivate::WelcomeTabPrivate() {
}

WelcomeTabPrivate::~WelcomeTabPrivate() {
}

void WelcomeTabPrivate::init() {
    Q_Q(WelcomeTab);
    content = new WelcomeTabContent();

    scroll = new LinearScrollArea(Qt::Vertical);
    scroll->setWidget(content);

    layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(scroll);

    q->setLayout(layout);
}

QString WelcomeTabPrivate::tabText() {
    return QObject::tr("Welcome");
}
