#ifndef TRUBBERBANDEVENT_H
#define TRUBBERBANDEVENT_H

#include "TPianoRollEvent.h"

class TRubberBandEvent : public TPianoRollEvent {
public:
    TRubberBandEvent(const QRectF &rect);
    ~TRubberBandEvent();

    inline const QRectF &rect() const {
        return r;
    }

protected:
    QRectF r;
};

#endif // TRUBBERBANDEVENT_H
