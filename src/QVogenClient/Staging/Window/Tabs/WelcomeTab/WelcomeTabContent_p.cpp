#include "WelcomeTabContent_p.h"

#include "DataManager.h"

WelcomeTabContentPrivate::WelcomeTabContentPrivate() {
}

WelcomeTabContentPrivate::~WelcomeTabContentPrivate() {
}

#define NEW_LAYOUT(X, TYPE)                                                                        \
    {                                                                                              \
        X = new TYPE();                                                                            \
        X->setMargin(0);                                                                           \
        X->setSpacing(0);                                                                          \
    }

void WelcomeTabContentPrivate::init() {
    Q_Q(WelcomeTabContent);

    // Labels
    titleLabel = new QLabel();
    titleLabel->setObjectName("title-label");

    subtitleLabel = new QLabel();
    subtitleLabel->setObjectName("subtitle-label");

    // Panels
    startPanel = new CentralPanel();
    startPanel->setObjectName("boot-panel");

    recentPanel = new CentralPanel();
    recentPanel->setObjectName("recent-panel");

    circumPanel = new CentralPanel();
    circumPanel->setObjectName("circum-panel");

    // Panel Layout
    NEW_LAYOUT(panelLayout, QGridLayout);

    panelLayout->addWidget(startPanel, 0, 0);
    panelLayout->addWidget(recentPanel, 1, 0);
    panelLayout->addWidget(circumPanel, 0, 1, 2, 1);

    // Main
    NEW_LAYOUT(layout, QVBoxLayout);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addLayout(panelLayout);

    q->setLayout(layout);

    // Boot Panel
    startLabel = new QLabel();
    startLabel->setObjectName("boot-label");

    NEW_LAYOUT(startLayout, QVBoxLayout);
    startLayout->addWidget(startLabel);

    startPanel->setLayout(startLayout);

    // Recent Panel
    recentLabel = new QLabel();
    recentLabel->setObjectName("recent-label");

    NEW_LAYOUT(recentLayout, QVBoxLayout);
    recentLayout->addWidget(recentLabel);

    recentPanel->setLayout(recentLayout);

    // Circum Panel
    circumLabel = new QLabel();
    circumLabel->setObjectName("circum-label");

    NEW_LAYOUT(circumLayout, QVBoxLayout);
    circumLayout->addWidget(circumLabel);

    circumPanel->setLayout(circumLayout);
}

void WelcomeTabContentPrivate::reloadStrings_helper() {
    titleLabel->setText(qData->appName());
    subtitleLabel->setText(WelcomeTabContent::tr("Tuning Evoved"));

    startLabel->setText(WelcomeTabContent::tr("Start"));
    recentLabel->setText(WelcomeTabContent::tr("Recent"));
    circumLabel->setText(WelcomeTabContent::tr("More"));
}
