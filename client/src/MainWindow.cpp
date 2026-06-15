#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), chatClient_(new ChatClient(this)) {
    setupUi();
    connect(chatClient_, &ChatClient::connected, this, &MainWindow::onConnected);
    connect(chatClient_, &ChatClient::disconnected, this, &MainWindow::onDisconnected);
    connect(chatClient_, &ChatClient::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(chatClient_, &ChatClient::loginFailed, this, &MainWindow::onLoginFailed);
    connect(chatClient_, &ChatClient::registerSuccess, this, &MainWindow::onRegisterSuccess);
    connect(chatClient_, &ChatClient::registerFailed, this, &MainWindow::onRegisterFailed);
    connect(chatClient_, &ChatClient::messageReceived, this, &MainWindow::onMessageReceived);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    stackedWidget_ = new QStackedWidget(this);
    setCentralWidget(stackedWidget_);
    
    loginPage_ = new QWidget();
    QVBoxLayout* loginLayout = new QVBoxLayout(loginPage_);
    
    leUsername_ = new QLineEdit();
    leUsername_->setPlaceholderText("Username");
    loginLayout->addWidget(leUsername_);
    
    lePassword_ = new QLineEdit();
    lePassword_->setPlaceholderText("Password");
    lePassword_->setEchoMode(QLineEdit::Password);
    loginLayout->addWidget(lePassword_);
    
    leNickname_ = new QLineEdit();
    leNickname_->setPlaceholderText("Nickname (for register)");
    leNickname_->hide();
    loginLayout->addWidget(leNickname_);
    
    btnLogin_ = new QPushButton("Login");
    connect(btnLogin_, &QPushButton::clicked, this, &MainWindow::onLogin);
    loginLayout->addWidget(btnLogin_);
    
    btnRegister_ = new QPushButton("Register");
    btnRegister_->hide();
    connect(btnRegister_, &QPushButton::clicked, this, &MainWindow::onRegister);
    loginLayout->addWidget(btnRegister_);
    
    btnSwitchToRegister_ = new QPushButton("Switch to Register");
    connect(btnSwitchToRegister_, &QPushButton::clicked, [this]() {
        leNickname_->show();
        btnLogin_->hide();
        btnRegister_->show();
        btnSwitchToRegister_->hide();
        btnSwitchToLogin_->show();
    });
    loginLayout->addWidget(btnSwitchToRegister_);
    
    btnSwitchToLogin_ = new QPushButton("Switch to Login");
    btnSwitchToLogin_->hide();
    connect(btnSwitchToLogin_, &QPushButton::clicked, [this]() {
        leNickname_->hide();
        btnLogin_->show();
        btnRegister_->hide();
        btnSwitchToRegister_->show();
        btnSwitchToLogin_->hide();
    });
    loginLayout->addWidget(btnSwitchToLogin_);
    
    stackedWidget_->addWidget(loginPage_);
    
    chatPage_ = new QWidget();
    QHBoxLayout* chatLayout = new QHBoxLayout(chatPage_);
    
    lwFriends_ = new QListWidget();
    lwFriends_->setFixedWidth(200);
    connect(lwFriends_, &QListWidget::itemClicked, this, &MainWindow::onSelectFriend);
    chatLayout->addWidget(lwFriends_);
    
    QVBoxLayout* rightLayout = new QVBoxLayout();
    teChat_ = new QTextEdit();
    teChat_->setReadOnly(true);
    rightLayout->addWidget(teChat_);
    
    QHBoxLayout* inputLayout = new QHBoxLayout();
    leInput_ = new QLineEdit();
    inputLayout->addWidget(leInput_);
    
    btnSend_ = new QPushButton("Send");
    connect(btnSend_, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    inputLayout->addWidget(btnSend_);
    rightLayout->addLayout(inputLayout);
    
    chatLayout->addLayout(rightLayout);
    stackedWidget_->addWidget(chatPage_);
}

void MainWindow::onLogin() {
    QString username = leUsername_->text();
    QString password = lePassword_->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please fill in username and password");
        return;
    }
    
    chatClient_->connectToServer("localhost", 8080);
    chatClient_->sendLogin(username, password);
}

void MainWindow::onRegister() {
    QString username = leUsername_->text();
    QString password = lePassword_->text();
    QString nickname = leNickname_->text();
    
    if (username.isEmpty() || password.isEmpty() || nickname.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please fill in all fields");
        return;
    }
    
    chatClient_->connectToServer("localhost", 8080);
    chatClient_->sendRegister(username, password, nickname);
}

void MainWindow::onSendMessage() {
    QString content = leInput_->text();
    if (content.isEmpty() || currentChatId_ == 0) {
        return;
    }
    
    chatClient_->sendMessage(currentChatId_, content);
    addMessage("Me", content, true);
    leInput_->clear();
}

void MainWindow::onSelectFriend(QListWidgetItem* item) {
    QString text = item->text();
    QStringList parts = text.split(" (");
    currentChatName_ = parts[0];
    currentChatId_ = parts[1].replace(")", "").toInt();
    teChat_->clear();
}

void MainWindow::onLoginSuccess(const QJsonObject& user) {
    currentUserId_ = user["id"].toInt();
    lwFriends_->clear();
    lwFriends_->addItem("Friend 1 (1)");
    lwFriends_->addItem("Friend 2 (2)");
    lwFriends_->addItem("Group Chat (101)");
    showChatPage();
}

void MainWindow::onLoginFailed(const QString& message) {
    QMessageBox::warning(this, "Login Failed", message);
}

void MainWindow::onRegisterSuccess() {
    QMessageBox::information(this, "Success", "Register success! Please login.");
    leNickname_->hide();
    btnLogin_->show();
    btnRegister_->hide();
    btnSwitchToRegister_->show();
    btnSwitchToLogin_->hide();
}

void MainWindow::onRegisterFailed(const QString& message) {
    QMessageBox::warning(this, "Register Failed", message);
}

void MainWindow::onMessageReceived(const QJsonObject& message) {
    QString sender = message["from_id"].toInt() == currentUserId_ ? "Me" : "Friend";
    QString content = message["content"].toString();
    addMessage(sender, content);
}

void MainWindow::onConnected() {}

void MainWindow::onDisconnected() {}

void MainWindow::showLoginPage() {
    stackedWidget_->setCurrentWidget(loginPage_);
}

void MainWindow::showChatPage() {
    stackedWidget_->setCurrentWidget(chatPage_);
}

void MainWindow::addMessage(const QString& sender, const QString& content, bool isSelf) {
    QString color = isSelf ? "blue" : "green";
    QString align = isSelf ? "right" : "left";
    teChat_->append(QString("<p style='text-align:%1; color:%2;'>%3: %4</p>").arg(align).arg(color).arg(sender).arg(content));
}
