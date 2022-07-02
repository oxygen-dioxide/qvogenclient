#ifndef TMULTIPLAYERPRIVATE_H
#define TMULTIPLAYERPRIVATE_H

#include "TMultiPlayer.h"

#include "Import/QWaveInfo.h"
#include "TPlayerWorker.h"

#include <QThread>

class TMultiPlayerPrivate {
    Q_DECLARE_PUBLIC(TMultiPlayer)
public:
    TMultiPlayerPrivate();
    virtual ~TMultiPlayerPrivate();

    friend class TPlayerWorker;

    void init();

    TMultiPlayer *q_ptr;

    bool start_helper(qint64 time);
    void stop_helper();

    struct Track {
        qint64 time;
        QWaveInfo *wave;
    };

    QList<Track> tracks;

    QThread *playThread;
    TPlayerWorker *worker;
};

#endif // TMULTIPLAYERPRIVATE_H
