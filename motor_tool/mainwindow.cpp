#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "otaThread.h"
const int MainWindow::tabWeight[PARA_TAB_COLUMN_NUM] = {100, 200, 100, 80, 100, 100, 200};

QString MainWindow::color2Str(const QColor &color)
{
    return QString("rgb(%1, %2, %3)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/ico/resources/tool.png"));

    baudTabPoint = 1;

    extern float P_MIN;

    ui->comboBoxCom->clear();
    // find the available serial ports
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.description().contains("CH340"))
        {
            QSerialPort serial;
            serial.setPort(info);
            ui->comboBoxCom->addItem(info.portName());
        }
    }

    ui->labelOtaInfo->setStyleSheet("color:black;");
    ui->labelOtaInfo->setText("");
    ui->statusBar->showMessage("请选择串口并打开！");

    ui->pushButtonInOtaMode->setText("启动升级");
    intoOtaFlag = true;

    myCom = new QSerialPort;
    IsComOpen = false;

    isSaveLogOpen = false;

    FilePath = "C:/work/11_mitDog/servo_joint_motor/tools/bin/gd32_debug.bin";
    ui->lineEditSelFile->setText(FilePath);

    ui->pushButtonGetDevInfo->setEnabled(false);

    ui->progressBar->setValue(0);

    ui->comboBoxDev->clear();

    uart2can.devCanId = 1;

    ui->labelSetCanId->setText("");
    ui->lineEditSetCanId->setText("1");
    uart2can.configCanId = 1;

    noCanDevConfig();

    ui->lineEditSetPos->setText("0");
    ui->lineEditSetKp->setText("0");
    ui->lineEditSetVel->setText("0");
    ui->lineEditSetKd->setText("0");
    ui->lineEditSetTorque->setText("0");
    ui->lineEditSetEchoFre->setText("100");
    uart2can.chEchoFreHz = 100;

    ui->lineEditSetP_max->setText(QString::number(P_MAX, 'f', 2));
    ui->lineEditSetP_max->setEnabled(false);
    ui->lineEditSetP_min->setText(QString::number(P_MIN, 'f', 2));
    ui->lineEditSetP_min->setEnabled(false);
    ui->lineEditSetV_max->setText(QString::number(V_MAX, 'f', 1));
    ui->lineEditSetV_max->setEnabled(false);
    ui->lineEditSetV_min->setText(QString::number(V_MIN, 'f', 1));
    ui->lineEditSetV_min->setEnabled(false);
    ui->lineEditSetT_max->setText(QString::number(T_MAX, 'f', 1));
    ui->lineEditSetT_max->setEnabled(false);
    ui->lineEditSetT_min->setText(QString::number(T_MIN, 'f', 1));
    ui->lineEditSetT_min->setEnabled(false);
    ui->lineEditSetKP_min->setText(QString::number(KP_MIN, 'f', 1));
    ui->lineEditSetKP_min->setEnabled(false);
    ui->lineEditSetKP_max->setText(QString::number(KP_MAX, 'f', 1));
    ui->lineEditSetKP_max->setEnabled(false);
    ui->lineEditSetKD_min->setText(QString::number(KD_MIN, 'f', 1));
    ui->lineEditSetKD_min->setEnabled(false);
    ui->lineEditSetKD_max->setText(QString::number(KD_MAX, 'f', 1));
    ui->lineEditSetKD_max->setEnabled(false);

    ui->comboBoxSelMode->setCurrentIndex(1);

    uart2can.chEchoFreHzMax = CAN_SCOPE_FRE_MAX; //(quint16)(uart2can.baudTab[0]/10/CAN_FRAME_BYTE_LEN);
    uart2can.chEchoFreHzMin = 1;

    ui->labelParaInfo->clear();

    //    ui->tableWidgetPara->setProperty("RowHeight", 60);
    ui->tableWidgetPara->verticalHeader()->setDefaultSectionSize(25);
    paraTableClear(ui->tableWidgetPara);

    connect(myCom, SIGNAL(readyRead()), this, SLOT(readFromCom()));

    // signal from the child thread
    /* fzh20221028,sendState/StateUpdate、scopeToShow/waveFreShow是库函数/非CAN通信函数
     * 只有sendRecvData/getSerialData、WriteToSerial/writeToCom是收发函数，所以只需要更新这两个函数即可
     */
    connect(&this->uart2can, SIGNAL(sendState()), this, SLOT(StateUpdate()));
    connect(this, SIGNAL(sendRecvData(QByteArray)), &this->uart2can, SLOT(getSerialData(QByteArray)));
    connect(&this->uart2can, SIGNAL(WriteToSerial(QByteArray, qint16)), this, SLOT(writeToCom(QByteArray, qint16))); /* emit */
    connect(&this->uart2can, SIGNAL(scopeToShow(QVector<QCPGraphData> *)), this, SLOT(waveFreShow(QVector<QCPGraphData> *)));

    // 计算dpi
    QList<QScreen *> screens = QApplication::screens();
    QScreen *screen = screens[0];
    qreal dpi = screen->logicalDotsPerInch();
    // 计算dpi对应的缩放比例
    double objectRate = dpi / 96.0;
    changeObjectSize(*this, objectRate);
    resize(width() * objectRate, height() * objectRate);

    setFixedSize(this->width(), this->height());

    // start the thread
    uart2can.start();
}

// 没有检测到CAN设备的配置
void MainWindow::noCanDevConfig()
{
    ui->comboBoxDev->clear();
    ui->pushButtonCaliEncoder->setEnabled(false);
    ui->pushButtonCfgReset->setEnabled(false);
    ui->pushButtonSetCanId->setEnabled(false);
    ui->pushButtonZeroMech->setEnabled(false);
    ui->pushButtonCfgMotor->setEnabled(false);
    ui->pushButtonCfgBrake->setEnabled(false);
    ui->pushButtonCfgSingle->setEnabled(false);
    ui->pushButtonCfgContinous->setEnabled(false);
    ui->pushButtonInOtaMode->setEnabled(false);
    ui->pushButtonStartOta->setEnabled(false);
    ui->pushButtonRefreshTable->setEnabled(false);
    ui->pushButtonExportExcel->setEnabled(false);
    ui->pushButtonReadPara->setEnabled(false);
    ui->pushButtonWritePara->setEnabled(false);
    ui->pushButtonRecoverFac->setEnabled(false);

    if (isSaveLogOpen)
        saveLogFile.write(ui->textEditCfgInfo->toPlainText().toUtf8());

    ui->textEditCfgInfo->clear();

    readParaAllFlag = false;
    writeAllModyfiedFlag = false;
}

// 检测到CAN设备的配置
void MainWindow::detectedCanDevConfig()
{
    ui->comboBoxDev->clear();
    if (ui->comboBoxSelMode->currentIndex() == 1)
    {
        ui->pushButtonCaliEncoder->setEnabled(true);
    }
    ui->pushButtonCfgReset->setEnabled(true);
    ui->pushButtonSetCanId->setEnabled(true);
    ui->pushButtonZeroMech->setEnabled(true);
    ui->pushButtonCfgMotor->setEnabled(true);
    ui->pushButtonCfgBrake->setEnabled(true);
    ui->pushButtonCfgSingle->setEnabled(true);
    ui->pushButtonCfgContinous->setEnabled(true);
    ui->pushButtonInOtaMode->setEnabled(true);
    ui->pushButtonRefreshTable->setEnabled(true);
}

// 获取参数类型的字符串
QString MainWindow::getParaTypeStr(enum paraType type)
{
    QString str;

    switch (type)
    {
    case ParaType_U8:
        str = "uint8";
        break;
    case ParaType_S8:
        str = "int8";
        break;
    case ParaType_U16:
        str = "uint16";
        break;
    case ParaType_S16:
        str = "int16";
        break;
    case ParaType_U32:
        str = "uint32";
        break;
    case ParaType_S32:
        str = "int32";
        break;
    case ParaType_FLOAT:
        str = "float";
        break;
    case ParaType_U64:
        str = "uint64";
        break;
    case ParaType_S64:
        str = "int64";
        break;
    case ParaType_DOUBLE:
        str = "double";
        break;
    default:
        str = "none";
        break;
    }
    return str;
}

// 获取参数限制阈值的字符串
QString MainWindow::getParaLimitStr(enum paraType type, double val, int prec)
{
    QString str;

    switch (type)
    {
    case ParaType_U8:
    case ParaType_S8:
    case ParaType_U16:
    case ParaType_S16:
    case ParaType_U32:
    case ParaType_S32:
    case ParaType_U64:
    case ParaType_S64:
        str = QString::number(val, 10, 0);
        break;
    case ParaType_FLOAT:
    case ParaType_DOUBLE:
        str = QString::number(val, 10, prec);
        break;
    default:
        str = "none";
        break;
    }
    return str;
}

// 获取参数值的字符串
QString MainWindow::getParaValStr(enum paraType type, const quint8 *add)
{
    QString str;

    switch (type)
    {
    case ParaType_U8:
        quint8 tempU8;
        memcpy(&tempU8, add, 1);
        str = QString::number(tempU8, 10);
        break;
    case ParaType_S8:
        qint8 tempS8;
        memcpy(&tempS8, add, 1);
        str = QString::number(tempS8, 10);
        break;
    case ParaType_U16:
        quint16 tempU16;
        memcpy(&tempU16, add, 2);
        str = QString::number(tempU16, 10);
        break;
    case ParaType_S16:
        qint16 tempS16;
        memcpy(&tempS16, add, 2);
        str = QString::number(tempS16, 10);
        break;
    case ParaType_U32:
        quint32 tempU32;
        memcpy(&tempU32, add, 4);
        str = QString::number(tempU32, 10);
        break;
    case ParaType_S32:
        qint32 tempS32;
        memcpy(&tempS32, add, 4);
        str = QString::number(tempS32, 10);
        break;
    case ParaType_U64:
        quint64 tempU64;
        memcpy(&tempU64, add, 8);
        str = QString::number(tempU64, 10);
        break;
    case ParaType_S64:
        qint64 tempS64;
        memcpy(&tempS64, add, 8);
        str = QString::number(tempS64, 10);
        break;
    case ParaType_FLOAT:
        float tempF;
        memcpy(&tempF, add, 4);
        str = QString::number(tempF, 10, 6);
        break;
    case ParaType_DOUBLE:
        double tempD;
        memcpy(&tempD, add, 8);
        str = QString::number(tempD, 10, 12);
        break;
    default:
        str = "none";
        break;
    }
    return str;
}

// 设置参数值
QString MainWindow::setParaValStr(enum paraType type, quint8 *const add, double val)
{
    QString str;

    quint8 tempU8 = 0;
    qint8 tempS8 = 0;
    quint16 tempU16 = 0;
    qint16 tempS16 = 0;
    quint32 tempU32 = 0;
    qint32 tempS32 = 0;
    quint64 tempU64 = 0;
    qint64 tempS64 = 0;
    float tempF = 0;
    double tempD = 0;

    switch (type)
    {
    case ParaType_U8:
        tempU8 = val;
        memcpy(add, &tempU8, 1);
        str = QString::number(tempU8, 10);
        break;
    case ParaType_S8:
        tempS8 = val;
        memcpy(add, &tempS8, 1);
        str = QString::number(tempS8, 10);
        break;
    case ParaType_U16:
        tempU16 = val;
        memcpy(add, &tempU16, 2);
        str = QString::number(tempU16, 10);
        break;
    case ParaType_S16:
        tempS16 = val;
        memcpy(add, &tempS16, 2);
        str = QString::number(tempS16, 10);
        break;
    case ParaType_U32:
        tempU32 = val;
        memcpy(add, &tempU32, 4);
        str = QString::number(tempU32, 10);
        break;
    case ParaType_S32:
        tempS32 = val;
        memcpy(add, &tempS32, 4);
        str = QString::number(tempS32, 10);
        break;
    case ParaType_U64:
        tempU64 = val;
        memcpy(add, &tempU64, 8);
        str = QString::number(tempU64, 10);
        break;
    case ParaType_S64:
        tempS64 = val;
        memcpy(add, &tempS64, 8);
        str = QString::number(tempS64, 10);
        break;
    case ParaType_FLOAT:
        tempF = val;
        memcpy(add, &tempF, 4);
        str = QString::number(tempF, 10, 6);
        break;
    case ParaType_DOUBLE:
        tempD = val;
        memcpy(add, &tempD, 8);
        str = QString::number(tempD, 10, 12);
        break;
    default:
        str = "0";
        memset(add, 0, 8);
        break;
    }
    return str;
}

// 清除参数表
void MainWindow::paraTableClear(QTableWidget *widget)
{
    widget->blockSignals(true);
    widget->clear();

    readParaAllFlag = false;
    writeAllModyfiedFlag = false;

    widget->verticalHeader()->hide(); // 隐藏行号方法

    widget->setColumnCount(PARA_TAB_COLUMN_NUM);

    widget->setColumnWidth(0, tabWeight[0]);
    widget->setColumnWidth(1, tabWeight[1]);
    widget->setColumnWidth(2, tabWeight[2]);
    widget->setColumnWidth(3, tabWeight[3]);
    widget->setColumnWidth(4, tabWeight[4]);
    widget->setColumnWidth(5, tabWeight[5]);
    widget->setColumnWidth(6, 10);

    // 自动调整最后一列的宽度使它和表格的右边界对齐
    widget->horizontalHeader()->setStretchLastSection(true);
    widget->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                              "color: black;border: 1px solid #6c6c6c;}");

    QStringList headerLabels = {"功能码", "名称", "参数类型", "属性", "最大值", "最小值", "当前值"};
    widget->setHorizontalHeaderLabels(headerLabels);

    // widget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    widget->verticalHeader()->setDefaultSectionSize(25);

    widget->setRowCount(30);

    selConfigInitChannal();
}

// 参数表更新-可写字符串
void MainWindow::paraTableLoadWriteStr(QTableWidget *widget, quint16 rowStart)
{
    QTableWidgetItem *pItem;

    quint16 i;
    int row, column;
    QString str;

    for (i = 0; i < uart2can.paraInfo.numTab[WRITE_STR]; i++)
    {
        row = i + rowStart;
        uart2can.paraInfo.writeStr[i].modifyFlag = false;
        for (column = 0; column < PARA_TAB_COLUMN_NUM; column++)
        {
            pItem = new QTableWidgetItem;
            pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            pItem->setBackground(QColor("#a0f446"));

            str.clear();

            switch (column)
            {
            case 0:
                str = "0X" + QString("%1").arg(i | ((quint16)WRITE_STR << 12), 4, 16, QLatin1Char('0'));
                break;
            case 1:
                str = uart2can.paraInfo.writeStr[i].name;
                break;
            case 2:
                str = "String";
                break;
            case 3:
                str = "读/写";
                break;
            case PARA_TAB_EDIT_COLUMN:
                pItem->setFlags(pItem->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));
                pItem->setFont(QFont("Times", 10, QFont::Black));
                str = uart2can.paraInfo.writeStr[i].str;
                break;
            }

            pItem->setText(str);
            widget->setItem(row, column, pItem);
        }
    }
}

// 参数表更新-只读字符串
void MainWindow::paraTableLoadReadStr(QTableWidget *widget, quint16 rowStart)
{
    QTableWidgetItem *pItem;

    quint16 i;
    int row, column;
    QString str;

    for (i = 0; i < uart2can.paraInfo.numTab[READ_STR]; i++)
    {
        row = i + rowStart;
        for (column = 0; column < PARA_TAB_COLUMN_NUM; column++)
        {
            pItem = new QTableWidgetItem;
            pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pItem->setBackground(QColor("#d4e8c8"));

            str.clear();

            switch (column)
            {
            case 0:
                str = "0X" + QString("%1").arg(i | ((quint16)READ_STR << 12), 4, 16, QLatin1Char('0'));
                break;
            case 1:
                str = uart2can.paraInfo.readStr[i].name;
                break;
            case 2:
                str = "String";
                break;
            case 3:
                str = "只读";
                break;
            case PARA_TAB_EDIT_COLUMN:
                str = uart2can.paraInfo.readStr[i].str;
                break;
            }

            pItem->setText(str);
            widget->setItem(row, column, pItem);
        }
    }
}

// 参数表更新-可写数据
void MainWindow::paraTableLoadWritePara(QTableWidget *widget, quint16 rowStart)
{
    QTableWidgetItem *pItem;

    quint16 i;
    int row, column;
    QString str;

    for (i = 0; i < uart2can.paraInfo.numTab[WRITE_PARA]; i++)
    {
        row = i + rowStart;

        uart2can.paraInfo.writePara[i].modifyFlag = false;
        for (column = 0; column < PARA_TAB_COLUMN_NUM; column++)
        {
            pItem = new QTableWidgetItem;
            pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pItem->setBackground(QColor("#63e2ee"));

            str.clear();

            switch (column)
            {
            case 0:
                str = "0X" + QString("%1").arg(i | ((quint16)WRITE_PARA << 12), 4, 16, QLatin1Char('0'));
                break;
            case 1:
                str = uart2can.paraInfo.writePara[i].name;
                if (i > uart2can.paraInfo.echoNum)
                    selCh_list.append(str);
                break;
            case 2:
                str = getParaTypeStr(uart2can.paraInfo.writePara[i].type);
                break;
            case 3:
                str = (i >= uart2can.paraInfo.echoNum) ? (uart2can.paraInfo.writePara[i].wPerm ? "读/写" : "设定") : "配置";
                break;
            case 4:
                str = getParaLimitStr(uart2can.paraInfo.writePara[i].type,
                                      uart2can.paraInfo.writePara[i].max,
                                      uart2can.paraInfo.writePara[i].prec);
                break;
            case 5:
                str = getParaLimitStr(uart2can.paraInfo.writePara[i].type,
                                      uart2can.paraInfo.writePara[i].min,
                                      uart2can.paraInfo.writePara[i].prec);
                break;
            case PARA_TAB_EDIT_COLUMN:
                if ((i >= uart2can.paraInfo.echoNum) && (uart2can.paraInfo.writePara[i].wPerm))
                {
                    pItem->setFlags(pItem->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));
                    pItem->setFont(QFont("Times", 10, QFont::Black)); //,QFont::Bold )
                    // pItem->setForeground(QColor(Qt::red));
                }
                str = getParaValStr(uart2can.paraInfo.writePara[i].type,
                                    uart2can.paraInfo.writePara[i].valR);
                uart2can.paraInfo.writePara[i].valStr = str;
                break;
            }

            pItem->setText(str);
            widget->setItem(row, column, pItem);
        }
    }
}

// 参数表更新-只读数据
void MainWindow::paraTableLoadReadPara(QTableWidget *widget, quint16 rowStart)
{
    QTableWidgetItem *pItem;

    quint16 i;
    int row, column;
    QString str;

    for (i = 0; i < uart2can.paraInfo.numTab[READ_PARA]; i++)
    {
        row = i + rowStart;
        for (column = 0; column < PARA_TAB_COLUMN_NUM; column++)
        {
            pItem = new QTableWidgetItem;
            pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pItem->setBackground(QColor("#bbd5d7"));

            str.clear();

            switch (column)
            {
            case 0:
                str = "0X" + QString("%1").arg(i | ((quint16)READ_PARA << 12), 4, 16, QLatin1Char('0'));
                break;
            case 1:
                str = uart2can.paraInfo.readPara[i].name;
                selCh_list.append(str);
                break;
            case 2:
                str = getParaTypeStr(uart2can.paraInfo.readPara[i].type);
                break;
            case 3:
                str = "只读";
                break;
            case PARA_TAB_EDIT_COLUMN:
                str = getParaValStr(uart2can.paraInfo.readPara[i].type,
                                    uart2can.paraInfo.readPara[i].valR);
                break;
            }

            pItem->setText(str);
            widget->setItem(row, column, pItem);
        }
    }
}

// 读取修改字符串
void MainWindow::paraTableModifyStr(QTableWidget *widget, quint16 code)
{
    int row = code + uart2can.paraInfo.posArea[WRITE_STR].start;
    int column = PARA_TAB_EDIT_COLUMN;

    QString str = widget->item(row, column)->text();

    char *ch;
    QByteArray ba = str.toUtf8();
    ch = ba.data();
    qint8 flag = 0;

    for (int i = 0; i < ba.size(); i++)
    {
        if (ch[i] & 0X80)
        {
            flag = 1;
            break;
        }
    }

    if (flag)
    {
        ui->labelParaInfo->setStyleSheet("color:red;");
        ui->labelParaInfo->setText("修改可写字符0X" + QString("%1").arg(code | ((quint16)WRITE_STR << 12), 4, 16, QLatin1Char('0')) + ":输入字符含有非ASCII字符!");
        widget->item(row, column)->setText(uart2can.paraInfo.writeStr[code].str);
        widget->item(row, column)->setForeground(QColor(Qt::black));
        uart2can.paraInfo.writeStr[code].modifyFlag = false;
    }
    else
    {
        ui->labelParaInfo->setStyleSheet("color:green;");
        ui->labelParaInfo->setText("修改可写字符0X" + QString("%1").arg(code | ((quint16)WRITE_STR << 12), 4, 16, QLatin1Char('0')) + ":" + str);

        if (str != uart2can.paraInfo.writeStr[code].str)
        {
            widget->item(row, column)->setForeground(QColor(Qt::red));

            int byteLen = (ba.size() < STR_NAME_LEN) ? ba.size() : STR_NAME_LEN;
            memcpy(uart2can.paraInfo.writeStr[code].strEdit, ch, byteLen);
            uart2can.paraInfo.writeStr[code].strEdit[byteLen] = 0;
            uart2can.paraInfo.writeStr[code].modifyFlag = true;
            ui->pushButtonWritePara->setEnabled(true);
        }
    }
}

// 读取修改参数
void MainWindow::paraTableModifyPara(QTableWidget *widget, quint16 code)
{
    int row = code + uart2can.paraInfo.posArea[WRITE_PARA].start;
    int column = PARA_TAB_EDIT_COLUMN;

    QString str = widget->item(row, column)->text();

    bool ok;

    int64_t sp;
    uint64_t up;
    double dp;

    str.toInt(&ok);

    switch (uart2can.paraInfo.writePara[code].type)
    {
    case ParaType_U8:
    case ParaType_U16:
    case ParaType_U32:
    case ParaType_U64:
        up = str.toULongLong(&ok);
        dp = up;
        break;
    case ParaType_S8:
    case ParaType_S16:
    case ParaType_S32:
    case ParaType_S64:
        sp = str.toLongLong(&ok);
        dp = sp;
        break;
    case ParaType_FLOAT:
    case ParaType_DOUBLE:
        dp = str.toDouble(&ok);
        break;
    default:
        ok = false;
        break;
    }

    if (ok == false)
    {
        ui->labelParaInfo->setStyleSheet("color:red;");
        ui->labelParaInfo->setText("修改可写参数0X" + QString("%1").arg(code | ((quint16)WRITE_PARA << 12), 4, 16, QLatin1Char('0')) + ":输入数据有误!");
        widget->item(row, column)->setText(uart2can.paraInfo.writePara[code].valStr);
        widget->item(row, column)->setForeground(QColor(Qt::black));
        uart2can.paraInfo.writePara[code].modifyFlag = false;
        return;
    }

    if ((dp > uart2can.paraInfo.writePara[code].max) || (dp < uart2can.paraInfo.writePara[code].min))
    {
        ui->labelParaInfo->setStyleSheet("color:red;");
        ui->labelParaInfo->setText("修改可写参数0X" + QString("%1").arg(code | ((quint16)WRITE_PARA << 12), 4, 16, QLatin1Char('0')) + ":输入数据超出范围!");
        widget->item(row, column)->setText(uart2can.paraInfo.writePara[code].valStr);
        widget->item(row, column)->setForeground(QColor(Qt::black));
        uart2can.paraInfo.writePara[code].modifyFlag = false;
        return;
    }

    str = setParaValStr(uart2can.paraInfo.writePara[code].type, uart2can.paraInfo.writePara[code].valM, dp);

    if (str != uart2can.paraInfo.writePara[code].valStr)
    {
        ui->labelParaInfo->setStyleSheet("color:green;");
        ui->labelParaInfo->setText("修改可写参数0X" + QString("%1").arg(code | ((quint16)WRITE_PARA << 12), 4, 16, QLatin1Char('0')) + ":" + str);
        widget->item(row, column)->setText(str);
        widget->item(row, column)->setForeground(QColor(Qt::red));

        uart2can.paraInfo.writePara[code].modifyFlag = true;
        ui->pushButtonWritePara->setEnabled(true);
    }
    else
    {
        widget->item(row, column)->setText(uart2can.paraInfo.writePara[code].valStr);
        widget->item(row, column)->setForeground(QColor(Qt::black));
        uart2can.paraInfo.writePara[code].modifyFlag = false;
    }
}

// 参数表更新-全部
void MainWindow::paraTableUpdateAll(QTableWidget *widget)
{
    quint16 num = 0, i;

    for (i = 0, num = 0; i < PARA_STR_TOTAL; i++)
    {
        num += uart2can.paraInfo.numTab[i];
    }
    widget->setRowCount(num);
    // 设置表格的选择方式
    widget->setSelectionBehavior(QAbstractItemView::SelectItems);

    // 设置编辑方式
    widget->setEditTriggers(QAbstractItemView::DoubleClicked);
    widget->setStyleSheet("selection-background-color:lightblue;"); // 设置选中背景色

    selCh_list.clear();
    selCh_list.append(tr(""));

    num = 0;
    uart2can.paraInfo.posArea[WRITE_STR].start = num;
    paraTableLoadWriteStr(widget, num);
    num += uart2can.paraInfo.numTab[WRITE_STR];
    uart2can.paraInfo.posArea[WRITE_STR].end = num - 1;

    uart2can.paraInfo.posArea[READ_STR].start = num;
    paraTableLoadReadStr(widget, num);
    num += uart2can.paraInfo.numTab[READ_STR];
    uart2can.paraInfo.posArea[READ_STR].end = num - 1;

    uart2can.paraInfo.posArea[WRITE_PARA].start = num;
    paraTableLoadWritePara(widget, num);
    num += uart2can.paraInfo.numTab[WRITE_PARA];
    uart2can.paraInfo.posArea[WRITE_PARA].end = num - 1;

    uart2can.paraInfo.posArea[READ_PARA].start = num;
    paraTableLoadReadPara(widget, num);
    num += uart2can.paraInfo.numTab[READ_PARA];
    uart2can.paraInfo.posArea[READ_PARA].end = num - 1;

    selConfigAllChannal();

    // widget->blockSignals(false);
}

// 初始化所有配置通道
void MainWindow::selConfigInitChannal()
{
    selCh_list.clear();
    selCh_list.append(tr(""));

    chSelTotalNum = 0;

    getComboxConfig(ui->comboBoxCh1, false);
    ui->comboBoxCh1->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[0]) + "}");
    getComboxConfig(ui->comboBoxCh2, false);
    ui->comboBoxCh2->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[1]) + "}");
    getComboxConfig(ui->comboBoxCh3, false);
    ui->comboBoxCh3->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[2]) + "}");
    getComboxConfig(ui->comboBoxCh4, false);
    ui->comboBoxCh4->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[3]) + "}");
    getComboxConfig(ui->comboBoxCh5, false);
    ui->comboBoxCh5->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[4]) + "}");
    getComboxConfig(ui->comboBoxCh6, false);
    ui->comboBoxCh6->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[5]) + "}");
    getComboxConfig(ui->comboBoxCh7, false);
    ui->comboBoxCh7->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[6]) + "}");
    getComboxConfig(ui->comboBoxCh8, false);
    ui->comboBoxCh8->setStyleSheet("QComboBox{background:" + color2Str(ChannelColor::Color[7]) + "}");

    ui->label_selChInfo->setStyleSheet("color:green;");
    ui->label_selChInfo->setText("请更新参数表！");
    ui->lineEditSetEchoFre->setEnabled(false);
    ui->pushButtonEchoStart->setEnabled(false);
    ui->pushButtonEchoStart->setText("启动");
    ui->pushButtonEchoSet->setEnabled(false);
    ui->pushButtonSaveWave->setEnabled(false);

    ui->lineEditSetEchoFre->setText("100");
    uart2can.chEchoFreHz = 100;

    uart2can.chEchoFreHzMax = CAN_SCOPE_FRE_MAX; //(quint16)(uart2can.baudTab[0]/10/CAN_FRAME_BYTE_LEN);
    uart2can.chEchoFreHzMin = 1;

    uart2can.chSelNum = 0;
    for (quint8 i = 0; i < CHANNAL_SEL_NUM; i++)
    {
        uart2can.chSelCode[i] = 0;
        uart2can.chSelIndex[i] = 0;
    }
}
// Combox控件配置
void MainWindow::getComboxConfig(QComboBox *comboBoxCh, bool Flag)
{
    comboBoxCh->clear();
    comboBoxCh->addItems(selCh_list);
    comboBoxCh->setCurrentIndex(0);
    comboBoxCh->setEnabled(Flag);
    // 添加提示框
    QStandardItemModel *model = new QStandardItemModel(); // 添加提示tootip
    for (int i = 0; i < selCh_list.size(); ++i)
    {
        QStandardItem *item = new QStandardItem(selCh_list.at(i));
        ;
        item->setToolTip(selCh_list.at(i));
        model->appendRow(item);
        // editor->addItem(Items.at(i));
    }
    comboBoxCh->setModel(model);
    // 添加模糊搜索
    comboBoxCh->setEditable(true); //
    QCompleter *pCompleter = new QCompleter(comboBoxCh->model());
    pCompleter->setFilterMode(Qt::MatchContains);
    pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    pCompleter->setCompletionMode(QCompleter::PopupCompletion);
    comboBoxCh->setCompleter(pCompleter);
}

// 初始化所有配置通道
void MainWindow::selConfigAllChannal()
{
    chSelTotalNum = selCh_list.count();

    getComboxConfig(ui->comboBoxCh1, true);
    getComboxConfig(ui->comboBoxCh2, true);
    getComboxConfig(ui->comboBoxCh3, true);
    getComboxConfig(ui->comboBoxCh4, true);
    getComboxConfig(ui->comboBoxCh5, true);
    getComboxConfig(ui->comboBoxCh6, true);
    getComboxConfig(ui->comboBoxCh7, true);
    getComboxConfig(ui->comboBoxCh8, true);

    selConfigRefrenPara();

    ui->lineEditSetEchoFre->setEnabled(true);
    ui->pushButtonEchoSet->setEnabled(true);
    ui->pushButtonEchoStart->setEnabled(false);
    ui->pushButtonEchoStart->setText("启动");
    ui->pushButtonSaveWave->setEnabled(false);
}

// 初始化所有配置通道
void MainWindow::selConfigRefrenPara()
{
    int index;
    if (uart2can.paraInfo.echoNum >= 1)
    {
        index = uart2can.paraInfo.writePara[0].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh1->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 2)
    {
        index = uart2can.paraInfo.writePara[1].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh2->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 3)
    {
        index = uart2can.paraInfo.writePara[2].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh3->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 4)
    {
        index = uart2can.paraInfo.writePara[3].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh4->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 5)
    {
        index = uart2can.paraInfo.writePara[4].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh5->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 6)
    {
        index = uart2can.paraInfo.writePara[5].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh6->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 7)
    {
        index = uart2can.paraInfo.writePara[6].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh7->setCurrentIndex(index);
    }
    if (uart2can.paraInfo.echoNum >= 8)
    {
        index = uart2can.paraInfo.writePara[7].valStr.toInt() - uart2can.paraInfo.echoNum - 1;
        if (index > 0)
            ui->comboBoxCh8->setCurrentIndex(index);
    }

    if (uart2can.paraInfo.echoNum > 0)
    {
        QString str = uart2can.paraInfo.writePara[uart2can.paraInfo.echoNum].valStr;
        ui->lineEditSetEchoFre->setText(str);
        uart2can.chEchoFreHz = str.toInt();
        uart2can.chEchoFreHzMax = ((quint16)uart2can.paraInfo.writePara[uart2can.paraInfo.echoNum].max < uart2can.chEchoFreHzMax) ? (quint16)uart2can.paraInfo.writePara[uart2can.paraInfo.echoNum].max : uart2can.chEchoFreHzMax;
        uart2can.chEchoFreHzMin = ((quint16)uart2can.paraInfo.writePara[uart2can.paraInfo.echoNum].min > uart2can.chEchoFreHzMin) ? (quint16)uart2can.paraInfo.writePara[uart2can.paraInfo.echoNum].min : uart2can.chEchoFreHzMin;
    }
}

// 更新修改标识
void MainWindow::paraTableRefreshModifyFlags(QTableWidget *widget, quint16 code, bool setModifiedFlag)
{
    enum paraStrItemType type = (enum paraStrItemType)((code & 0XF000) >> 12);
    quint16 pos = code & 0X0FFF;

    if (((type == WRITE_STR) && (type == WRITE_PARA)) || (pos >= uart2can.paraInfo.numTab[type]))
        return;

    int row = pos + uart2can.paraInfo.posArea[type].start;
    int column = PARA_TAB_EDIT_COLUMN;
    bool hasAnyModfied = false;

    if (setModifiedFlag == true)
    {
        widget->item(row, column)->setForeground(QColor(Qt::red));

        switch (type)
        {
        case WRITE_STR:
            uart2can.paraInfo.writeStr[pos].modifyFlag = true;
            break;
        case WRITE_PARA:
            uart2can.paraInfo.writePara[pos].modifyFlag = true;
            break;
        default:
            break;
        }

        hasAnyModfied = true;
    }
    else
    {
        widget->item(row, column)->setForeground(QColor(Qt::black));

        switch (type)
        {
        case WRITE_STR:
            uart2can.paraInfo.writeStr[pos].modifyFlag = false;
            break;
        case WRITE_PARA:
            uart2can.paraInfo.writePara[pos].modifyFlag = false;
            break;
        default:
            break;
        }

        for (pos = 0; pos < uart2can.paraInfo.numTab[WRITE_STR]; pos++)
        {
            if (uart2can.paraInfo.writeStr[pos].modifyFlag)
                hasAnyModfied = true;
        }
        for (pos = 0; pos < uart2can.paraInfo.numTab[WRITE_PARA]; pos++)
        {
            if (uart2can.paraInfo.writePara[pos].modifyFlag)
                hasAnyModfied = true;
        }
    }
    ui->pushButtonWritePara->setEnabled(hasAnyModfied);
}

// 更新已被修改的参数
void MainWindow::paraTableRefreshModified(QTableWidget *widget, quint16 code)
{
    paraTableRefreshModifyFlags(widget, code, true);
    enum paraStrItemType type = (enum paraStrItemType)((code & 0XF000) >> 12);
    quint16 pos = code & 0X0FFF;

    if (((type == WRITE_STR) && (type == WRITE_PARA)) || (pos >= uart2can.paraInfo.numTab[type]))
        return;

    int row = pos + uart2can.paraInfo.posArea[type].start;
    int column = PARA_TAB_EDIT_COLUMN;

    widget->item(row, column)->setForeground(QColor(Qt::black));

    switch (type)
    {
    case WRITE_STR:
        uart2can.paraInfo.writeStr[pos].str = widget->item(row, column)->text();
        break;
    case WRITE_PARA:
        uart2can.paraInfo.writePara[pos].valStr = widget->item(row, column)->text();
        break;
    default:
        break;
    }
    paraTableRefreshModifyFlags(widget, code, false);
}

// 参数表更新
void MainWindow::paraTableRefreshVal(QTableWidget *widget, quint16 code)
{
    enum paraStrItemType type = (enum paraStrItemType)((code & 0XF000) >> 12);
    quint16 pos = code & 0X0FFF;

    if ((type >= PARA_STR_TOTAL) || (pos >= uart2can.paraInfo.numTab[type]))
        return;

    QString str;

    switch (type)
    {
    case WRITE_STR:
        str = uart2can.paraInfo.writeStr[pos].str;
        break;
    case READ_STR:
        str = uart2can.paraInfo.readStr[pos].str;
        break;
    case WRITE_PARA:
        str = getParaValStr(uart2can.paraInfo.writePara[pos].type,
                            uart2can.paraInfo.writePara[pos].valR);
        uart2can.paraInfo.writePara[pos].valStr = str;
        break;
    case READ_PARA:
        str = getParaValStr(uart2can.paraInfo.readPara[pos].type,
                            uart2can.paraInfo.readPara[pos].valR);
        break;
    default:
        return;
    }

    int row = pos + uart2can.paraInfo.posArea[type].start;
    int column = PARA_TAB_EDIT_COLUMN;
    widget->item(row, column)->setText(str);
    paraTableRefreshModifyFlags(widget, code, false);
}

// 修改所有控件尺寸
void MainWindow::changeObjectSize(const QObject &o, double objectRate)
{
    for (int i = 0; i < o.children().size(); ++i)
    {
        QWidget *pWidget = qobject_cast<QWidget *>(o.children().at(i));
        if (pWidget != nullptr)
        {
            pWidget->setGeometry(pWidget->x() * objectRate, pWidget->y() * objectRate,
                                 pWidget->width() * objectRate, pWidget->height() * objectRate);
            changeObjectSize(*(o.children().at(i)), objectRate);
        }
    }
}

MainWindow::~MainWindow()
{
    uart2can.quit();
    if (uart2can.wait(100))
    {
        // qDebug("thread_send exit successful");
    }
    else
    {
        // qDebug("thread_send terminate");
        uart2can.terminate();
    }
    delete ui;
}

void MainWindow::readFromCom()
{
    QByteArray SerialRecv;

    while (!((SerialRecv = myCom->readAll()).isNull()))
    {
        // qDebug()<<"RxData:"<<SerialRecv.toHex();
        emit sendRecvData(SerialRecv);
    }
}

void MainWindow::writeToCom(QByteArray Pack, qint16 len)
{
    myCom->readAll(); // let the buffer null
    myCom->write(Pack.data(), len);

    if (true == myCom->flush())
    { /* 2022/11/16 ：有任何内容被写入则将内容显示在发送区 */
        qDebug() << "TxHex:" << Pack.toHex();
        ui->lineEditTxData->setText(Pack.toHex());
    }
}

void MainWindow::waveFreShow(QVector<QCPGraphData> *paraTab)
{
    ui->waveShow->setData(paraTab);
}

void MainWindow::on_pushButtonOpenCom_clicked()
{
    myCom->setPortName(ui->comboBoxCom->currentText());

    noCanDevConfig();
    ui->comboBoxDev->clear();

    uart2can.mcuBuf.point = 0;

    ui->progressBar->setValue(0);

    if (false == IsComOpen)
    {
        IsComOpen = myCom->open(QIODevice::ReadWrite); // 以读写的方式打开串口
        if (true == IsComOpen)
        {
            // 以下是串口设置。
            myCom->setBaudRate(uart2can.baudTab[0]);
            myCom->setDataBits(QSerialPort::Data8);
            myCom->setParity(QSerialPort::NoParity);
            myCom->setStopBits(QSerialPort::OneStop);
            myCom->setFlowControl(QSerialPort::NoFlowControl);
            myCom->setReadBufferSize(2048);
            IsComOpen = true;

            ui->statusBar->showMessage("成功打开串口：" + QString::number(uart2can.baudTab[0], 10) + ",8,1,0"); // 这个跟显示的可能不太一样，可能是版本的问题

            ui->pushButtonOpenCom->setText("关闭串口");
            ui->comboBoxCom->setEnabled(false);
            ui->pushButtonRefreshCom->setEnabled(false);

            baudTabPoint = 1;
            uart2can.setCmd(STEP_INIT);
        }
        else
        {
            ui->statusBar->showMessage("无法打开串口或是串口被占用！");
        }
    }
    else
    {
        myCom->close();

        ui->pushButtonOpenCom->setText("打开串口");
        IsComOpen = false;
        ui->pushButtonInOtaMode->setEnabled(false);
        ui->pushButtonRefreshCom->setEnabled(true);

        ui->comboBoxCom->setEnabled(true);

        ui->statusBar->showMessage("请选择串口并打开！");

        uart2can.setCmd(STEP_STOP);
    }
}

void MainWindow::on_pushButtonOpenFile_clicked() // 选择文件操作
{
    qDebug() << "path last is : " << path;
    if (path.isEmpty() == true)
    {
        if (FilePath.isEmpty() == false) //  xj 20230906 加入此部分在误操作时记录路径
        {
            path = FilePath;
        }
        else
        {
            path = ".";
        }
    }
    FilePath = QFileDialog::getOpenFileName(this, "Open Binary File", path, "Binary File(*.bin *.hex)");
    qDebug() << "path is : " << path;
    // the path is ok
    if (FilePath.isEmpty() == false)
    {
        ui->lineEditSelFile->setText(FilePath);
        path = FilePath;
    }
    qDebug() << "path lineEditSelFile is : " << path;
}

void MainWindow::on_pushButtonInOtaMode_clicked() // OTA的按钮
{
    // qDebug("into OTA mode!");
    ui->progressBar->setValue(0);

    if (intoOtaFlag == true)
    {
        int pos = ui->comboBoxDev->currentIndex();

        uart2can.mcuBuf.usePos = (quint8)pos; //

        uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];

        ui->pushButtonInOtaMode->setText("停止升级");
        intoOtaFlag = false;
        uart2can.setCmd(STEP_IN_OTA);

        ui->labelOtaInfo->setStyleSheet("color:blue;");
        ui->labelOtaInfo->setText("启动升级中，需要擦除flash,请等待...");

        // ui->statusBar->showMessage("启动升级中，需要擦除flash,请等待...");
    }
    else
    {
        ui->pushButtonInOtaMode->setText("启动升级");
        intoOtaFlag = true;
        uart2can.setCmd(STEP_SEND_END);
    }
    ui->pushButtonStartOta->setEnabled(false);
}

void MainWindow::on_pushButtonStartOta_clicked()
{
    uart2can.BinData.clear(); // file buf clear

    // the path is ok
    if (FilePath.length())
    {
        QFile file(FilePath);
        if (file.open(QIODevice::ReadOnly))
        {
            uart2can.BinData = file.readAll(); // read file
            file.close();
        }
    }
    // qDebug("send file info!");
    ui->progressBar->setValue(0);
    uart2can.setCmd(STEP_SEND_INFO);
    ui->pushButtonStartOta->setEnabled(false); // 使按钮失活
}

// 更新状态
void MainWindow::StateUpdate()
{
    ui->tableWidgetPara->blockSignals(true);
    QString str;
    quint8 tempU8;
    static quint8 Encode_error = 0;
    quint8 fault_encode = 0;
    switch (uart2can.otaStatus)
    {
    case OTA_STA_DETECT_MCU:
        ui->labelOtaInfo->setStyleSheet("color:green;");

        str = "检测到设备,mcuId:0X" + QString::number(uart2can.mcuId, 16) + " canId:" + QString::number(uart2can.mcuCanId, 10);
        ui->labelOtaInfo->setText(str);
        ui->statusBar->showMessage(str);

        if (uart2can.mcuBuf.changeFlag == true)
        {
            detectedCanDevConfig();

            for (quint8 i = 0; i < uart2can.mcuBuf.point; i++)
            {
                str = "CAN:" + QString::number(uart2can.mcuBuf.canId[i], 10) + "  id:" + QString::number(uart2can.mcuBuf.id[i], 16);
                ui->comboBoxDev->addItem(str); /* 检测到设备，显示ID信息 */
            }
        }
        break;
    case OTA_STA_CONNECT_OVER_TIME:
        myCom->setBaudRate(uart2can.baudTab[1]);
        ui->statusBar->showMessage("配置串口：" + QString::number(uart2can.baudTab[1], 10));
        baudTabPoint = 1;
        uart2can.setCmd(STEP_INCONFIG);
        break;
    case OTA_STA_CONNECT_SUCCESS:
        ui->pushButtonGetDevInfo->setEnabled(true);
        ui->statusBar->showMessage("CAN模块配置成功！");
        break;
    case OTA_STA_INCONFIG_OVER_TIME:

        baudTabPoint++;
        if (baudTabPoint >= BAUD_TAB_SIZE)
        {
            ui->statusBar->showMessage("CAN模块配置失败，请检查CAN设备及配置！");
        }
        else
        {
            myCom->setBaudRate(uart2can.baudTab[baudTabPoint]);
            ui->statusBar->showMessage("配置串口：" + QString::number(uart2can.baudTab[baudTabPoint], 10));
            uart2can.setCmd(STEP_INCONFIG);
        }
        break;
    case OTA_STA_INCONFIG_SUCCESS:

        uart2can.setCmd(STEP_CONFIG);
        break;
    case OTA_STA_CONFIG_OVER_TIME:
        ui->statusBar->showMessage("CAN模块配置失败，请检查CAN设备及配置！");
        break;
    case OTA_STA_CONFIG_SUCCESS:
        myCom->setBaudRate(uart2can.baudTab[0]);
        ui->statusBar->showMessage("CAN模块配置成功！");
        uart2can.setCmd(STEP_INIT);
        break;
    case OTA_STA_IN_OTA_MODE_SUCCESS:
        ui->labelOtaInfo->setStyleSheet("color:green;");
        ui->labelOtaInfo->setText("设备已进入升级模式！");
        ui->pushButtonStartOta->setEnabled(true);
        break;
    case OTA_STA_IN_OTA_MODE_OVER_TIME:
        ui->labelOtaInfo->setStyleSheet("color:red;");
        ui->labelOtaInfo->setText("设备进入升级模式失败！");
        break;
    case OTA_STA_BINFILE_EMPTY:
        ui->labelOtaInfo->setStyleSheet("color:red;");
        ui->labelOtaInfo->setText("bin文件没有数据！");
        break;
    case OTA_STA_BINFILE_SIZE_OVER_LIMIT:
        ui->labelOtaInfo->setStyleSheet("color:red;");
        ui->labelOtaInfo->setText("BIN文件大小超过512KB!");
        break;
    case OTA_STA_OTA_INFO_SUCCESS:
        ui->labelOtaInfo->setStyleSheet("color:black;");
        ui->labelOtaInfo->setText("传输中...");
        ui->progressBar->setValue(0);
        ui->progressBar->setRange(0, uart2can.PackNumber);
        break;
    case OTA_STA_OTA_INFO_OVER_TIME:
        ui->labelOtaInfo->setStyleSheet("color:red;");
        ui->labelOtaInfo->setText("发送bin文件信息失败！");
        break;
    case OTA_STA_SEND_BIN_RETRY:
    case OTA_STA_SEND_BIN_OK:
        ui->progressBar->setValue(uart2can.PackCnt);
        break;

    case OTA_STA_SEND_BIN_DONE:
        ui->labelOtaInfo->setStyleSheet("color:black;");
        ui->labelOtaInfo->setText("文件传输完成！");
        break;
    case OTA_STA_SUCCESS:
        ui->progressBar->setValue(uart2can.PackNumber);
        ui->labelOtaInfo->setStyleSheet("color:green;");
        ui->labelOtaInfo->setText("升级成功！");
        if (intoOtaFlag == false)
        {
            ui->pushButtonInOtaMode->setText("启动升级");
            intoOtaFlag = true;
        }
        break;
    case OTA_STA_FAILED:
        ui->labelOtaInfo->setStyleSheet("color:red;");
        ui->labelOtaInfo->setText("升级失败");
        if (intoOtaFlag == false)
        {
            ui->pushButtonInOtaMode->setText("启动升级");
            intoOtaFlag = true;
        }
        break;

    case OTA_STA_SET_ID_SUCCESS:
        ui->labelSetCanId->setStyleSheet("color:green;");
        ui->labelSetCanId->setText("设置CAN_ID成功");
        ui->textEditCfgInfo->append("设置CAN_ID成功!" + ui->comboBoxDev->currentText());
        uart2can.configCanId++;
        ui->lineEditSetCanId->setText(QString::number(uart2can.configCanId, 10));
        break;

    case OTA_STA_SET_ID_OVER_TIME:
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("设置CAN_ID失败");
        ui->textEditCfgInfo->append("设置CAN_ID失败!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_RST_MODE_SUCCESS:
        ui->textEditCfgInfo->append("设置RESET模式成功!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_RST_MODE_OVER_TIME:
        ui->textEditCfgInfo->append("设置RESET模式失败!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_MOTOR_MODE_SUCCESS:
        ui->textEditCfgInfo->append("设置MOTOR模式成功!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_MOTOR_MODE_OVER_TIME:
        ui->textEditCfgInfo->append("设置MOTOR模式失败!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_BRAKE_MODE_SUCCESS:
        ui->textEditCfgInfo->append("设置BRAKE模式成功!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_BRAKE_MODE_OVER_TIME:
        ui->textEditCfgInfo->append("设置BRAKE模式失败!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_CALI_MODE_SUCCESS:
        ui->textEditCfgInfo->append("设置高速编码器标定模式成功!" + ui->comboBoxDev->currentText());
        ui->textEditCfgInfo->append("请耐心等待标定完成...");
        break;

    case OTA_STA_CALI_MODE_OVER_TIME:
        ui->textEditCfgInfo->append("设置高速编码器标定模式失败!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_ZERO_MODE_SUCCESS:
        ui->textEditCfgInfo->append("设置机械零位成功!" + ui->comboBoxDev->currentText());
        break;

    case OTA_STA_ZERO_MODE_OVER_TIME:
        ui->textEditCfgInfo->append("设置机械零位失败!" + ui->comboBoxDev->currentText());
        break;
    case OTA_STA_ENCODER_CALI_ING:
        ui->textEditCfgInfo->append("编码器标定中-转子角度：" + QString::number(uart2can.currentAngle, 'f', 4) + " 定子磁链角度：" + QString::number(uart2can.rotorAngle, 'f', 4) + " 传感器值：" + QString::number(uart2can.caliRaw, 10));
        if (uart2can.caliRaw < 20 || uart2can.caliRaw > MAX_POS_VAL)
        {
            Encode_error++;
        }
        else
        {
            Encode_error = 0;
        }

        break;
    case OTA_STA_SWEEP_RESULT:
       ui->textEditCfgInfo->append("扫频模式-频率索引："+QString::number(uart2can.frequency)+" 幅值："+QString::number(uart2can.amplitude)+"db"+" 相位："+QString::number(uart2can.phase)+"度");
       break;
    case OTA_STA_ENCODER_CALI_RESULT:
        ui->textEditCfgInfo->append("编码器标定结果-设备：" + QString::number(uart2can.canIdGet, 10) + " 编号：" + QString::number(uart2can.caliI, 10) + " 数组序号：" + QString::number(uart2can.caliInd, 10) + " 校正值：" + QString::number(uart2can.caliLut, 10));
        break;
    case OTA_STA_ENCODER_CALI_DONE:
        ui->textEditCfgInfo->append("编码器标定完成-设备：" + QString::number(uart2can.canIdGet, 10) + " Encoder Electrical Offset：" + QString::number(uart2can.caliOffset, 'f', 6) + "电角度均方差：" + QString::number(uart2can.caliMSE, 'f', 6));
        if (Encode_error > 5)
        {
            fault_encode = 1;
        }
        str = "编码器标定完成-设备：" + QString::number(uart2can.canIdGet, 10) + " 故障：" //+QString::number(uart2can.hallError,16)
              + ((fault_encode) ? "/异常" : "")

            ;

        ui->textEditCfgInfo->append(str);
        // 这部分做的是显示用的。
        if (uart2can.calerrorcode)
            ui->labelSetCanId->setStyleSheet("color:red;");
        else
            ui->labelSetCanId->setStyleSheet("color:green;");
        ui->labelSetCanId->setText(str);
        break;

    case OTA_STA_HALL_CALI_ING:
        ui->textEditCfgInfo->append("Hall标定中-设备：" + QString::number(uart2can.canIdGet, 10) + " 序号：" + QString::number(uart2can.hallCount, 10) + " hall值 1：" + QString::number(uart2can.hallRaw[0], 10) + " 2：" + QString::number(uart2can.hallRaw[1], 10) + " 3：" + QString::number(uart2can.hallRaw[2], 10) + " 4：" + QString::number(uart2can.hallRaw[3], 10) + " 5：" + QString::number(uart2can.hallRaw[4], 10) + " 6：" + QString::number(uart2can.hallRaw[5], 10));
        break;
    case OTA_STA_HALL_CALI_RESULT1:
        ui->textEditCfgInfo->append("Hall标定结果1-设备：" + QString::number(uart2can.canIdGet, 10) + " 序号：" + QString::number(uart2can.hallCount, 10) + " 查表值(右-左)：" + QString::number(uart2can.hallRdLval, 10));
        break;
    case OTA_STA_HALL_CALI_RESULT2:
        ui->textEditCfgInfo->append("Hall标定结果2-设备：" + QString::number(uart2can.canIdGet, 10) + " HALL编号：" //+QString::number(uart2can.hallCount,10)
                                    + " 最大值：" + QString::number(uart2can.hallInfo[uart2can.hallCount].maxVal, 10) + " 最小值：" + QString::number(uart2can.hallInfo[uart2can.hallCount].minVal, 10) + " 起点：" + QString::number(uart2can.hallInfo[uart2can.hallCount].minPos, 10) + " 中点：" + QString::number(uart2can.hallInfo[uart2can.hallCount].midPos, 10) + " 终点：" + QString::number(uart2can.hallInfo[uart2can.hallCount].maxPos, 10) + " 非单调：" + QString::number(uart2can.hallInfo[uart2can.hallCount].noLinearCount, 10) + " 故障：" + QString::number(uart2can.hallInfo[uart2can.hallCount].fault, 16) + " 警告：" + QString::number(uart2can.hallInfo[uart2can.hallCount].warn, 16));
        break;
    case OTA_STA_HALL_CALI_DONE:
        str = "HALL标定完成-设备：" + QString::number(uart2can.canIdGet, 10) + " 故障：" //+QString::number(uart2can.hallError,16)
              + ((uart2can.hallError & HALL_FAULT_MAX) ? "/极大值" : "") + ((uart2can.hallError & HALL_FAULT_MIN) ? "/极小值" : "") + ((uart2can.hallError & HALL_FAULT_POS) ? "/定位点" : "") + ((uart2can.hallError & HALL_FAULT_VAL) ? "/动态值" : "") + ((uart2can.hallError & HALL_FAULT_NOCALI) ? "/未标定" : "") + " 警告：" + QString::number(uart2can.hallWarn, 16) + ((uart2can.hallWarn & HALL_WARN_LINEAR) ? "/非单调" : "") + ((uart2can.hallWarn & HALL_WARN_DMAX) ? "/查表极大值超限" : "") + ((uart2can.hallWarn & HALL_WARN_DMIN) ? "/查表极小值超限" : "");

        ui->textEditCfgInfo->append(str);
        // 这部分做的是显示用的。
        if (uart2can.hallError)
            ui->labelSetCanId->setStyleSheet("color:red;");
        else if (uart2can.hallWarn)
            ui->labelSetCanId->setStyleSheet("color:blue;");
        else
            ui->labelSetCanId->setStyleSheet("color:green;");
        ui->labelSetCanId->setText(str);

        for (tempU8 = 0; tempU8 < 6; tempU8++)
        {

            ui->textEditCfgInfo->append("HALL  编号：" + QString::number(tempU8 + 1, 10) + " 故障："                                                                                                                                                                                                                                                                                                                                                                                                   //+QString::number(uart2can.hallInfo[tempU8].fault,16)
                                        + ((uart2can.hallInfo[tempU8].fault & HALL_FAULT_MAX) ? "/极大值" : "") + ((uart2can.hallInfo[tempU8].fault & HALL_FAULT_MIN) ? "/极小值" : "") + ((uart2can.hallInfo[tempU8].fault & HALL_FAULT_POS) ? "/定位点" : "") + ((uart2can.hallInfo[tempU8].fault & HALL_FAULT_POS) ? "/定位点" : "") + ((uart2can.hallInfo[tempU8].fault & HALL_FAULT_VAL) ? "/动态值" : "") + ((uart2can.hallInfo[tempU8].fault & HALL_FAULT_NOCALI) ? "/未标定" : "") + " 警告：" //+QString::number(uart2can.hallInfo[tempU8].warn,16)
                                        + ((uart2can.hallInfo[tempU8].warn & HALL_WARN_LINEAR) ? "/非单调" : "") + ((uart2can.hallInfo[tempU8].warn & HALL_WARN_DMAX) ? "/查表极大值超限" : "") + ((uart2can.hallInfo[tempU8].warn & HALL_WARN_DMIN) ? "/查表极小值超限" : ""));
        }
        break;

    case OTA_STA_MOTOR_FEEDBACK:

        str = "电机：" + QString::number(uart2can.canIdGet, 10) + " 位置：" + QString::number(uart2can.posGet, 'f', 4) + " 速度：" + QString::number(uart2can.velGet, 'f', 4) + " 力矩：" + QString::number(uart2can.torqueGet, 'f', 4)
              + " 模式：" + ((uart2can.mtStatus.mode == MT_MODE_CALI) ? "标定" : ((uart2can.mtStatus.mode == MT_MODE_MOTOR) ? "3_PARALL高编" : ((uart2can.mtStatus.mode == MT_MODE_MOTOR_4) ? "4_CASCADE高编" :
            ((uart2can.mtStatus.mode == MT_MODE_MOTOR_248) ? "248_扭矩步进" : ((uart2can.mtStatus.mode == MT_MODE_MOTOR_253) ? "253_开环转" :  ((uart2can.mtStatus.mode == MT_MODE_MOTOR_17) ? "17_扫频":((uart2can.mtStatus.mode == MT_MODE_BRAKE) ? "BRAKE" : "RESET")))))))
           + " 故障：" + (uart2can.mtStatus.underVoltFault ? "/低压" : "") + (uart2can.mtStatus.overCurFault ? "/过流" : "") + (uart2can.mtStatus.overTempFault ? "/过温" : "") + (uart2can.mtStatus.encoderFault ? "/编码器" : "")
           + (uart2can.mtStatus.hallFault ? "/霍尔" : "") + (uart2can.mtStatus.noCaliFault ? "/未标定" : "");

        ui->textEditCfgInfo->append(str);

        break;
    case OTA_STA_MOTOR_FAULTWARN:

        str = "电机：" + QString::number(uart2can.canIdGet, 10) + "\r\n故障：\r\n" + ((uart2can.faultSta & FAULT_MC_OVT) ? "  电机温度过高【>100℃】\r\n" : "") + ((uart2can.faultSta & FAULT_DRV) ? "  驱动芯片故障\r\n" : "") + ((uart2can.faultSta & FAULT_LOW_VOLT) ? "  母线电压过低【<10V】\r\n" : "") + ((uart2can.faultSta & FAULT_HIGH_VOLT) ? "  母线电压过高【>30V】\r\n" : "") + ((uart2can.faultSta & FAULT_OC_IB) ? "  B相电流过大【>78A】\r\n" : "") + ((uart2can.faultSta & FAULT_OC_IC) ? "  C相电流过大【>78A】\r\n" : "") + ((uart2can.faultSta & FAULT_ENCODER_CALI) ? "  编码器未标定\r\n" : "") + "\r\n警告：\r\n" + ((uart2can.warnSta & WARN_MC_TEMP) ? "  电机高温预警【>65℃】\r\n" : "") + ((uart2can.warnSta & WARN_HALL_CALI) ? "  HALL未标定\r\n" : "") + ((uart2can.warnSta & WARN_TORQUE_CALI) ? "  力矩标定故障\r\n" : "") + ((uart2can.warnSta & WARN_TORQUE_COEF) ? "  力矩标定拟合零偏过大【>0.3nm】\r\n" : "");

        ui->textEditCfgInfo->append(str);
        break;
    case OTA_STA_GET_PARA_INFO_SUCCESS:
        ui->labelParaInfo->setStyleSheet("color:green;");
        ui->labelParaInfo->setText("更新参数表成功！");
        ui->label_selChInfo->setStyleSheet("color:green;");
        ui->label_selChInfo->setText("更新参数表成功！");
        paraTableUpdateAll(ui->tableWidgetPara);
        // ui->pushButtonRefreshTable->setEnabled(true);
        ui->pushButtonReadPara->setEnabled(true);
        ui->pushButtonWritePara->setEnabled(false);
        ui->pushButtonExportExcel->setEnabled(true);

        if (ui->comboBoxSelMode->currentIndex() == 1)
            ui->pushButtonRecoverFac->setEnabled(true);
        break;

    case OTA_STA_GET_PARA_INFO_OVER_TIME:
        ui->labelParaInfo->setStyleSheet("color:red;");
        ui->labelParaInfo->setText("更新参数表失败！");
        ui->pushButtonRefreshTable->setEnabled(true);
        break;

    case OTA_STA_GET_PARA_INFO_ERROR:
        ui->labelParaInfo->setStyleSheet("color:red;");

        str = "更新参数表发生错误 code:0X" + QString::number(uart2can.paraErrorCode, 16) + ((uart2can.paraErrorCode & PARA_ERROR_NO_TYPE) ? "/非法类型" : "") + ((uart2can.paraErrorCode & PARA_ERROR_TYPE) ? "/类型漏帧" : "") + ((uart2can.paraErrorCode & PARA_ERROR_POINT) ? "/编号漏帧" : "") + ((uart2can.paraErrorCode & PARA_ERROR_POINT_CONT) ? "/编号不连续" : "") + ((uart2can.paraErrorCode & PARA_ERROR_NUM) ? "/超存储限制" : "") + ((uart2can.paraErrorCode & PARA_ERROR_ITEM) ? "/漏帧" : "") + ((uart2can.paraErrorCode & PARA_ERROR_ITEM_MATCH) ? "/类型和数据帧不匹配" : "") + ((uart2can.paraErrorCode & PARA_ERROR_WRITE_STR_NUM_MATCH) ? "/可写字符串数量错误" : "") + ((uart2can.paraErrorCode & PARA_ERROR_READ_STR_NUM_MATCH) ? "/只读字符串数量错误" : "") + ((uart2can.paraErrorCode & PARA_ERROR_WRITE_PARA_NUM_MATCH) ? "/可写参数数量错误" : "") + ((uart2can.paraErrorCode & PARA_ERROR_READ_PARA_NUM_MATCH) ? "/只读参数数量错误" : "");
        ui->labelParaInfo->setText(str);
        ui->pushButtonRefreshTable->setEnabled(true);
        break;

    case OTA_STA_READ_PARA_SUCCESS:
        ui->labelParaInfo->setStyleSheet("color:green;");
        ui->labelParaInfo->setText("成功读取参数:0X" + QString("%1").arg(uart2can.paraInfo.codeNeed, 4, 16, QLatin1Char('0')));
        paraTableRefreshVal(ui->tableWidgetPara, uart2can.paraInfo.codeNeed);

        if (readParaAllFlag)
        {
            uart2can.paraInfo.posIndex++;

            if (uart2can.paraInfo.posIndex >= uart2can.paraInfo.numTab[uart2can.paraInfo.itemIndex])
            {
                uart2can.paraInfo.posIndex = 0;

                uart2can.paraInfo.itemIndex = (enum paraStrItemType)((qint8)uart2can.paraInfo.itemIndex + 1);

                if (uart2can.paraInfo.itemIndex >= PARA_STR_TOTAL)
                {
                    ui->pushButtonReadPara->setEnabled(true);
                    ui->labelParaInfo->setText("成功读取所有参数！");
                    readParaAllFlag = false;
                }
            }

            if (uart2can.paraInfo.itemIndex < PARA_STR_TOTAL)
                uart2can.setCmd(STEP_READ_PARA_STR);
        }
        break;

    case OTA_STA_READ_PARA_OVER_TIME:
        ui->labelParaInfo->setStyleSheet("color:red;");
        ui->labelParaInfo->setText("读取参数超时：0X" + QString("%1").arg(uart2can.paraInfo.codeNeed, 4, 16, QLatin1Char('0')));
        ui->pushButtonReadPara->setEnabled(true);
        break;

    case OTA_STA_READ_PARA_ERROR:
        ui->labelParaInfo->setStyleSheet("color:red;");

        str = "读取参数0X" + QString("%1").arg(uart2can.paraInfo.codeNeed, 4, 16, QLatin1Char('0')) + "发生错误:" + ((uart2can.paraErrorCode & PARA_READ_ERROR_CODE) ? "/无此功能码" : "") + ((uart2can.paraErrorCode & PARA_READ_ERROR_TYPE) ? "/类型错误" : "") + ((uart2can.paraErrorCode & PARA_READ_ERROR_INDEX) ? "/漏帧" : "") + ((uart2can.paraErrorCode & PARA_READ_ERROR_NOT_NEED) ? "/编号不匹配" : "");
        ui->labelParaInfo->setText(str);
        ui->pushButtonReadPara->setEnabled(true);
        break;

    case OTA_STA_SAVE_PARA_SUCCESS:
        ui->labelParaInfo->setStyleSheet("color:green;");
        ui->labelParaInfo->setText("所有参数保存成功！");
        break;

    case OTA_STA_WRITE_PARA_SUCCESS:
        ui->labelParaInfo->setStyleSheet("color:green;");
        ui->labelParaInfo->setText("成功写参数:0X" + QString("%1").arg(uart2can.paraInfo.codeNeed, 4, 16, QLatin1Char('0')));

        ui->tableWidgetPara->item(uart2can.paraInfo.posIndex + uart2can.paraInfo.posArea[uart2can.paraInfo.itemIndex].start,
                                  PARA_TAB_EDIT_COLUMN)
            ->setForeground(QColor(Qt::black));

        paraTableRefreshModified(ui->tableWidgetPara, uart2can.paraInfo.codeNeed);

        if (writeAllModyfiedFlag)
        {
            while (uart2can.paraInfo.posIndex < uart2can.paraInfo.numTab[uart2can.paraInfo.itemIndex])
            {
                uart2can.paraInfo.posIndex++;

                if (uart2can.paraInfo.posIndex >= uart2can.paraInfo.numTab[uart2can.paraInfo.itemIndex])
                {
                    uart2can.paraInfo.posIndex = 0;
                    if (uart2can.paraInfo.itemIndex == WRITE_STR)
                        uart2can.paraInfo.itemIndex = WRITE_PARA;
                    else if (uart2can.paraInfo.itemIndex == WRITE_PARA)
                        uart2can.paraInfo.itemIndex = PARA_STR_TOTAL;
                }

                if (uart2can.paraInfo.itemIndex == WRITE_STR)
                {
                    if (uart2can.paraInfo.writeStr[uart2can.paraInfo.posIndex].modifyFlag)
                        break;
                }
                else if (uart2can.paraInfo.itemIndex == WRITE_PARA)
                {
                    if (uart2can.paraInfo.writePara[uart2can.paraInfo.posIndex].modifyFlag)
                        break;
                }
                else
                    break;
            }

            if (uart2can.paraInfo.itemIndex < PARA_STR_TOTAL)
                uart2can.setCmd(STEP_WRITE_PARA_STR);
            else
                uart2can.setCmd(STEP_SAVE_PARA_STR);
        }
        break;

    case OTA_STA_WRITE_PARA_OVER_TIME:
        ui->labelParaInfo->setStyleSheet("color:red;");
        ui->labelParaInfo->setText("写参数超时：0X" + QString("%1").arg(uart2can.paraInfo.codeNeed, 4, 16, QLatin1Char('0')));
        ui->pushButtonWritePara->setEnabled(true);
        break;

    case OTA_STA_WRITE_PARA_ERROR:
        ui->labelParaInfo->setStyleSheet("color:red;");

        str = "写参数0X" + QString("%1").arg(uart2can.paraInfo.codeNeed, 4, 16, QLatin1Char('0')) + "发生错误:" + ((uart2can.paraErrorCode & PARA_WR_ERROR_NO_CODE) ? "/无此参数" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_FORBID_MODIFY) ? "/不可修改" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_TYTE) ? "/类型不匹配" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_LOSS_FRAME) ? "/漏帧" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_FRAME) ? "/错帧" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_LIMIT) ? "/设置数据超范围" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_MODE_FORBID_MODIFY) ? "/当前模式下不可修改" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_MODE_FORBID_SAVE) ? "/当前模式下不可保存" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_SAVE) ? "/保存失败" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_NO_CMD) ? "/命令错误" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_CMD_MATCH) ? "/命令不匹配" : "") + ((uart2can.paraErrorCode & PARA_WR_ERROR_CODE_MATCH) ? "/参数不匹配" : "");
        ui->labelParaInfo->setText(str);
        ui->pushButtonWritePara->setEnabled(true);
        break;

    case OTA_STA_PARA_UPDATE_ERROR:
        ui->label_selChInfo->setStyleSheet("color:red;");

        str = "示波器配置发送错误:" + QString::number(uart2can.paraErrorCode, 16) + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_CMD) ? "/未知命令" : "") + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_FRESET) ? "/频率配置超出范围" : "") + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_CHSET_NOCODE) ? "/未知变量" : "") + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_CHSET_FRE) ? "/数据超出频率可输出数量" : "") + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_CHSET_LOSE) ? "/错帧" : "") + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_START) ? "/启动错误" : "") + ((uart2can.paraErrorCode & PARA_UPDATE_ERROR_CHSET_TYPE) ? "/类型错误" : "");

        if (uart2can.paraErrorCode & PARA_UPDATE_ERROR_FRESET)
            str += QString::number(uart2can.chEchoDevFreMin) + "~" + QString::number(uart2can.chEchoDevFreMax);
        ui->label_selChInfo->setText(str);
        break;
    case OTA_STA_PARA_UPDATE_OVER_TIME:
        ui->label_selChInfo->setStyleSheet("color:red;");
        ui->label_selChInfo->setText("示波器配置超时！");
        break;
    case OTA_STA_PARA_UPDATE_SUCCESS_FRESET:
        ui->label_selChInfo->setStyleSheet("color:green;");
        ui->label_selChInfo->setText("频率配置成功！");
        uart2can.chSelPos = 0;
        uart2can.setCmd(STEP_SCOPE_SET_CHANNAL);
        break;
    case OTA_STA_PARA_UPDATE_SUCCESS_CHSET:
        ui->waveShow->clearAllChannal();
        ui->waveShow->configChannal(uart2can.chSelIndex, uart2can.chSelNum);
        ui->pushButtonEchoStart->setEnabled(true);
        ui->pushButtonEchoStart->setText("启动");
        ui->pushButtonSaveWave->setEnabled(true);
        ui->label_selChInfo->setStyleSheet("color:green;");
        ui->label_selChInfo->setText("通道配置成功！");
        break;
    case OTA_STA_PARA_UPDATE_SUCCESS_START:
        ui->label_selChInfo->setStyleSheet("color:green;");
        ui->label_selChInfo->setText("成功启动！");
        break;
    case OTA_STA_PARA_UPDATE_SUCCESS_END:
        ui->label_selChInfo->setStyleSheet("color:green;");
        ui->label_selChInfo->setText("成功停止");
        ui->pushButtonEchoStart->setText("启动");
        ui->pushButtonEchoSet->setEnabled(true);
        ui->lineEditSetEchoFre->setEnabled(true);
        ui->comboBoxCh1->setEnabled(true);
        ui->comboBoxCh2->setEnabled(true);
        ui->comboBoxCh3->setEnabled(true);
        ui->comboBoxCh4->setEnabled(true);
        ui->comboBoxCh5->setEnabled(true);
        ui->comboBoxCh6->setEnabled(true);
        ui->comboBoxCh7->setEnabled(true);
        ui->comboBoxCh8->setEnabled(true);
        break;
    default:
        break;
    }
    ui->tableWidgetPara->blockSignals(false);
}

void MainWindow::on_pushButtonGetDevInfo_clicked()
{
    noCanDevConfig();

    uart2can.mcuBuf.point = 0;
    uart2can.setCmd(STEP_GET_DEV_INFO); // 上位机给电机驱动器发送一个
}

void MainWindow::on_lineEditSetCanId_textChanged(const QString &arg1)
{
    bool ok;

    quint32 id = arg1.toUInt(&ok);

    if ((ok == false) || (id > 127) || (id < 1))
    {
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("设备ID范围：1~127");

        if (id > 127)
            ui->lineEditSetCanId->setText("127");
        else
            ui->lineEditSetCanId->setText("1");
    }
    else if (id == 127)
    {
        ui->labelSetCanId->setStyleSheet("color:blue;");
        ui->labelSetCanId->setText("设备ID:127,该地址是默认can设备ID");
    }
    else
    {
        ui->labelSetCanId->setStyleSheet("color:black;");
        ui->labelSetCanId->setText("设备ID:" + QString::number(id));
    }

    uart2can.configCanId = (quint8)id;
}

void MainWindow::on_pushButtonSetCanId_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n设置CAN_ID--设备：" + ui->comboBoxDev->currentText() + "  设置ID:" + QString::number(uart2can.configCanId, 10));

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_SET_CAN_ID);
}

void MainWindow::on_pushButtonCfgReset_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n设置复位模式--设备：" + ui->comboBoxDev->currentText());

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_SET_RST_MODE);
}

void MainWindow::on_pushButtonCaliEncoder_clicked()
{
    int pos = ui->comboBoxDev->currentIndex(); // ui表示整个界面，comboBoxDev是CAN_ID的复选框，该语句是获取CAN——ID
    uart2can.mcuBuf.usePos = (quint8)pos;

    // textEditCfgInfo是指显示信息的白框
    ui->textEditCfgInfo->append("\r\n设置高速编码器标定模式--设备：" + ui->comboBoxDev->currentText());

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("编码器&HALL标定中，请等待...");

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_SET_CALI_ENCODER_MODE);
}

void MainWindow::on_pushButtonZeroMech_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n设置机械零点--设备：" + ui->comboBoxDev->currentText());

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_SET_ZERO_MECH_MODE);
}

void MainWindow::on_lineEditSetPos_textChanged(const QString &arg1)
{
    bool ok;

    float val = arg1.toFloat(&ok);

    if ((ok == false) || (val > P_MAX) || (val < P_MIN))
    {
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("角度设置范围：" + QString::number(P_MIN, 'f', 2) + "~" + QString::number(P_MAX, 'f', 2) + " rad");

        if (val > P_MAX)
            ui->lineEditSetPos->setText(QString::number(P_MAX, 'f', 2));
        else if (val < P_MIN)
            ui->lineEditSetPos->setText(QString::number(P_MIN, 'f', 2));
    }
    else
    {
        ui->labelSetCanId->setStyleSheet("color:blue;");
        ui->labelSetCanId->setText("角度输入： " + arg1 + " rad");
    }
}

void MainWindow::on_lineEditSetPos_editingFinished()
{
    bool ok;

    float val = ui->lineEditSetPos->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.posSet = val;
    }
    else
    {
        uart2can.posSet = 0;
        ui->lineEditSetPos->setText("0");
    }

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("角度设置： " + QString::number(uart2can.posSet, 'f', 4) + " rad");
}

void MainWindow::on_lineEditSetKp_textChanged(const QString &arg1)
{
    bool ok;

    float val = arg1.toFloat(&ok);

    if ((ok == false) || (val > KP_MAX) || (val < KP_MIN))
    {
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("角度增益设置范围：" + QString::number(KP_MIN, 'f', 2) + "~" + QString::number(KP_MAX, 'f', 2) + " nm/rad");

        if (val > KP_MAX)
            ui->lineEditSetKp->setText(QString::number(KP_MAX, 'f', 2));
        else if (val < KP_MIN)
            ui->lineEditSetKp->setText(QString::number(KP_MIN, 'f', 2));
    }
    else
    {
        ui->labelSetCanId->setStyleSheet("color:blue;");
        ui->labelSetCanId->setText("角度增益输入： " + arg1 + " nm/rad");
    }
}

void MainWindow::on_lineEditSetKp_editingFinished()
{
    bool ok;

    float val = ui->lineEditSetKp->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.kpSet = val;
    }
    else
    {
        uart2can.kpSet = 0;
        ui->lineEditSetKp->setText("0");
    }

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("角度增益设置： " + QString::number(uart2can.kpSet, 'f', 4) + " nm/rad");
}

void MainWindow::on_lineEditSetVel_textChanged(const QString &arg1)
{
    bool ok;

    float val = arg1.toFloat(&ok);

    if ((ok == false) || (val > V_MAX) || (val < V_MIN))
    {
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("角速度设置范围：" + QString::number(V_MIN, 'f', 2) + "~" + QString::number(V_MAX, 'f', 2) + " rad.s-1");

        if (val > V_MAX)
            ui->lineEditSetVel->setText(QString::number(V_MAX, 'f', 2));
        else if (val < V_MIN)
            ui->lineEditSetVel->setText(QString::number(V_MIN, 'f', 2));
    }
    else
    {
        ui->labelSetCanId->setStyleSheet("color:blue;");
        ui->labelSetCanId->setText("角速度输入： " + arg1 + " rad.s-1");
    }
}

void MainWindow::on_lineEditSetVel_editingFinished()
{
    bool ok;
    float val = ui->lineEditSetVel->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.velSet = val;
    }
    else
    {
        uart2can.velSet = 0;
        ui->lineEditSetVel->setText("0");
    }

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("角速度设置： " + QString::number(uart2can.velSet, 'f', 4) + " rad.s-1");
}

void MainWindow::on_lineEditSetKd_textChanged(const QString &arg1)
{
    bool ok;

    float val = arg1.toFloat(&ok);

    if ((ok == false) || (val > KD_MAX) || (val < KD_MIN))
    {
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("角速度增益设置范围：" + QString::number(KD_MIN, 'f', 2) + "~" + QString::number(KD_MAX, 'f', 2) + " nm/rad.s-1");

        if (val > KD_MAX)
            ui->lineEditSetKd->setText(QString::number(KD_MAX, 'f', 2));
        else if (val < KD_MIN)
            ui->lineEditSetKd->setText(QString::number(KD_MIN, 'f', 2));
    }
    else
    {
        ui->labelSetCanId->setStyleSheet("color:blue;");
        ui->labelSetCanId->setText("角速度增益输入： " + arg1 + " nm/rad.s-1");
    }
}

void MainWindow::on_lineEditSetKd_editingFinished()
{
    bool ok;

    float val = ui->lineEditSetKd->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.kdSet = val;
    }
    else
    {
        uart2can.kdSet = 0;
        ui->lineEditSetKd->setText("0");
    }

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("角速度增益设置： " + QString::number(uart2can.kdSet, 'f', 4) + " nm/rad.s-1");
}

void MainWindow::on_lineEditSetTorque_textChanged(const QString &arg1)
{
    bool ok;

    float val = arg1.toFloat(&ok);

    if ((ok == false) || (val > T_MAX) || (val < T_MIN))
    {
        ui->labelSetCanId->setStyleSheet("color:red;");
        ui->labelSetCanId->setText("力矩设置范围：" + QString::number(T_MIN, 'f', 2) + "~" + QString::number(T_MAX, 'f', 2) + " nm");

        if (val > T_MAX)
            ui->lineEditSetTorque->setText(QString::number(T_MAX, 'f', 2));
        else if (val < T_MIN)
            ui->lineEditSetTorque->setText(QString::number(T_MIN, 'f', 2));
    }
    else
    {
        ui->labelSetCanId->setStyleSheet("color:blue;");
        ui->labelSetCanId->setText("力矩输入： " + arg1 + " nm");
    }
}

void MainWindow::on_lineEditSetTorque_editingFinished()
{
    bool ok;

    float val = ui->lineEditSetTorque->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.torqueSet = val;
    }
    else
    {
        uart2can.torqueSet = 0;
        ui->lineEditSetTorque->setText("0");
    }

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("力矩设置： " + QString::number(uart2can.torqueSet, 'f', 4) + " nm");
}

void MainWindow::on_pushButtonCfgSingle_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n单次控制--设备：" + ui->comboBoxDev->currentText());

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_MOTOR_CTRL_SINGLE);
}

void MainWindow::on_pushButtonCfgContinous_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n连续控制--设备：" + ui->comboBoxDev->currentText());

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_MOTOR_CTRL_CONTINUOUS);
}

void MainWindow::on_pushButtonCfgMotor_clicked() // 对应运行按钮
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n设置motor模式--设备：" + ui->comboBoxDev->currentText());

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_SET_MOTOR_MODE);
}

void MainWindow::on_pushButtonCfgBrake_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();

    uart2can.mcuBuf.usePos = (quint8)pos;

    ui->textEditCfgInfo->append("\r\n设置brake模式--设备：" + ui->comboBoxDev->currentText());

    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    uart2can.setCmd(STEP_SET_BRAKE_MODE);
}

void MainWindow::on_pushButtonRefreshCom_clicked()
{
    ui->comboBoxCom->clear();
    // find the available serial ports
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.description().contains("CH340"))
        {
            QSerialPort serial;
            serial.setPort(info);
            ui->comboBoxCom->addItem(info.portName()); // 显示端口名字
        }
    }
}

void MainWindow::on_comboBoxSelMode_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        ui->pushButtonCaliEncoder->setEnabled(false);
        ui->pushButtonRecoverFac->setEnabled(false);
        break;
    case 1:
        if (ui->pushButtonZeroMech->isEnabled())
            ui->pushButtonCaliEncoder->setEnabled(true);
        else
            ui->pushButtonCaliEncoder->setEnabled(false);

        if (ui->pushButtonReadPara->isEnabled())
            ui->pushButtonRecoverFac->setEnabled(true);
        else
            ui->pushButtonRecoverFac->setEnabled(false);

        break;
    default:
        break;
    }
}

void MainWindow::on_pushButtonSaveLog_clicked()
{
    if (isSaveLogOpen == false)
    {
        saveLogFileName.clear();
        saveLogFileName = QFileDialog::getSaveFileName(this, tr("日志保存"),
                                                       QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"), tr("Log File(*.log)"));

        if (saveLogFileName.isEmpty())
        {
            ui->labelOtaInfo->setStyleSheet("color:blue;");
            ui->labelOtaInfo->setText("未指定日志文件！");
        }
        else
        {
            saveLogFile.setFileName(saveLogFileName);

            if (saveLogFile.open(QIODevice::ReadWrite | QIODevice::Append))
            {
                saveLogFile.write(ui->textEditCfgInfo->toPlainText().toUtf8());
                ui->textEditCfgInfo->clear();
                isSaveLogOpen = true;
                ui->pushButtonSaveLog->setText("取消保存");
            }
            else
            {
                ui->labelOtaInfo->setStyleSheet("color:red;");
                ui->labelOtaInfo->setText("日志文件无效！");
            }
        }
    }
    else
    {
        saveLogFile.write(ui->textEditCfgInfo->toPlainText().toUtf8());
        ui->textEditCfgInfo->clear();
        saveLogFile.close();
        saveLogFileName.clear();
        isSaveLogOpen = false;
        ui->pushButtonSaveLog->setText("日志保存");
    }
}

void MainWindow::on_comboBoxDev_currentIndexChanged(int index)
{
    uart2can.mcuBuf.usePos = (quint8)index;
    paraTableClear(ui->tableWidgetPara);
    ui->pushButtonRefreshTable->setEnabled(true);
    ui->pushButtonReadPara->setEnabled(false);
    ui->pushButtonWritePara->setEnabled(false);
    ui->pushButtonExportExcel->setEnabled(false);
    ui->pushButtonRecoverFac->setEnabled(false);

    ui->labelParaInfo->clear();
}

void MainWindow::on_pushButtonRefreshTable_clicked()
{
    ui->tableWidgetPara->blockSignals(true);
    int pos = ui->comboBoxDev->currentIndex();

    ui->pushButtonRefreshTable->setEnabled(false);

    ui->labelParaInfo->setStyleSheet("color:green;");
    ui->labelParaInfo->setText("启动更新参数表！");

    uart2can.mcuBuf.usePos = (quint8)pos;
    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];

    uart2can.setCmd(STEP_ASK_PARA_STR_INFO);
}

void MainWindow::on_tableWidgetPara_cellChanged(int row, int column)
{
    if (column != PARA_TAB_EDIT_COLUMN)
    {
        return;
    }

    ui->tableWidgetPara->blockSignals(true);

    qDebug() << "change row:" << QString::number(row, 10) << " column:" << QString::number(column, 10);

    if ((row <= uart2can.paraInfo.posArea[WRITE_STR].end) && (row >= uart2can.paraInfo.posArea[WRITE_STR].start))
    {
        paraTableModifyStr(ui->tableWidgetPara, row - uart2can.paraInfo.posArea[WRITE_STR].start);
    }
    else if ((row <= uart2can.paraInfo.posArea[WRITE_PARA].end) && (row >= uart2can.paraInfo.posArea[WRITE_PARA].start))
    {
        paraTableModifyPara(ui->tableWidgetPara, row - uart2can.paraInfo.posArea[WRITE_PARA].start);
    }

    ui->tableWidgetPara->blockSignals(false);
}

// 读所有参数
void MainWindow::on_pushButtonReadPara_clicked()
{
    readParaAllFlag = true;

    ui->pushButtonReadPara->setEnabled(false);
    uart2can.setCmd(STEP_READ_PARA_STR_START);
}

// 写被修改的参数
void MainWindow::on_pushButtonWritePara_clicked()
{
    writeAllModyfiedFlag = true;

    ui->pushButtonWritePara->setEnabled(false);
    uart2can.setCmd(STEP_WRITE_PARA_STR_START);
}

void MainWindow::on_pushButtonRecoverFac_clicked()
{
    int pos = ui->comboBoxDev->currentIndex();
    uart2can.mcuBuf.usePos = (quint8)pos;
    uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
    ui->pushButtonRecoverFac->setEnabled(false);
    uart2can.setCmd(STEP_RECOVER_FAC);
}

void MainWindow::on_pushButtonExportExcel_clicked()
{
    QDateTime time;
    QString temp;
    time = QDateTime::currentDateTime();
    temp = time.toString("yyyy_MM_dd_hhmmss");

    ui->labelParaInfo->setStyleSheet("color:green;");
    ui->labelParaInfo->setText("文件导出中，请等待...");

    QString fileName = QFileDialog::getSaveFileName(this, tr("Excel file"), QString("./%1%2.xls").arg("mcu_").arg(temp), tr("Excel Files(*.xls)")); // 设置保存的文件名
    if (fileName != NULL)
    {
        QAxObject *excel = new QAxObject;
        if (excel->setControl("Excel.Application"))
        {
            excel->dynamicCall("SetVisible (bool Visible)", false);
            excel->setProperty("DisplayAlerts", false);
            QAxObject *workbooks = excel->querySubObject("WorkBooks");     // 获取工作簿集合
            workbooks->dynamicCall("Add");                                 // 新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook"); // 获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
            QAxObject *cell;

            /*添加Excel表头数据*/
            for (int i = 1; i <= ui->tableWidgetPara->columnCount(); i++)
            {
                cell = worksheet->querySubObject("Cells(int,int)", 1, i);
                cell->setProperty("RowHeight", 60);
                cell->setProperty("ColumnWidth", tabWeight[i - 1] / 8);
                cell->setProperty("HorizontalAlignment", -4108); // 左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                cell->setProperty("VerticalAlignment", -4108);   // 上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                cell->dynamicCall("SetValue(const QString&)", ui->tableWidgetPara->horizontalHeaderItem(i - 1)->data(0).toString());
            }

            /*将form列表中的数据依此保存到Excel文件中*/
            for (int j = 0; j < ui->tableWidgetPara->rowCount(); j++)
            {
                for (int k = 0; k < ui->tableWidgetPara->columnCount(); k++)
                {
                    cell = worksheet->querySubObject("Cells(int,int)", j + 2, k + 1);
                    cell->dynamicCall("SetValue(const QString&)", ui->tableWidgetPara->item(j, k)->text());
                }
            }

            /*将生成的Excel文件保存到指定目录下*/
            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName)); // 保存至fileName
            workbook->dynamicCall("Close()");                                                    // 关闭工作簿
            excel->dynamicCall("Quit()");                                                        // 关闭excel
            delete excel;
            excel = NULL;

            ui->labelParaInfo->setText("文件导出完成!");

            if (QMessageBox::question(NULL, QString::fromUtf8("完成"), QString::fromUtf8("文件已经导出，是否现在打开？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
    }
}

void MainWindow::on_pushButtonEchoSet_clicked()
{
    quint16 indexTab[CHANNAL_SEL_NUM];
    ui->pushButtonEchoStart->setEnabled(false);
    ui->pushButtonEchoStart->setText("启动");

    indexTab[0] = (quint16)ui->comboBoxCh1->currentIndex();
    indexTab[1] = (quint16)ui->comboBoxCh2->currentIndex();
    indexTab[2] = (quint16)ui->comboBoxCh3->currentIndex();
    indexTab[3] = (quint16)ui->comboBoxCh4->currentIndex();
    indexTab[4] = (quint16)ui->comboBoxCh5->currentIndex();
    indexTab[5] = (quint16)ui->comboBoxCh6->currentIndex();
    indexTab[6] = (quint16)ui->comboBoxCh7->currentIndex();
    indexTab[7] = (quint16)ui->comboBoxCh8->currentIndex();

    uart2can.chSelNum = 0;
    uart2can.chNeedByteTotal = 2;
    for (quint8 i = 0; i < CHANNAL_SEL_NUM; i++)
    {
        uart2can.selCh_index2code(indexTab[i], &uart2can.chSelCode[i], &uart2can.chTypeNeed[i]); // 发送的时候要根据不同类型做不同的处理
        if (uart2can.chSelCode[i] != 0)                                                          // 不在读写的参数范围内是零
        {
            uart2can.chSelIndex[uart2can.chSelNum] = i;
            uart2can.chNeedByteTotal += uart2can.paraByteNumTab[uart2can.chTypeNeed[uart2can.chSelIndex[uart2can.chSelNum]]];
            uart2can.chSelNum++;
        }
    }
    quint32 needFre = uart2can.chEchoFreHz * ((uart2can.chNeedByteTotal / 8) + ((uart2can.chNeedByteTotal % 8) ? 1 : 0));

    if (uart2can.isWaveFileOpen)
    {
        uart2can.waveFile.close();
        uart2can.isWaveFileOpen = false;
        ui->pushButtonSaveWave->setText("数据保存");
    }

    if (uart2can.chSelNum == 0)
    {
        ui->label_selChInfo->setStyleSheet("color:red;");
        ui->label_selChInfo->setText("没有通道有效，请配置通道!");
    }
    else if (needFre > uart2can.chEchoFreHzMax)
    {
        ui->label_selChInfo->setStyleSheet("color:red;");
        ui->label_selChInfo->setText("设置频率过高!");
    }
    else
    {
        uart2can.setCmd(STEP_SCOPE_SET_FRE);
    }
}

void MainWindow::on_pushButtonEchoStart_clicked()
{
    QString str = ui->pushButtonEchoStart->text();

    if (str == "启动")
    {
        ui->pushButtonEchoStart->setText("停止");
        ui->pushButtonEchoSet->setEnabled(false);  // 通道设置按钮失活
        ui->lineEditSetEchoFre->setEnabled(false); // 设置采样频率的文本文本况失活
        ui->comboBoxCh1->setEnabled(false);
        ui->comboBoxCh2->setEnabled(false);
        ui->comboBoxCh3->setEnabled(false);
        ui->comboBoxCh4->setEnabled(false);
        ui->comboBoxCh5->setEnabled(false);
        ui->comboBoxCh6->setEnabled(false);
        ui->comboBoxCh7->setEnabled(false);
        ui->comboBoxCh8->setEnabled(false);
        ui->label_selChInfo->setStyleSheet("color:green;"); // 标签的设置
        ui->label_selChInfo->setText("示波器开启！");

        disconnect(ui->waveShow, SIGNAL(mouseMove(QMouseEvent *)), this, SLOT(myMoveEvent(QMouseEvent *)));

        if (uart2can.Burst_mode == 1)
        {
            int pos = ui->comboBoxDev->currentIndex();

            uart2can.mcuBuf.usePos = (quint8)pos;

            ui->textEditCfgInfo->append("\r\n单次控制--设备：" + ui->comboBoxDev->currentText());

            uart2can.devCanId = uart2can.mcuBuf.canId[uart2can.mcuBuf.usePos];
            uart2can.setCmd(STEP_MOTOR_CTRL_SINGLE);
        }
        uart2can.setCmd(STEP_SCOPE_START); //
    }
    else
    {
        ui->pushButtonEchoStart->setEnabled(false);
        ui->pushButtonEchoStart->setText("启动");
        ui->pushButtonEchoSet->setEnabled(true);
        ui->lineEditSetEchoFre->setEnabled(true);
        ui->comboBoxCh1->setEnabled(true);
        ui->comboBoxCh2->setEnabled(true);
        ui->comboBoxCh3->setEnabled(true);
        ui->comboBoxCh4->setEnabled(true);
        ui->comboBoxCh5->setEnabled(true);
        ui->comboBoxCh6->setEnabled(true);
        ui->comboBoxCh7->setEnabled(true);
        ui->comboBoxCh8->setEnabled(true);

        connect(ui->waveShow, SIGNAL(mouseMove(QMouseEvent *)), this, SLOT(myMoveEvent(QMouseEvent *)));

        uart2can.setCmd(STEP_SCOPE_STOP);
    }
}

void MainWindow::on_lineEditSetEchoFre_textChanged(const QString &arg1)
{
    bool ok;

    int val = arg1.toInt(&ok); // 这里是类型转换从字符串转换成整型

    if ((ok == false) || (val > uart2can.chEchoFreHzMax) || (val < uart2can.chEchoFreHzMin))
    {
        ui->label_selChInfo->setStyleSheet("color:red;");
        ui->label_selChInfo->setText("输出频率设置范围：" + QString::number(uart2can.chEchoFreHzMin, 10) + "~" + QString::number(uart2can.chEchoFreHzMax, 10) + " HZ");

        ui->lineEditSetEchoFre->blockSignals(true);
        if (val > uart2can.chEchoFreHzMax)
        {
            ui->lineEditSetEchoFre->setText(QString::number(uart2can.chEchoFreHzMax, 10));
            uart2can.chEchoFreHz = uart2can.chEchoFreHzMax;
        }
        else if (val < uart2can.chEchoFreHzMin)
        {
            ui->lineEditSetEchoFre->setText(QString::number(uart2can.chEchoFreHzMin, 10));
            uart2can.chEchoFreHz = uart2can.chEchoFreHzMin;
        }
    }
    else
    {
        uart2can.chEchoFreHz = val;
        ui->label_selChInfo->setStyleSheet("color:green;");
        ui->label_selChInfo->setText("输出频率设置：" + QString::number(val, 10) + " HZ");
    }
    ui->lineEditSetEchoFre->blockSignals(false);
}

void MainWindow::on_pushButtonSaveWave_clicked()
{

    if (uart2can.isWaveFileOpen)
    {
        uart2can.waveFile.close();
        uart2can.isWaveFileOpen = false;
        ui->pushButtonSaveWave->setText("数据保存");
    }
    else
    {
        QDateTime time;
        QString temp;
        time = QDateTime::currentDateTime();       // 获取当前的时间和日期
        temp = time.toString("yyyy_MM_dd_hhmmss"); // 把时间格式转换成指定字符串格式

        QString fileName = QFileDialog::getSaveFileName(this, tr("Wave file"), QString("./%1%2.txt").arg("wave_").arg(temp), tr("TXT Files(*.txt)")); // 设置保存的文件名
        if (fileName != NULL)
        {
            uart2can.waveFile.setFileName(fileName);

            if (uart2can.waveFile.open(QIODevice::ReadWrite | QIODevice::Append))
            {
                uart2can.isWaveFileOpen = true;
                ui->pushButtonSaveWave->setText("停止保存");
            }
            else
            {
                ui->label_selChInfo->setStyleSheet("color:red;");
                ui->label_selChInfo->setText("数据保存文件无效！");
            }
        }
        else
        {
            ui->label_selChInfo->setStyleSheet("color:blue;");
            ui->label_selChInfo->setText("未指定数据保存文件！");
        }
    }
}

void MainWindow::on_comboBoxSelFrameType_currentIndexChanged(int index)
{
    //    if(ui->comboBoxSelFrameType->itemText(index) == "标准帧") {
    //        uart2can.canFrameType = FRAME_STD;
    //    } else {        /* 否则为扩展帧 */
    //        uart2can.canFrameType = FRAME_EXD;
    //    }
    switch (index)
    {
    case 0:
        uart2can.canFrameType = FRAME_STD;
        break; /* 标准帧 */
    case 1:
        uart2can.canFrameType = FRAME_EXD;
        break; /* 扩展帧 */
    }
}

//void MainWindow::myMoveEvent(QMouseEvent *e)
//{
//    int wave_num = ui->waveShow->xAxis->graphs().count();

//    /* 获取光标位置 */
//    int x_pos = e->pos().x();

//    /* 转化为坐标系位置 */
//    double xv = ui->waveShow->xAxis->pixelToCoord(x_pos);

//    QString str;
//    str += "x = " + QString::number(xv) + "s";

//    if (ui->checkBox_show_value->isChecked() == true)
//    {
//        /* 遍历两个图 */
//        for (int i = 0; i < wave_num; i++)
//        {
//            double y = 0;
//            int start = 0, end = ui->waveShow->graph(i)->data()->size(); //, cur = start;
//            /* 二分查找 */
//            if (ui->waveShow->graph(i)->data()->isEmpty() == false)
//            {
//                do
//                {
//                    if (xv <= ui->waveShow->graph(i)->data()->at((start + end) / 2)->key)
//                    {
//                        end = (start + end) / 2;
//                    }
//                    else
//                    {
//                        start = (start + end) / 2;
//                    }
//                    y = ui->waveShow->graph(i)->data()->at(end)->value;

//                } while (end - start > 1);

//                /* 显示文本 */
//                str += QString("\n通道%1 = %3").arg(i + 1).arg(QString::asprintf("%.3f", y));
//            }
//        }
//        if (str.length() <= 100)
//        {
//            QToolTip::showText(cursor().pos(), str, ui->waveShow);
//        }
//    }
//}
void MainWindow::myMoveEvent(QMouseEvent* e)
{
    // 获取鼠标在绘图区域内的坐标（考虑偏移量）
    QPoint mousePos = e->pos();
    double xv = ui->waveShow->xAxis->pixelToCoord(mousePos.x());
    QString str;
    str += "x = " + QString::number(xv, 'f', 3) + "s";

    if (ui->checkBox_show_value->isChecked())
    {
        int wave_num = ui->waveShow->graphCount();

        for (int i = 0; i < wave_num; i++)
        {
            QCPGraph* graph = ui->waveShow->graph(i);
            if (!graph || graph->data()->isEmpty())
                continue;

            // 查找最近数据点
            double y = 0;
            bool foundPoint = false;

            // 方法1：使用QCPDataContainer的findBegin（推荐）
            auto it = graph->data()->findBegin(xv, false);
            if (it != graph->data()->end())
            {
                // 检查前后两个点，找到最接近的点
                if (it != graph->data()->begin())
                {
                    auto prevIt = it - 1;
                    if (std::abs(prevIt->key - xv) < std::abs(it->key - xv))
                    {
                        y = prevIt->value;
                        foundPoint = true;
                    }
                }

                if (!foundPoint)
                {
                    y = it->value;
                    foundPoint = true;
                }
            }

            // 方法2：备用方法，如果上述方法无效
            if (!foundPoint)
            {
                // 实现一个更稳健的二分查找
                int size = graph->data()->size();
                int low = 0, high = size - 1;
                int idx = -1;

                while (low <= high)
                {
                    int mid = (low + high) / 2;
                    double midKey = graph->data()->at(mid)->key;

                    if (midKey < xv) {
                        low = mid + 1;
                    } else if (midKey > xv) {
                        high = mid - 1;
                    } else {
                        idx = mid;
                        break;
                    }
                }

                if (idx == -1) idx = low; // 如果没有精确匹配，取第一个大于xv的点

                // 确保索引有效
                if (idx >= 0 && idx < size)
                {
                    // 比较前后点找到最接近的
                    if (idx > 0 &&
                        std::abs(graph->data()->at(idx-1)->key - xv) <
                        std::abs(graph->data()->at(idx)->key - xv))
                    {
                        y = graph->data()->at(idx-1)->value;
                    }
                    else
                    {
                        y = graph->data()->at(idx)->value;
                    }
                    foundPoint = true;
                }
            }

            if (foundPoint)
            {
                str += QString("\n通道%1 = %2").arg(i + 1).arg(y, 0, 'f', 3);
            }
        }

        // 显示提示（移除长度限制或增加限制值）
        if (!str.isEmpty())
        {
            QToolTip::showText(e->globalPos(), str, ui->waveShow);
        }
    }

}

void MainWindow::on_pushButton_clicked()
{
    ui->waveShow->rescaleAxes();
    ui->waveShow->replot();
}

void MainWindow::on_combox_motor_ID_currentIndexChanged(int index)
{
    index = ui->combox_motor_ID->currentIndex();
    if(ui->combox_motor_ID->currentText() == "3")
    {

        P_MIN = -12.5f;
        P_MAX = 12.5f;
        V_MIN = -25.0f;
        V_MAX = 25.0f;
        KP_MIN = 0.0f;
        KP_MAX = 500.0f;
        KD_MIN = 0.0f;
        KD_MAX = 5.0f;
        T_MIN = -525.0f;
        T_MAX = 525.0f;
        is_waterproof_motor = false;
    }
    else if (ui->combox_motor_ID->currentText() == "4")
    {
        P_MIN = -12.5f;
        P_MAX = 12.5f;
        V_MIN = -11.4f;
        V_MAX = 11.4f;
        KP_MIN = 0.0f;
        KP_MAX = 500.0f;
        KD_MIN = 0.0f;
        KD_MAX = 5.0f;
        T_MIN = -525.0f;
        T_MAX = 525.0f;
        is_waterproof_motor = false;
    }
    else if (ui->combox_motor_ID->currentText() == "D60")
    {
        P_MIN = -3.14f;
        P_MAX = 3.14f;
        V_MIN = -10.0f;
        V_MAX = 10.0f;
        KP_MIN = 0.0f;
        KP_MAX = 1500.0f;
        KD_MIN = 0.0f;
        KD_MAX = 200.0f;
        T_MIN = -30.0f;
        T_MAX = 30.0f;
        is_waterproof_motor = false;
    }
    else if (ui->combox_motor_ID->currentText() == "D120")
    {
        P_MIN = -3.14f;
        P_MAX = 3.14f;
        V_MIN = -10.0f;
        V_MAX = 10.0f;
        KP_MIN = 0.0f;
        KP_MAX = 1500.0f;
        KD_MIN = 0.0f;
        KD_MAX = 200.0f;
        T_MIN = -250.0f;
        T_MAX = 250.0f;
        is_waterproof_motor = false;
    }
    else if (ui->combox_motor_ID->currentText() == "自定义")
    {
        P_MIN = P_MIN_Tmp;
        P_MAX = P_MAX_Tmp;
        V_MIN = V_MIN_Tmp;
        V_MAX = V_MAX_Tmp;
        KP_MIN = KP_MIN_Tmp;
        KP_MAX = KP_MAX_Tmp;
        KD_MIN = KD_MIN_Tmp;
        KD_MAX = KD_MAX_Tmp;
        T_MIN = T_MIN_Tmp;
        T_MAX = T_MAX_Tmp;
        is_waterproof_motor = false;
    }
    else if (ui->combox_motor_ID->currentText() == "RZ2L电机")
    {
        P_MIN = -12.5f;
        P_MAX = 12.5f;
        V_MIN = -45.0f;
        V_MAX = 45.0f;
        KP_MIN = 0.0f;
        KP_MAX = 255.0f;
        KD_MIN = 0.0f;
        KD_MAX = 255.0f;
        KI_MIN = 0.0f;
        KI_MAX = 255.0f;
        T_MIN = -3276.0f;
        T_MAX = 3276.0f;
        is_waterproof_motor = true;
    }
    RefrechData();
    qDebug()<<"Index"<< index <<": "<< ui->combox_motor_ID->currentText() << "P_MIN" << P_MIN;
}
void MainWindow::RefrechData()
{
    bool ModeSelectFlag = false;
    ModeSelectFlag = (ui->combox_motor_ID->currentText() == "自定义") ? true : false;
    if(ModeSelectFlag) // 自定义模式
    {
        P_MIN = P_MIN_Tmp;
        P_MAX = P_MAX_Tmp;
        V_MIN = V_MIN_Tmp;
        V_MAX = V_MAX_Tmp;
        KP_MIN = KP_MIN_Tmp;
        KP_MAX = KP_MAX_Tmp;
        KD_MIN = KD_MIN_Tmp;
        KD_MAX = KD_MAX_Tmp;
        T_MIN = T_MIN_Tmp;
        T_MAX = T_MAX_Tmp;
    }
    else
    {
        ui->lineEditSetP_max->setText(QString::number(P_MAX, 'f', 2));
        ui->lineEditSetP_min->setText(QString::number(P_MIN, 'f', 2));
        ui->lineEditSetV_max->setText(QString::number(V_MAX, 'f', 1));
        ui->lineEditSetV_min->setText(QString::number(V_MIN, 'f', 1));
        ui->lineEditSetT_max->setText(QString::number(T_MAX, 'f', 1));
        ui->lineEditSetT_min->setText(QString::number(T_MIN, 'f', 1));
        ui->lineEditSetKP_min->setText(QString::number(KP_MIN, 'f', 1));
        ui->lineEditSetKP_max->setText(QString::number(KP_MAX, 'f', 1));
        ui->lineEditSetKD_min->setText(QString::number(KD_MIN, 'f', 1));
        ui->lineEditSetKD_max->setText(QString::number(KD_MAX, 'f', 1));
    }
    // 设置是否可编辑
    ui->lineEditSetP_max->setEnabled(ModeSelectFlag);
    ui->lineEditSetP_min->setEnabled(ModeSelectFlag);
    ui->lineEditSetV_max->setEnabled(ModeSelectFlag);
    ui->lineEditSetV_min->setEnabled(ModeSelectFlag);
    ui->lineEditSetT_max->setEnabled(ModeSelectFlag);
    ui->lineEditSetT_min->setEnabled(ModeSelectFlag);
    ui->lineEditSetKP_min->setEnabled(ModeSelectFlag);
    ui->lineEditSetKP_max->setEnabled(ModeSelectFlag);
    ui->lineEditSetKD_min->setEnabled(ModeSelectFlag);
    ui->lineEditSetKD_max->setEnabled(ModeSelectFlag);
    // 设置是否可读写
    ui->lineEditSetP_max->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetP_min->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetV_max->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetV_min->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetT_max->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetT_min->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetKP_min->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetKP_max->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetKD_min->setReadOnly(!ModeSelectFlag);
    ui->lineEditSetKD_max->setReadOnly(!ModeSelectFlag);

    if(is_waterproof_motor){
        ui->label_Ki->setVisible(true);
        ui->label_mot_mode->setVisible(true);
        ui->comboBox_mot_mode->setVisible(true);
        ui->lineEditSetKi->setVisible(true);
    }
    else{
        ui->label_Ki->setVisible(false);
        ui->label_mot_mode->setVisible(false);
        ui->comboBox_mot_mode->setVisible(false);
        ui->lineEditSetKi->setVisible(false);
    }

}
void MainWindow::on_lineEditSetP_max_textChanged(const QString &arg1)
{
    P_MAX_Tmp = arg1.toFloat();
    RefrechData();
    qDebug()<< "P_MAX is " << P_MAX;
}

void MainWindow::on_lineEditSetP_min_textChanged(const QString &arg1)
{
    P_MIN_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetV_max_textChanged(const QString &arg1)
{
    V_MAX_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetV_min_textChanged(const QString &arg1)
{
    V_MIN_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetT_max_textChanged(const QString &arg1)
{
    T_MAX_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetT_min_textChanged(const QString &arg1)
{
    T_MIN_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetKP_max_textChanged(const QString &arg1)
{
    KP_MAX_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetKP_min_textChanged(const QString &arg1)
{
    KP_MIN_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetKD_max_textChanged(const QString &arg1)
{
    KD_MAX_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_lineEditSetKD_min_textChanged(const QString &arg1)
{
    KD_MIN_Tmp = arg1.toFloat();
    RefrechData();
}

void MainWindow::on_comboBox_mot_mode_currentIndexChanged(int index)
{
    index = ui->comboBox_mot_mode->currentIndex();
    if(ui->comboBox_mot_mode->currentText() == "200 复位")
    {
        motor_mode_set = MOTOR_RESET_200;
    }
    else if (ui->comboBox_mot_mode->currentText() == "3 PARALL高编")
    {
        motor_mode_set = MOTOR_PARALL_CLOSE_LOOP_HIGHSIDE_3;
    }
    else if (ui->comboBox_mot_mode->currentText() == "4 CASCADE高编")
    {
        motor_mode_set = MOTOR_CASCADE_CLOSE_LOOP_HIGHSIDE_4;
    }
    else if (ui->comboBox_mot_mode->currentText() == "248 扭矩步进")
    {
        motor_mode_set = MOTOR_TORQU_STEP_248;
    }
    else if (ui->comboBox_mot_mode->currentText() == "253 开环转")
    {
        motor_mode_set = MOTOR_OPENLOOP_253;
    }
    else if (ui->comboBox_mot_mode->currentText() == "17 电机扫频")
    {
        motor_mode_set = MOTOR_FREQUENCY_SWEEP_17 ;
    }

}


void MainWindow::on_lineEditSetKi_editingFinished()
{
    bool ok;

    float val = ui->lineEditSetKi->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.kiSet = val;
    }
    else
    {
        uart2can.kiSet = 0;
        ui->lineEditSetKi->setText("0");
    }

    ui->labelSetCanId->setStyleSheet("color:green;");
    ui->labelSetCanId->setText("积分增益设置： " + QString::number(uart2can.kiSet, 'f', 4) + " ");
}


void MainWindow::on_lineEditSetSelfMode_textChanged(const QString &arg1)
{
    bool ok;

    float val = arg1.toFloat(&ok);

    if ((ok == false) || (val > 255) || (val < 0))
    {
        if (val > 255)
            ui->lineEditSetSelfMode->setText(QString::number(255));
        else if (val < 0)
            ui->lineEditSetSelfMode->setText(QString::number(0));
    }

}

void MainWindow::on_lineEditSetSelfMode_editingFinished()
{
    bool ok;

    float val = ui->lineEditSetSelfMode->text().toFloat(&ok);

    if (ok == true)
    {
        uart2can.SelfMode = val;
    }
    else
    {
        uart2can.SelfMode = 0;
        ui->lineEditSetSelfMode->setText("0");
    }
}
