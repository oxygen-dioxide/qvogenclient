#include "TOperateEvent.h"

#include <QCoreApplication>

using namespace QEventImpl;

TOperateEvent::TOperateEvent() : TPianoRollEvent(Operate) {
}

TOperateEvent::~TOperateEvent() {
    if (m_data) {
        delete m_data;
    }
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
