#pragma once

#include <QTcpSocket>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

class ChatClient : public QObject {
    Q_OBJECT

public:
    explicit ChatClient(QObject *parent = nullptr);
    ~ChatClient();

    void connectToServer(const QString& host, int port);
    void disconnectFromServer();
    bool isConnected() const;

    void sendLogin(const QString& username, const QString& password);
    void sendRegister(const QString& username, const QString& password, const QString& nickname);
    void sendMessage(int to_id, const QString& content, bool is_group = false, int group_id = 0);
    void sendAddFriend(int to_id, const QString& message = "");
    void sendCreateGroup(const QString& name, const QString& description);
    void sendJoinGroup(int group_id);
    void sendJson(const QJsonObject& data);

signals:
    void connected();
    void disconnected();
    void loginSuccess(const QJsonObject& user, const QJsonArray& friends, const QJsonArray& groups, const QJsonArray& offline_messages);
    void loginFailed(const QString& message);
    void registerSuccess();
    void registerFailed(const QString& message);
    void messageReceived(const QJsonObject& message);
    void offlineMessagesReceived(const QJsonArray& messages);
    void friendRequestReceived(const QJsonObject& request);
    void addFriendResponse(bool success);
    void createGroupResponse(bool success, int group_id);
    void joinGroupResponse(bool success);
    void error(const QString& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    void flushQueue();

    QTcpSocket* socket_;
    bool connected_;
    QList<QJsonObject> messageQueue_;
};
