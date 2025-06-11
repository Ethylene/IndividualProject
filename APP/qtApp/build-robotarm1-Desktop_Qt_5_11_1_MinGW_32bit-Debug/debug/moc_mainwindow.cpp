/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../robotarm1/mainwindow.h"
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
    char stringdata0[469];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 24), // "on_connectButton_clicked"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 27), // "on_disconnectButton_clicked"
QT_MOC_LITERAL(4, 65, 24), // "on_refreshButton_clicked"
QT_MOC_LITERAL(5, 90, 22), // "on_resetButton_clicked"
QT_MOC_LITERAL(6, 113, 23), // "on_statusButton_clicked"
QT_MOC_LITERAL(7, 137, 23), // "on_limitsButton_clicked"
QT_MOC_LITERAL(8, 161, 24), // "on_sendAllButton_clicked"
QT_MOC_LITERAL(9, 186, 25), // "on_clearLogButton_clicked"
QT_MOC_LITERAL(10, 212, 26), // "on_baseSlider_valueChanged"
QT_MOC_LITERAL(11, 239, 5), // "value"
QT_MOC_LITERAL(12, 245, 30), // "on_shoulderSlider_valueChanged"
QT_MOC_LITERAL(13, 276, 27), // "on_elbowSlider_valueChanged"
QT_MOC_LITERAL(14, 304, 29), // "on_gripperSlider_valueChanged"
QT_MOC_LITERAL(15, 334, 27), // "on_baseSpinBox_valueChanged"
QT_MOC_LITERAL(16, 362, 31), // "on_shoulderSpinBox_valueChanged"
QT_MOC_LITERAL(17, 394, 28), // "on_elbowSpinBox_valueChanged"
QT_MOC_LITERAL(18, 423, 30), // "on_gripperSpinBox_valueChanged"
QT_MOC_LITERAL(19, 454, 14) // "readSerialData"

    },
    "MainWindow\0on_connectButton_clicked\0"
    "\0on_disconnectButton_clicked\0"
    "on_refreshButton_clicked\0"
    "on_resetButton_clicked\0on_statusButton_clicked\0"
    "on_limitsButton_clicked\0"
    "on_sendAllButton_clicked\0"
    "on_clearLogButton_clicked\0"
    "on_baseSlider_valueChanged\0value\0"
    "on_shoulderSlider_valueChanged\0"
    "on_elbowSlider_valueChanged\0"
    "on_gripperSlider_valueChanged\0"
    "on_baseSpinBox_valueChanged\0"
    "on_shoulderSpinBox_valueChanged\0"
    "on_elbowSpinBox_valueChanged\0"
    "on_gripperSpinBox_valueChanged\0"
    "readSerialData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x08 /* Private */,
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    0,  102,    2, 0x08 /* Private */,
       6,    0,  103,    2, 0x08 /* Private */,
       7,    0,  104,    2, 0x08 /* Private */,
       8,    0,  105,    2, 0x08 /* Private */,
       9,    0,  106,    2, 0x08 /* Private */,
      10,    1,  107,    2, 0x08 /* Private */,
      12,    1,  110,    2, 0x08 /* Private */,
      13,    1,  113,    2, 0x08 /* Private */,
      14,    1,  116,    2, 0x08 /* Private */,
      15,    1,  119,    2, 0x08 /* Private */,
      16,    1,  122,    2, 0x08 /* Private */,
      17,    1,  125,    2, 0x08 /* Private */,
      18,    1,  128,    2, 0x08 /* Private */,
      19,    0,  131,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_connectButton_clicked(); break;
        case 1: _t->on_disconnectButton_clicked(); break;
        case 2: _t->on_refreshButton_clicked(); break;
        case 3: _t->on_resetButton_clicked(); break;
        case 4: _t->on_statusButton_clicked(); break;
        case 5: _t->on_limitsButton_clicked(); break;
        case 6: _t->on_sendAllButton_clicked(); break;
        case 7: _t->on_clearLogButton_clicked(); break;
        case 8: _t->on_baseSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_shoulderSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_elbowSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_gripperSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->on_baseSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->on_shoulderSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->on_elbowSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->on_gripperSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->readSerialData(); break;
        default: ;
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
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
