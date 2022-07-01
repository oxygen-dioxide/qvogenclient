#include "TNController.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

#include <QApplication>

TNController::TNController(TNotesArea *parent) : QObject(parent), a(parent) {
}

TNController::~TNController() {
}

void TNController::install() {
}

void TNController::sendInterrupt() {
    QEventImpl::InterruptEvent ei;
    QApplication::sendEvent(a->view()->window(), &ei);
}

void TNController::forceStopPlay() {
    if (a->isPlaying()) {
        a->stop();
    }
}
