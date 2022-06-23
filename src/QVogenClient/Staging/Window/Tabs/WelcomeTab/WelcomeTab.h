#ifndef WELCOMETAB_H
#define WELCOMETAB_H

#include "CentralTab.h"

class WelcomeTabPrivate;

class WelcomeTab : public CentralTab {
    Q_OBJECT
    Q_DECLARE_PRIVATE(WelcomeTab)
public:
    explicit WelcomeTab(QWidget *parent = nullptr);
    ~WelcomeTab();

    void reloadStrings();

    Type type() const override;

protected:
    WelcomeTab(WelcomeTabPrivate &d, QWidget *parent = nullptr);
};

#endif // WELCOMETAB_H
