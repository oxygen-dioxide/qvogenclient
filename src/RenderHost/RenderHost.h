#ifndef RENDERHOST_H
#define RENDERHOST_H

#include "Global.h"
#include "RHUtils.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

RENDER_HOST_BEGIN_NAMESPACE

class RENDER_HOST_API RenderHost : public QObject {
    Q_OBJECT
public:
    explicit RenderHost(int port, QObject *parent = nullptr);
    ~RenderHost();

    // Commands
    bool launch(const QStringList &arguments);
    bool getVoiceLibs(QList<VoiceLibMetadata> *metas);
    bool install(const QString &path, VoiceLibMetadata *meta, int *code);
    bool uninstall(const QString &id, int *code);
    bool synthAll(const SynthArgs &args, QList<double> *pitches, int *code);

public:
    int timeout() const;
    void setTimeout(int timeout);

    int port() const;

    QString tempDir() const;
    void setTempDir(const QString &tempDir);

protected:
    int m_port;
    int m_timeout;
    QNetworkAccessManager manager;

    QString m_tempDir;

    QNetworkReply::NetworkError err;

    bool sendRequest(const char *url, const QByteArray &content = QByteArray(),
                     QByteArray *resp = nullptr);
};

RENDER_HOST_END_NAMESPACE

#endif // RENDERHOST_H
