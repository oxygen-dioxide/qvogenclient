#ifndef TMULTIPLAYER_H
#define TMULTIPLAYER_H

#include <QObject>

#include "Import/QWaveInfo.h"

class TMultiPlayerPrivate;

class TMultiPlayer : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(TMultiPlayer)
public:
    TMultiPlayer(QObject *parent = nullptr);
    ~TMultiPlayer();

public:
    void addTrack(qint64 time, QWaveInfo *wave);
    void clear();

    bool start(qint64 time);
    void stop();
    bool isRunning() const;

    qint64 pos() const;

protected:
    TMultiPlayer(TMultiPlayerPrivate &d, QObject *parent = nullptr);

    QScopedPointer<TMultiPlayerPrivate> d_ptr;
};

#endif // TMULTIPLAYER_H
