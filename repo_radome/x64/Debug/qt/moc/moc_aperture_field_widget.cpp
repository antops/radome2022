/****************************************************************************
** Meta object code from reading C++ file 'aperture_field_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../qt/aperture_field_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'aperture_field_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ApertureFieldWidget_t {
    QByteArrayData data[11];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ApertureFieldWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ApertureFieldWidget_t qt_meta_stringdata_ApertureFieldWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ApertureFieldWidget"
QT_MOC_LITERAL(1, 20, 10), // "EfileEeven"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 14), // "on_widthChange"
QT_MOC_LITERAL(4, 47, 3), // "var"
QT_MOC_LITERAL(5, 51, 14), // "on_depthChange"
QT_MOC_LITERAL(6, 66, 9), // "SaveParam"
QT_MOC_LITERAL(7, 76, 15), // "vtkJson::Value*"
QT_MOC_LITERAL(8, 92, 8), // "param_js"
QT_MOC_LITERAL(9, 101, 9), // "LoadParam"
QT_MOC_LITERAL(10, 111, 14) // "vtkJson::Value"

    },
    "ApertureFieldWidget\0EfileEeven\0\0"
    "on_widthChange\0var\0on_depthChange\0"
    "SaveParam\0vtkJson::Value*\0param_js\0"
    "LoadParam\0vtkJson::Value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ApertureFieldWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    1,   40,    2, 0x08 /* Private */,
       5,    1,   43,    2, 0x08 /* Private */,
       6,    1,   46,    2, 0x08 /* Private */,
       9,    1,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10,    8,

       0        // eod
};

void ApertureFieldWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ApertureFieldWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EfileEeven(); break;
        case 1: _t->on_widthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->on_depthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->SaveParam((*reinterpret_cast< vtkJson::Value*(*)>(_a[1]))); break;
        case 4: _t->LoadParam((*reinterpret_cast< const vtkJson::Value(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ApertureFieldWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<GraphTransWidget::staticMetaObject>(),
    qt_meta_stringdata_ApertureFieldWidget.data,
    qt_meta_data_ApertureFieldWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ApertureFieldWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ApertureFieldWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ApertureFieldWidget.stringdata0))
        return static_cast<void*>(this);
    return GraphTransWidget::qt_metacast(_clname);
}

int ApertureFieldWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphTransWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
