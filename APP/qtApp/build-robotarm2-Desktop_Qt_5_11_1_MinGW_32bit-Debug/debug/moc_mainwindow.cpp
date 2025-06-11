/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../robotarm2/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[20];
    char stringdata0[324];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 18), // "on_connect_clicked"
QT_MOC_LITERAL(4, 53, 21), // "on_disconnect_clicked"
QT_MOC_LITERAL(5, 75, 17), // "on_slider_clicked"
QT_MOC_LITERAL(6, 93, 23), // "on_pushButton_2_clicked"
QT_MOC_LITERAL(7, 117, 18), // "onDeviceDiscovered"
QT_MOC_LITERAL(8, 136, 20), // "QBluetoothDeviceInfo"
QT_MOC_LITERAL(9, 157, 6), // "device"
QT_MOC_LITERAL(10, 164, 19), // "onDiscoveryFinished"
QT_MOC_LITERAL(11, 184, 20), // "onBluetoothConnected"
QT_MOC_LITERAL(12, 205, 23), // "onBluetoothDisconnected"
QT_MOC_LITERAL(13, 229, 16), // "onBluetoothError"
QT_MOC_LITERAL(14, 246, 5), // "error"
QT_MOC_LITERAL(15, 252, 21), // "onBluetoothLogMessage"
QT_MOC_LITERAL(16, 274, 7), // "message"
QT_MOC_LITERAL(17, 282, 14), // "onDataReceived"
QT_MOC_LITERAL(18, 297, 4), // "data"
QT_MOC_LITERAL(19, 302, 21) // "onControlWindowClosed"

    },
    "MainWindow\0on_pushButton_clicked\0\0"
    "on_connect_clicked\0on_disconnect_clicked\0"
    "on_slider_clicked\0on_pushButton_2_clicked\0"
    "onDeviceDiscovered\0QBluetoothDeviceInfo\0"
    "device\0onDiscoveryFinished\0"
    "onBluetoothConnected\0onBluetoothDisconnected\0"
    "onBluetoothError\0error\0onBluetoothLogMessage\0"
    "message\0onDataReceived\0data\0"
    "onControlWindowClosed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    0,   83,    2, 0x08 /* Private */,
       7,    1,   84,    2, 0x08 /* Private */,
      10,    0,   87,    2, 0x08 /* Private */,
      11,    0,   88,    2, 0x08 /* Private */,
      12,    0,   89,    2, 0x08 /* Private */,
      13,    1,   90,    2, 0x08 /* Private */,
      15,    1,   93,    2, 0x08 /* Private */,
      17,    1,   96,    2, 0x08 /* Private */,
      19,    0,   99,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_clicked(); break;
        case 1: _t->on_connect_clicked(); break;
        case 2: _t->on_disconnect_clicked(); break;
        case 3: _t->on_slider_clicked(); break;
        case 4: _t->on_pushButton_2_clicked(); break;
        case 5: _t->onDeviceDiscovered((*reinterpret_cast< const QBluetoothDeviceInfo(*)>(_a[1]))); break;
        case 6: _t->onDiscoveryFinished(); break;
        case 7: _t->onBluetoothConnected(); break;
        case 8: _t->onBluetoothDisconnected(); break;
        case 9: _t->onBluetoothError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->onBluetoothLogMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->onDataReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->onControlWindowClosed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QBluetoothDeviceInfo >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
