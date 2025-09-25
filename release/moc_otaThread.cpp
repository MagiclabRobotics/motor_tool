/****************************************************************************
** Meta object code from reading C++ file 'otaThread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../motor_tool/otaThread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'otaThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_otaThread_t {
    QByteArrayData data[7];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_otaThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_otaThread_t qt_meta_stringdata_otaThread = {
    {
QT_MOC_LITERAL(0, 0, 9), // "otaThread"
QT_MOC_LITERAL(1, 10, 9), // "sendState"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 13), // "WriteToSerial"
QT_MOC_LITERAL(4, 35, 11), // "scopeToShow"
QT_MOC_LITERAL(5, 47, 22), // "QVector<QCPGraphData>*"
QT_MOC_LITERAL(6, 70, 13) // "getSerialData"

    },
    "otaThread\0sendState\0\0WriteToSerial\0"
    "scopeToShow\0QVector<QCPGraphData>*\0"
    "getSerialData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_otaThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    2,   35,    2, 0x06 /* Public */,
       4,    1,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Short,    2,    2,
    QMetaType::Void, 0x80000000 | 5,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    2,

       0        // eod
};

void otaThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<otaThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendState(); break;
        case 1: _t->WriteToSerial((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< qint16(*)>(_a[2]))); break;
        case 2: _t->scopeToShow((*reinterpret_cast< QVector<QCPGraphData>*(*)>(_a[1]))); break;
        case 3: _t->getSerialData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (otaThread::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&otaThread::sendState)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (otaThread::*)(QByteArray , qint16 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&otaThread::WriteToSerial)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (otaThread::*)(QVector<QCPGraphData> * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&otaThread::scopeToShow)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject otaThread::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_otaThread.data,
    qt_meta_data_otaThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *otaThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *otaThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_otaThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int otaThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void otaThread::sendState()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void otaThread::WriteToSerial(QByteArray _t1, qint16 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void otaThread::scopeToShow(QVector<QCPGraphData> * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
