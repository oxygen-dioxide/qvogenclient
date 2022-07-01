#include "TMultiPlayer.h"
#include "TMultiPlayer_p.h"

TMultiPlayer::TMultiPlayer(QObject *parent) : TMultiPlayer(*new TMultiPlayerPrivate(), parent) {
}

TMultiPlayer::~TMultiPlayer() {
    if (isRunning()){
        stop();
    }
}

TMultiPlayer::TMultiPlayer(TMultiPlayerPrivate &d, QObject *parent) : QObject(parent), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}
void TMultiPlayer::addTrack(qint64 time, QWaveInfo *wave) {
    Q_D(TMultiPlayer);

    if (isRunning()) {
        qDebug() << "[TMultiPlayer] Modify when player is running";
        return;
    }

    // Bin search
    auto &tracks = d->tracks;
    int i, j, mid;
    i = 0;
    j = tracks.size() - 1;
    while (i <= j) {
        mid = i + (j - i) / 2;
        if (tracks.at(mid).time < time) {
            i = mid + 1;
        } else {
            j = mid - 1;
        }
    }
    d->tracks.insert(i, TMultiPlayerPrivate::Track{time, wave});
}

void TMultiPlayer::clear() {
    Q_D(TMultiPlayer);
    if (isRunning()) {
        qDebug() << "[TMultiPlayer] Modify when player is running";
        return;
    }
    d->tracks.clear();
}

void TMultiPlayer::start(qint64 time) {
    Q_D(TMultiPlayer);
    if (isRunning()) {
        qDebug() << "[TMultiPlayer] Player is running";
        return;
    }
    d->start_helper(time);
}

void TMultiPlayer::stop() {
    Q_D(TMultiPlayer);
    if (!isRunning()) {
        qDebug() << "[TMultiPlayer] Player is not running";
        return;
    }
    d->stop_helper();
}

bool TMultiPlayer::isRunning() const {
    Q_D(const TMultiPlayer);
    return d->playThread->isRunning();
}

qint64 TMultiPlayer::pos() const {
    if (!isRunning()) {
        return 0;
    }
    Q_D(const TMultiPlayer);
    return d->worker->time;
}
