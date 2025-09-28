/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include <wavedisplay.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab_serial;
    QPushButton *pushButtonOpenCom;
    QLabel *label_2;
    QComboBox *comboBoxCom;
    QPushButton *pushButtonRefreshCom;
    QWidget *tab_ota;
    QPushButton *pushButtonInOtaMode;
    QLabel *label_3;
    QLabel *labelOtaInfo;
    QProgressBar *progressBar;
    QLabel *label_4;
    QPushButton *pushButtonStartOta;
    QPushButton *pushButtonOpenFile;
    QLineEdit *lineEditSelFile;
    QWidget *tab_config;
    QTextEdit *textEditCfgInfo;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_16;
    QLineEdit *lineEditTxData;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButtonSaveLog;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_36;
    QLineEdit *lineEditCustomPara0;
    QPushButton *pushButtonCustomeCMD;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButtonZeroMech;
    QPushButton *pushButtonCaliEncoder;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout;
    QLabel *label_19;
    QLineEdit *lineEditCustomPara1;
    QLabel *label_20;
    QLineEdit *lineEditCustomPara2;
    QLabel *label_21;
    QLineEdit *lineEditCustomPara3;
    QLabel *label_22;
    QLineEdit *lineEditCustomPara4;
    QLabel *label_26;
    QLineEdit *lineEditCustomPara5;
    QLabel *label_27;
    QLineEdit *lineEditCustomPara6;
    QLabel *label_34;
    QLineEdit *lineEditCustomPara7;
    QLabel *label_35;
    QLineEdit *lineEditCustomPara8;
    QWidget *tab_setPara;
    QTableWidget *tableWidgetPara;
    QPushButton *pushButtonRefreshTable;
    QPushButton *pushButtonReadPara;
    QPushButton *pushButtonWritePara;
    QLabel *labelParaInfo;
    QPushButton *pushButtonRecoverFac;
    QPushButton *pushButtonExportExcel;
    QPushButton *pushButtonSavePara;
    QPushButton *pushButtonTxtParaInsert;
    QWidget *tab_Oscilloscope;
    QComboBox *comboBoxCh1;
    QComboBox *comboBoxCh2;
    QComboBox *comboBoxCh3;
    QComboBox *comboBoxCh4;
    QComboBox *comboBoxCh5;
    QComboBox *comboBoxCh6;
    QComboBox *comboBoxCh7;
    QComboBox *comboBoxCh8;
    QLineEdit *lineEditSetEchoFre;
    QLabel *label_13;
    QLabel *label_15;
    QPushButton *pushButtonEchoStart;
    QLabel *label_selChInfo;
    QPushButton *pushButtonEchoSet;
    waveDisplay *waveShow;
    QPushButton *pushButtonSaveWave;
    QCheckBox *checkBox_show_value;
    QCheckBox *checkBoxCh1;
    QCheckBox *checkBoxCh2;
    QCheckBox *checkBoxCh3;
    QCheckBox *checkBoxCh4;
    QCheckBox *checkBoxCh5;
    QCheckBox *checkBoxCh6;
    QCheckBox *checkBoxCh7;
    QCheckBox *checkBoxCh8;
    QComboBox *comboBoxDev;
    QPushButton *pushButtonGetDevInfo;
    QLabel *label_6;
    QPushButton *pushButtonCfgMotor;
    QPushButton *pushButtonCfgReset;
    QPushButton *pushButtonCfgBrake;
    QPushButton *pushButtonCfgSingle;
    QPushButton *pushButtonCfgContinous;
    QComboBox *comboBox_CtrlMode;
    QLabel *labelCtrlInfo;
    QWidget *layoutWidget3;
    QFormLayout *formLayout;
    QLabel *label_14;
    QLineEdit *lineEditSetPos_Tar;
    QLabel *label_23;
    QLineEdit *lineEditSetPos_Kp;
    QLabel *label_28;
    QLineEdit *lineEditSetPos_Ki;
    QWidget *widget_20;
    QLabel *label_30;
    QLabel *label_17;
    QLineEdit *lineEditSetVel_Tar;
    QLabel *label_24;
    QLineEdit *lineEditSetVel_Kp;
    QLabel *label_32;
    QLineEdit *lineEditSetVel_Ki;
    QWidget *widget_21;
    QLabel *label_31;
    QLabel *label_18;
    QLineEdit *lineEditSetTor_Tar;
    QLabel *label_25;
    QLineEdit *lineEditSetTor_Kp;
    QLabel *label_33;
    QLineEdit *lineEdit_Err_Code;
    QLabel *label_7;
    QWidget *layoutWidget4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_mode_self;
    QLineEdit *lineEdit_Mode_Self;
    QLineEdit *lineEdit_Cur_Stat;
    QLabel *label_29;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1120, 649);
        MainWindow->setIconSize(QSize(35, 30));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 30, 901, 591));
        QFont font;
        font.setPointSize(9);
        tabWidget->setFont(font);
        tab_serial = new QWidget();
        tab_serial->setObjectName(QString::fromUtf8("tab_serial"));
        pushButtonOpenCom = new QPushButton(tab_serial);
        pushButtonOpenCom->setObjectName(QString::fromUtf8("pushButtonOpenCom"));
        pushButtonOpenCom->setGeometry(QRect(380, 160, 81, 23));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(9);
        pushButtonOpenCom->setFont(font1);
        label_2 = new QLabel(tab_serial);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(300, 110, 61, 31));
        label_2->setFont(font1);
        comboBoxCom = new QComboBox(tab_serial);
        comboBoxCom->setObjectName(QString::fromUtf8("comboBoxCom"));
        comboBoxCom->setGeometry(QRect(380, 115, 81, 22));
        comboBoxCom->setFont(font1);
        pushButtonRefreshCom = new QPushButton(tab_serial);
        pushButtonRefreshCom->setObjectName(QString::fromUtf8("pushButtonRefreshCom"));
        pushButtonRefreshCom->setGeometry(QRect(489, 114, 75, 23));
        tabWidget->addTab(tab_serial, QString());
        tab_ota = new QWidget();
        tab_ota->setObjectName(QString::fromUtf8("tab_ota"));
        pushButtonInOtaMode = new QPushButton(tab_ota);
        pushButtonInOtaMode->setObjectName(QString::fromUtf8("pushButtonInOtaMode"));
        pushButtonInOtaMode->setGeometry(QRect(720, 100, 81, 23));
        pushButtonInOtaMode->setFont(font1);
        pushButtonInOtaMode->setCursor(QCursor(Qt::ArrowCursor));
        label_3 = new QLabel(tab_ota);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 130, 91, 31));
        label_3->setFont(font1);
        labelOtaInfo = new QLabel(tab_ota);
        labelOtaInfo->setObjectName(QString::fromUtf8("labelOtaInfo"));
        labelOtaInfo->setGeometry(QRect(150, 90, 531, 31));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font2.setPointSize(9);
        font2.setUnderline(false);
        labelOtaInfo->setFont(font2);
        labelOtaInfo->setCursor(QCursor(Qt::ArrowCursor));
        progressBar = new QProgressBar(tab_ota);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(150, 180, 541, 23));
        progressBar->setValue(24);
        label_4 = new QLabel(tab_ota);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(50, 176, 71, 31));
        label_4->setFont(font1);
        pushButtonStartOta = new QPushButton(tab_ota);
        pushButtonStartOta->setObjectName(QString::fromUtf8("pushButtonStartOta"));
        pushButtonStartOta->setGeometry(QRect(720, 180, 81, 23));
        pushButtonStartOta->setFont(font1);
        pushButtonOpenFile = new QPushButton(tab_ota);
        pushButtonOpenFile->setObjectName(QString::fromUtf8("pushButtonOpenFile"));
        pushButtonOpenFile->setGeometry(QRect(720, 140, 81, 23));
        pushButtonOpenFile->setFont(font1);
        lineEditSelFile = new QLineEdit(tab_ota);
        lineEditSelFile->setObjectName(QString::fromUtf8("lineEditSelFile"));
        lineEditSelFile->setGeometry(QRect(150, 140, 531, 20));
        tabWidget->addTab(tab_ota, QString());
        tab_config = new QWidget();
        tab_config->setObjectName(QString::fromUtf8("tab_config"));
        textEditCfgInfo = new QTextEdit(tab_config);
        textEditCfgInfo->setObjectName(QString::fromUtf8("textEditCfgInfo"));
        textEditCfgInfo->setGeometry(QRect(10, 100, 871, 421));
        layoutWidget = new QWidget(tab_config);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 530, 871, 31));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_16 = new QLabel(layoutWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setFont(font1);

        horizontalLayout_3->addWidget(label_16);

        lineEditTxData = new QLineEdit(layoutWidget);
        lineEditTxData->setObjectName(QString::fromUtf8("lineEditTxData"));
        lineEditTxData->setMinimumSize(QSize(550, 0));
        lineEditTxData->setFont(font1);
        lineEditTxData->setLayoutDirection(Qt::LeftToRight);
        lineEditTxData->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditTxData->setReadOnly(true);

        horizontalLayout_3->addWidget(lineEditTxData);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        pushButtonSaveLog = new QPushButton(layoutWidget);
        pushButtonSaveLog->setObjectName(QString::fromUtf8("pushButtonSaveLog"));
        pushButtonSaveLog->setFont(font1);

        horizontalLayout_3->addWidget(pushButtonSaveLog);

        layoutWidget1 = new QWidget(tab_config);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(11, 11, 871, 31));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_36 = new QLabel(layoutWidget1);
        label_36->setObjectName(QString::fromUtf8("label_36"));
        label_36->setFont(font1);

        horizontalLayout_2->addWidget(label_36);

        lineEditCustomPara0 = new QLineEdit(layoutWidget1);
        lineEditCustomPara0->setObjectName(QString::fromUtf8("lineEditCustomPara0"));
        lineEditCustomPara0->setFont(font1);
        lineEditCustomPara0->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara0->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara0->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(lineEditCustomPara0);

        pushButtonCustomeCMD = new QPushButton(layoutWidget1);
        pushButtonCustomeCMD->setObjectName(QString::fromUtf8("pushButtonCustomeCMD"));
        pushButtonCustomeCMD->setFont(font1);

        horizontalLayout_2->addWidget(pushButtonCustomeCMD);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pushButtonZeroMech = new QPushButton(layoutWidget1);
        pushButtonZeroMech->setObjectName(QString::fromUtf8("pushButtonZeroMech"));
        pushButtonZeroMech->setFont(font1);

        horizontalLayout_2->addWidget(pushButtonZeroMech);

        pushButtonCaliEncoder = new QPushButton(layoutWidget1);
        pushButtonCaliEncoder->setObjectName(QString::fromUtf8("pushButtonCaliEncoder"));
        pushButtonCaliEncoder->setFont(font1);

        horizontalLayout_2->addWidget(pushButtonCaliEncoder);

        layoutWidget2 = new QWidget(tab_config);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(11, 42, 871, 61));
        gridLayout = new QGridLayout(layoutWidget2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_19 = new QLabel(layoutWidget2);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setFont(font1);

        gridLayout->addWidget(label_19, 0, 0, 1, 1);

        lineEditCustomPara1 = new QLineEdit(layoutWidget2);
        lineEditCustomPara1->setObjectName(QString::fromUtf8("lineEditCustomPara1"));
        lineEditCustomPara1->setFont(font1);
        lineEditCustomPara1->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara1->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara1, 0, 1, 1, 1);

        label_20 = new QLabel(layoutWidget2);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setFont(font1);

        gridLayout->addWidget(label_20, 0, 2, 1, 1);

        lineEditCustomPara2 = new QLineEdit(layoutWidget2);
        lineEditCustomPara2->setObjectName(QString::fromUtf8("lineEditCustomPara2"));
        lineEditCustomPara2->setFont(font1);
        lineEditCustomPara2->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara2->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara2, 0, 3, 1, 1);

        label_21 = new QLabel(layoutWidget2);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setFont(font1);

        gridLayout->addWidget(label_21, 0, 4, 1, 1);

        lineEditCustomPara3 = new QLineEdit(layoutWidget2);
        lineEditCustomPara3->setObjectName(QString::fromUtf8("lineEditCustomPara3"));
        lineEditCustomPara3->setFont(font1);
        lineEditCustomPara3->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara3->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara3, 0, 5, 1, 1);

        label_22 = new QLabel(layoutWidget2);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setFont(font1);

        gridLayout->addWidget(label_22, 0, 6, 1, 1);

        lineEditCustomPara4 = new QLineEdit(layoutWidget2);
        lineEditCustomPara4->setObjectName(QString::fromUtf8("lineEditCustomPara4"));
        lineEditCustomPara4->setFont(font1);
        lineEditCustomPara4->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara4->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara4, 0, 7, 1, 1);

        label_26 = new QLabel(layoutWidget2);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setFont(font1);

        gridLayout->addWidget(label_26, 1, 0, 1, 1);

        lineEditCustomPara5 = new QLineEdit(layoutWidget2);
        lineEditCustomPara5->setObjectName(QString::fromUtf8("lineEditCustomPara5"));
        lineEditCustomPara5->setFont(font1);
        lineEditCustomPara5->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara5->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara5, 1, 1, 1, 1);

        label_27 = new QLabel(layoutWidget2);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        label_27->setFont(font1);

        gridLayout->addWidget(label_27, 1, 2, 1, 1);

        lineEditCustomPara6 = new QLineEdit(layoutWidget2);
        lineEditCustomPara6->setObjectName(QString::fromUtf8("lineEditCustomPara6"));
        lineEditCustomPara6->setFont(font1);
        lineEditCustomPara6->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara6->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara6, 1, 3, 1, 1);

        label_34 = new QLabel(layoutWidget2);
        label_34->setObjectName(QString::fromUtf8("label_34"));
        label_34->setFont(font1);

        gridLayout->addWidget(label_34, 1, 4, 1, 1);

        lineEditCustomPara7 = new QLineEdit(layoutWidget2);
        lineEditCustomPara7->setObjectName(QString::fromUtf8("lineEditCustomPara7"));
        lineEditCustomPara7->setFont(font1);
        lineEditCustomPara7->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara7->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara7, 1, 5, 1, 1);

        label_35 = new QLabel(layoutWidget2);
        label_35->setObjectName(QString::fromUtf8("label_35"));
        label_35->setFont(font1);

        gridLayout->addWidget(label_35, 1, 6, 1, 1);

        lineEditCustomPara8 = new QLineEdit(layoutWidget2);
        lineEditCustomPara8->setObjectName(QString::fromUtf8("lineEditCustomPara8"));
        lineEditCustomPara8->setFont(font1);
        lineEditCustomPara8->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditCustomPara8->setLayoutDirection(Qt::LeftToRight);
        lineEditCustomPara8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEditCustomPara8, 1, 7, 1, 1);

        tabWidget->addTab(tab_config, QString());
        tab_setPara = new QWidget();
        tab_setPara->setObjectName(QString::fromUtf8("tab_setPara"));
        tableWidgetPara = new QTableWidget(tab_setPara);
        tableWidgetPara->setObjectName(QString::fromUtf8("tableWidgetPara"));
        tableWidgetPara->setGeometry(QRect(1, 30, 801, 521));
        pushButtonRefreshTable = new QPushButton(tab_setPara);
        pushButtonRefreshTable->setObjectName(QString::fromUtf8("pushButtonRefreshTable"));
        pushButtonRefreshTable->setGeometry(QRect(807, 3, 81, 23));
        pushButtonRefreshTable->setFont(font1);
        pushButtonReadPara = new QPushButton(tab_setPara);
        pushButtonReadPara->setObjectName(QString::fromUtf8("pushButtonReadPara"));
        pushButtonReadPara->setGeometry(QRect(810, 60, 71, 23));
        pushButtonReadPara->setFont(font1);
        pushButtonWritePara = new QPushButton(tab_setPara);
        pushButtonWritePara->setObjectName(QString::fromUtf8("pushButtonWritePara"));
        pushButtonWritePara->setGeometry(QRect(811, 110, 71, 23));
        pushButtonWritePara->setFont(font1);
        labelParaInfo = new QLabel(tab_setPara);
        labelParaInfo->setObjectName(QString::fromUtf8("labelParaInfo"));
        labelParaInfo->setGeometry(QRect(4, 0, 791, 31));
        labelParaInfo->setFont(font2);
        labelParaInfo->setCursor(QCursor(Qt::ArrowCursor));
        pushButtonRecoverFac = new QPushButton(tab_setPara);
        pushButtonRecoverFac->setObjectName(QString::fromUtf8("pushButtonRecoverFac"));
        pushButtonRecoverFac->setGeometry(QRect(810, 310, 71, 23));
        pushButtonRecoverFac->setFont(font1);
        pushButtonExportExcel = new QPushButton(tab_setPara);
        pushButtonExportExcel->setObjectName(QString::fromUtf8("pushButtonExportExcel"));
        pushButtonExportExcel->setGeometry(QRect(810, 390, 71, 23));
        pushButtonExportExcel->setFont(font1);
        pushButtonSavePara = new QPushButton(tab_setPara);
        pushButtonSavePara->setObjectName(QString::fromUtf8("pushButtonSavePara"));
        pushButtonSavePara->setGeometry(QRect(810, 160, 71, 23));
        pushButtonSavePara->setFont(font1);
        pushButtonTxtParaInsert = new QPushButton(tab_setPara);
        pushButtonTxtParaInsert->setObjectName(QString::fromUtf8("pushButtonTxtParaInsert"));
        pushButtonTxtParaInsert->setGeometry(QRect(800, 220, 91, 28));
        pushButtonTxtParaInsert->setIconSize(QSize(20, 20));
        tabWidget->addTab(tab_setPara, QString());
        tab_Oscilloscope = new QWidget();
        tab_Oscilloscope->setObjectName(QString::fromUtf8("tab_Oscilloscope"));
        comboBoxCh1 = new QComboBox(tab_Oscilloscope);
        comboBoxCh1->addItem(QString());
        comboBoxCh1->setObjectName(QString::fromUtf8("comboBoxCh1"));
        comboBoxCh1->setGeometry(QRect(777, 32, 111, 22));
        comboBoxCh2 = new QComboBox(tab_Oscilloscope);
        comboBoxCh2->addItem(QString());
        comboBoxCh2->setObjectName(QString::fromUtf8("comboBoxCh2"));
        comboBoxCh2->setGeometry(QRect(777, 62, 111, 22));
        comboBoxCh3 = new QComboBox(tab_Oscilloscope);
        comboBoxCh3->addItem(QString());
        comboBoxCh3->setObjectName(QString::fromUtf8("comboBoxCh3"));
        comboBoxCh3->setGeometry(QRect(777, 92, 111, 22));
        comboBoxCh4 = new QComboBox(tab_Oscilloscope);
        comboBoxCh4->addItem(QString());
        comboBoxCh4->setObjectName(QString::fromUtf8("comboBoxCh4"));
        comboBoxCh4->setGeometry(QRect(777, 122, 111, 22));
        comboBoxCh5 = new QComboBox(tab_Oscilloscope);
        comboBoxCh5->addItem(QString());
        comboBoxCh5->setObjectName(QString::fromUtf8("comboBoxCh5"));
        comboBoxCh5->setGeometry(QRect(777, 152, 111, 22));
        comboBoxCh6 = new QComboBox(tab_Oscilloscope);
        comboBoxCh6->addItem(QString());
        comboBoxCh6->setObjectName(QString::fromUtf8("comboBoxCh6"));
        comboBoxCh6->setGeometry(QRect(777, 182, 111, 22));
        comboBoxCh7 = new QComboBox(tab_Oscilloscope);
        comboBoxCh7->addItem(QString());
        comboBoxCh7->setObjectName(QString::fromUtf8("comboBoxCh7"));
        comboBoxCh7->setGeometry(QRect(777, 213, 111, 22));
        comboBoxCh8 = new QComboBox(tab_Oscilloscope);
        comboBoxCh8->addItem(QString());
        comboBoxCh8->setObjectName(QString::fromUtf8("comboBoxCh8"));
        comboBoxCh8->setGeometry(QRect(777, 243, 111, 22));
        lineEditSetEchoFre = new QLineEdit(tab_Oscilloscope);
        lineEditSetEchoFre->setObjectName(QString::fromUtf8("lineEditSetEchoFre"));
        lineEditSetEchoFre->setGeometry(QRect(760, 299, 121, 21));
        lineEditSetEchoFre->setFont(font1);
        lineEditSetEchoFre->setLayoutDirection(Qt::LeftToRight);
        lineEditSetEchoFre->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_13 = new QLabel(tab_Oscilloscope);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(770, 269, 101, 31));
        label_13->setFont(font1);
        label_15 = new QLabel(tab_Oscilloscope);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(767, 0, 71, 31));
        label_15->setFont(font1);
        pushButtonEchoStart = new QPushButton(tab_Oscilloscope);
        pushButtonEchoStart->setObjectName(QString::fromUtf8("pushButtonEchoStart"));
        pushButtonEchoStart->setGeometry(QRect(830, 360, 51, 23));
        pushButtonEchoStart->setFont(font1);
        label_selChInfo = new QLabel(tab_Oscilloscope);
        label_selChInfo->setObjectName(QString::fromUtf8("label_selChInfo"));
        label_selChInfo->setGeometry(QRect(5, 0, 571, 31));
        label_selChInfo->setFont(font1);
        pushButtonEchoSet = new QPushButton(tab_Oscilloscope);
        pushButtonEchoSet->setObjectName(QString::fromUtf8("pushButtonEchoSet"));
        pushButtonEchoSet->setGeometry(QRect(760, 360, 51, 23));
        pushButtonEchoSet->setFont(font1);
        waveShow = new waveDisplay(tab_Oscilloscope);
        waveShow->setObjectName(QString::fromUtf8("waveShow"));
        waveShow->setGeometry(QRect(0, 30, 751, 521));
        pushButtonSaveWave = new QPushButton(tab_Oscilloscope);
        pushButtonSaveWave->setObjectName(QString::fromUtf8("pushButtonSaveWave"));
        pushButtonSaveWave->setGeometry(QRect(785, 485, 81, 23));
        pushButtonSaveWave->setFont(font1);
        checkBox_show_value = new QCheckBox(tab_Oscilloscope);
        checkBox_show_value->setObjectName(QString::fromUtf8("checkBox_show_value"));
        checkBox_show_value->setGeometry(QRect(770, 430, 121, 31));
        checkBox_show_value->setChecked(true);
        checkBoxCh1 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh1->setObjectName(QString::fromUtf8("checkBoxCh1"));
        checkBoxCh1->setGeometry(QRect(760, 30, 16, 22));
        checkBoxCh2 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh2->setObjectName(QString::fromUtf8("checkBoxCh2"));
        checkBoxCh2->setGeometry(QRect(760, 60, 16, 22));
        checkBoxCh3 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh3->setObjectName(QString::fromUtf8("checkBoxCh3"));
        checkBoxCh3->setGeometry(QRect(760, 90, 16, 22));
        checkBoxCh4 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh4->setObjectName(QString::fromUtf8("checkBoxCh4"));
        checkBoxCh4->setGeometry(QRect(760, 120, 16, 22));
        checkBoxCh5 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh5->setObjectName(QString::fromUtf8("checkBoxCh5"));
        checkBoxCh5->setGeometry(QRect(760, 150, 16, 22));
        checkBoxCh6 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh6->setObjectName(QString::fromUtf8("checkBoxCh6"));
        checkBoxCh6->setGeometry(QRect(760, 180, 16, 22));
        checkBoxCh7 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh7->setObjectName(QString::fromUtf8("checkBoxCh7"));
        checkBoxCh7->setGeometry(QRect(760, 210, 16, 22));
        checkBoxCh8 = new QCheckBox(tab_Oscilloscope);
        checkBoxCh8->setObjectName(QString::fromUtf8("checkBoxCh8"));
        checkBoxCh8->setGeometry(QRect(760, 240, 16, 22));
        tabWidget->addTab(tab_Oscilloscope, QString());
        comboBoxDev = new QComboBox(centralwidget);
        comboBoxDev->setObjectName(QString::fromUtf8("comboBoxDev"));
        comboBoxDev->setGeometry(QRect(82, 5, 501, 22));
        comboBoxDev->setFont(font1);
        pushButtonGetDevInfo = new QPushButton(centralwidget);
        pushButtonGetDevInfo->setObjectName(QString::fromUtf8("pushButtonGetDevInfo"));
        pushButtonGetDevInfo->setGeometry(QRect(590, 5, 81, 23));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        pushButtonGetDevInfo->setFont(font3);
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(6, 2, 71, 31));
        label_6->setFont(font1);
        pushButtonCfgMotor = new QPushButton(centralwidget);
        pushButtonCfgMotor->setObjectName(QString::fromUtf8("pushButtonCfgMotor"));
        pushButtonCfgMotor->setGeometry(QRect(921, 169, 151, 25));
        pushButtonCfgMotor->setFont(font3);
        pushButtonCfgReset = new QPushButton(centralwidget);
        pushButtonCfgReset->setObjectName(QString::fromUtf8("pushButtonCfgReset"));
        pushButtonCfgReset->setGeometry(QRect(680, 5, 61, 23));
        pushButtonCfgReset->setFont(font3);
        pushButtonCfgBrake = new QPushButton(centralwidget);
        pushButtonCfgBrake->setObjectName(QString::fromUtf8("pushButtonCfgBrake"));
        pushButtonCfgBrake->setGeometry(QRect(750, 5, 61, 23));
        pushButtonCfgBrake->setFont(font3);
        pushButtonCfgSingle = new QPushButton(centralwidget);
        pushButtonCfgSingle->setObjectName(QString::fromUtf8("pushButtonCfgSingle"));
        pushButtonCfgSingle->setGeometry(QRect(921, 200, 72, 25));
        pushButtonCfgSingle->setFont(font3);
        pushButtonCfgContinous = new QPushButton(centralwidget);
        pushButtonCfgContinous->setObjectName(QString::fromUtf8("pushButtonCfgContinous"));
        pushButtonCfgContinous->setGeometry(QRect(999, 200, 71, 25));
        pushButtonCfgContinous->setFont(font3);
        comboBox_CtrlMode = new QComboBox(centralwidget);
        comboBox_CtrlMode->setObjectName(QString::fromUtf8("comboBox_CtrlMode"));
        comboBox_CtrlMode->setGeometry(QRect(921, 111, 151, 23));
        comboBox_CtrlMode->setFont(font1);
        labelCtrlInfo = new QLabel(centralwidget);
        labelCtrlInfo->setObjectName(QString::fromUtf8("labelCtrlInfo"));
        labelCtrlInfo->setGeometry(QRect(920, 60, 151, 31));
        labelCtrlInfo->setFont(font2);
        labelCtrlInfo->setCursor(QCursor(Qt::ArrowCursor));
        layoutWidget3 = new QWidget(centralwidget);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(920, 241, 179, 416));
        formLayout = new QFormLayout(layoutWidget3);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_14 = new QLabel(layoutWidget3);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setFont(font1);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_14);

        lineEditSetPos_Tar = new QLineEdit(layoutWidget3);
        lineEditSetPos_Tar->setObjectName(QString::fromUtf8("lineEditSetPos_Tar"));
        lineEditSetPos_Tar->setFont(font3);
        lineEditSetPos_Tar->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEditSetPos_Tar->setLayoutDirection(Qt::LeftToRight);
        lineEditSetPos_Tar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEditSetPos_Tar);

        label_23 = new QLabel(layoutWidget3);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setFont(font1);

        formLayout->setWidget(3, QFormLayout::LabelRole, label_23);

        lineEditSetPos_Kp = new QLineEdit(layoutWidget3);
        lineEditSetPos_Kp->setObjectName(QString::fromUtf8("lineEditSetPos_Kp"));
        lineEditSetPos_Kp->setFont(font3);
        lineEditSetPos_Kp->setLayoutDirection(Qt::LeftToRight);
        lineEditSetPos_Kp->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::FieldRole, lineEditSetPos_Kp);

        label_28 = new QLabel(layoutWidget3);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        label_28->setFont(font1);

        formLayout->setWidget(4, QFormLayout::LabelRole, label_28);

        lineEditSetPos_Ki = new QLineEdit(layoutWidget3);
        lineEditSetPos_Ki->setObjectName(QString::fromUtf8("lineEditSetPos_Ki"));
        lineEditSetPos_Ki->setFont(font3);
        lineEditSetPos_Ki->setLayoutDirection(Qt::LeftToRight);
        lineEditSetPos_Ki->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(4, QFormLayout::FieldRole, lineEditSetPos_Ki);

        widget_20 = new QWidget(layoutWidget3);
        widget_20->setObjectName(QString::fromUtf8("widget_20"));

        formLayout->setWidget(5, QFormLayout::LabelRole, widget_20);

        label_30 = new QLabel(layoutWidget3);
        label_30->setObjectName(QString::fromUtf8("label_30"));
        label_30->setFont(font1);

        formLayout->setWidget(5, QFormLayout::FieldRole, label_30);

        label_17 = new QLabel(layoutWidget3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setFont(font1);

        formLayout->setWidget(6, QFormLayout::LabelRole, label_17);

        lineEditSetVel_Tar = new QLineEdit(layoutWidget3);
        lineEditSetVel_Tar->setObjectName(QString::fromUtf8("lineEditSetVel_Tar"));
        lineEditSetVel_Tar->setFont(font3);
        lineEditSetVel_Tar->setLayoutDirection(Qt::LeftToRight);
        lineEditSetVel_Tar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(6, QFormLayout::FieldRole, lineEditSetVel_Tar);

        label_24 = new QLabel(layoutWidget3);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setFont(font1);

        formLayout->setWidget(7, QFormLayout::LabelRole, label_24);

        lineEditSetVel_Kp = new QLineEdit(layoutWidget3);
        lineEditSetVel_Kp->setObjectName(QString::fromUtf8("lineEditSetVel_Kp"));
        lineEditSetVel_Kp->setFont(font3);
        lineEditSetVel_Kp->setContextMenuPolicy(Qt::DefaultContextMenu);
        lineEditSetVel_Kp->setLayoutDirection(Qt::LeftToRight);
        lineEditSetVel_Kp->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(7, QFormLayout::FieldRole, lineEditSetVel_Kp);

        label_32 = new QLabel(layoutWidget3);
        label_32->setObjectName(QString::fromUtf8("label_32"));
        label_32->setFont(font1);

        formLayout->setWidget(8, QFormLayout::LabelRole, label_32);

        lineEditSetVel_Ki = new QLineEdit(layoutWidget3);
        lineEditSetVel_Ki->setObjectName(QString::fromUtf8("lineEditSetVel_Ki"));
        lineEditSetVel_Ki->setFont(font3);
        lineEditSetVel_Ki->setContextMenuPolicy(Qt::DefaultContextMenu);
        lineEditSetVel_Ki->setLayoutDirection(Qt::LeftToRight);
        lineEditSetVel_Ki->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(8, QFormLayout::FieldRole, lineEditSetVel_Ki);

        widget_21 = new QWidget(layoutWidget3);
        widget_21->setObjectName(QString::fromUtf8("widget_21"));

        formLayout->setWidget(9, QFormLayout::LabelRole, widget_21);

        label_31 = new QLabel(layoutWidget3);
        label_31->setObjectName(QString::fromUtf8("label_31"));
        label_31->setFont(font1);

        formLayout->setWidget(9, QFormLayout::FieldRole, label_31);

        label_18 = new QLabel(layoutWidget3);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setFont(font1);

        formLayout->setWidget(10, QFormLayout::LabelRole, label_18);

        lineEditSetTor_Tar = new QLineEdit(layoutWidget3);
        lineEditSetTor_Tar->setObjectName(QString::fromUtf8("lineEditSetTor_Tar"));
        lineEditSetTor_Tar->setFont(font3);
        lineEditSetTor_Tar->setLayoutDirection(Qt::LeftToRight);
        lineEditSetTor_Tar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(10, QFormLayout::FieldRole, lineEditSetTor_Tar);

        label_25 = new QLabel(layoutWidget3);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setFont(font1);

        formLayout->setWidget(11, QFormLayout::LabelRole, label_25);

        lineEditSetTor_Kp = new QLineEdit(layoutWidget3);
        lineEditSetTor_Kp->setObjectName(QString::fromUtf8("lineEditSetTor_Kp"));
        lineEditSetTor_Kp->setFont(font3);
        lineEditSetTor_Kp->setLayoutDirection(Qt::LeftToRight);
        lineEditSetTor_Kp->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(11, QFormLayout::FieldRole, lineEditSetTor_Kp);

        label_33 = new QLabel(layoutWidget3);
        label_33->setObjectName(QString::fromUtf8("label_33"));
        label_33->setFont(font1);

        formLayout->setWidget(12, QFormLayout::LabelRole, label_33);

        lineEdit_Err_Code = new QLineEdit(layoutWidget3);
        lineEdit_Err_Code->setObjectName(QString::fromUtf8("lineEdit_Err_Code"));
        lineEdit_Err_Code->setFont(font3);
        lineEdit_Err_Code->setLayoutDirection(Qt::LeftToRight);
        lineEdit_Err_Code->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(12, QFormLayout::FieldRole, lineEdit_Err_Code);

        label_7 = new QLabel(layoutWidget3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFont(font1);

        formLayout->setWidget(1, QFormLayout::FieldRole, label_7);

        layoutWidget4 = new QWidget(centralwidget);
        layoutWidget4->setObjectName(QString::fromUtf8("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(920, 140, 151, 32));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget4);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_mode_self = new QLabel(layoutWidget4);
        label_mode_self->setObjectName(QString::fromUtf8("label_mode_self"));
        label_mode_self->setFont(font1);

        horizontalLayout_4->addWidget(label_mode_self);

        lineEdit_Mode_Self = new QLineEdit(layoutWidget4);
        lineEdit_Mode_Self->setObjectName(QString::fromUtf8("lineEdit_Mode_Self"));
        lineEdit_Mode_Self->setFont(font3);
        lineEdit_Mode_Self->setContextMenuPolicy(Qt::ActionsContextMenu);
        lineEdit_Mode_Self->setLayoutDirection(Qt::LeftToRight);
        lineEdit_Mode_Self->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(lineEdit_Mode_Self);

        lineEdit_Cur_Stat = new QLineEdit(centralwidget);
        lineEdit_Cur_Stat->setObjectName(QString::fromUtf8("lineEdit_Cur_Stat"));
        lineEdit_Cur_Stat->setGeometry(QRect(900, 10, 107, 23));
        lineEdit_Cur_Stat->setFont(font3);
        lineEdit_Cur_Stat->setLayoutDirection(Qt::LeftToRight);
        lineEdit_Cur_Stat->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_29 = new QLabel(centralwidget);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        label_29->setGeometry(QRect(830, 10, 71, 23));
        label_29->setFont(font1);
        MainWindow->setCentralWidget(centralwidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\347\224\265\346\234\272\344\270\262\345\217\243\350\260\203\350\257\225\345\267\245\345\205\267(\347\211\210\346\234\254\357\274\232V1.7.1)", nullptr));
        pushButtonOpenCom->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\344\270\262\345\217\243", nullptr));
        pushButtonRefreshCom->setText(QApplication::translate("MainWindow", "\345\210\267\346\226\260\344\270\262\345\217\243", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_serial), QApplication::translate("MainWindow", "\344\270\262\345\217\243\350\256\276\347\275\256", nullptr));
        pushButtonInOtaMode->setText(QApplication::translate("MainWindow", "\345\220\257\345\212\250\345\215\207\347\272\247", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251bin\346\226\207\344\273\266", nullptr));
        labelOtaInfo->setText(QApplication::translate("MainWindow", "info", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "\345\215\207\347\272\247\350\277\233\345\272\246", nullptr));
        pushButtonStartOta->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\345\215\207\347\272\247", nullptr));
        pushButtonOpenFile->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_ota), QApplication::translate("MainWindow", "\347\250\213\345\272\217\345\215\207\347\272\247(\344\270\215\345\217\257\347\224\250)", nullptr));
        label_16->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201\345\255\227\350\212\202", nullptr));
        pushButtonSaveLog->setText(QApplication::translate("MainWindow", "\346\227\245\345\277\227\344\277\235\345\255\230", nullptr));
        label_36->setText(QApplication::translate("MainWindow", "\350\207\252\345\256\232\344\271\211\346\214\207\344\273\244\357\274\232", nullptr));
        lineEditCustomPara0->setText(QString());
        pushButtonCustomeCMD->setText(QApplication::translate("MainWindow", "\344\270\213\345\217\221\350\207\252\345\256\232\344\271\211\346\214\207\344\273\244", nullptr));
        pushButtonZeroMech->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\234\272\346\242\260\351\233\266\344\275\215", nullptr));
        pushButtonCaliEncoder->setText(QApplication::translate("MainWindow", "\347\224\265\350\247\222\345\272\246\346\240\207\345\256\232", nullptr));
        label_19->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2601\357\274\232", nullptr));
        label_20->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2602\357\274\232", nullptr));
        label_21->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2603\357\274\232", nullptr));
        label_22->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2604\357\274\232", nullptr));
        label_26->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2605\357\274\232", nullptr));
        label_27->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2606\357\274\232", nullptr));
        label_34->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2607\357\274\232", nullptr));
        label_35->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\2608\357\274\232", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_config), QApplication::translate("MainWindow", "\347\224\265\346\234\272\351\205\215\347\275\256", nullptr));
        pushButtonRefreshTable->setText(QApplication::translate("MainWindow", "\345\217\202\346\225\260\350\241\250\345\210\267\346\226\260", nullptr));
        pushButtonReadPara->setText(QApplication::translate("MainWindow", "\350\257\273\345\217\202\346\225\260", nullptr));
        pushButtonWritePara->setText(QApplication::translate("MainWindow", "\345\206\231\345\217\202\346\225\260", nullptr));
        labelParaInfo->setText(QApplication::translate("MainWindow", "info", nullptr));
        pushButtonRecoverFac->setText(QApplication::translate("MainWindow", "\346\201\242\345\244\215\345\207\272\345\216\202", nullptr));
        pushButtonExportExcel->setText(QApplication::translate("MainWindow", "\345\257\274\345\207\272", nullptr));
        pushButtonSavePara->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\345\217\202\346\225\260", nullptr));
        pushButtonTxtParaInsert->setText(QApplication::translate("MainWindow", "txt\345\217\202\346\225\260\345\257\274\345\205\245", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_setPara), QApplication::translate("MainWindow", "\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        comboBoxCh1->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh2->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh3->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh4->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh5->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh6->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh7->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        comboBoxCh8->setItemText(0, QApplication::translate("MainWindow", "\346\227\240", nullptr));

        label_13->setText(QApplication::translate("MainWindow", "\350\276\223\345\207\272\351\242\221\347\216\207\357\274\210Hz\357\274\211", nullptr));
        label_15->setText(QApplication::translate("MainWindow", "\351\200\232\351\201\223\351\200\211\346\213\251", nullptr));
        pushButtonEchoStart->setText(QApplication::translate("MainWindow", "\345\220\257\345\212\250", nullptr));
        label_selChInfo->setText(QApplication::translate("MainWindow", "info", nullptr));
        pushButtonEchoSet->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        pushButtonSaveWave->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\344\277\235\345\255\230", nullptr));
        checkBox_show_value->setText(QApplication::translate("MainWindow", " \345\205\211\346\240\207\346\230\276\347\244\272\345\235\220\346\240\207", nullptr));
        checkBoxCh1->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh2->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh3->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh4->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh5->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh6->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh7->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        checkBoxCh8->setText(QApplication::translate("MainWindow", "CheckBox", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Oscilloscope), QApplication::translate("MainWindow", "\347\244\272\346\263\242\345\231\250", nullptr));
        pushButtonGetDevInfo->setText(QApplication::translate("MainWindow", "\346\243\200\346\265\213\350\256\276\345\244\207", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\350\256\276\345\244\207", nullptr));
        pushButtonCfgMotor->setText(QApplication::translate("MainWindow", "\350\277\220\350\241\214", nullptr));
        pushButtonCfgReset->setText(QApplication::translate("MainWindow", "\345\244\215\344\275\215", nullptr));
        pushButtonCfgBrake->setText(QApplication::translate("MainWindow", "\345\210\271\350\275\246", nullptr));
        pushButtonCfgSingle->setText(QApplication::translate("MainWindow", "\345\215\225\346\254\241\346\216\247\345\210\266", nullptr));
        pushButtonCfgContinous->setText(QApplication::translate("MainWindow", "\350\277\236\347\273\255\350\276\223\345\207\272", nullptr));
        labelCtrlInfo->setText(QString());
        label_14->setText(QApplication::translate("MainWindow", "\347\233\256\346\240\207", nullptr));
        label_23->setText(QApplication::translate("MainWindow", "Kp", nullptr));
        label_28->setText(QApplication::translate("MainWindow", "Ki", nullptr));
        label_30->setText(QApplication::translate("MainWindow", "\350\247\222\351\200\237\345\272\246\357\274\210rad.s-1\357\274\211", nullptr));
        label_17->setText(QApplication::translate("MainWindow", "\347\233\256\346\240\207", nullptr));
        label_24->setText(QApplication::translate("MainWindow", "Kp", nullptr));
        label_32->setText(QApplication::translate("MainWindow", "Ki", nullptr));
        label_31->setText(QApplication::translate("MainWindow", "\345\212\233\347\237\251\357\274\210nm\357\274\211", nullptr));
        label_18->setText(QApplication::translate("MainWindow", "\347\233\256\346\240\207", nullptr));
        label_25->setText(QApplication::translate("MainWindow", "Kp", nullptr));
        label_33->setText(QApplication::translate("MainWindow", "\346\225\205\351\232\234\357\274\232", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "\350\247\222\345\272\246\357\274\210rad\357\274\211", nullptr));
        label_mode_self->setText(QApplication::translate("MainWindow", "\350\207\252\347\224\261\346\250\241\345\274\217\357\274\232", nullptr));
        label_29->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\346\250\241\345\274\217\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
