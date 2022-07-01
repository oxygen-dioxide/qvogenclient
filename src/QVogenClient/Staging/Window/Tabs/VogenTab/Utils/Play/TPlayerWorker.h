#ifndef TPLAYERWORKER_H
#define TPLAYERWORKER_H

#include <QObject>
#include <QAudioOutput>

class TMultiPlayerPrivate;

class TPlayerWorker : public QObject {
    Q_OBJECT
public:
    explicit TPlayerWorker(QObject *parent = nullptr);
    ~TPlayerWorker();

public:
    const TMultiPlayerPrivate *ref;

    QAtomicInteger<qint64> time;
    QAtomicInt sig;

    void start();

signals:
    void finished();
};

#endif // TPLAYERWORKER_H
