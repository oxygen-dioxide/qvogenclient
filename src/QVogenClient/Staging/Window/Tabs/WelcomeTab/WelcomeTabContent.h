#ifndef WELCOMETABCONTENT_H
#define WELCOMETABCONTENT_H

#include "../CentralTabContent.h"

class WelcomeTabContentPrivate;

class WelcomeTabContent : public CentralTabContent {
    Q_OBJECT
    Q_DECLARE_PRIVATE(WelcomeTabContent)
public:
    WelcomeTabContent(QWidget *parent = nullptr);
    ~WelcomeTabContent();

    void reloadStrings();

protected:
    WelcomeTabContent(WelcomeTabContentPrivate &d, QWidget *parent = nullptr);
};

#endif // WELCOMETABCONTENT_H
