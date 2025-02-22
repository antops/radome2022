/****************************************************************************
** Meta object code from reading C++ file 'taile_only_source_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Qt/taile_only_source_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'taile_only_source_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TaileOnlySourceWidget_t {
    QByteArrayData data[5];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TaileOnlySourceWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TaileOnlySourceWidget_t qt_meta_stringdata_TaileOnlySourceWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "TaileOnlySourceWidget"
QT_MOC_LITERAL(1, 22, 14), // "on_widthChange"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 3), // "var"
QT_MOC_LITERAL(4, 42, 14) // "on_depthChange"

    },
    "TaileOnlySourceWidget\0on_widthChange\0"
    "\0var\0on_depthChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TaileOnlySourceWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       4,    1,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void TaileOnlySourceWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TaileOnlySourceWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_widthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->on_depthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TaileOnlySourceWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<GraphTransWidget::staticMetaObject>(),
    qt_meta_stringdata_TaileOnlySourceWidget.data,
    qt_meta_data_TaileOnlySourceWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TaileOnlySourceWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TaileOnlySourceWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TaileOnlySourceWidget.stringdata0))
        return static_cast<void*>(this);
    return GraphTransWidget::qt_metacast(_clname);
}

int TaileOnlySourceWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphTransWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
