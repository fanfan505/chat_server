/****************************************************************************
** Meta object code from reading C++ file 'ChatClient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../client/include/ChatClient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChatClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10ChatClientE_t {};
} // unnamed namespace

template <> constexpr inline auto ChatClient::qt_create_metaobjectdata<qt_meta_tag_ZN10ChatClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ChatClient",
        "connected",
        "",
        "disconnected",
        "loginSuccess",
        "QJsonObject",
        "user",
        "QJsonArray",
        "friends",
        "groups",
        "offline_messages",
        "loginFailed",
        "message",
        "registerSuccess",
        "registerFailed",
        "messageReceived",
        "offlineMessagesReceived",
        "messages",
        "friendRequestReceived",
        "request",
        "addFriendResponse",
        "success",
        "createGroupResponse",
        "group_id",
        "joinGroupResponse",
        "friendAccepted",
        "friend_id",
        "friendDeleted",
        "friendRemoved",
        "error",
        "onConnected",
        "onDisconnected",
        "onReadyRead",
        "onError",
        "QAbstractSocket::SocketError",
        "socketError"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loginSuccess'
        QtMocHelpers::SignalData<void(const QJsonObject &, const QJsonArray &, const QJsonArray &, const QJsonArray &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 }, { 0x80000000 | 7, 8 }, { 0x80000000 | 7, 9 }, { 0x80000000 | 7, 10 },
        }}),
        // Signal 'loginFailed'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Signal 'registerSuccess'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'registerFailed'
        QtMocHelpers::SignalData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Signal 'messageReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 12 },
        }}),
        // Signal 'offlineMessagesReceived'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 17 },
        }}),
        // Signal 'friendRequestReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 19 },
        }}),
        // Signal 'addFriendResponse'
        QtMocHelpers::SignalData<void(bool)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Signal 'createGroupResponse'
        QtMocHelpers::SignalData<void(bool, int)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 }, { QMetaType::Int, 23 },
        }}),
        // Signal 'joinGroupResponse'
        QtMocHelpers::SignalData<void(bool)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Signal 'friendAccepted'
        QtMocHelpers::SignalData<void(int)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 26 },
        }}),
        // Signal 'friendDeleted'
        QtMocHelpers::SignalData<void(bool)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Signal 'friendRemoved'
        QtMocHelpers::SignalData<void(int)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 26 },
        }}),
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'onConnected'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 34, 35 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ChatClient, qt_meta_tag_ZN10ChatClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ChatClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10ChatClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10ChatClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10ChatClientE_t>.metaTypes,
    nullptr
} };

void ChatClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChatClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->loginSuccess((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[4]))); break;
        case 3: _t->loginFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->registerSuccess(); break;
        case 5: _t->registerFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->messageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 7: _t->offlineMessagesReceived((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 8: _t->friendRequestReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 9: _t->addFriendResponse((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->createGroupResponse((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 11: _t->joinGroupResponse((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->friendAccepted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->friendDeleted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->friendRemoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->onConnected(); break;
        case 17: _t->onDisconnected(); break;
        case 18: _t->onReadyRead(); break;
        case 19: _t->onError((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)()>(_a, &ChatClient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)()>(_a, &ChatClient::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QJsonObject & , const QJsonArray & , const QJsonArray & , const QJsonArray & )>(_a, &ChatClient::loginSuccess, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QString & )>(_a, &ChatClient::loginFailed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)()>(_a, &ChatClient::registerSuccess, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QString & )>(_a, &ChatClient::registerFailed, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QJsonObject & )>(_a, &ChatClient::messageReceived, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QJsonArray & )>(_a, &ChatClient::offlineMessagesReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QJsonObject & )>(_a, &ChatClient::friendRequestReceived, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(bool )>(_a, &ChatClient::addFriendResponse, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(bool , int )>(_a, &ChatClient::createGroupResponse, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(bool )>(_a, &ChatClient::joinGroupResponse, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(int )>(_a, &ChatClient::friendAccepted, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(bool )>(_a, &ChatClient::friendDeleted, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(int )>(_a, &ChatClient::friendRemoved, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatClient::*)(const QString & )>(_a, &ChatClient::error, 15))
            return;
    }
}

const QMetaObject *ChatClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10ChatClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ChatClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void ChatClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ChatClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ChatClient::loginSuccess(const QJsonObject & _t1, const QJsonArray & _t2, const QJsonArray & _t3, const QJsonArray & _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 3
void ChatClient::loginFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ChatClient::registerSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ChatClient::registerFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ChatClient::messageReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ChatClient::offlineMessagesReceived(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ChatClient::friendRequestReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void ChatClient::addFriendResponse(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void ChatClient::createGroupResponse(bool _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void ChatClient::joinGroupResponse(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void ChatClient::friendAccepted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}

// SIGNAL 13
void ChatClient::friendDeleted(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void ChatClient::friendRemoved(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}

// SIGNAL 15
void ChatClient::error(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1);
}
QT_WARNING_POP
