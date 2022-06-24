#ifndef CRECORDDATA_H
#define CRECORDDATA_H

#include <QFile>
#include <QMap>
#include <QRect>

#include "QFileSet.h"

class CRecordData {
public:
    CRecordData();
    ~CRecordData();

private:
    void init();

public:
    void reset();

public:
    QFileSet projects;
    QFileSet folders;

    QRect windowRect;
    bool windowMaximized;

    int currentQuantize;
    int playheadState;

    bool spriteVisibility;
    double spriteOpacity;
    int spritePosition;

    int themeIndex;
    int translationIndex;

    QMap<QString, QString> lastOpenPaths;
};

#endif // CRECORDDATA_H
