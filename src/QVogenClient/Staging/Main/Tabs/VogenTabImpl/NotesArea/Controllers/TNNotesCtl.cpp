#include "TNNotesCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

bool TNNotesCtl::eventFilter(QObject *obj, QEvent *event) {
    return TNController::eventFilter(obj, event);
}
