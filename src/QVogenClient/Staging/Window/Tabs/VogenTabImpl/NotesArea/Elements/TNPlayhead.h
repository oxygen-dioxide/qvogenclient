#ifndef TNPLAYHEAD_H
#define TNPLAYHEAD_H

#include "TNRectObject.h"

class TNPlayhead : public TNRectObject {
    Q_OBJECT
public:
    explicit TNPlayhead(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNPlayhead();

public:
    double left() const;
    double right() const;

    bool playing() const;
    void setPlaying(bool playing);

    bool alwaysShow() const;
    void setAlwaysShow(bool alwaysShow);

private:
    QPainterPath shape() const override;

    bool m_playing;
    bool m_alwaysShow;
};

#endif // TNPLAYHEAD_H
