/****************************************************************************
** Meta object code from reading C++ file 'qscilexercpp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Qsci/qscilexercpp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscilexercpp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QsciLexerCPP_t {
    QByteArrayData data[9];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QsciLexerCPP_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QsciLexerCPP_t qt_meta_stringdata_QsciLexerCPP = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QsciLexerCPP"
QT_MOC_LITERAL(1, 13, 13), // "setFoldAtElse"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "fold"
QT_MOC_LITERAL(4, 33, 15), // "setFoldComments"
QT_MOC_LITERAL(5, 49, 14), // "setFoldCompact"
QT_MOC_LITERAL(6, 64, 19), // "setFoldPreprocessor"
QT_MOC_LITERAL(7, 84, 20), // "setStylePreprocessor"
QT_MOC_LITERAL(8, 105, 5) // "style"

    },
    "QsciLexerCPP\0setFoldAtElse\0\0fold\0"
    "setFoldComments\0setFoldCompact\0"
    "setFoldPreprocessor\0setStylePreprocessor\0"
    "style"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QsciLexerCPP[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       4,    1,   42,    2, 0x0a /* Public */,
       5,    1,   45,    2, 0x0a /* Public */,
       6,    1,   48,    2, 0x0a /* Public */,
       7,    1,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    8,

       0        // eod
};

void QsciLexerCPP::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QsciLexerCPP *_t = static_cast<QsciLexerCPP *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setFoldAtElse((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setFoldComments((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setFoldCompact((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setFoldPreprocessor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setStylePreprocessor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QsciLexerCPP::staticMetaObject = {
    { &QsciLexer::staticMetaObject, qt_meta_stringdata_QsciLexerCPP.data,
      qt_meta_data_QsciLexerCPP,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QsciLexerCPP::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QsciLexerCPP::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QsciLexerCPP.stringdata0))
        return static_cast<void*>(const_cast< QsciLexerCPP*>(this));
    return QsciLexer::qt_metacast(_clname);
}

int QsciLexerCPP::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QsciLexer::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
