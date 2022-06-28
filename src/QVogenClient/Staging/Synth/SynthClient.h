#ifndef SYNTHCLIENT_H
#define SYNTHCLIENT_H

#include <QObject>

class SynthClient : public QObject {
    Q_OBJECT
public:
    explicit SynthClient(QObject *parent = nullptr);
    ~SynthClient();

    void httpTest();

signals:
};

#endif // SYNTHCLIENT_H
