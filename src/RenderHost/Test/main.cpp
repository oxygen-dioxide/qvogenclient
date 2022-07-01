#include <QCoreApplication>

#include "RenderHost.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qDebug() << "[Test Connection]";
    RH::RenderHost host(14251);
    if (host.launch(a.arguments())) {
        qDebug() << "OK";
    }

    ::_sleep(1000);

    qDebug() << " ";
    qDebug() << "[Test Get Voice Libs]";
    {
        QList<RH::VoiceLibMetadata> metas;
        qDebug() << host.getVoiceLibs(&metas);
        for (int i = 0; i < metas.size(); ++i) {
            qDebug() << metas[i];
        }
    }

    ::_sleep(1000);

    qDebug() << " ";
    qDebug() << "[Test Install Voice Lib]";
    {
        RH::VoiceLibMetadata meta;
        int code;
        qDebug() << host.install(
            R"(D:\Green Softwares\Vogen\Vogen.Client.v0.1.1.5\voicelib.silvia.vogeon)", &meta, &code);
        qDebug() << code << meta;
    }

    ::_sleep(1000);

    qDebug() << " ";
    qDebug() << "[Test Uninstall Voice Lib]";
    {
        int code;
        qDebug() << host.uninstall("Silvia", &code);
        qDebug() << code;
    }

    return a.exec();
}
