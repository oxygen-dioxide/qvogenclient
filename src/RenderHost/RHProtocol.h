#ifndef RHPROTOCOL_H
#define RHPROTOCOL_H

#include "Global.h"

#include <QString>

RENDER_HOST_BEGIN_NAMESPACE

// Deprecated
enum MsgType {
    IPC_REQUEST,
};

class Message {
public:
    int msgType;
    QString command;
    QString content;

    Message();
    ~Message();

    static Message Unmarshall(const QByteArray &bytes);
    QByteArray Marshall() const;
};

enum InstallCode {
    INSTALL_SUCCESS = 1,
    INSTALL_REPEATED,
    INSTALL_INVALID_PACKAGE,
    INSTALL_FAILED,
};

enum UninstallCode {
    UNINSTALL_SUCCESS = 1,
    UNINSTALL_NOT_FOUND,
    UNINSTALL_FAILED,
};

enum SynthCode {
    SYNTH_SUCCESS = 1,
    SYNTH_FAILED,
    SYNTH_SINGER_NOT_FOUND,
};

RENDER_HOST_END_NAMESPACE

#endif // RHPROTOCOL_H
