/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../motor_tool/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[57];
    char stringdata0[1535];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 12), // "sendRecvData"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 11), // "StateUpdate"
QT_MOC_LITERAL(4, 37, 11), // "readFromCom"
QT_MOC_LITERAL(5, 49, 10), // "writeToCom"
QT_MOC_LITERAL(6, 60, 11), // "waveFreShow"
QT_MOC_LITERAL(7, 72, 22), // "QVector<QCPGraphData>*"
QT_MOC_LITERAL(8, 95, 11), // "myMoveEvent"
QT_MOC_LITERAL(9, 107, 12), // "QMouseEvent*"
QT_MOC_LITERAL(10, 120, 10), // "t0_process"
QT_MOC_LITERAL(11, 131, 28), // "on_pushButtonOpenCom_clicked"
QT_MOC_LITERAL(12, 160, 29), // "on_pushButtonOpenFile_clicked"
QT_MOC_LITERAL(13, 190, 30), // "on_pushButtonInOtaMode_clicked"
QT_MOC_LITERAL(14, 221, 29), // "on_pushButtonStartOta_clicked"
QT_MOC_LITERAL(15, 251, 31), // "on_pushButtonGetDevInfo_clicked"
QT_MOC_LITERAL(16, 283, 29), // "on_pushButtonCfgReset_clicked"
QT_MOC_LITERAL(17, 313, 32), // "on_pushButtonCaliEncoder_clicked"
QT_MOC_LITERAL(18, 346, 29), // "on_pushButtonZeroMech_clicked"
QT_MOC_LITERAL(19, 376, 30), // "on_pushButtonCfgSingle_clicked"
QT_MOC_LITERAL(20, 407, 33), // "on_pushButtonCfgContinous_cli..."
QT_MOC_LITERAL(21, 441, 29), // "on_pushButtonCfgMotor_clicked"
QT_MOC_LITERAL(22, 471, 31), // "on_pushButtonRefreshCom_clicked"
QT_MOC_LITERAL(23, 503, 38), // "on_comboBoxSelMode_currentInd..."
QT_MOC_LITERAL(24, 542, 5), // "index"
QT_MOC_LITERAL(25, 548, 28), // "on_pushButtonSaveLog_clicked"
QT_MOC_LITERAL(26, 577, 34), // "on_comboBoxDev_currentIndexCh..."
QT_MOC_LITERAL(27, 612, 33), // "on_pushButtonRefreshTable_cli..."
QT_MOC_LITERAL(28, 646, 30), // "on_tableWidgetPara_cellChanged"
QT_MOC_LITERAL(29, 677, 3), // "row"
QT_MOC_LITERAL(30, 681, 6), // "column"
QT_MOC_LITERAL(31, 688, 29), // "on_pushButtonCfgBrake_clicked"
QT_MOC_LITERAL(32, 718, 29), // "on_pushButtonReadPara_clicked"
QT_MOC_LITERAL(33, 748, 30), // "on_pushButtonWritePara_clicked"
QT_MOC_LITERAL(34, 779, 31), // "on_pushButtonRecoverFac_clicked"
QT_MOC_LITERAL(35, 811, 32), // "on_pushButtonExportExcel_clicked"
QT_MOC_LITERAL(36, 844, 28), // "on_pushButtonEchoSet_clicked"
QT_MOC_LITERAL(37, 873, 30), // "on_pushButtonEchoStart_clicked"
QT_MOC_LITERAL(38, 904, 33), // "on_lineEditSetEchoFre_textCha..."
QT_MOC_LITERAL(39, 938, 4), // "arg1"
QT_MOC_LITERAL(40, 943, 29), // "on_pushButtonSaveWave_clicked"
QT_MOC_LITERAL(41, 973, 40), // "on_comboBox_CtrlMode_currentI..."
QT_MOC_LITERAL(42, 1014, 37), // "on_lineEditSetPos_Tar_editing..."
QT_MOC_LITERAL(43, 1052, 36), // "on_lineEditSetPos_Kp_editingF..."
QT_MOC_LITERAL(44, 1089, 36), // "on_lineEditSetPos_Ki_editingF..."
QT_MOC_LITERAL(45, 1126, 37), // "on_lineEditSetVel_Tar_editing..."
QT_MOC_LITERAL(46, 1164, 36), // "on_lineEditSetVel_Kp_editingF..."
QT_MOC_LITERAL(47, 1201, 36), // "on_lineEditSetVel_Ki_editingF..."
QT_MOC_LITERAL(48, 1238, 37), // "on_lineEditSetTor_Tar_editing..."
QT_MOC_LITERAL(49, 1276, 36), // "on_lineEditSetTor_Kp_editingF..."
QT_MOC_LITERAL(50, 1313, 29), // "on_pushButtonSavePara_clicked"
QT_MOC_LITERAL(51, 1343, 37), // "on_lineEdit_Mode_Self_editing..."
QT_MOC_LITERAL(52, 1381, 38), // "on_lineEditCustomPara0_editin..."
QT_MOC_LITERAL(53, 1420, 31), // "on_pushButtonCustomeCMD_clicked"
QT_MOC_LITERAL(54, 1452, 34), // "on_pushButtonTxtParaInsert_cl..."
QT_MOC_LITERAL(55, 1487, 29), // "onChannelCheckBoxStateChanged"
QT_MOC_LITERAL(56, 1517, 17) // "updateWaveDisplay"

    },
    "MainWindow\0sendRecvData\0\0StateUpdate\0"
    "readFromCom\0writeToCom\0waveFreShow\0"
    "QVector<QCPGraphData>*\0myMoveEvent\0"
    "QMouseEvent*\0t0_process\0"
    "on_pushButtonOpenCom_clicked\0"
    "on_pushButtonOpenFile_clicked\0"
    "on_pushButtonInOtaMode_clicked\0"
    "on_pushButtonStartOta_clicked\0"
    "on_pushButtonGetDevInfo_clicked\0"
    "on_pushButtonCfgReset_clicked\0"
    "on_pushButtonCaliEncoder_clicked\0"
    "on_pushButtonZeroMech_clicked\0"
    "on_pushButtonCfgSingle_clicked\0"
    "on_pushButtonCfgContinous_clicked\0"
    "on_pushButtonCfgMotor_clicked\0"
    "on_pushButtonRefreshCom_clicked\0"
    "on_comboBoxSelMode_currentIndexChanged\0"
    "index\0on_pushButtonSaveLog_clicked\0"
    "on_comboBoxDev_currentIndexChanged\0"
    "on_pushButtonRefreshTable_clicked\0"
    "on_tableWidgetPara_cellChanged\0row\0"
    "column\0on_pushButtonCfgBrake_clicked\0"
    "on_pushButtonReadPara_clicked\0"
    "on_pushButtonWritePara_clicked\0"
    "on_pushButtonRecoverFac_clicked\0"
    "on_pushButtonExportExcel_clicked\0"
    "on_pushButtonEchoSet_clicked\0"
    "on_pushButtonEchoStart_clicked\0"
    "on_lineEditSetEchoFre_textChanged\0"
    "arg1\0on_pushButtonSaveWave_clicked\0"
    "on_comboBox_CtrlMode_currentIndexChanged\0"
    "on_lineEditSetPos_Tar_editingFinished\0"
    "on_lineEditSetPos_Kp_editingFinished\0"
    "on_lineEditSetPos_Ki_editingFinished\0"
    "on_lineEditSetVel_Tar_editingFinished\0"
    "on_lineEditSetVel_Kp_editingFinished\0"
    "on_lineEditSetVel_Ki_editingFinished\0"
    "on_lineEditSetTor_Tar_editingFinished\0"
    "on_lineEditSetTor_Kp_editingFinished\0"
    "on_pushButtonSavePara_clicked\0"
    "on_lineEdit_Mode_Self_editingFinished\0"
    "on_lineEditCustomPara0_editingFinished\0"
    "on_pushButtonCustomeCMD_clicked\0"
    "on_pushButtonTxtParaInsert_clicked\0"
    "onChannelCheckBoxStateChanged\0"
    "updateWaveDisplay"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      49,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  259,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  262,    2, 0x0a /* Public */,
       4,    0,  263,    2, 0x0a /* Public */,
       5,    2,  264,    2, 0x0a /* Public */,
       6,    1,  269,    2, 0x0a /* Public */,
       8,    1,  272,    2, 0x0a /* Public */,
      10,    0,  275,    2, 0x0a /* Public */,
      11,    0,  276,    2, 0x08 /* Private */,
      12,    0,  277,    2, 0x08 /* Private */,
      13,    0,  278,    2, 0x08 /* Private */,
      14,    0,  279,    2, 0x08 /* Private */,
      15,    0,  280,    2, 0x08 /* Private */,
      16,    0,  281,    2, 0x08 /* Private */,
      17,    0,  282,    2, 0x08 /* Private */,
      18,    0,  283,    2, 0x08 /* Private */,
      19,    0,  284,    2, 0x08 /* Private */,
      20,    0,  285,    2, 0x08 /* Private */,
      21,    0,  286,    2, 0x08 /* Private */,
      22,    0,  287,    2, 0x08 /* Private */,
      23,    1,  288,    2, 0x08 /* Private */,
      25,    0,  291,    2, 0x08 /* Private */,
      26,    1,  292,    2, 0x08 /* Private */,
      27,    0,  295,    2, 0x08 /* Private */,
      28,    2,  296,    2, 0x08 /* Private */,
      31,    0,  301,    2, 0x08 /* Private */,
      32,    0,  302,    2, 0x08 /* Private */,
      33,    0,  303,    2, 0x08 /* Private */,
      34,    0,  304,    2, 0x08 /* Private */,
      35,    0,  305,    2, 0x08 /* Private */,
      36,    0,  306,    2, 0x08 /* Private */,
      37,    0,  307,    2, 0x08 /* Private */,
      38,    1,  308,    2, 0x08 /* Private */,
      40,    0,  311,    2, 0x08 /* Private */,
      41,    1,  312,    2, 0x08 /* Private */,
      42,    0,  315,    2, 0x08 /* Private */,
      43,    0,  316,    2, 0x08 /* Private */,
      44,    0,  317,    2, 0x08 /* Private */,
      45,    0,  318,    2, 0x08 /* Private */,
      46,    0,  319,    2, 0x08 /* Private */,
      47,    0,  320,    2, 0x08 /* Private */,
      48,    0,  321,    2, 0x08 /* Private */,
      49,    0,  322,    2, 0x08 /* Private */,
      50,    0,  323,    2, 0x08 /* Private */,
      51,    0,  324,    2, 0x08 /* Private */,
      52,    0,  325,    2, 0x08 /* Private */,
      53,    0,  326,    2, 0x08 /* Private */,
      54,    0,  327,    2, 0x08 /* Private */,
      55,    0,  328,    2, 0x08 /* Private */,
      56,    0,  329,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Short,    2,    2,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   29,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   39,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendRecvData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->StateUpdate(); break;
        case 2: _t->readFromCom(); break;
        case 3: _t->writeToCom((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< qint16(*)>(_a[2]))); break;
        case 4: _t->waveFreShow((*reinterpret_cast< QVector<QCPGraphData>*(*)>(_a[1]))); break;
        case 5: _t->myMoveEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 6: _t->t0_process(); break;
        case 7: _t->on_pushButtonOpenCom_clicked(); break;
        case 8: _t->on_pushButtonOpenFile_clicked(); break;
        case 9: _t->on_pushButtonInOtaMode_clicked(); break;
        case 10: _t->on_pushButtonStartOta_clicked(); break;
        case 11: _t->on_pushButtonGetDevInfo_clicked(); break;
        case 12: _t->on_pushButtonCfgReset_clicked(); break;
        case 13: _t->on_pushButtonCaliEncoder_clicked(); break;
        case 14: _t->on_pushButtonZeroMech_clicked(); break;
        case 15: _t->on_pushButtonCfgSingle_clicked(); break;
        case 16: _t->on_pushButtonCfgContinous_clicked(); break;
        case 17: _t->on_pushButtonCfgMotor_clicked(); break;
        case 18: _t->on_pushButtonRefreshCom_clicked(); break;
        case 19: _t->on_comboBoxSelMode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->on_pushButtonSaveLog_clicked(); break;
        case 21: _t->on_comboBoxDev_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->on_pushButtonRefreshTable_clicked(); break;
        case 23: _t->on_tableWidgetPara_cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 24: _t->on_pushButtonCfgBrake_clicked(); break;
        case 25: _t->on_pushButtonReadPara_clicked(); break;
        case 26: _t->on_pushButtonWritePara_clicked(); break;
        case 27: _t->on_pushButtonRecoverFac_clicked(); break;
        case 28: _t->on_pushButtonExportExcel_clicked(); break;
        case 29: _t->on_pushButtonEchoSet_clicked(); break;
        case 30: _t->on_pushButtonEchoStart_clicked(); break;
        case 31: _t->on_lineEditSetEchoFre_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 32: _t->on_pushButtonSaveWave_clicked(); break;
        case 33: _t->on_comboBox_CtrlMode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 34: _t->on_lineEditSetPos_Tar_editingFinished(); break;
        case 35: _t->on_lineEditSetPos_Kp_editingFinished(); break;
        case 36: _t->on_lineEditSetPos_Ki_editingFinished(); break;
        case 37: _t->on_lineEditSetVel_Tar_editingFinished(); break;
        case 38: _t->on_lineEditSetVel_Kp_editingFinished(); break;
        case 39: _t->on_lineEditSetVel_Ki_editingFinished(); break;
        case 40: _t->on_lineEditSetTor_Tar_editingFinished(); break;
        case 41: _t->on_lineEditSetTor_Kp_editingFinished(); break;
        case 42: _t->on_pushButtonSavePara_clicked(); break;
        case 43: _t->on_lineEdit_Mode_Self_editingFinished(); break;
        case 44: _t->on_lineEditCustomPara0_editingFinished(); break;
        case 45: _t->on_pushButtonCustomeCMD_clicked(); break;
        case 46: _t->on_pushButtonTxtParaInsert_clicked(); break;
        case 47: _t->onChannelCheckBoxStateChanged(); break;
        case 48: _t->updateWaveDisplay(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::sendRecvData)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


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
        if (_id < 49)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 49;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 49)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 49;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::sendRecvData(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
