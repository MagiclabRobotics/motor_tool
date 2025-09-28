#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QScreen>
#include <QMainWindow>
#include <QFileDialog>
#include <QDateTime>
#include <ActiveQt/QAxObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include <QTableWidget>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#include "otaThread.h"
#include "wavedisplay.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

#define PARA_TAB_COLUMN_NUM (7)
#define PARA_TAB_EDIT_COLUMN (6)

#define COM_BOUND_DEFAULT (921600)

#define TXT_ID_MIN 0x200f
#define TXT_ID_MAX 0x208e
typedef enum
{
    MOTOR_RESET = 200,     // reset state
    DRV8353_RESTART = 100, // restart drv8353

    MOTOR_PARALL_CLOSE_LOOP_LOWSIDE = 1,   // parallel close loop control
    MOTOR_CASCADE_CLOSE_LOOP_LOWSIDE = 2,  // cascade close loop control
    MOTOR_PARALL_CLOSE_LOOP_HIGHSIDE = 3,  // parallel close loop control
    MOTOR_CASCADE_CLOSE_LOOP_HIGHSIDE = 4, // cascade close loop control
    MOTOR_ADRC_CLOSE_LOOP = 5,             // adrc close loop control
    MOTOR_TORQUE_CTRL_LOWSIDE = 6,         // torque control
    MOTOR_VELOCITY_CTRL_LOWSIDE = 7,       // velocity control
    MOTOR_ZERO_POS_CALI = 8,
    MOTOR_POS_LIMIT_NEGATIVE = 9,
    MOTOR_POS_LIMIT_POSITIVE = 10,
    MOTOR_SOFT_POS_LIMIT_NEGATIVE = 11,
    MOTOR_SOFT_POS_LIMIT_POSITIVE = 12,
    MOTOR_DIR_REVERSE = 13,
    MOTOR_IMAX_VMAX_SET = 14,
    // MOTOR_POS_GET=15,
    MOTOR_TORQ_CALI_SAVE = 16,
    MOTOR_CURLOOP_FRQ_SWEEP = 17,   // Current Loop Frequency Sweep
    MOTOR_LS_ENCODER_AUTOCALI = 18, // Lowside Encoder Calibration
    MOTOR_HS_LS_ENCODER_CALI = 19,  // Highside Encoder to calibrate Lowside

    MOTOR_POSITION_PROFILE_TEST = 246, // position profile
    MOTOR_VELOCITY_PROFILE_TEST = 247, // velocity profile
    MOTOR_TORQ_CALI_TimeMode = 248,    // Kt标定模式
    MOTOR_BISS_ENCODER_CALI = 249,     // open loop and disable the reading of IC Haus

    MOTOR_COGGSTUDY_MODE2 = 251, // cogging study base on ADRC
    MOTOR_COGGSTUDY_MODE = 252,  // cogging study
    MOTOR_OPEN_LOOP = 253,       // open loop
    MOTOR_EOFFSET_CALI = 254,    // eoffset calibration
    MOTOR_DISENABLE = 255,

} MOTOR_DRV_STATE;

struct _MODE_NAME
{
    quint8 m_num;
    QString m_name;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    otaThread uart2can; /* 添加can成员 */

    QSerialPort *myCom;
    QTimer *timer0;

    QString FilePath;

signals:
    void sendRecvData(QByteArray);

public slots:
    void StateUpdate();
    void readFromCom();
    void writeToCom(QByteArray, qint16);
    void waveFreShow(QVector<QCPGraphData> *);
    void myMoveEvent(QMouseEvent *);
    void t0_process();

private slots:
    void on_pushButtonOpenCom_clicked();

    void on_pushButtonOpenFile_clicked();

    void on_pushButtonInOtaMode_clicked();

    void on_pushButtonStartOta_clicked();

    void on_pushButtonGetDevInfo_clicked();

    // void on_pushButtonSetCanId_clicked();

    void on_pushButtonCfgReset_clicked();

    void on_pushButtonCaliEncoder_clicked();

    void on_pushButtonZeroMech_clicked();

    void on_pushButtonCfgSingle_clicked();

    void on_pushButtonCfgContinous_clicked();

    void on_pushButtonCfgMotor_clicked();

    void on_pushButtonRefreshCom_clicked();

    void on_comboBoxSelMode_currentIndexChanged(int index);

    void on_pushButtonSaveLog_clicked();

    void on_comboBoxDev_currentIndexChanged(int index);

    void on_pushButtonRefreshTable_clicked();

    void on_tableWidgetPara_cellChanged(int row, int column);

    void on_pushButtonCfgBrake_clicked();

    void on_pushButtonReadPara_clicked();

    void on_pushButtonWritePara_clicked();

    void on_pushButtonRecoverFac_clicked();

    void on_pushButtonExportExcel_clicked();

    void on_pushButtonEchoSet_clicked();

    void on_pushButtonEchoStart_clicked();

    void on_lineEditSetEchoFre_textChanged(const QString &arg1);

    void on_pushButtonSaveWave_clicked();

    //void on_pushButton_clicked();

    void on_comboBox_CtrlMode_currentIndexChanged(int index);

    void on_lineEditSetPos_Tar_editingFinished();
    void on_lineEditSetPos_Kp_editingFinished();
    void on_lineEditSetPos_Ki_editingFinished();
    void on_lineEditSetVel_Tar_editingFinished();
    void on_lineEditSetVel_Kp_editingFinished();
    void on_lineEditSetVel_Ki_editingFinished();
    void on_lineEditSetTor_Tar_editingFinished();
    void on_lineEditSetTor_Kp_editingFinished();
    // void on_lineEditSetTor_Ki_editingFinished();
    void on_pushButtonSavePara_clicked();
    void on_lineEdit_Mode_Self_editingFinished();
    void on_lineEditCustomPara0_editingFinished();
    void on_pushButtonCustomeCMD_clicked();

    void on_pushButtonTxtParaInsert_clicked();
    void onChannelCheckBoxStateChanged();
    void updateWaveDisplay();
private:
    Ui::MainWindow *ui;
    void changeObjectSize(const QObject &o, double objectRate);
    void noCanDevConfig();
    void detectedCanDevConfig();
    void paraTableClear(QTableWidget *widget);
    void paraTableLoadWritePara(QTableWidget *widget, quint16 rowStart);
    void paraTableLoadReadPara(QTableWidget *widget, quint16 rowStart);
    void paraTableLoadWriteStr(QTableWidget *widget, quint16 rowStart);
    void paraTableLoadReadStr(QTableWidget *widget, quint16 rowStart);
    void paraTableModifyStr(QTableWidget *widget, quint16 code);
    void paraTableModifyPara(QTableWidget *widget, quint16 code);

    void importParametersFromTxt(QTableWidget *widget);

    QString getParaTypeStr(enum paraType type);
    QString getParaLimitStr(enum paraType type, double val, int prec);
    QString getParaValStr(enum paraType type, const quint8 *add);
    QString setParaValStr(enum paraType type, quint8 *const add, double val);

    void paraTableUpdateAll(QTableWidget *widget);
    void paraTableRefreshVal(QTableWidget *widget, quint16 code);
    void paraTableRefreshModified(QTableWidget *widget, quint16 code);

    void selConfigInitChannal();
    void selConfigAllChannal();
    void selConfigRefrenPara();
    QString color2Str(const QColor &color);

    void paraTableRefreshModifyFlags(QTableWidget *widget, quint16 code, bool setModifiedFlag);

    bool intoOtaFlag; // ota升级模式标志位
    bool IsComOpen;   // serial port is open

    // 保存日志文件相关
    QFile saveLogFile;
    QString saveLogFileName;
    bool isSaveLogOpen;

    bool readParaAllFlag;      // 是否在读取参数
    bool writeAllModyfiedFlag; // 是否在写入参数

    const static int tabWeight[PARA_TAB_COLUMN_NUM];

    QStringList selCh_list; // 参数表使用字符串

    void getComboxConfig(QComboBox *comboBoxCh, bool Flag);

    QString path;
};

#endif // MAINWINDOW_H
