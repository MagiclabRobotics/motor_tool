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
#define TXT_ID_MIN 0x200f
#define TXT_ID_MAX 0x208e

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    otaThread uart2can; /* 添加can成员 */

    QSerialPort *myCom;

    QString FilePath;

signals:
    void sendRecvData(QByteArray);

public slots:
    void StateUpdate();
    void readFromCom();
    void writeToCom(QByteArray, qint16);
    void waveFreShow(QVector<QCPGraphData> *);
    void myMoveEvent(QMouseEvent *);

private slots:
    void on_pushButtonOpenCom_clicked();

    void on_pushButtonOpenFile_clicked();

    void on_pushButtonInOtaMode_clicked();

    void on_pushButtonStartOta_clicked();

    void on_pushButtonGetDevInfo_clicked();

    void on_lineEditSetCanId_textChanged(const QString &arg1);

    void on_pushButtonSetCanId_clicked();

    void on_pushButtonCfgReset_clicked();

    void on_pushButtonCaliEncoder_clicked();

    void on_pushButtonZeroMech_clicked();

    void on_lineEditSetPos_textChanged(const QString &arg1);

    void on_lineEditSetPos_editingFinished();

    void on_lineEditSetKp_textChanged(const QString &arg1);

    void on_lineEditSetKp_editingFinished();

    void on_lineEditSetVel_textChanged(const QString &arg1);

    void on_lineEditSetVel_editingFinished();

    void on_lineEditSetKd_textChanged(const QString &arg1);

    void on_lineEditSetKd_editingFinished();

    void on_lineEditSetTorque_textChanged(const QString &arg1);

    void on_lineEditSetTorque_editingFinished();

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


    void on_comboBoxSelFrameType_currentIndexChanged(int index);

    //void on_pushButton_clicked();

    void on_combox_motor_ID_currentIndexChanged(int index);

    void on_lineEditSetP_max_textChanged(const QString &arg1);

    void on_lineEditSetP_min_textChanged(const QString &arg1);

    void on_lineEditSetV_max_textChanged(const QString &arg1);

    void on_lineEditSetV_min_textChanged(const QString &arg1);

    void on_lineEditSetT_max_textChanged(const QString &arg1);

    void on_lineEditSetT_min_textChanged(const QString &arg1);

    void on_lineEditSetKP_max_textChanged(const QString &arg1);

    void on_lineEditSetKP_min_textChanged(const QString &arg1);

    void on_lineEditSetKD_max_textChanged(const QString &arg1);

    void on_lineEditSetKD_min_textChanged(const QString &arg1);

    void on_comboBox_mot_mode_currentIndexChanged(int index);

    void on_lineEditSetKi_editingFinished();
    void on_lineEditSetSelfMode_textChanged(const QString &arg1);
    void on_lineEditSetSelfMode_editingFinished();
    void on_pushButtonTxtParaInsert_clicked();

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
    void importParametersFromTxt(QTableWidget *widget);
    QString color2Str(const QColor &color);

    void paraTableRefreshModifyFlags(QTableWidget *widget, quint16 code, bool setModifiedFlag);

    bool intoOtaFlag;
    bool IsComOpen; // serial port is open

    QFile saveLogFile;
    QString saveLogFileName;
    bool isSaveLogOpen;

    bool readParaAllFlag;
    bool writeAllModyfiedFlag;

    const static int tabWeight[PARA_TAB_COLUMN_NUM];

    qint8 baudTabPoint;

    quint16 chSelTotalNum;

    QStringList selCh_list;

    void getComboxConfig(QComboBox *comboBoxCh, bool Flag);

    QString path;
    float P_MIN_Tmp,P_MAX_Tmp, V_MIN_Tmp, V_MAX_Tmp, KP_MIN_Tmp, KP_MAX_Tmp, KD_MIN_Tmp, KD_MAX_Tmp, T_MIN_Tmp, T_MAX_Tmp;

    void RefrechData();
};

#endif // MAINWINDOW_H
