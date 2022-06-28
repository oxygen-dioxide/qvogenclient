#include "TRubberBandEvent.h"

TRubberBandEvent::TRubberBandEvent(const QRectF &rect) : TPianoRollEvent(RubberBand), r(rect) {
}

TRubberBandEvent::~TRubberBandEvent() {
}
