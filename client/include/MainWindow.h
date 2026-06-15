#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
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
    void onLoginSuccess(const QJsonObject& user);
    void onLoginFailed(const QString& message);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& message);
    void onMessageReceived(const QJsonObject& message);
    void onConnected();
    void onDisconnected();
    void showLoginPage();
    void showChatPage();
    
private:
    void setupUi();
    void addMessage(const QString& sender, const QString& content, bool isSelf = false);
    
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
    QListWidget* lwFriends_;
    QTextEdit* teChat_;
    QLineEdit* leInput_;
    QPushButton* btnSend_;
    
    int currentUserId_;
    int currentChatId_;
    QString currentChatName_;
};
