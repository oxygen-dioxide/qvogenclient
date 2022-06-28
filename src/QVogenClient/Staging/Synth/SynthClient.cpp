#include "SynthClient.h"

#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>

SynthClient::SynthClient(QObject *parent) : QObject(parent) {
}

SynthClient::~SynthClient() {
}

void SynthClient::httpTest() {
    int startTime = QTime::currentTime().msec();
    // 生成对应的网络请求
    QNetworkRequest request;
    request.setUrl(QUrl("https://blog.csdn.net/yang1fei2/article/details/119118974"));

    // 发送Get请求
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(request);
    QEventLoop eventLoop;

    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);

    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    // 对请求的返回异常进行处理
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error();
        qDebug() << "reply->error()";
        return;
    }

    // 解析返回的Json结果
    QByteArray replyData = reply->readAll();
    qDebug() << QString(replyData);

    qDebug() << QTime::currentTime().msec() - startTime;
}

void SynthClient::launchServer() {
    QProcess p;
    p.setProgram("D:/qvogenserver/CSRenderHost.exe");
    p.startDetached();
}
