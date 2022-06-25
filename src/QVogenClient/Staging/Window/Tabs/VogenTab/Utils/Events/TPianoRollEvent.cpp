#include "TPianoRollEvent.h"

#include <QApplication>

TPianoRollEvent::TPianoRollEvent(int type) : QEventImpl::EditorUpdateEvent(PianoRoll), pt(type) {
}

TPianoRollEvent::~TPianoRollEvent() {
}

void TPianoRollEvent::dispatch(QObject *current) {
    auto obj = current;
    while (qstrcmp(obj->metaObject()->className(), "VogenTab") != 0) {
        obj = obj->parent();
        Q_ASSERT(obj);
    }
    qApp->sendEvent(obj, this);
}
