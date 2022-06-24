#include "CRecordHolder.h"

#include <QJsonDocument>

#include "MathHelper.h"
#include "QJsonFormatter.h"
#include "SystemHelper.h"

// Temp
const char SECTION_NAME_TEMP[] = "Temporary";

// GUI
const char SECTION_NAME_GUI[] = "GUI";

const char KEY_NAME_THEME_INDEX[] = "ThemeIndex";
const char KEY_NAME_TRANSLATION_INDEX[] = "TranslationIndex";

// State
const char KEY_NAME_CURRENT_QUANTIZE[] = "CurrentQuantize";
const char KEY_NAME_PLAYHEAD_STATE[] = "PlayheadState";

// GUI/MainWindow
const char KEY_NAME_MAINWINDOW[] = "MainWindow";

const char KEY_NAME_MAINWINDOW_X[] = "X";
const char KEY_NAME_MAINWINDOW_Y[] = "Y";
const char KEY_NAME_MAINWINDOW_WIDTH[] = "Width";
const char KEY_NAME_MAINWINDOW_HEIGHT[] = "Height";
const char KEY_NAME_MAINWINDOW_MAXIMIZED[] = "Maximized";

// GUI/Foreground
const char KEY_NAME_FOREGROUND_STATE[] = "ForegroundState";

const char KEY_NAME_SPRITE_OPACITY[] = "SpriteAlpha";
const char KEY_NAME_SPRITE_VISIBILITY[] = "SpriteVisible";
const char KEY_NAME_SPRITE_POSITION[] = "SpritePosition";

// Paths
const char SECTION_NAME_PATHS[] = "Paths";

const char SECTION_NAME_PROJECT[] = "Projects";
const char SECTION_NAME_VOICE[] = "Voices";

const char SECTION_NAME_LAST_OPEN[] = "LastOpen";

Q_SINGLETON_DECLARE(CRecordHolder)

CRecordHolder::CRecordHolder(QObject *parent) : FileHolder(parent) {
    construct();
}

CRecordHolder::CRecordHolder(const QString &filename, QObject *parent) : FileHolder(parent) {
    construct();
    setFilename(filename);
}

QString CRecordHolder::filename() const {
    return m_filename;
}

void CRecordHolder::setFilename(const QString &filename) {
    m_filename = filename;
}

bool CRecordHolder::load() {
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray bytes = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qDebug() << "Record error.";
        return false;
    }

    CRecordData temp;
    QJsonObject objDoc = doc.object();

    double x = 0, y = 0, w = 0, h = 0, p = temp.spritePosition, o = temp.spriteOpacity,
           l = temp.currentQuantize, ph = temp.playheadState, t = temp.themeIndex,
           t2 = temp.translationIndex;
    QStringList ps, vs;
    QMap<QString, QString> ls;

    bool res = false;
    {
        QJsonFormatter f;
        res = f.parse(
            objDoc,
            f.OT({{SECTION_NAME_GUI,
                   f.OT({
                       {KEY_NAME_MAINWINDOW,
                        f.OT({{KEY_NAME_MAINWINDOW_X, f.D(&x)},
                              {KEY_NAME_MAINWINDOW_Y, f.D(&y)},
                              {KEY_NAME_MAINWINDOW_WIDTH, f.D(&w)},
                              {KEY_NAME_MAINWINDOW_HEIGHT, f.D(&h)},
                              {KEY_NAME_MAINWINDOW_MAXIMIZED, f.B(&temp.windowMaximized)}})},

                       {KEY_NAME_FOREGROUND_STATE,
                        f.OT({{KEY_NAME_SPRITE_OPACITY, f.D(&o)},
                              {KEY_NAME_SPRITE_POSITION, f.D(&p)},
                              {KEY_NAME_SPRITE_VISIBILITY, f.B(&temp.spriteVisibility)}})},

                       {KEY_NAME_CURRENT_QUANTIZE, f.D(&l)},
                       {KEY_NAME_PLAYHEAD_STATE, f.D(&ph)},

                       {KEY_NAME_THEME_INDEX, f.D(&t)},
                       {KEY_NAME_TRANSLATION_INDEX, f.D(&t2)},
                   })},

                  {SECTION_NAME_TEMP, f.OT({})},

                  {SECTION_NAME_PATHS, f.OT({{SECTION_NAME_PROJECT, f.SL(&ps)},
                                             {SECTION_NAME_VOICE, f.SL(&vs)},
                                             {SECTION_NAME_LAST_OPEN, f.SM(&ls)}})}}));
        f.releaseMemory();
    }

    if (!res) {
        return false;
    }

    QSet<int> values{0, 1, 2, 4, 6, 8, 16, 24, 32};
    if (values.contains(l)) {
        temp.currentQuantize = l;
    }
    if (ph >= 0 && ph < 2) {
        temp.playheadState = ph;
    }
    if (t >= 0) {
        temp.themeIndex = t;
    }
    if (t2 >= 0) {
        temp.translationIndex = t2;
    }
    if (o >= 0 && o <= 1) {
        temp.spriteOpacity = o;
    }
    if (p >= 0 && p <= 3) {
        temp.spritePosition = p;
    }
    if (w > 0 && h > 0 && !temp.windowMaximized) {
        temp.windowRect = QRect(x, y, w, h);
    }

    temp.projects.set(
        Math::batchReplace(Math::batchFilter(ps, Sys::isFileExist), QDir::fromNativeSeparators));
    temp.folders.set(
        Math::batchReplace(Math::batchFilter(vs, Sys::isDirExist), QDir::fromNativeSeparators));
    temp.lastOpenPaths = Math::batchReplaceByVal(Math::batchFilterByVal(ls, Sys::isDirExist),
                                                 QDir::fromNativeSeparators);

    m_data = temp;
    return true;
}

bool CRecordHolder::save() {
    QFile file(m_filename);
    if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
        return false;
    }

    const CRecordData &temp = m_data;
    QRect rect = temp.windowRect;

    QStringList ps, vs;
    QMap<QString, QString> ls;

    ps = Math::batchReplace(temp.projects.valid(), QDir::toNativeSeparators);
    vs = Math::batchReplace(temp.folders.valid(), QDir::toNativeSeparators);
    ls = Math::batchReplaceByVal(Math::batchFilterByVal(temp.lastOpenPaths, Sys::isDirExist),
                                 QDir::toNativeSeparators);

#define OBJ QJsonObject
    QJsonObject objDoc(
        {{SECTION_NAME_GUI,
          OBJ({{KEY_NAME_MAINWINDOW, OBJ({{KEY_NAME_MAINWINDOW_X, rect.x()},
                                          {KEY_NAME_MAINWINDOW_Y, rect.y()},
                                          {KEY_NAME_MAINWINDOW_WIDTH, rect.width()},
                                          {KEY_NAME_MAINWINDOW_HEIGHT, rect.height()},
                                          {KEY_NAME_MAINWINDOW_MAXIMIZED, temp.windowMaximized}})},

               {KEY_NAME_FOREGROUND_STATE,
                OBJ({{KEY_NAME_SPRITE_OPACITY, temp.spriteOpacity},
                     {KEY_NAME_SPRITE_POSITION, temp.spritePosition},
                     {KEY_NAME_SPRITE_VISIBILITY, temp.spriteVisibility}})},

               {KEY_NAME_CURRENT_QUANTIZE, temp.currentQuantize},
               {KEY_NAME_PLAYHEAD_STATE, temp.playheadState},
               {KEY_NAME_THEME_INDEX, temp.themeIndex},
               {KEY_NAME_TRANSLATION_INDEX, temp.translationIndex}})},

         {SECTION_NAME_TEMP, OBJ({})},

         {SECTION_NAME_PATHS, OBJ({{SECTION_NAME_PROJECT, QJsonArray::fromStringList(ps)},
                                   {SECTION_NAME_VOICE, QJsonArray::fromStringList(vs)},
                                   {SECTION_NAME_LAST_OPEN, QJsonFormatter::mapToObject(ls)}})}});
#undef OBJ

    QJsonDocument doc;
    doc.setObject(objDoc);

    file.write(doc.toJson());
    file.close();

    return true;
}

void CRecordHolder::reset() {
    m_data.reset();
}

const CRecordData &CRecordHolder::constData() const {
    return m_data;
}

CRecordData &CRecordHolder::data() {
    return m_data;
}

void CRecordHolder::commitRecent(CRecordHolder::RecentType rType, CRecordHolder::ChangeType cType,
                                 const QString &filename) {
    QFileSet *fs;
    switch (rType) {
    case Project:
        fs = &m_data.projects;
        break;
    case Folder:
        fs = &m_data.folders;
        break;
    default:
        break;
    }
    switch (cType) {
    case Push:
        fs->push(filename);
        break;
    case Unshift:
        fs->unshift(filename);
        break;
    case Advance:
        fs->advance(filename);
        break;
    case Remove:
        fs->remove(filename);
        break;
    default:
        fs->clear();
        break;
    }
    emit recentCommited(rType);
}

void CRecordHolder::commitEState(CRecordHolder::EStateType eType, int state) {
    switch (eType) {
    case Quantization: {
        m_data.currentQuantize = state;
        break;
    }
    case Playhead: {
        m_data.playheadState = state;
        break;
    }
    default:
        break;
    }
    emit eStateCommited(eType);
}
