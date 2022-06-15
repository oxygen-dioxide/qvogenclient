#include "SystemHelper.h"
#include "Macros.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDirIterator>
#include <QGuiApplication>
#include <QProcess>

static const char Slash = '/';

QString Sys::PathFindFileName(const QString &path) {
    QFileInfo info(path);
    if (info.isRoot()) {
        return path;
    }
    return info.fileName();
}

QString Sys::PathFindNextDir(const QString &path, const QString &dir) {
    if (!path.startsWith(dir)) {
        return "";
    }
    QString suffix = path.mid(dir.size());
    if (suffix.startsWith(Slash)) {
        suffix = suffix.mid(1);
    }
    int slashIndex = suffix.indexOf(Slash);
    if (slashIndex < 0) {
        return suffix;
    }
    return suffix.mid(0, slashIndex);
}


bool Sys::combine(const QString &fileName1, const QString &fileName2, const QString &newName) {
    QFile file1(fileName1);
    QFile file2(fileName2);
    QFile file3(newName);

    if (file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly) &&
        file3.open(QIODevice::WriteOnly)) {
    } else {
        return false;
    }

    QByteArray bytes1, bytes2;
    bytes1 = file1.readAll();
    bytes2 = file2.readAll();

    file3.write(bytes1 + bytes2);

    file1.close();
    file2.close();
    file3.close();

    return true;
}

void Sys::reveal(const QString &filename) {
    QFileInfo info(filename);
#if defined(Q_OS_WINDOWS)
    if (info.isFile()) {
        QStringList cmds;
        cmds << "/e,"
             << "/select," << QDir::toNativeSeparators(filename);
        QProcess::startDetached("explorer.exe", cmds);
    } else if (info.isDir()) {
        QStringList cmds;
        cmds << "/e,"
             << "/root," << QDir::toNativeSeparators(filename);
        QProcess::startDetached("explorer.exe", cmds);
    }
#elif defined(Q_OS_MAC)
    if (info.isDir()) {
        QString dirname = filename;
        if (!dirname.endsWith(Slash)) {
            dirname.append(Slash);
        }
        QProcess::startDetached("bash", {"-c", "open \'" + dirname + "\'"});
    } else if (info.isFile()) {
        QStringList scriptArgs;
        scriptArgs << QLatin1String("-e")
                   << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                          .arg(filename);
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
        scriptArgs.clear();
        scriptArgs << QLatin1String("-e")
                   << QLatin1String("tell application \"Finder\" to activate");
        QProcess::execute("/usr/bin/osascript", scriptArgs);
    }
#else
    if (info.isDir()) {
        QProcess::startDetached("bash", {"-c", "xdg-open \'" + filename + "\'"});
    } else if (info.isFile()) {
        QString arg = info.absolutePath();
        QProcess::startDetached("bash", {"-c", "xdg-open \'" + arg + "\'"});
    }
#endif
}

int Sys::rmPreStr(const QString &dirname, const QString &prefix) {
    if (!isDirExist(dirname)) {
        return 0;
    }

    Q_D_EXPLORE(dirname)

    int cnt = 0;
    QFileInfo cur;
    for (auto it = d.rbegin(); it != d.rend(); ++it) {
        cur = *it;
        if (prefix.isEmpty() || cur.fileName().startsWith(prefix)) {
            QFile file(cur.filePath());
            if (file.remove()) {
                cnt++;
            }
        }
    }
    return cnt;
}

int Sys::rmPreNum(const QString &dirname, int prefix) {
    if (!isDirExist(dirname)) {
        return 0;
    }

    Q_D_EXPLORE(dirname)

    int cnt = 0;
    QFileInfo cur;
    for (auto it = d.rbegin(); it != d.rend(); ++it) {
        cur = *it;
        QString num = QString::number(prefix);
        QString filename = cur.fileName();
        if (filename.startsWith(num) &&
            (filename.size() == num.size() || !filename.at(num.size()).isNumber())) {
            QFile file(cur.filePath());
            if (file.remove()) {
                cnt++;
            }
        }
    }

    return cnt;
}

QString Sys::removeTailSlashes(const QString &dirname) {
    QString path = dirname;
    while (!path.isEmpty() && (path.endsWith('/') || path.endsWith('\\'))) {
        path = path.mid(0, path.size() - 1);
    }
    return path;
}

QString Sys::appPath() {
    return qApp->applicationDirPath();
}

void Sys::enableScaling() {
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

QPoint Sys::fixDesktopPos(const QPoint &pos, const QSize &size) {
    QPoint target(pos);
    if (target.x() < 0) {
        target.setX(0);
    }
    if (target.y() < 0) {
        target.setY(0);
    }
    int dw = qApp->desktop()->width();
    int dh = qApp->desktop()->height();
    int w = size.width();
    int h = size.height();
    if (target.x() + w > dw) {
        target.setX(dw - w);
    }
    if (target.y() + h > dh) {
        target.setY(dh - h);
    }
    return target;
}

QStringList Sys::FindRecursiveDirs(const QString &base, int max) {
    QDir dir(base);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    if (!dir.exists()) {
        return {};
    }

    QDirIterator iter(dir, QDirIterator::Subdirectories);
    QStringList res;

    while (iter.hasNext()) {
        if (max >= 0 && res.size() >= max) {
            break;
        }
        iter.next();
        QFileInfo aInfo = iter.fileInfo();
        res.append(aInfo.absoluteFilePath());
    }

    return res;
}

void Sys::exitApp(int code) {
    ::exit(code);
}
