#include "TSetPlayheadEvent.h"

TSetPlayheadEvent::TSetPlayheadEvent() : TPianoRollEvent(SetPlayhead) {
    x = 0;
}

TSetPlayheadEvent::~TSetPlayheadEvent() {
}
