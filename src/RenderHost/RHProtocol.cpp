#include "RHProtocol.h"

#include "Utils/QJsonFormatter.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

RENDER_HOST_USE_NAMESPACE

static const char KEY_NAME_MESSAGE_TYPE[] = "MsgType";
static const char KEY_NAME_MESSAGE_COMMAND[] = "Command";
static const char KEY_NAME_MESSAGE_CONTENT[] = "Content";

Message::Message() : msgType(-1) {
}

Message::~Message() {
}

Message Message::Unmarshall(const QByteArray &bytes) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return Message();
    }

    QJsonObject obj = doc.object();

    auto it1 = obj.find(KEY_NAME_MESSAGE_TYPE);
    if (it1 == obj.end() || !it1->isDouble()) {
        return Message();
    }

    auto it2 = obj.find(KEY_NAME_MESSAGE_COMMAND);
    if (it2 == obj.end() || !it2->isString()) {
        return Message();
    }

    auto it3 = obj.find(KEY_NAME_MESSAGE_CONTENT);
    if (it1 == obj.end() || !it3->isString()) {
        return Message();
    }

    Message msg;
    msg.msgType = it1->toDouble();
    msg.command = it2->toString();
    msg.content = it3->toString();
    return msg;
}

QByteArray Message::Marshall() const {
    QJsonObject obj;
    obj.insert(KEY_NAME_MESSAGE_TYPE, msgType);
    obj.insert(KEY_NAME_MESSAGE_COMMAND, command);
    obj.insert(KEY_NAME_MESSAGE_CONTENT, content);

    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

