#include "ChatClient.h"

ChatClient::ChatClient(QObject *parent) : QObject(parent), socket_(new QTcpSocket(this)), connected_(false) {
    connect(socket_, &QTcpSocket::connected, this, &ChatClient::onConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &ChatClient::onDisconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    connect(socket_, &QTcpSocket::errorOccurred, this, &ChatClient::onError);
}

ChatClient::~ChatClient() {
    disconnectFromServer();
}

void ChatClient::connectToServer(const QString& host, int port) {
    if (connected_) {
        disconnectFromServer();
    }
    socket_->connectToHost(host, port);
}

void ChatClient::disconnectFromServer() {
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->disconnectFromHost();
    }
    connected_ = false;
}

bool ChatClient::isConnected() const {
    return connected_;
}

void ChatClient::sendLogin(const QString& username, const QString& password) {
    QJsonObject data;
    data["type"] = 1001;
    data["username"] = username;
    data["password"] = password;
    sendJson(data);
}

void ChatClient::sendRegister(const QString& username, const QString& password, const QString& nickname) {
    QJsonObject data;
    data["type"] = 1003;
    data["username"] = username;
    data["password"] = password;
    data["nickname"] = nickname;
    sendJson(data);
}

void ChatClient::sendMessage(int to_id, const QString& content, bool is_group, int group_id) {
    QJsonObject data;
    data["type"] = 3001;
    data["to_id"] = to_id;
    data["content"] = content;
    data["is_group"] = is_group;
    if (is_group) {
        data["group_id"] = group_id;
    }
    sendJson(data);
}

void ChatClient::sendAddFriend(int to_id, const QString& message) {
    QJsonObject data;
    data["type"] = 2001;
    data["to_id"] = to_id;
    data["message"] = message;
    sendJson(data);
}

void ChatClient::sendCreateGroup(const QString& name, const QString& description) {
    QJsonObject data;
    data["type"] = 2004;
    data["name"] = name;
    data["description"] = description;
    sendJson(data);
}

void ChatClient::sendJoinGroup(int group_id) {
    QJsonObject data;
    data["type"] = 2005;
    data["group_id"] = group_id;
    sendJson(data);
}

void ChatClient::sendJson(const QJsonObject& data) {
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(data);
        QByteArray bytes = doc.toJson(QJsonDocument::Compact);
        socket_->write(bytes);
        socket_->flush();
    } else {
        messageQueue_.append(data);
    }
}

void ChatClient::flushQueue() {
    while (!messageQueue_.isEmpty()) {
        QJsonObject data = messageQueue_.takeFirst();
        QJsonDocument doc(data);
        QByteArray bytes = doc.toJson(QJsonDocument::Compact);
        socket_->write(bytes);
        socket_->flush();
    }
}

void ChatClient::onConnected() {
    connected_ = true;
    flushQueue();
    emit connected();
}

void ChatClient::onDisconnected() {
    connected_ = false;
    emit disconnected();
}

void ChatClient::onReadyRead() {
    QByteArray data = socket_->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        int type = obj["type"].toInt();

        switch (type) {
            case 1002: {
                bool success = obj["success"].toBool();
                if (success) {
                    QJsonObject user = obj["user"].toObject();
                    QJsonArray friends = obj.contains("friends") ? obj["friends"].toArray() : QJsonArray();
                    QJsonArray groups = obj.contains("groups") ? obj["groups"].toArray() : QJsonArray();
                    QJsonArray offline_messages = obj.contains("offline_messages") ? obj["offline_messages"].toArray() : QJsonArray();
                    emit loginSuccess(user, friends, groups, offline_messages);
                } else {
                    emit loginFailed(obj["message"].toString());
                }
                break;
            }
            case 1004: {
                bool success = obj["success"].toBool();
                if (success) {
                    emit registerSuccess();
                } else {
                    emit registerFailed(obj["message"].toString());
                }
                break;
            }
            case 2002: {
                emit addFriendResponse(obj["success"].toBool());
                break;
            }
            case 2003: {
                emit friendRequestReceived(obj);
                break;
            }
            case 2004: {
                emit createGroupResponse(obj["success"].toBool(), obj["group_id"].toInt());
                break;
            }
            case 2005: {
                emit joinGroupResponse(obj["success"].toBool());
                break;
            }
            case 2006: {
                emit friendAccepted(obj["friend_id"].toInt());
                break;
            }
            case 3001: {
                emit messageReceived(obj);
                break;
            }
            default:
                break;
        }
    }
}

void ChatClient::onError(QAbstractSocket::SocketError socketError) {
    emit error(socket_->errorString());
}
