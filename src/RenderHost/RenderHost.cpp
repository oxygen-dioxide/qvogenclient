#include "RenderHost.h"

#include "RHProtocol.h"
#include "Utils/QJsonFormatter.h"

#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

RENDER_HOST_USE_NAMESPACE

RenderHost::RenderHost(int port, QObject *parent) : QObject(parent), m_port(port) {
    m_timeout = 30000;
}

RenderHost::~RenderHost() {
}

bool RenderHost::launch(const QStringList &arguments) {
    return sendRequest("/launch", QJsonDocument(QJsonArray::fromStringList(arguments)).toJson());
}

bool RenderHost::getVoiceLibs(QList<VoiceLibMetadata> *metas) {
    QByteArray data;
    bool res = sendRequest("/get/voicelibs", QByteArray(), &data);
    if (!res) {
        return false;
    }

    // Get bytes
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
        return false;
    }

    // Unmarshall
    QMap<QString, VoiceLibMetadata> voiceLibMap;
    QJsonArray arr = doc.array();
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        if (it->isObject()) {
            QJsonObject obj = it->toObject();
            auto info = VoiceLibMetadata::fromObject(obj);
            if (info.id.isEmpty()) {
                continue;
            }
            voiceLibMap.insert(info.id, info);
        } else {
            continue;
        }
    }

    metas->clear();
    for (const auto &voiceLib : voiceLibMap) {
        metas->append(voiceLib);
    }
    return true;
}

bool RenderHost::install(const QString &path, VoiceLibMetadata *meta, int *code) {
    QByteArray data;
    bool res = sendRequest("/install", path.toUtf8(), &data);
    if (!res) {
        return false;
    }

    // Get bytes
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonObject objDoc = doc.object();
    int codeTmp;
    QJsonObject metaTmp;

    // Unmarshall
    QJsonFormatter f;
    bool parsed = f.parse(objDoc, f.OT({
                                      {"code", f.I(&codeTmp)},
                                      {"meta", f.O(&metaTmp)},
                                  }));
    if (!parsed) {
        return false;
    }

    *meta = VoiceLibMetadata::fromObject(metaTmp);
    *code = codeTmp;

    return true;
}

bool RenderHost::uninstall(const QString &id, int *code) {
    QByteArray data;
    bool res = sendRequest("/uninstall", id.toUtf8(), &data);
    if (!res) {
        return false;
    }

    *code = data.toInt();
    return true;
}

bool RenderHost::synthAll(const SynthArgs &args, QList<double> *pitches, int *code) {
    // Write to temporary file
    QString tmpFileName = m_tempDir + "/synth_" + makeRandomString(8) + ".tmp";
    QFile file(tmpFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    file.write(QJsonDocument(args.toObject()).toJson());
    file.close();

    QByteArray data;
    bool res = sendRequest("/synth/all", tmpFileName.toUtf8(), &data);

    if (!res) {
        return false;
    }

    if (data != "OK") {
        return false;
    }

    // Read back
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    data = file.readAll();
    file.close();
    file.remove();

    // Get bytes
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    // Unmarshall
    QJsonObject objDoc = doc.object();
    QList<double> pitchesTmp;
    int codeTmp;

    QJsonFormatter f;
    bool parsed = f.parse(objDoc, f.OT({
                                      {"code", f.I(&codeTmp)},
                                      {"pitches", f.DL(&pitchesTmp)},
                                  }));
    if (!parsed) {
        return false;
    }

    *pitches = std::move(pitchesTmp);
    *code = codeTmp;

    return true;
}

int RenderHost::timeout() const {
    return m_timeout;
}

void RenderHost::setTimeout(int timeout) {
    m_timeout = timeout;
}

int RenderHost::port() const {
    return m_port;
}

QString RenderHost::tempDir() const {
    return m_tempDir;
}

void RenderHost::setTempDir(const QString &tempDir) {
    m_tempDir = tempDir;
    QDir dir(m_tempDir);
    if (!dir.exists()) {
        dir.mkpath(m_tempDir);
    }
}

bool RenderHost::sendRequest(const char *url, const QByteArray &content, QByteArray *resp) {
    QNetworkRequest request;
    request.setTransferTimeout(m_timeout);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/html; charset=utf-8");
    request.setUrl(QUrl(QString::asprintf("http://localhost:%d%s", m_port, url)));

    qDebug() << "Post url:" << url;

    // Start timer
    QElapsedTimer timer;
    timer.start();

    // Send get request
    QNetworkReply *reply = manager.post(request, content);
    QEventLoop loop;

    // Wait for response
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    qDebug() << "Connect time:" << timer.elapsed() << "ms";

    err = reply->error();
    if (err != QNetworkReply::NoError) {
        return false;
    }

    auto data = reply->readAll();
    // qDebug().noquote() << data;

    if (resp) {
        *resp = std::move(data);
    }

    return true;
}
