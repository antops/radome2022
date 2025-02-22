/****************************************************************************
** Meta object code from reading C++ file 'far_field_show_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Qt/far_field_show_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'far_field_show_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FarFieldShowWidget_t {
    QByteArrayData data[10];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FarFieldShowWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FarFieldShowWidget_t qt_meta_stringdata_FarFieldShowWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "FarFieldShowWidget"
QT_MOC_LITERAL(1, 19, 13), // "UpdatePlotPhi"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 15), // "UpdatePlotTheta"
QT_MOC_LITERAL(4, 50, 19), // "OnSwitchClickedUnit"
QT_MOC_LITERAL(5, 70, 11), // "OnPlotClick"
QT_MOC_LITERAL(6, 82, 21), // "QCPAbstractPlottable*"
QT_MOC_LITERAL(7, 104, 12), // "QMouseEvent*"
QT_MOC_LITERAL(8, 117, 12), // "OnPlotClick2"
QT_MOC_LITERAL(9, 130, 9) // "OnAbOkBtn"

    },
    "FarFieldShowWidget\0UpdatePlotPhi\0\0"
    "UpdatePlotTheta\0OnSwitchClickedUnit\0"
    "OnPlotClick\0QCPAbstractPlottable*\0"
    "QMouseEvent*\0OnPlotClick2\0OnAbOkBtn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FarFieldShowWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       3,    1,   47,    2, 0x08 /* Private */,
       4,    0,   50,    2, 0x08 /* Private */,
       5,    3,   51,    2, 0x08 /* Private */,
       8,    3,   58,    2, 0x08 /* Private */,
       9,    0,   65,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int, 0x80000000 | 7,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int, 0x80000000 | 7,    2,    2,    2,
    QMetaType::Void,

       0        // eod
};

void FarFieldShowWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FarFieldShowWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->UpdatePlotPhi((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->UpdatePlotTheta((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->OnSwitchClickedUnit(); break;
        case 3: _t->OnPlotClick((*reinterpret_cast< QCPAbstractPlottable*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QMouseEvent*(*)>(_a[3]))); break;
        case 4: _t->OnPlotClick2((*reinterpret_cast< QCPAbstractPlottable*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QMouseEvent*(*)>(_a[3]))); break;
        case 5: _t->OnAbOkBtn(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPAbstractPlottable* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPAbstractPlottable* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FarFieldShowWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_FarFieldShowWidget.data,
    qt_meta_data_FarFieldShowWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FarFieldShowWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FarFieldShowWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FarFieldShowWidget.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FarFieldShowWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
