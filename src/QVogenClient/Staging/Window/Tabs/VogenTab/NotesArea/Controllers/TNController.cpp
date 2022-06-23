#include "TNController.h"
#include "../TNotesArea.h"

TNController::TNController(TNotesArea *parent) : QObject(parent), a(parent) {
}

TNController::~TNController() {
}

void TNController::install() {
}
