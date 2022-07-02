#include "TMultiPlayer_p.h"

TMultiPlayerPrivate::TMultiPlayerPrivate() {
}

TMultiPlayerPrivate::~TMultiPlayerPrivate() {
}

void TMultiPlayerPrivate::init() {
    Q_Q(TMultiPlayer);
    worker = nullptr;
    playThread = new QThread(q);
}

bool TMultiPlayerPrivate::start_helper(qint64 time) {
    Q_Q(TMultiPlayer);

    // New Worker
    worker = new TPlayerWorker();
    worker->ref = this;
    worker->time = time;

    if (!worker->test()) {
        return false;
    }

    // Add to Thread
    worker->moveToThread(playThread);
    q->connect(playThread, &QThread::started, worker, &TPlayerWorker::start);
    q->connect(worker, &TPlayerWorker::finished, playThread, &QThread::quit);

    // Start
    playThread->start();

    return true;
}

void TMultiPlayerPrivate::stop_helper() {
    worker->sig = 1;

    // Wait for Thread to Quit
    playThread->quit();
    playThread->wait();

    // Remove Worker
    worker->deleteLater();
    worker = nullptr;
}
