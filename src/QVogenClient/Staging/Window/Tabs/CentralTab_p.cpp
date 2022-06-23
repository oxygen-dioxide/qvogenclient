#include "CentralTab_p.h"

#include "DataManager.h"

CentralTabPrivate::CentralTabPrivate() {
    edited = false;
}

CentralTabPrivate::~CentralTabPrivate() {
}

void CentralTabPrivate::init() {
}

bool CentralTabPrivate::earliest() const {
    return false;
}

bool CentralTabPrivate::latest() const {
    return false;
}

QString CentralTabPrivate::setTabNameProxy(const QString &tabName) {
    return (edited ? qData->unsavedPrefix() : "") + tabName;
}

QString CentralTabPrivate::tabText() {
    return QString();
}
