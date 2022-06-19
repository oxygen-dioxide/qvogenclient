#include "ExtensionManager.h"
#include "ExtensionManager_p.h"

#include "Logs/CRecordHolder.h"

#include <QApplication>
#include <QDebug>
#include <QScreen>

#include "Utils/QCssAnalyzer.h"

#include "DataManager.h"

#include <QMessageBox>

Q_SINGLETON_DECLARE(ExtensionManager)

ExtensionManager::ExtensionManager(QObject *parent)
    : ExtensionManager(*new ExtensionManagerPrivate(), parent) {
}

ExtensionManager::~ExtensionManager() {
}

bool ExtensionManager::load() {
    Q_D(ExtensionManager);

    // Load Voice
    reloadVoiceList();

    // Load Theme
    if (qRecordCData.themeIndex < 0 || qRecordCData.themeIndex > themeCount()) {
        qRecordData.themeIndex = 0;
    }
    themeLoad(qRecordCData.themeIndex);

    return true;
}

bool ExtensionManager::save() {
    return true;
}

void ExtensionManager::themeLoad(int index) {
    Q_D(ExtensionManager);

    QCssAnalyzer qss;

#if defined(Q_OS_MAC)
    if (index == 0) {
        qss.setFileName(":/themes/theme-mac.qss");
    } else if (index == 1) {
        qss.setFileName(":/themes/theme-mac-dark.qss");
    }
#else
    if (index == 0) {
        qss.setFileName(":/themes/dark-v3.qss");
    } else if (index == 1) {
        qss.setFileName(":/themes/dark-v3.qss");
    } else if (index == 2) {
        qss.setFileName(":/themes/dark-v3.qss");
    } else if (index == 3) {
        qApp->setStyleSheet(QString());
        return;
    }
#endif

    double ratio = qApp->primaryScreen()->logicalDotsPerInch() / 96.0;
    qss.setRatio(0.8 * ratio);
    if (qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qApp->setStyleSheet(qss.readAndApply());
        qss.close();
    }

    d->reloadAppFont();
}

int ExtensionManager::themeCount() const {
    return 4;
}

QStringList ExtensionManager::themeNames() const {
    QStringList list = {tr("Multi-Color (Default)"), //
                        tr("Dark (Default)"),        //
                        tr("Light (Default)"),       //
                        tr("None")};
    return list;
}

void ExtensionManager::reloadVoiceList() {
    Q_D(ExtensionManager);
    d->voices.clear();
    QDir voiceDir(qData->getStandardPath(DataManager::Voice));
    if (voiceDir.exists()) {
        QStringList flist = voiceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (auto it = flist.begin(); it != flist.end(); ++it) {
            QString subPath = voiceDir.absoluteFilePath(*it);
            QVogenVoiceInfo info(subPath);
            if (info.load()) {
                d->voices.append(info);
            }
        }
    }
}

const QList<QVogenVoiceInfo> &ExtensionManager::voiceList() const {
    Q_D(const ExtensionManager);
    return d->voices;
}

ExtensionManager::ExtensionManager(ExtensionManagerPrivate &d, QObject *parent)
    : BaseManager(d, parent) {
    construct();

    d.init();
}
