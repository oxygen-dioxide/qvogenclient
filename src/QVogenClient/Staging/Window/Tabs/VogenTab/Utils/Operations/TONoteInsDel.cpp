#include "TONoteInsDel.h"

TONoteInsDel::TONoteInsDel(int type) : TBaseOperation(NoteAddDelete), c(type) {
}

TONoteInsDel::~TONoteInsDel() {
}
