#include "CentralTabWidget_p.h"

#include "Buttons/FullIconButton.h"

CentralTabWidgetPrivate::CentralTabWidgetPrivate() {
}

CentralTabWidgetPrivate::~CentralTabWidgetPrivate() {
}

void CentralTabWidgetPrivate::init() {
    tabBar->setAutoHide(true);

    // Insert placeholder widget
    placeholderWidget = createPlaceholderWidget();
    mainLayout->addWidget(placeholderWidget);

    updateVisibility();
}

void CentralTabWidgetPrivate::updateVisibility() {
    if (stack->count() == 0) {
        stack->setVisible(false);
        placeholderWidget->setVisible(true);
    } else {
        stack->setVisible(true);
        placeholderWidget->setVisible(false);
    }
}

QWidget *CentralTabWidgetPrivate::createPlaceholderWidget() const {
    auto w = new QWidget();
    w->setAttribute(Qt::WA_StyledBackground);
    w->setObjectName("placeholder-widget");

    auto phb = new FullIconButton();
    phb->setObjectName("icon-button");
    phb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto layout0 = new QHBoxLayout();
    layout0->setMargin(0);
    layout0->setSpacing(0);

    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->setAlignment(phb, Qt::AlignCenter);
    layout->addWidget(phb);

    // To be added...

    layout0->addStretch();
    layout0->addLayout(layout);
    layout0->addStretch();
    layout0->setStretchFactor(layout, 1);
    w->setLayout(layout0);

    return w;
}
