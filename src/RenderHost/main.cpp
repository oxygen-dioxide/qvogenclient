#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include "Meta/Help.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    app.setApplicationName("RenderHost");
    app.setApplicationVersion(APP_VERSION);

    // Parse CommandLine
    QCommandLineParser parser;

    QCommandLineOption helpOption(QStringList{"h", "help"});
    QCommandLineOption versionOption(QStringList{"v", "version"});
    QCommandLineOption serverOption(QStringList{"s", "server"});
    QCommandLineOption fileOption(QStringList{"f"});

    parser.addOption(helpOption);
    parser.addOption(versionOption);
    parser.addOption(serverOption);
    parser.addOption(fileOption);

    parser.process(app.arguments());

    if (parser.isSet(helpOption)) {
        return Help::help();
    } else if (parser.isSet(versionOption)) {
        return Help::version();
    } else {
        return Help::help();
    }

    return app.exec();
}
