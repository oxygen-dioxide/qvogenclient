#include "Help.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSysInfo>

#define QPRINT qDebug().noquote().nospace()

int Help::help() {
    QPRINT << "QVogenClient synthesizing backend.";
    QPRINT << " ";

    QPRINT << "Usage:";
    QPRINT << QString("  %1 [options]").arg(QCoreApplication::applicationName());
    QPRINT << " ";

    QPRINT << "Options:";
    QPRINT << QString::asprintf("  %-16s %s", "-f <filename>", "Specify a configuration");
    QPRINT << QString::asprintf("  %-16s %s", "-s, --server", "Start as a server");
    QPRINT << QString::asprintf("  %-16s %s", "-v, --version", "Show version");
    QPRINT << QString::asprintf("  %-16s %s", "-h, --help", "Show this message");
    return 0;
}

int Help::version() {
    QPRINT << QString("%1 version %2 %3/%4")
                  .arg(QCoreApplication::applicationName(),    //
                       QCoreApplication::applicationVersion(), //
                       QSysInfo::productType(),                 //
                       QSysInfo::currentCpuArchitecture());

    QPRINT << QString("Qt version %1").arg(QT_VERSION_STR);
    return 0;
}
