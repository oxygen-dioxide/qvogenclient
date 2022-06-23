#ifndef TNSCREENCTL_H
#define TNSCREENCTL_H

#include "TNController.h"
#include "../Elements/TNRectScreen.h"

class TNScreenCtl : public TNController {
    Q_OBJECT
public:
    explicit TNScreenCtl(TNotesArea *parent = nullptr);
    ~TNScreenCtl();

public:
    void install() override;

protected:
    TNRectScreen *m_screen;

    void updateScreen();

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNSCREENCTL_H
