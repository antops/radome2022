/****************************************************************************
** Meta object code from reading C++ file 'far_field_show_v2_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Qt/far_field_show_v2_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'far_field_show_v2_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FarFieldShowV2Widget_t {
    QByteArrayData data[9];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FarFieldShowV2Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FarFieldShowV2Widget_t qt_meta_stringdata_FarFieldShowV2Widget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "FarFieldShowV2Widget"
QT_MOC_LITERAL(1, 21, 9), // "OnAbOkBtn"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 8), // "OnAddBtn"
QT_MOC_LITERAL(4, 41, 10), // "OnClearBtn"
QT_MOC_LITERAL(5, 52, 16), // "OnComboBoxChange"
QT_MOC_LITERAL(6, 69, 5), // "index"
QT_MOC_LITERAL(7, 75, 19), // "OnSwitchClickedUnit"
QT_MOC_LITERAL(8, 95, 22) // "OnNormalizeClickedUnit"

    },
    "FarFieldShowV2Widget\0OnAbOkBtn\0\0"
    "OnAddBtn\0OnClearBtn\0OnComboBoxChange\0"
    "index\0OnSwitchClickedUnit\0"
    "OnNormalizeClickedUnit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FarFieldShowV2Widget[] = {

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
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    1,   47,    2, 0x08 /* Private */,
       7,    0,   50,    2, 0x08 /* Private */,
       8,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FarFieldShowV2Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FarFieldShowV2Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnAbOkBtn(); break;
        case 1: _t->OnAddBtn(); break;
        case 2: _t->OnClearBtn(); break;
        case 3: _t->OnComboBoxChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->OnSwitchClickedUnit(); break;
        case 5: _t->OnNormalizeClickedUnit(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FarFieldShowV2Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_FarFieldShowV2Widget.data,
    qt_meta_data_FarFieldShowV2Widget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FarFieldShowV2Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FarFieldShowV2Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FarFieldShowV2Widget.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FarFieldShowV2Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
