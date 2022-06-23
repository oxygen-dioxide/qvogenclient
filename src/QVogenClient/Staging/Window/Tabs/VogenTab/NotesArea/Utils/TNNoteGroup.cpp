#include "TNNoteGroup.h"

Q_DECLARE_ENTITY_LIST_IMPLEMENT(TNNoteList, TNRectNote)

TNNoteGroup::TNNoteGroup(QObject *parent) : TNNoteList(parent) {
}

TNNoteGroup::~TNNoteGroup() {
}
