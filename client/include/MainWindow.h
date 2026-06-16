#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTabWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "ChatClient.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onLogin();
    void onRegister();
    void onSendMessage();
    void onSelectFriend(QListWidgetItem* item);
    void onSelectGroup(QListWidgetItem* item);
    void onLoginSuccess(const QJsonObject& user, const QJsonArray& friends, const QJsonArray& groups, const QJsonArray& offline_messages);
    void onLoginFailed(const QString& message);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& message);
    void onMessageReceived(const QJsonObject& message);
    void onConnected();
    void onDisconnected();
    void onAddFriend();
    void onCreateGroup();
    void onJoinGroup();
    void onFriendRequestReceived(const QJsonObject& request);
    
private:
    void setupUi();
    void showLoginPage();
    void showChatPage();
    void addMessage(const QString& sender, const QString& content, bool isSelf = false);
    void loadFriends(const QJsonArray& friends);
    void loadGroups(const QJsonArray& groups);
    void loadOfflineMessages(const QJsonArray& messages);
    
    ChatClient* chatClient_;
    
    QStackedWidget* stackedWidget_;
    
    QWidget* loginPage_;
    QLineEdit* leUsername_;
    QLineEdit* lePassword_;
    QLineEdit* leNickname_;
    QPushButton* btnLogin_;
    QPushButton* btnRegister_;
    QPushButton* btnSwitchToRegister_;
    QPushButton* btnSwitchToLogin_;
    
    QWidget* chatPage_;
    QTabWidget* tabWidget_;
    
    QWidget* friendsTab_;
    QListWidget* lwFriends_;
    
    QWidget* groupsTab_;
    QListWidget* lwGroups_;
    
    QTextEdit* teChat_;
    QLineEdit* leInput_;
    QPushButton* btnSend_;
    QPushButton* btnAddFriend_;
    QPushButton* btnCreateGroup_;
    QPushButton* btnJoinGroup_;
    
    int currentUserId_;
    int currentChatId_;
    QString currentChatName_;
    bool isGroupChat_;
    int currentGroupId_;
};