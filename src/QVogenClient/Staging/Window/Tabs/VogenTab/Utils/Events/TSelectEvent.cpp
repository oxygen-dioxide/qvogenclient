#include "TSelectEvent.h"
#include <QCoreApplication>

using namespace QEventImpl;

TSelectEvent::TSelectEvent() : PianoRollChangeEvent(PianoRollChangeEvent::Select) {
}

TSelectEvent::~TSelectEvent() {
}

void TSelectEvent::dispatch(QObject *current) {
    auto obj = current;
    while (qstrcmp(obj->metaObject()->className(), "VogenTab") != 0) {
        obj = obj->parent();
        Q_ASSERT(obj);
    }
    qApp->sendEvent(obj, this);
}
