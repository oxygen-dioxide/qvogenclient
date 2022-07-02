#ifndef TPLAYERWORKER_H
#define TPLAYERWORKER_H

#include <QAudioOutput>
#include <QObject>

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

    QAudioFormat format;

    bool test();
    void start();

signals:
    void finished();
};

#endif // TPLAYERWORKER_H
