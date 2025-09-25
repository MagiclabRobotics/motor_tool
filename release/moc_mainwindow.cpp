/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../motor_tool/mainwindow.h"
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
    QByteArrayData data[68];
    char stringdata0[1901];
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
QT_MOC_LITERAL(10, 120, 28), // "on_pushButtonOpenCom_clicked"
QT_MOC_LITERAL(11, 149, 29), // "on_pushButtonOpenFile_clicked"
QT_MOC_LITERAL(12, 179, 30), // "on_pushButtonInOtaMode_clicked"
QT_MOC_LITERAL(13, 210, 29), // "on_pushButtonStartOta_clicked"
QT_MOC_LITERAL(14, 240, 31), // "on_pushButtonGetDevInfo_clicked"
QT_MOC_LITERAL(15, 272, 31), // "on_lineEditSetCanId_textChanged"
QT_MOC_LITERAL(16, 304, 4), // "arg1"
QT_MOC_LITERAL(17, 309, 29), // "on_pushButtonSetCanId_clicked"
QT_MOC_LITERAL(18, 339, 29), // "on_pushButtonCfgReset_clicked"
QT_MOC_LITERAL(19, 369, 32), // "on_pushButtonCaliEncoder_clicked"
QT_MOC_LITERAL(20, 402, 29), // "on_pushButtonZeroMech_clicked"
QT_MOC_LITERAL(21, 432, 29), // "on_lineEditSetPos_textChanged"
QT_MOC_LITERAL(22, 462, 33), // "on_lineEditSetPos_editingFini..."
QT_MOC_LITERAL(23, 496, 28), // "on_lineEditSetKp_textChanged"
QT_MOC_LITERAL(24, 525, 32), // "on_lineEditSetKp_editingFinished"
QT_MOC_LITERAL(25, 558, 29), // "on_lineEditSetVel_textChanged"
QT_MOC_LITERAL(26, 588, 33), // "on_lineEditSetVel_editingFini..."
QT_MOC_LITERAL(27, 622, 28), // "on_lineEditSetKd_textChanged"
QT_MOC_LITERAL(28, 651, 32), // "on_lineEditSetKd_editingFinished"
QT_MOC_LITERAL(29, 684, 32), // "on_lineEditSetTorque_textChanged"
QT_MOC_LITERAL(30, 717, 36), // "on_lineEditSetTorque_editingF..."
QT_MOC_LITERAL(31, 754, 30), // "on_pushButtonCfgSingle_clicked"
QT_MOC_LITERAL(32, 785, 33), // "on_pushButtonCfgContinous_cli..."
QT_MOC_LITERAL(33, 819, 29), // "on_pushButtonCfgMotor_clicked"
QT_MOC_LITERAL(34, 849, 31), // "on_pushButtonRefreshCom_clicked"
QT_MOC_LITERAL(35, 881, 38), // "on_comboBoxSelMode_currentInd..."
QT_MOC_LITERAL(36, 920, 5), // "index"
QT_MOC_LITERAL(37, 926, 28), // "on_pushButtonSaveLog_clicked"
QT_MOC_LITERAL(38, 955, 34), // "on_comboBoxDev_currentIndexCh..."
QT_MOC_LITERAL(39, 990, 33), // "on_pushButtonRefreshTable_cli..."
QT_MOC_LITERAL(40, 1024, 30), // "on_tableWidgetPara_cellChanged"
QT_MOC_LITERAL(41, 1055, 3), // "row"
QT_MOC_LITERAL(42, 1059, 6), // "column"
QT_MOC_LITERAL(43, 1066, 29), // "on_pushButtonCfgBrake_clicked"
QT_MOC_LITERAL(44, 1096, 29), // "on_pushButtonReadPara_clicked"
QT_MOC_LITERAL(45, 1126, 30), // "on_pushButtonWritePara_clicked"
QT_MOC_LITERAL(46, 1157, 31), // "on_pushButtonRecoverFac_clicked"
QT_MOC_LITERAL(47, 1189, 32), // "on_pushButtonExportExcel_clicked"
QT_MOC_LITERAL(48, 1222, 28), // "on_pushButtonEchoSet_clicked"
QT_MOC_LITERAL(49, 1251, 30), // "on_pushButtonEchoStart_clicked"
QT_MOC_LITERAL(50, 1282, 33), // "on_lineEditSetEchoFre_textCha..."
QT_MOC_LITERAL(51, 1316, 29), // "on_pushButtonSaveWave_clicked"
QT_MOC_LITERAL(52, 1346, 43), // "on_comboBoxSelFrameType_curre..."
QT_MOC_LITERAL(53, 1390, 38), // "on_combox_motor_ID_currentInd..."
QT_MOC_LITERAL(54, 1429, 31), // "on_lineEditSetP_max_textChanged"
QT_MOC_LITERAL(55, 1461, 31), // "on_lineEditSetP_min_textChanged"
QT_MOC_LITERAL(56, 1493, 31), // "on_lineEditSetV_max_textChanged"
QT_MOC_LITERAL(57, 1525, 31), // "on_lineEditSetV_min_textChanged"
QT_MOC_LITERAL(58, 1557, 31), // "on_lineEditSetT_max_textChanged"
QT_MOC_LITERAL(59, 1589, 31), // "on_lineEditSetT_min_textChanged"
QT_MOC_LITERAL(60, 1621, 32), // "on_lineEditSetKP_max_textChanged"
QT_MOC_LITERAL(61, 1654, 32), // "on_lineEditSetKP_min_textChanged"
QT_MOC_LITERAL(62, 1687, 32), // "on_lineEditSetKD_max_textChanged"
QT_MOC_LITERAL(63, 1720, 32), // "on_lineEditSetKD_min_textChanged"
QT_MOC_LITERAL(64, 1753, 40), // "on_comboBox_mot_mode_currentI..."
QT_MOC_LITERAL(65, 1794, 32), // "on_lineEditSetKi_editingFinished"
QT_MOC_LITERAL(66, 1827, 34), // "on_lineEditSetSelfMode_textCh..."
QT_MOC_LITERAL(67, 1862, 38) // "on_lineEditSetSelfMode_editin..."

    },
    "MainWindow\0sendRecvData\0\0StateUpdate\0"
    "readFromCom\0writeToCom\0waveFreShow\0"
    "QVector<QCPGraphData>*\0myMoveEvent\0"
    "QMouseEvent*\0on_pushButtonOpenCom_clicked\0"
    "on_pushButtonOpenFile_clicked\0"
    "on_pushButtonInOtaMode_clicked\0"
    "on_pushButtonStartOta_clicked\0"
    "on_pushButtonGetDevInfo_clicked\0"
    "on_lineEditSetCanId_textChanged\0arg1\0"
    "on_pushButtonSetCanId_clicked\0"
    "on_pushButtonCfgReset_clicked\0"
    "on_pushButtonCaliEncoder_clicked\0"
    "on_pushButtonZeroMech_clicked\0"
    "on_lineEditSetPos_textChanged\0"
    "on_lineEditSetPos_editingFinished\0"
    "on_lineEditSetKp_textChanged\0"
    "on_lineEditSetKp_editingFinished\0"
    "on_lineEditSetVel_textChanged\0"
    "on_lineEditSetVel_editingFinished\0"
    "on_lineEditSetKd_textChanged\0"
    "on_lineEditSetKd_editingFinished\0"
    "on_lineEditSetTorque_textChanged\0"
    "on_lineEditSetTorque_editingFinished\0"
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
    "on_pushButtonSaveWave_clicked\0"
    "on_comboBoxSelFrameType_currentIndexChanged\0"
    "on_combox_motor_ID_currentIndexChanged\0"
    "on_lineEditSetP_max_textChanged\0"
    "on_lineEditSetP_min_textChanged\0"
    "on_lineEditSetV_max_textChanged\0"
    "on_lineEditSetV_min_textChanged\0"
    "on_lineEditSetT_max_textChanged\0"
    "on_lineEditSetT_min_textChanged\0"
    "on_lineEditSetKP_max_textChanged\0"
    "on_lineEditSetKP_min_textChanged\0"
    "on_lineEditSetKD_max_textChanged\0"
    "on_lineEditSetKD_min_textChanged\0"
    "on_comboBox_mot_mode_currentIndexChanged\0"
    "on_lineEditSetKi_editingFinished\0"
    "on_lineEditSetSelfMode_textChanged\0"
    "on_lineEditSetSelfMode_editingFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      60,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  314,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  317,    2, 0x0a /* Public */,
       4,    0,  318,    2, 0x0a /* Public */,
       5,    2,  319,    2, 0x0a /* Public */,
       6,    1,  324,    2, 0x0a /* Public */,
       8,    1,  327,    2, 0x0a /* Public */,
      10,    0,  330,    2, 0x08 /* Private */,
      11,    0,  331,    2, 0x08 /* Private */,
      12,    0,  332,    2, 0x08 /* Private */,
      13,    0,  333,    2, 0x08 /* Private */,
      14,    0,  334,    2, 0x08 /* Private */,
      15,    1,  335,    2, 0x08 /* Private */,
      17,    0,  338,    2, 0x08 /* Private */,
      18,    0,  339,    2, 0x08 /* Private */,
      19,    0,  340,    2, 0x08 /* Private */,
      20,    0,  341,    2, 0x08 /* Private */,
      21,    1,  342,    2, 0x08 /* Private */,
      22,    0,  345,    2, 0x08 /* Private */,
      23,    1,  346,    2, 0x08 /* Private */,
      24,    0,  349,    2, 0x08 /* Private */,
      25,    1,  350,    2, 0x08 /* Private */,
      26,    0,  353,    2, 0x08 /* Private */,
      27,    1,  354,    2, 0x08 /* Private */,
      28,    0,  357,    2, 0x08 /* Private */,
      29,    1,  358,    2, 0x08 /* Private */,
      30,    0,  361,    2, 0x08 /* Private */,
      31,    0,  362,    2, 0x08 /* Private */,
      32,    0,  363,    2, 0x08 /* Private */,
      33,    0,  364,    2, 0x08 /* Private */,
      34,    0,  365,    2, 0x08 /* Private */,
      35,    1,  366,    2, 0x08 /* Private */,
      37,    0,  369,    2, 0x08 /* Private */,
      38,    1,  370,    2, 0x08 /* Private */,
      39,    0,  373,    2, 0x08 /* Private */,
      40,    2,  374,    2, 0x08 /* Private */,
      43,    0,  379,    2, 0x08 /* Private */,
      44,    0,  380,    2, 0x08 /* Private */,
      45,    0,  381,    2, 0x08 /* Private */,
      46,    0,  382,    2, 0x08 /* Private */,
      47,    0,  383,    2, 0x08 /* Private */,
      48,    0,  384,    2, 0x08 /* Private */,
      49,    0,  385,    2, 0x08 /* Private */,
      50,    1,  386,    2, 0x08 /* Private */,
      51,    0,  389,    2, 0x08 /* Private */,
      52,    1,  390,    2, 0x08 /* Private */,
      53,    1,  393,    2, 0x08 /* Private */,
      54,    1,  396,    2, 0x08 /* Private */,
      55,    1,  399,    2, 0x08 /* Private */,
      56,    1,  402,    2, 0x08 /* Private */,
      57,    1,  405,    2, 0x08 /* Private */,
      58,    1,  408,    2, 0x08 /* Private */,
      59,    1,  411,    2, 0x08 /* Private */,
      60,    1,  414,    2, 0x08 /* Private */,
      61,    1,  417,    2, 0x08 /* Private */,
      62,    1,  420,    2, 0x08 /* Private */,
      63,    1,  423,    2, 0x08 /* Private */,
      64,    1,  426,    2, 0x08 /* Private */,
      65,    0,  429,    2, 0x08 /* Private */,
      66,    1,  430,    2, 0x08 /* Private */,
      67,    0,  433,    2, 0x08 /* Private */,

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
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   41,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
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
        case 6: _t->on_pushButtonOpenCom_clicked(); break;
        case 7: _t->on_pushButtonOpenFile_clicked(); break;
        case 8: _t->on_pushButtonInOtaMode_clicked(); break;
        case 9: _t->on_pushButtonStartOta_clicked(); break;
        case 10: _t->on_pushButtonGetDevInfo_clicked(); break;
        case 11: _t->on_lineEditSetCanId_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->on_pushButtonSetCanId_clicked(); break;
        case 13: _t->on_pushButtonCfgReset_clicked(); break;
        case 14: _t->on_pushButtonCaliEncoder_clicked(); break;
        case 15: _t->on_pushButtonZeroMech_clicked(); break;
        case 16: _t->on_lineEditSetPos_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->on_lineEditSetPos_editingFinished(); break;
        case 18: _t->on_lineEditSetKp_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->on_lineEditSetKp_editingFinished(); break;
        case 20: _t->on_lineEditSetVel_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: _t->on_lineEditSetVel_editingFinished(); break;
        case 22: _t->on_lineEditSetKd_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 23: _t->on_lineEditSetKd_editingFinished(); break;
        case 24: _t->on_lineEditSetTorque_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 25: _t->on_lineEditSetTorque_editingFinished(); break;
        case 26: _t->on_pushButtonCfgSingle_clicked(); break;
        case 27: _t->on_pushButtonCfgContinous_clicked(); break;
        case 28: _t->on_pushButtonCfgMotor_clicked(); break;
        case 29: _t->on_pushButtonRefreshCom_clicked(); break;
        case 30: _t->on_comboBoxSelMode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: _t->on_pushButtonSaveLog_clicked(); break;
        case 32: _t->on_comboBoxDev_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->on_pushButtonRefreshTable_clicked(); break;
        case 34: _t->on_tableWidgetPara_cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 35: _t->on_pushButtonCfgBrake_clicked(); break;
        case 36: _t->on_pushButtonReadPara_clicked(); break;
        case 37: _t->on_pushButtonWritePara_clicked(); break;
        case 38: _t->on_pushButtonRecoverFac_clicked(); break;
        case 39: _t->on_pushButtonExportExcel_clicked(); break;
        case 40: _t->on_pushButtonEchoSet_clicked(); break;
        case 41: _t->on_pushButtonEchoStart_clicked(); break;
        case 42: _t->on_lineEditSetEchoFre_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 43: _t->on_pushButtonSaveWave_clicked(); break;
        case 44: _t->on_comboBoxSelFrameType_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 45: _t->on_combox_motor_ID_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 46: _t->on_lineEditSetP_max_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 47: _t->on_lineEditSetP_min_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 48: _t->on_lineEditSetV_max_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 49: _t->on_lineEditSetV_min_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 50: _t->on_lineEditSetT_max_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 51: _t->on_lineEditSetT_min_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 52: _t->on_lineEditSetKP_max_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 53: _t->on_lineEditSetKP_min_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 54: _t->on_lineEditSetKD_max_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 55: _t->on_lineEditSetKD_min_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 56: _t->on_comboBox_mot_mode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 57: _t->on_lineEditSetKi_editingFinished(); break;
        case 58: _t->on_lineEditSetSelfMode_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 59: _t->on_lineEditSetSelfMode_editingFinished(); break;
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
        if (_id < 60)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 60;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 60)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 60;
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
