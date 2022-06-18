#ifndef TNSELECTCTL_H
#define TNSELECTCTL_H

#include "TNController.h"
#include "../Elements/TNRubber.h"

class TNSelectCtl : public TNController {
    Q_OBJECT
public:
    explicit TNSelectCtl(TNotesArea *parent = nullptr);
    ~TNSelectCtl();

public:
    void install() override;

    bool isSelecting() const;

protected:
    TNRubber *m_rubber;

    void stopSelecting();

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNSELECTCTL_H
