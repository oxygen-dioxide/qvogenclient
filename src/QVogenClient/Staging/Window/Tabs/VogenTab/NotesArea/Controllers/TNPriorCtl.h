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

    QGraphicsItem *itemUnderMouse() const;

protected:
    QGraphicsItem *m_itemUnderMouse;
    Qt::MouseButton m_downButton;

    void openContextMenu();

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNPRIORCTL_H
