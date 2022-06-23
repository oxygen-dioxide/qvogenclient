#ifndef TNPRIORCTL_H
#define TNPRIORCTL_H

#include "TNController.h"

#include <QGraphicsItem>

class TNPriorCtl : public TNController {
    Q_OBJECT
public:
    explicit TNPriorCtl(TNotesArea *parent = nullptr);
    ~TNPriorCtl();

public:
    void install() override;

    bool isMousePressed() const;
    QGraphicsItem *itemUnderMouse() const;

protected:
    bool m_pressed;

    QGraphicsItem *m_itemUnderMouse;

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNPRIORCTL_H
