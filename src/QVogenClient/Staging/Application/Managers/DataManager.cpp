#include "DataManager.h"
#include "DataManager_p.h"

#include "SystemHelper.h"

#include "Logs/CRecordHolder.h"
#include "Namespace.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QStandardPaths>

static const char Slash = '/';

static const QChar ZH_CN_UTFCODE[] = {0x7B80, 0x4F53, 0x4E2D, 0x6587};
static const QChar ZH_CHT_UTFCODE[] = {0x7E41, 0x9AD4, 0x4E2D, 0x6587};
static const QChar JA_JP_UTFCODE[] = {0x65E5, 0x672C, 0x8A9E};

static const char RANDOM_CHAR_SET[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

#define Q_FROM_UTFCODE(Name)                                                                       \
    QString::fromRawData(Name##_UTFCODE, sizeof(Name##_UTFCODE) / sizeof(QChar))

static QString makeRandomString(int length) {
    QString res;
    int len = qstrlen(RANDOM_CHAR_SET);
    int idx;
    for (int i = 0; i < length; ++i) {
        idx = QRandomGenerator::global()->bounded(len);
        QChar ch = RANDOM_CHAR_SET[idx];
        res.append(ch);
    }
    return res;
}

Q_SINGLETON_DECLARE(DataManager)

DataManager::DataManager(QObject *parent) : DataManager(*new DataManagerPrivate(), parent) {
}

DataManager::~DataManager() {
}

bool DataManager::load() {
    Q_D(DataManager);

    qDebug() << "Data Path:" << d->appDataPath;

    // Create directories
    if (!Sys::isDirExist(d->appDataPath)) {
        if (!Sys::mkDir(d->appDataPath)) {
            QMessageBox::warning(nullptr, mainTitle(), tr("Failed to make application data path!"));
            return false;
        }
    }
    if (!Sys::isDirExist(d->appTempPath)) {
        if (!Sys::mkDir(d->appTempPath)) {
            QMessageBox::warning(nullptr, mainTitle(), tr("Failed to make temporary path!"));
            return false;
        }
    }

    // Load Record
    qRecord->setFilename(qData->getStandardPath(DataManager::Record));
    qRecord->load();

    if (qRecordCData.translationIndex < 0 || qRecordCData.translationIndex > localeCount()) {
        QLocale ql;
        switch (ql.country()) {
            case QLocale::China: {
                qRecordData.translationIndex = 1;
                break;
            }
            case QLocale::HongKong:
            case QLocale::Macau:
            case QLocale::Taiwan: {
                qRecordData.translationIndex = 2;
                break;
            }
            case QLocale::Japan: {
                qRecordData.translationIndex = 3;
                break;
            }
            default: {
                qRecordData.translationIndex = 0;
                break;
            }
        }
    }
    localeLoad(qRecordCData.translationIndex);

    d->lastOpenPaths = qRecordData.lastOpenPaths;
    return true;
}

bool DataManager::save() {
    Q_D(DataManager);

    qRecordData.lastOpenPaths = d->lastOpenPaths;

    // Save Record
    if (QApplication::keyboardModifiers() != Qt::ControlModifier) {
        qRecord->save(); // Do not save if ctrl is pressed
    }

    return true;
}

void DataManager::reloadStrings() {
    Q_D(DataManager);

    QString allFiles;

#if defined(Q_OS_WINDOWS)
    allFiles = tr("*.*");
#else
    allFiles = tr("*");
#endif

    d->projectFilter = tr("Vogen Package Files(*.vog);;"
                          "All Files(%1)")
                           .arg(allFiles);

    d->importFilter = tr("Standard MIDI Files(*.mid);;"
                         "UTAU Sequence Text(*.ust);;"
                         "Synthesizer V Files(*.svp *.s5p);;"
                         "VOCALOID Files(*.vsqx *.vsq);;"
                         "OpenUtau Files(*.ustx);;"
                         "All Files(%1)")
                          .arg(allFiles);

    d->appendFilter = tr("Standard MIDI Files(*.mid);;"
                         "UTAU Sequence Text(*.ust);;"
                         "Synthesizer V Files(*.svp *.s5p);;"
                         "VOCALOID Files(*.vsqx *.vsq);;"
                         "OpenUtau Files(*.ustx);;"
                         "All Files(%1)")
                          .arg(allFiles);

    d->exportSelectionFilter = tr("Standard MIDI Files(*.mid);;"
                                  "UTAU Sequence Text(*.ust);;"
                                  "Vogen Package Files(*.vog);;"
                                  "Synthesizer V Files(*.svp *.s5p);;"
                                  "VOCALOID Files(*.vsqx *.vsq);;"
                                  "OpenUtau Files(*.ustx);;"
                                  "All Files(%1)")
                                   .arg(allFiles);

    d->exportTrackFilter = tr("Standard MIDI Files(*.mid);;"
                              "UTAU Sequence Text(*.ust);;"
                              "Vogen Package Files(*.vog);;"
                              "Synthesizer V Files(*.svp *.s5p);;"
                              "VOCALOID Files(*.vsqx *.vsq);;"
                              "OpenUtau Files(*.ustx);;"
                              "All Files(%1)")
                               .arg(allFiles);

    d->exportOtoIniFilter = tr("Voice Configurations(oto.ini);;"
                               "All Files(%1)")
                                .arg(allFiles);

    d->imageFilter = tr("Image Files(*.bmp *.jpg *.jpeg *.png *.gif *.webp);;"
                        "All Files(%1)")
                         .arg(allFiles);

    d->audioFilter = tr("Audio Files(*.wav);;"
                        "All Files(%1)")
                         .arg(allFiles);

#if defined(Q_OS_WINDOWS)
    d->toolsFilter = tr("Executable(*.exe);;All Files(%1)").arg(allFiles);
#else
    d->toolsFilter = tr("Executable(*)");
#endif

    d->voiceFilter = tr("Vogen Voice Package(*.vogeon);;"
                        "All Files(%1)")
                         .arg(allFiles);

    emit stringUpdated();
}

DataManager::DataManager(DataManagerPrivate &d, QObject *parent) : BaseManager(d, parent) {
    construct();

    d.init();
    reloadStrings();
}

void DataManager::localeLoad(int index) {
    Q_D(DataManager);
    d->eliminate();
    switch (index) {
        case 0:
            reloadStrings();
            break;
        case 1:
            d->translate(Sys::appPath() + "/translations/qvogenclient_zh_CN.qm");
            d->translate(Sys::appPath() + "/translations/qtbase_zh_CN.qm");
            break;
        case 2:
            reloadStrings();
            break;
        case 3:
            reloadStrings();
            break;
        default:
            break;
    }
}

int DataManager::localeCount() const {
    return 4;
}

QString DataManager::localeName(int index) const {
    QString res;
    switch (index) {
        case 0:
            res = "English"; // English
            break;
        case 1:
            res = Q_FROM_UTFCODE(ZH_CN); // 简体中文
            break;
        case 2:
            res = Q_FROM_UTFCODE(ZH_CHT); // 繁體中文
            break;
        case 3:
            res = Q_FROM_UTFCODE(JA_JP); // 日本語
            break;
        default:
            break;
    }
    return res;
}

QString DataManager::openFile(const QString &title, const QString &filter, const QString &flag,
                              QWidget *parent) {
    QString path = QFileDialog::getOpenFileName(parent, title, getLastOpenPath(flag), filter);
    if (!path.isEmpty()) {
        saveLastOpenDir(flag, path);
    }
    return path;
}

QStringList DataManager::openFiles(const QString &title, const QString &filter, const QString &flag,
                                   QWidget *parent) {
    QStringList paths = QFileDialog::getOpenFileNames(parent, title, getLastOpenPath(flag), filter);
    if (!paths.isEmpty()) {
        saveLastOpenDir(flag, paths.back());
    }
    return paths;
}

QString DataManager::openDir(const QString &title, const QString &flag, QWidget *parent) {
    QString path = QFileDialog::getExistingDirectory(parent, title, getLastOpenPath(flag));
    if (!path.isEmpty()) {
        saveLastOpenDir(flag, path);
    }
    return path;
}

QString DataManager::saveFile(const QString &title, const QString &filename, const QString &filter,
                              const QString &flag, QWidget *parent) {
    QFileInfo info(filename);
    if (info.isRelative() || !Sys::isDirExist(info.absolutePath())) {
        info.setFile(getLastOpenPath(flag) + Slash + info.fileName());
    }
    QString path =
        QFileDialog::getSaveFileName(parent, title, info.absoluteFilePath(), filter, nullptr);
    if (!path.isEmpty()) {
        saveLastOpenDir(flag, path);
    }
    return path;
}

QString DataManager::getLastOpenPath(const QString &type) {
    Q_D(DataManager);
    auto it = d->lastOpenPaths.find(type);
    if (it == d->lastOpenPaths.end()) {
        it = d->lastOpenPaths.insert(type, desktopDir());
    } else if (!Sys::isDirExist(it.value())) {
        it.value() = desktopDir();
    }
    return it.value();
}

void DataManager::saveLastOpenDir(const QString &type, const QString &path) {
    Q_D(DataManager);
    d->lastOpenPaths.insert(type, Sys::PathFildDirPath(path));
}

QString DataManager::getFileFilter(DataManager::FileFilter f) const {
    Q_D(const DataManager);
    QString res;
    switch (f) {
        case ImportFile:
            res = d->importFilter;
            break;
        case AppendFile:
            res = d->appendFilter;
            break;
        case ExportSelection:
            res = d->exportSelectionFilter;
            break;
        case ExportTrack:
            res = d->exportSelectionFilter;
            break;
        case ExportOtoIni:
            res = d->exportOtoIniFilter;
            break;
        case ProjectFiles:
            res = d->projectFilter;
            break;
        case ImageFiles:
            res = d->imageFilter;
            break;
        case AudioFiles:
        case ExportRecent:
            res = d->audioFilter;
            break;
        case ExecutableFiles:
            res = d->toolsFilter;
            break;
        case VoicePackage:
            res = d->voiceFilter;
    };
    return res;
}

QString DataManager::getStandardPath(DataManager::StandardPath s) const {
    Q_D(const DataManager);
    QString res;
    switch (s) {
        case Voice:
            res = d->appDataPath + Slash + Qs::DIR_NAME_VOICE;
            break;
        case Plugins:
            break;
        case Extensions:
            break;
        case Fonts:
            res = Sys::appPath() + Slash + Qs::DIR_NAME_CONFIG_FONTS;
            break;
        case Record:
            res = d->appDataPath + Slash + Qs::FILE_NAME_RECORD_JSON;
            break;
        case SettingConfig:
            break;
        case KeyboardConfig:
            break;
        case AppData:
            res = d->appDataPath;
            break;
        case AppTemp:
            res = d->appTempPath;
            break;
    }
    return res;
}

QString DataManager::desktopDir() {
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString DataManager::appName() const {
    return Qs::AppName;
}

QString DataManager::mainTitle() const {
    return Qs::AppName;
}

QString DataManager::windowTitle() const {
    return mainTitle() + QString(" %1").arg(Qs::Version);
}

QString DataManager::errorTitle() const {
    return tr("Error");
}

QString DataManager::untitledFileName() const {
    return tr("Untitled");
}

QString DataManager::unsavedPrefix() const {
    return "*";
}

QString DataManager::deletedPrefix() const {
    return tr("(Deleted)");
}

QString DataManager::fileManagerName() const {
#ifdef Q_OS_WINDOWS
    return tr("Explorer");
#elif defined(Q_OS_MAC)
    return tr("Finder");
#else
    return tr("File Manager");
#endif
}

QString DataManager::getRandomDirName(const QString &prefix) const {
    return QString("%1_%2").arg(prefix, makeRandomString(8));
}

QString DataManager::allocGlobalTempDirName() const {
    Q_D(const DataManager);
    return d->appTempPath + Slash + "global" + Slash + getRandomDirName();
}

QString DataManager::allocVoiceTempDirName() const {
    return getStandardPath(Voice) + Slash + getRandomDirName();
}

QString DataManager::allocProjectTempDirName() const {
    Q_D(const DataManager);
    return d->appTempPath + Slash + "projects" + Slash + getRandomDirName("Vog");
}
