#ifndef WELCOMETABPRIVATE_H
#define WELCOMETABPRIVATE_H

#include "CentralTab_p.h"

#include "WelcomeTab.h"

#include "WelcomeTabContent.h"
#include "Widgets/LinearScrollArea.h"

#include <QVBoxLayout>

class WelcomeTabPrivate : public CentralTabPrivate {
    Q_DECLARE_PUBLIC(WelcomeTab)
public:
    WelcomeTabPrivate();
    ~WelcomeTabPrivate();

    LinearScrollArea *scroll;
    WelcomeTabContent *content;

    QVBoxLayout *layout;

    void init();

    QString tabText() override;
};

#endif // WELCOMETABPRIVATE_H
