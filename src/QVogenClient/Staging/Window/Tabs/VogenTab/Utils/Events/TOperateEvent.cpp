#include "TOperateEvent.h"

#include <QCoreApplication>

using namespace QEventImpl;

TOperateEvent::TOperateEvent() : PianoRollChangeEvent(PianoRollChangeEvent::Operate) {
}

TOperateEvent::~TOperateEvent() {
    if (m_data) {
        delete m_data;
    }
}

void TOperateEvent::dispatch(QObject *current) {
    auto obj = current;
    while (qstrcmp(obj->metaObject()->className(), "VogenTab") != 0) {
        obj = obj->parent();
        Q_ASSERT(obj);
    }
    qApp->sendEvent(obj, this);
}

TBaseOperation *TOperateEvent::data() const {
    return m_data;
}

void TOperateEvent::setData(TBaseOperation *data) {
    m_data = data;
}

TBaseOperation *TOperateEvent::takeData() {
    auto data = m_data;
    m_data = nullptr;
    return data;
}
