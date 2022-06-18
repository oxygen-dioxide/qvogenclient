#ifndef TNNOTESCTL_H
#define TNNOTESCTL_H

#include "TNController.h"

class TNNotesCtl : public TNController {
    Q_OBJECT
public:
    explicit TNNotesCtl(TNotesArea *parent = nullptr);
    ~TNNotesCtl();

public:
    void install() override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNNOTESCTL_H
