#include <QCoreApplication>

#include "RenderHost.h"

#define TEST_VOICE_LIB 0

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    RH::RenderHost host(14251);

    qDebug() << "[Test Connection]";
    {
        if (host.launch(a.arguments())) {
            qDebug() << "OK";
        } else {
            qDebug() << "Err:" << host.lastErr();
            goto fail;
        }
    }

    ::_sleep(1000);

    qDebug() << " ";
    qDebug() << "[Test Get Voice Libs]";
    {
        QList<RH::VoiceLibMetadata> metas;
        if (host.getVoiceLibs(&metas)) {
            for (int i = 0; i < metas.size(); ++i) {
                qDebug() << metas[i];
            }
        } else {
            qDebug() << "Err:" << host.lastErr();
            goto fail;
        }
    }

    if (TEST_VOICE_LIB) {
        ::_sleep(1000);

        qDebug() << " ";
        qDebug() << "[Test Install Voice Lib]";
        {
            RH::VoiceLibMetadata meta;
            int code;
            if (host.install(
                    R"(D:\Green Softwares\Vogen\Vogen.Client.v0.1.1.5\voicelib.silvia.vogeon)",
                    &meta, &code)) {
                qDebug() << code << meta;
            } else {
                qDebug() << "Err:" << host.lastErr();
                goto fail;
            }
        }

        ::_sleep(1000);

        qDebug() << " ";
        qDebug() << "[Test Uninstall Voice Lib]";
        {
            int code;
            if (host.uninstall("Silvia", &code)) {
                qDebug() << code;
            } else {
                qDebug() << "Err:" << host.lastErr();
                goto fail;
            }
        }
    }

    qDebug() << " ";
    qDebug() << "Test Finished";
    goto out;

fail:
    qDebug() << " ";
    qDebug() << "Test Failed";

out:
    return a.exec();
}
