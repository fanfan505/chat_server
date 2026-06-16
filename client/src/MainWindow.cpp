#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>

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
    
    QWidget* leftPanel = new QWidget();
    leftPanel->setFixedWidth(220);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    
    tabWidget_ = new QTabWidget();
    leftLayout->addWidget(tabWidget_);
    
    friendsTab_ = new QWidget();
    QVBoxLayout* friendsLayout = new QVBoxLayout(friendsTab_);
    lwFriends_ = new QListWidget();
    connect(lwFriends_, &QListWidget::itemClicked, this, &MainWindow::onSelectFriend);
    friendsLayout->addWidget(lwFriends_);
    tabWidget_->addTab(friendsTab_, "Friends");
    
    groupsTab_ = new QWidget();
    QVBoxLayout* groupsLayout = new QVBoxLayout(groupsTab_);
    lwGroups_ = new QListWidget();
    connect(lwGroups_, &QListWidget::itemClicked, this, &MainWindow::onSelectGroup);
    groupsLayout->addWidget(lwGroups_);
    tabWidget_->addTab(groupsTab_, "Groups");
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddFriend_ = new QPushButton("Add Friend");
    connect(btnAddFriend_, &QPushButton::clicked, this, &MainWindow::onAddFriend);
    btnLayout->addWidget(btnAddFriend_);
    
    btnCreateGroup_ = new QPushButton("Create Group");
    connect(btnCreateGroup_, &QPushButton::clicked, this, &MainWindow::onCreateGroup);
    btnLayout->addWidget(btnCreateGroup_);
    
    btnJoinGroup_ = new QPushButton("Join Group");
    connect(btnJoinGroup_, &QPushButton::clicked, this, &MainWindow::onJoinGroup);
    btnLayout->addWidget(btnJoinGroup_);
    leftLayout->addLayout(btnLayout);
    
    chatLayout->addWidget(leftPanel);
    
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    
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
    
    chatLayout->addWidget(rightPanel);
    
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
    
    chatClient_->sendMessage(currentChatId_, content, isGroupChat_, currentGroupId_);
    addMessage("Me", content, true);
    leInput_->clear();
}

void MainWindow::onSelectFriend(QListWidgetItem* item) {
    QString text = item->text();
    QStringList parts = text.split(" (");
    currentChatName_ = parts[0];
    currentChatId_ = parts[1].replace(")", "").toInt();
    isGroupChat_ = false;
    currentGroupId_ = 0;
    teChat_->clear();
    teChat_->append(QString("<h3 style='text-align:center;'>Chat with %1</h3>").arg(currentChatName_));
}

void MainWindow::onSelectGroup(QListWidgetItem* item) {
    QString text = item->text();
    QStringList parts = text.split(" (");
    currentChatName_ = parts[0];
    currentChatId_ = parts[1].replace(")", "").toInt();
    isGroupChat_ = true;
    currentGroupId_ = currentChatId_;
    teChat_->clear();
    teChat_->append(QString("<h3 style='text-align:center;'>Group: %1</h3>").arg(currentChatName_));
}

void MainWindow::onLoginSuccess(const QJsonObject& user, const QJsonArray& friends, const QJsonArray& groups, const QJsonArray& offline_messages) {
    currentUserId_ = user["id"].toInt();
    loadFriends(friends);
    loadGroups(groups);
    showChatPage();
    
    if (!offline_messages.isEmpty()) {
        loadOfflineMessages(offline_messages);
        QMessageBox::information(this, "Offline Messages", QString("You have %1 offline messages").arg(offline_messages.size()));
    }
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

void MainWindow::onAddFriend() {
    bool ok;
    int friendId = QInputDialog::getInt(this, "Add Friend", "Enter friend user ID:", 0, 0, 999999, 1, &ok);
    if (ok) {
        QString message = QInputDialog::getText(this, "Add Friend", "Enter request message:", QLineEdit::Normal, "", &ok);
        if (ok) {
            chatClient_->sendAddFriend(friendId, message);
            QMessageBox::information(this, "Success", "Friend request sent");
        }
    }
}

void MainWindow::onCreateGroup() {
    bool ok;
    QString name = QInputDialog::getText(this, "Create Group", "Enter group name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        QString description = QInputDialog::getText(this, "Create Group", "Enter group description:", QLineEdit::Normal, "", &ok);
        if (ok) {
            chatClient_->sendCreateGroup(name, description);
            QMessageBox::information(this, "Success", "Group created successfully");
        }
    }
}

void MainWindow::onJoinGroup() {
    bool ok;
    int groupId = QInputDialog::getInt(this, "Join Group", "Enter group ID:", 0, 0, 999999, 1, &ok);
    if (ok) {
        chatClient_->sendJoinGroup(groupId);
        QMessageBox::information(this, "Success", "Joined group successfully");
    }
}

void MainWindow::showLoginPage() {
    stackedWidget_->setCurrentWidget(loginPage_);
}

void MainWindow::showChatPage() {
    stackedWidget_->setCurrentWidget(chatPage_);
}

void MainWindow::addMessage(const QString& sender, const QString& content, bool isSelf) {
    QString color = isSelf ? "#2563eb" : "#16a34a";
    QString align = isSelf ? "right" : "left";
    QString bgColor = isSelf ? "#dbeafe" : "#dcfce7";
    teChat_->append(QString("<div style='text-align:%1; margin-bottom:8px;'>"
                            "<span style='display:inline-block; max-width:70%; padding:8px 12px; border-radius:12px; background:%2;'>"
                            "<strong style='color:%3;'>%4:</strong> %5"
                            "</span></div>").arg(align).arg(bgColor).arg(color).arg(sender).arg(content));
}

void MainWindow::loadFriends(const QJsonArray& friends) {
    lwFriends_->clear();
    for (const auto& friendItem : friends) {
        QJsonObject f = friendItem.toObject();
        QString status = f["online"].toBool() ? "<span style='color:green;'>●</span>" : "<span style='color:gray;'>●</span>";
        lwFriends_->addItem(QString("%1 %2 (%3)").arg(f["nickname"].toString()).arg(status).arg(f["id"].toInt()));
    }
}

void MainWindow::loadGroups(const QJsonArray& groups) {
    lwGroups_->clear();
    for (const auto& groupItem : groups) {
        QJsonObject g = groupItem.toObject();
        lwGroups_->addItem(QString("%1 (%2)").arg(g["name"].toString()).arg(g["id"].toInt()));
    }
}

void MainWindow::loadOfflineMessages(const QJsonArray& messages) {
    teChat_->append("<h3 style='text-align:center; color:orange;'>--- Offline Messages ---</h3>");
    for (const auto& msgItem : messages) {
        QJsonObject msg = msgItem.toObject();
        addMessage("Friend", msg["content"].toString(), false);
    }
    teChat_->append("<h3 style='text-align:center; color:orange;'>--- End of Offline Messages ---</h3>");
}