#include "DataManager_p.h"
#include "SystemHelper.h"

#include "Namespace.h"

#include <QApplication>
#include <QStandardPaths>
#include <QTranslator>

static const char Slash = '/';

DataManagerPrivate::DataManagerPrivate() {
}

DataManagerPrivate::~DataManagerPrivate() {
}

void DataManagerPrivate::init() {
#ifdef Q_OS_WINDOWS
    appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif
    if (!appDataPath.endsWith(Qs::AppName)) {
        appDataPath += Slash + Qs::AppName;
    }
    appTempPath = QDir::tempPath() + Slash + Qs::DIR_NAME_TEMP;
}

bool DataManagerPrivate::translate(const QString &filename) {
    Q_Q(DataManager);
    QTranslator *t = new QTranslator(q);

    if (t->load(filename)) {
        qApp->installTranslator(t);
        translators.insert(t);
        q->reloadStrings();
        return true;
    }

    delete t;
    return false;
}

void DataManagerPrivate::eliminate() {
    for (auto it = translators.begin(); it != translators.end(); ++it) {
        auto t = *it;
        qApp->removeTranslator(t);
        delete t;
    }
    translators.clear();
}
