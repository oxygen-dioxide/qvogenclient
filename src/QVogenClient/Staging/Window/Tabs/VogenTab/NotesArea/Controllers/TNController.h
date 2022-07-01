#ifndef TNCONTROLLER_H
#define TNCONTROLLER_H

#include <QObject>

class TNotesArea;

class TNController : public QObject {
    Q_OBJECT
public:
    explicit TNController(TNotesArea *parent = nullptr);
    ~TNController();

public:
    virtual void install();

    void sendInterrupt();
    void forceStopPlay();

protected:
    TNotesArea *a;

signals:
};

#endif // TNCONTROLLER_H
