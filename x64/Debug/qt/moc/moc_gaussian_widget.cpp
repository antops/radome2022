/****************************************************************************
** Meta object code from reading C++ file 'gaussian_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Qt/gaussian_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gaussian_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GaussianWidget_t {
    QByteArrayData data[6];
    char stringdata0[71];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GaussianWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GaussianWidget_t qt_meta_stringdata_GaussianWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "GaussianWidget"
QT_MOC_LITERAL(1, 15, 14), // "on_widthChange"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 3), // "var"
QT_MOC_LITERAL(4, 35, 14), // "on_depthChange"
QT_MOC_LITERAL(5, 50, 20) // "OnSourceDiffCombobox"

    },
    "GaussianWidget\0on_widthChange\0\0var\0"
    "on_depthChange\0OnSourceDiffCombobox"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GaussianWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08 /* Private */,
       4,    1,   32,    2, 0x08 /* Private */,
       5,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void GaussianWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GaussianWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_widthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->on_depthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->OnSourceDiffCombobox((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GaussianWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<GraphTransWidget::staticMetaObject>(),
    qt_meta_stringdata_GaussianWidget.data,
    qt_meta_data_GaussianWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GaussianWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GaussianWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GaussianWidget.stringdata0))
        return static_cast<void*>(this);
    return GraphTransWidget::qt_metacast(_clname);
}

int GaussianWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphTransWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
