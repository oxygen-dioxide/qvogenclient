#ifndef TOLYRICSCHANGE_H
#define TOLYRICSCHANGE_H

#include "TBaseOperation.h"

#include <QList>

class TOLyricsChange : public TBaseOperation {
public:
    TOLyricsChange();
    ~TOLyricsChange();

public:
    struct LyricsData {
        quint64 id;
        QString lrc;
        QString oldLrc;
    };

    QList<LyricsData> data;
};

#endif // TOLYRICSCHANGE_H
