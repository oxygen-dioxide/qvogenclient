#ifndef TNPLAYHEADCTL_H
#define TNPLAYHEADCTL_H

#include "../Elements/TNPlayhead.h"
#include "TNController.h"

class TNPlayheadCtl : public TNController {
    Q_OBJECT
public:
    explicit TNPlayheadCtl(TNotesArea *parent = nullptr);
    ~TNPlayheadCtl();

public:
    void install() override;

    TNPlayhead *playhead() const;

    void setPlaying(bool visible);
    bool isPlaying() const;

    void setCurrentTick(int tick, bool adjust = false);
    int currentTick() const;

protected:
    TNPlayhead *m_playhead;

    int m_playToTick;

    void updatePlayhead();

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNPLAYHEADCTL_H
