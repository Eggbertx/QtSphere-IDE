/****************************************************************************
** Meta object code from reading C++ file 'qscilexerperl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Qsci/qscilexerperl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscilexerperl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QsciLexerPerl_t {
    QByteArrayData data[5];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QsciLexerPerl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QsciLexerPerl_t qt_meta_stringdata_QsciLexerPerl = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QsciLexerPerl"
QT_MOC_LITERAL(1, 14, 15), // "setFoldComments"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 4), // "fold"
QT_MOC_LITERAL(4, 36, 14) // "setFoldCompact"

    },
    "QsciLexerPerl\0setFoldComments\0\0fold\0"
    "setFoldCompact"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QsciLexerPerl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void QsciLexerPerl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QsciLexerPerl *_t = static_cast<QsciLexerPerl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setFoldComments((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setFoldCompact((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QsciLexerPerl::staticMetaObject = {
    { &QsciLexer::staticMetaObject, qt_meta_stringdata_QsciLexerPerl.data,
      qt_meta_data_QsciLexerPerl,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QsciLexerPerl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QsciLexerPerl::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QsciLexerPerl.stringdata0))
        return static_cast<void*>(const_cast< QsciLexerPerl*>(this));
    return QsciLexer::qt_metacast(_clname);
}

int QsciLexerPerl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QsciLexer::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
