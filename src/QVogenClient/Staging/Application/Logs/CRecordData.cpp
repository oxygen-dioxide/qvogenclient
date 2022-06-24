#include "CRecordData.h"
#include "CRecordHolder.h"

#include <QDebug>

CRecordData::CRecordData() {
    init();
    reset();
}

CRecordData::~CRecordData() {
}

void CRecordData::init() {
    projects.setType(QFileSet::File);
    folders.setType(QFileSet::Directory);
}

void CRecordData::reset() {
    projects.clear();
    folders.clear();

    windowRect = QRect();
    windowMaximized = false;

    currentQuantize = 1;
    playheadState = 0;

    spriteVisibility = false;
    spriteOpacity = 0.3;
    spritePosition = 3;

    themeIndex = -1;
    translationIndex = -1;
}
