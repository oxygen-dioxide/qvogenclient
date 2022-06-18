#include "TNotesArea.h"
#include "TNotesScroll.h"

bool TNotesArea::isSelecting() const {
    return m_selectCtl->isSelecting();
}

bool TNotesArea::scrollZoomAllowed() const {
    return !isSelecting() && !visionMoving();
}
