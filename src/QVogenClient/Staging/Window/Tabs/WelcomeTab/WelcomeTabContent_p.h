#ifndef WELCOMETABCONTENTPRIVATE_H
#define WELCOMETABCONTENTPRIVATE_H

#include "CentralTabContent_p.h"

#include "CentralPanel.h"
#include "WelcomeTabContent.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>

class WelcomeTabContentPrivate : public CentralTabContentPrivate {
    Q_DECLARE_PUBLIC(WelcomeTabContent)
public:
    WelcomeTabContentPrivate();
    ~WelcomeTabContentPrivate();

    void init();

    void reloadStrings_helper();

    QVBoxLayout *layout;
    QGridLayout *panelLayout;

    QLabel *titleLabel;
    QLabel *subtitleLabel;

    CentralPanel *startPanel;
    CentralPanel *recentPanel;
    CentralPanel *circumPanel;

    QLabel *startLabel;
    QLabel *recentLabel;
    QLabel *circumLabel;

    QVBoxLayout *startLayout;
    QVBoxLayout *recentLayout;
    QVBoxLayout *circumLayout;
};

#endif // WELCOMETABCONTENTPRIVATE_H
