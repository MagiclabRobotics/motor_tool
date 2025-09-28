#ifndef OTA_THREAD_H
#define OTA_THREAD_H

#include <QDebug>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <qmutex.h>
#include "qcustomplot.h"
// #include "wavedisplay.h"

#define STR_NAME_LEN_MAX (18) // 名称字符串最大长度
#define STR_NAME_LEN (16)     // 名称字符串长度

#define WAVE_TIME_WINDOW (10) // S 示波器时间窗口
#define WAVE_SHOW_FRE (20)    // HZ 示波器显示频率

#define CHANNAL_SEL_NUM (8) // 示波器通道数量

#define MOT_ID_MAX (32)      // 电机ID最大值
#define SCOPE_FRE_MAX (1000) // 示波器最大频率

#define REC_HEAD1 (0x5a) // 接收串口数据帧头1
#define REC_HEAD2 (0xa5) // 接收串口数据帧头2

// 故障码
#define ENC_ERROR_HS 1          // 高速端编码器无数据；
#define ENC_ERROR_LS 2          // 低速端编码器无数据；
#define ERROR_OC 3              // 软件过流(过载)；
#define ERROR_OV 4              // 实际速度超速；
#define ERROR_hard 5            // 硬件drv8353故障；
#define ERROR_HVolt 6           // 软件过压；
#define ERROR_LVolt 7           // 软件欠压；
#define ERROR_CALI 8            // 编码器标定失败；
#define ERROR_POSREF 9          // 位置指令异常超速；
#define ERROR_OT 10             // MTU3中断持续时间超时；
#define ERROR_LACKPHASE 11      // 缺相故障；
#define ERROR_SHORTGND 12       // 接地短路故障；
#define ERROR_E2PROM 13         // E2PROM读写故障；
#define ERROR_IMAX 14           // IMAX值异常故障；
#define ERROR_VMAX 15           // VMAX值异常故障；
#define ERROR_Motor_OverTemp 16 // 电机过温故障；
#define ERROR_Mos_OverTemp 17   // MOS过温故障；
#define ERROR_Servo_Invalid 18  // SERVO状态切换异常

enum canComStep
{
    STEP_STOP,                  // 所有任务结束后的停止模式
    STEP_IN_OTA,                // OTA - 进入OTA模式
    STEP_SEND_INFO,             // OTA - 发送升级文件信息
    STEP_SEND_BIN,              // OTA - 发送升级文件
    STEP_SEND_END,              // OTA - 发送升级文件结束
    STEP_GET_DEV_INFO,          // 扫描设备，获取信息
    STEP_SET_CAN_ID,            // 设置设备ID指令
    STEP_SET_RST_MODE,          // 设置复位模式
    STEP_SET_MOTOR_MODE,        // 设置电机模式
    STEP_SET_BRAKE_MODE,        // 设置刹车模式
    STEP_SET_CALI_ENCODER_MODE, // 设置编码器标定模式
    STEP_SET_ZERO_MECH_MODE,    // 设置机械零位模式
    STEP_MOTOR_CTRL_SINGLE,     // 电机控制参数下发-单次
    STEP_MOTOR_CTRL_CONTINUOUS, // 电机控制参数下发-连续
    STEP_ASK_PARA_STR_INFO,     // 参数 - 询问参数字符串信息
    STEP_READ_PARA_STR_START,   // 参数 - 开始读取参数字符串
    STEP_READ_PARA_STR,         // 参数 - 按序号读取参数字符串
    STEP_WRITE_PARA_STR_START,  // 参数 - 开始写入参数字符串
    STEP_WRITE_PARA_STR,        // 参数 - 按序号写入参数字符串
    STEP_SAVE_PARA_STR,         // 参数 - 保存参数指令
    STEP_RECOVER_FAC,           // 恢复出厂设置
    STEP_SCOPE_SET_FRE,         // 示波器 - 设置频率及要使用的通道
    STEP_SCOPE_SET_CHANNAL,     // 示波器 - 设置通道 - 已弃用
    STEP_SCOPE_START,           // 示波器 - 开始示波
    STEP_SCOPE_STOP,            // 示波器 - 停止示波
    STEP_SCOPE_SET_MODE,        // 示波器 - 设置模式 - 未使用
    STEP_SCOPE_CONFIG,          // 示波器 - 检测配置指令是否超时
    STEP_SET_CUSTOME_CMD,       // 下发自定义指令
};

// enum canComMode
// {
//     CANCOM_ANNOUNCE_DEVID = 0, // 通告设备ID
//     CANCOM_MOTOR_CTRL,     // MOTOR-电机控制
//     CANCOM_MOTOR_FEEDBACK, // MOTOR-电机反馈
//     CANCOM_MOTOR_IN,       // MOTOR-进入电机模式
//     CANCOM_MOTOR_RESET,    // MOTOR-复位模式
//     CANCOM_MOTOR_CALI,     // MOTOR-高速编码器标定
//     CANCOM_MOTOR_ZERO,     // MOTOR-设置机械零位
//     CANCOM_MOTOR_ID,       // MOTOR-设置ID
//     CANCOM_PARA_WRITE,     // 参数-写入
//     CANCOM_PARA_READ,      // 参数-读取
//     CANCOM_PARA_UPDATE,    // 参数-更新上传
//     CANCOM_OTA_START,      // OTA-启动
//     CANCOM_OTA_INFO,       // OTA-升级文件描述
//     CANCOM_OTA_ING,        // OTA-升级中
//     CANCOM_OTA_END,        // OTA-升级完成
//     CANCOM_CALI_ING,       // 编码器标定中
//     CANCOM_CALI_RST,       // 编码器标定结果
//     CANCOM_HALLCALI_ING,   // 低速HALL标定中
//     CANCOM_HALLCALI_RST,   // 低速HALL标定结果
//     CANCOM_PARA_STR_INFO,  // 参数-字符串信息
//     CANCOM_MOTOR_BRAKE,    // MOTOR-进入刹车模式
//     CANCOM_FAULT_WARN,     // 故障和警告信息
//     CANCOM_SCOPE_SET_MODE, // 示波器设置模式
//     CANCOM_MODE_TOTAL, // 未开发功能
// };

enum motorMode
{
    MT_MODE_RESET = 0,
    MT_MODE_CALI,
    MT_MODE_MOTOR,
    MT_MODE_BRAKE,
};

struct motorStatus
{
    bool underVoltFault;
    bool overCurFault;
    bool overTempFault;
    bool encoderFault;
    bool hallFault;
    bool noCaliFault;
    enum motorMode mode;
};

// struct canPack
// {
//     struct exCanIdInfo exId;
//     quint8 len;
//     quint8 data[8];
// };

union typeConvert
{
    double d;
    float f[2];
    qint64 i64;
    quint64 u64;
    qint32 i32[2];
    quint32 u32[2];
    qint16 i16[4];
    quint16 u16[4];
    qint8 i8[8];
    quint8 u8[8];
};

/******************************************************************************
 * @brief 串口数据发送结构体
 *
 *****************************************************************************/
struct uartPack
{
    quint8 frame_id;
    quint8 mot_id;
    quint8 len;
    quint8 data[128];
};

enum paraStrItemType
{
    WRITE_STR = 0,
    READ_STR,
    WRITE_PARA,
    READ_PARA,
    PARA_STR_TOTAL,
};

enum canParaItemType
{
    PARA_NAME0 = 0,
    PARA_NAME1,
    PARA_NAME2,
    PARA_ATTR,
    PARA_VAL0,
    PARA_VAL1,
    PARA_STR0,
    PARA_STR1,
    PARA_STR2,
    PARA_DONE,
};

#define HALL_FAULT_MAX (1 << 0)    // 故障：极大值小于1024
#define HALL_FAULT_MIN (1 << 1)    // 故障：极小值大于0
#define HALL_FAULT_POS (1 << 2)    // 故障：定位点
#define HALL_FAULT_VAL (1 << 3)    // 故障：HALL极大值过低
#define HALL_FAULT_NOCALI (1 << 7) // 故障：HALL未标定
#define HALL_WARN_LINEAR (1 << 0)  // 警告：非单调/线性化
#define HALL_WARN_DMAX (1 << 1)    // 警告：差值极大值超限
#define HALL_WARN_DMIN (1 << 2)    // 警告：差值极小值超限

struct hallTabInfoType
{
    qint16 maxVal;
    qint16 minVal;
    quint16 midPos;
    quint16 maxPos;
    quint16 minPos;
    quint8 noLinearCount;
    quint8 fault : 4;
    quint8 warn : 4;
};

enum otaStatus
{
    OTA_STA_DETECT_MCU = 0,
    OTA_STA_IN_OTA_MODE_OVER_TIME,
    OTA_STA_IN_OTA_MODE_SUCCESS,
    OTA_STA_BINFILE_EMPTY,
    OTA_STA_BINFILE_SIZE_OVER_LIMIT,
    OTA_STA_OTA_INFO_SUCCESS,
    OTA_STA_OTA_INFO_OVER_TIME,
    OTA_STA_SEND_BIN_OK,
    OTA_STA_SEND_BIN_RETRY,
    OTA_STA_SEND_BIN_DONE,
    OTA_STA_SET_ID_SUCCESS,
    OTA_STA_SET_ID_OVER_TIME,
    OTA_STA_RST_MODE_SUCCESS,
    OTA_STA_RST_MODE_OVER_TIME,
    OTA_STA_MOTOR_MODE_SUCCESS,
    OTA_STA_MOTOR_MODE_OVER_TIME,
    OTA_STA_BRAKE_MODE_SUCCESS,
    OTA_STA_BRAKE_MODE_OVER_TIME,
    OTA_STA_CALI_MODE_SUCCESS,
    OTA_STA_CALI_MODE_OVER_TIME,
    OTA_STA_ZERO_MODE_SUCCESS,
    OTA_STA_ZERO_MODE_OVER_TIME,

    OTA_STA_MOTOR_FEEDBACK,
    OTA_STA_MOTOR_FAULTWARN,

    OTA_STA_ENCODER_CALI_ING,
    OTA_STA_ENCODER_CALI_RESULT,
    OTA_STA_ENCODER_CALI_DONE,
    OTA_STA_HALL_CALI_ING,
    OTA_STA_HALL_CALI_RESULT1,
    OTA_STA_HALL_CALI_RESULT2,
    OTA_STA_HALL_CALI_DONE,

    OTA_STA_GET_PARA_INFO_SUCCESS,
    OTA_STA_GET_PARA_INFO_OVER_TIME,
    OTA_STA_GET_PARA_INFO_ERROR,

    OTA_STA_READ_PARA_SUCCESS,
    OTA_STA_READ_PARA_OVER_TIME,
    OTA_STA_READ_PARA_ERROR,

    OTA_STA_WRITE_PARA_SUCCESS,
    OTA_STA_WRITE_PARA_OVER_TIME,
    OTA_STA_WRITE_PARA_ERROR,
    OTA_STA_SAVE_PARA_SUCCESS,

    OTA_STA_PARA_UPDATE_ERROR,
    OTA_STA_PARA_UPDATE_OVER_TIME,
    OTA_STA_PARA_UPDATE_SUCCESS_FRESET,
    OTA_STA_PARA_UPDATE_SUCCESS_CHSET,
    OTA_STA_PARA_UPDATE_SUCCESS_START,
    OTA_STA_PARA_UPDATE_SUCCESS_END,

    OTA_STA_SUCCESS,
    OTA_STA_FAILED,

    OTA_STA_GET_CUSTOME_STR,
    OTA_STA_SCOPE_CONTINUE_CONFLICT,
};

enum paraType
{
    ParaType_U8 = 0,
    ParaType_S8,
    ParaType_U16,
    ParaType_S16,
    ParaType_U32,
    ParaType_S32,
    ParaType_FLOAT,
    ParaType_U64,
    ParaType_S64,
    ParaType_DOUBLE,
    ParaType_STRING,
    ParaType_TOTAL,
};

enum paraWriteErrorCode
{
    PARA_WRITE_ERROR_NONE = 0,           // 修改/保存 成功
    PARA_WRITE_ERROR_NO_CODE,            // 修改/保存失败-无此编号
    PARA_WRITE_ERROR_FORBID_MODIFY,      // 修改/保存失败-不可更改
    PARA_WRITE_ERROR_TYTE,               // 修改/保存失败-类型不匹配
    PARA_WRITE_ERROR_LOSS_FRAME,         // 修改/保存失败-漏帧
    PARA_WRITE_ERROR_FRAME,              // 修改/保存失败-错帧
    PARA_WRITE_ERROR_LIMIT,              // 修改/保存失败-设置数据超范围
    PARA_WRITE_ERROR_MODE_FORBID_MODIFY, // 修改失败-当前模式下不可修改
    PARA_WRITE_ERROR_MODE_FORBID_SAVE,   // 保存失败-当前模式下不可保存-WRITE_CMD_MODIFY_SAVE
    PARA_WRITE_ERROR_SAVE,               // 保存失败-WRITE_CMD_SAVE_ONLY
    PARA_WRITE_ERROR_NO_CMD,             // 无此命令
    PARA_WRITE_ERROR_TOTAL,
};

enum UartFrameTpye
{
    UART_FRAME_ANNOUNCE_DEVID = 0, // 通告设备ID
    UART_FRAME_MOTOR_RESET,        // MOTOR-复位模式
    UART_FRAME_MOTOR_STATE,        // MOTOR-运行状态
    UART_FRAME_MOTOR_BRAKE,        // MOTOR-刹车模式
    UART_FRAME_MOTOR_ZERO_MECH,    // MOTOR-设置机械零位
    UART_FRAME_PARA_STR_INFO,      // 参数-字符串信息
    UART_FRAME_PARA_NUM_INFO,      // 参数-数据信息
    UART_FRAME_PARA_DONE,          // 参数-传输完成
    UART_FRAME_PARA_UPDATE_FRE,    // 参数-设置频率
    UART_FRAME_PARA_UPDATE_CHA,    // 参数-设置通道
    UART_FRAME_PARA_UPDATE_START,  // 参数-开始传输
    UART_FRAME_PARA_UPDATE_STOP,   // 参数-停止传输
    UART_FRAME_CTRL_SET_MODE,      // 设置控制模式
    UART_FRAME_CTRL_SET_PARA,      // 设置控制参数
    UART_FRAME_PARA_READ,          // 读参数
    UART_FRAME_PARA_WRITE,         // 写参数
    UART_FRAME_PARA_SAVE,          // 写参数
    UART_FRAME_RECOVER_FAC,        // 恢复出厂设置
    UART_FRAME_CALI_GET,           // 得到标定指令
    UART_FRAME_CALI_BEGIN,         // 标定开始
    UART_FRAME_CALI_ING,           // 标定中
    UART_FRAME_CALI_RST,           // 标定结果
    UART_FRAME_CALI_DONE,          // 标定结果
    UART_FRAME_CUSTOME_STR,        // 自定义字符串
    UART_FRAME_CUSTOME_DATA,       // 自定义数值
    UART_FRAME_CUSTOME_CMD,        // 自定义指令
};

// 字符串要显示的位置
enum StrPlaceTpye
{
    STR_PALCE_NONE = 0, // 无
    STR_PALCE_PARA_OK,  // 参数相关区域
    STR_PALCE_PARA_ERR, // 参数相关区域
};

#define MCU_NUM (14)

struct mcuInfo
{
    quint64 id[MCU_NUM];   // 节点唯一ID
    quint8 canId[MCU_NUM]; // 目标节点号
    quint8 point;          // 已检查到节点的总数
    quint8 usePos;         // 当前使用的节点号
    bool changeFlag;       // 是否检测到新节点号
};

#define SET_PARA_NUM (256)
struct setParaInfo
{
    QString name;
    QString valStr;
    float max;
    float min;
    quint8 prec;
    quint8 valR[8];
    quint8 valM[8];
    enum paraType type;
    bool modifyFlag;
    quint8 wPerm;
};

#define STATUS_PARA_NUM (256)
struct statusParaInfo
{
    QString name;
    quint8 valR[8];
    enum paraType type;
};

#define READ_STRING_NUM (32)
#define WRITE_STRING_NUM (32)
struct strWInfo
{
    QString name;
    QString str;
    char strEdit[STR_NAME_LEN_MAX];
    bool modifyFlag;
};
struct strRInfo
{
    QString name;
    QString str;
};

struct posAreaType
{
    quint16 start;
    quint16 end;
};

struct paraInfoType
{
    struct setParaInfo writePara[SET_PARA_NUM];
    struct statusParaInfo readPara[STATUS_PARA_NUM];
    struct strRInfo readStr[READ_STRING_NUM];
    struct strWInfo writeStr[WRITE_STRING_NUM];
    quint16 numTab[PARA_STR_TOTAL];
    struct posAreaType posArea[PARA_STR_TOTAL];

    enum paraStrItemType itemIndex; // 参数 - 类型
    quint16 posIndex;               // 参数 - 序号

    quint16 codeNeed;       // 参数 - 组合编号
    enum paraType typeNeed; // 参数 - 数据类型 校验用

    enum paraWriteErrorCode writeErrorCode; // 错误码

    quint8 echoNum;     // 指定类型参数数量
    quint8 meaTimerNum; // 指定类型参数数量
};

class otaThread : public QThread
{
    Q_OBJECT
public:
    otaThread();

    void setCmd(enum canComStep mode);

    void selCh_index2code(quint16 index, quint16 *pCode, enum paraType *pType);

    void delay_ms(uint ms);

    enum otaStatus otaStatus; // 用于记录希望在界面上显示的状态

    struct paraInfoType paraInfo; // 参数表信息
    quint32 paraErrorCode;        // 错误码
    quint32 err_cnt;              // 连续错误次数
    quint32 unAckedCnt;           // 未收到应答次数
    bool ScopeMode;               // 是否位于示波器模式
    bool ContinueMode;            // 是否连续发送模式

    typeConvert tc;    // 用于格式转换
    QByteArray RxData; // 保存串口接收到的数据

    QByteArray BinData; // 保存升级文件数据
    quint32 binSize;    // 升级文件大小
    quint32 PackNumber; // 升级文件包数
    quint16 PackCnt;    // 当前发送的包数

    quint64 mcuId;   // 检测到的节点唯一ID
    quint8 mcuCanId; // 接收到的电机ID
    quint8 devCanId; // 下发指令时使用的目标id

    quint8 configCanId; // 要将电机设置到的目标ID
    quint8 scanId;      // 扫描电机时使用的临时ID
    quint8 MotCtrlMode; // 电机控制模式

    struct mcuInfo mcuBuf; // 保存检测到的节点信息

    // 霍尔数据显示时使用变量
    struct hallTabInfoType hallInfo[6];
    qint16 hallRaw[6];
    qint16 hallRdLval;
    quint8 hallError;
    quint8 hallWarn;
    quint16 hallCount;

    // 电机控制参数
    float pos_tarSet;
    float pos_kpSet;
    float pos_kiSet;
    float vel_tarSet;
    float vel_kpSet;
    float vel_kiSet;
    float tor_tarSet;
    float tor_kpSet;
    float tor_kiSet;

    // 电机标定电角度时使用数据
    quint16 caliI;
    quint16 caliInd;
    float caliLut;
    qint32 caliRaw;
    float currentAngle;
    float rotorAngle;
    float caliOffset;
    float variance; // 均方差
    uint32_t calerrorcode;

    quint32 faultSta; // 电机故障状态
    quint32 warnSta;  // 电机警告状态

    // 电机反馈的状态信息
    float posGet;
    float velGet;
    float torqueGet;
    quint16 modeGet;
    quint8 faultGet;
    struct motorStatus mtStatus; // 电机状态

    QString custome_str;                // 自定义字符串，用于界面显示
    enum StrPlaceTpye custome_strPlace; // 自定义字符串显示位置

    quint8 cust_cmd;        // 自定义指令下发
    float cust_cmd_para[8]; // 自定义指令参数

    float chTickUs;                     // 示波器时间间隔
    quint8 chSelNum;                    // 被选中的通道数量
    quint8 Burst_mode;                  // 不知道
    quint16 chEchoFreHz;                // 示波器频率
    quint16 chEchoFreHzMax;             // 示波器最大频率
    quint16 chEchoFreHzMin;             // 示波器最小频率
    quint16 chEchoDevFreMax;            // 电机支持的示波器最大频率
    quint16 chEchoDevFreMin;            // 电机支持的示波器最小频率
    quint8 chSelIndex[CHANNAL_SEL_NUM]; // 标记是第几个被选中的通道，用于排除 chSelCode 中的无效数据
    quint16 chSelCode[CHANNAL_SEL_NUM]; // 记录根据选框所选通道的编号
    quint8 chRxdByte[128];              // 用于向示波器函数中传递数据
    quint16 chTickBak;                  // 示波器时间间隔备份
    quint16 chNeedByteTotal;            // 所有通道需要的总字节数
    double chTimeStamp;                 // 示波器时间戳
    // double  chParaTab[CHANNAL_SEL_NUM];
    enum paraType chTypeNeed[CHANNAL_SEL_NUM];
    enum paraType chTypeDev[CHANNAL_SEL_NUM]; // 电机反馈数据类型
    bool scopeIngFlag;                        // 示波器是否正在工作

    quint32 scopeShowNum;
    quint32 scopeShowCount;
    quint32 scopeWindowNum;
    quint32 scopeWindowCount;
    bool scopeWindowRemoveFlag;

    QVector<QCPGraphData> mData[CHANNAL_SEL_NUM]; // 示波器数据

    const static qint8 paraByteNumTab[ParaType_TOTAL];

    QFile waveFile;      // 示波器数据保存文件
    bool isWaveFileOpen; // 示波器数据保存文件是否打开

protected:
    void run();

signals:
    void sendState(); // emit when each packge send done
    void WriteToSerial(QByteArray, qint16);
    void scopeToShow(QVector<QCPGraphData> *);

private slots:
    void getSerialData(QByteArray);

private:
    int float_to_uint(float x, float x_min, float x_max, int bits);
    float uint_to_float(int x_int, float x_min, float x_max, int bits);
    void analysisRxdDatas(QByteArray);

    void rxdPack_CUSTOME_STR(QByteArray datas);
    void rxdPack_CUSTOME_DAT(QByteArray datas);
    void rxdPack_ANNOUNCE_DEVID(QByteArray datas);
    void rxdPack_MOTOR_RESET(QByteArray datas);
    void rxdPack_MOTOR_BRAKE(QByteArray datas);
    void rxdPack_MOTOR_Custom_Cmd(QByteArray datas);
    void rxdPack_MOTOR_Zero_Mech(QByteArray datas);
    void rxdPack_MOTOR_CTRL_SET(QByteArray datas);
    void rxdPack_MOTOR_CTRL_PARA(QByteArray datas);
    void rxdPack_MOTOR_PARA_SAVE(QByteArray datas);
    void rxdPack_PARA_STR_INFO(QByteArray datas);
    void rxdPack_PARA_DATA_INFO(QByteArray datas);
    void rxdPack_PARA_INFO_DONE(QByteArray datas);
    void rxdPack_PARA_UPDATE_FRE(QByteArray datas);
    void rxdPack_PARA_UPDATE_CHA(QByteArray datas);
    void rxdPack_PARA_UPDATE_START(QByteArray datas);
    void rxdPack_PARA_UPDATE_STOP(QByteArray datas);
    void rxdPack_MOTOR_STATE(QByteArray datas);
    void rxdPack_PARA_READ_com(QByteArray datas);
    void rxdPack_PARA_Write_com(QByteArray datas);
    void rxdPack_CALI_GET(QByteArray datas);
    void rxdPack_CALI_BEGIN(QByteArray datas);
    void rxdPack_CALI_ING(QByteArray datas);
    void rxdPack_CALI_RST(QByteArray datas);
    void rxdPack_CALI_DONE(QByteArray datas);
    // void rxdPack_MOTOR_FEEDBACK();
    // void rxdPack_HALLCALI_ING();
    // void rxdPack_HALLCALI_RST();
    // void rxdPack_MOTOR_CALI();
    // void rxdPack_PARA_STR_INFO_bak();
    // void rxdPack_PARA_READ();
    // void rxdPack_PARA_Write();
    // void rxdPack_PARA_UPDATE();
    // void rxdPack_FAULT_WARN();

    void scope_dataProcess();

    void uartPack(struct uartPack *pack);

    volatile enum canComStep runStep; // 线程中循环运行状态
    bool hasAcked;                    // 是否收到应答
    quint32 UseTimeMs;                // 用于记录等待时间

    enum canParaItemType paraItemBak;
    char p_buf[STR_NAME_LEN_MAX]; // 用于保存参数读取时的字符串

    const static qint16 paraNumTab[PARA_STR_TOTAL];
};

#define BIT(n) (1 << n)

// 故障状态
#define FAULT_MC_OVT BIT(0)       // 电机线圈过温
#define FAULT_DRV BIT(1)          // 驱动芯片故障
#define FAULT_LOW_VOLT BIT(2)     // 低压故障
#define FAULT_HIGH_VOLT BIT(3)    // 高压故障
#define FAULT_OC_IB BIT(4)        // B相电流采样过流
#define FAULT_OC_IC BIT(5)        // C相电流采样过流
#define FAULT_ENCODER_CALI BIT(7) // 编码器未标定

// 警告状态
#define WARN_MC_TEMP BIT(0)     // 电机线圈温度高
#define WARN_HALL_CALI BIT(1)   // 霍尔标定故障
#define WARN_TORQUE_CALI BIT(2) // 力矩标定数据非法
#define WARN_TORQUE_COEF BIT(3) // 力矩标定数据拟合零偏过大

#define PARA_ERROR_NO_TYPE (1 << 0)
#define PARA_ERROR_TYPE (1 << 1)
#define PARA_ERROR_POINT (1 << 2)
#define PARA_ERROR_POINT_CONT (1 << 3)
#define PARA_ERROR_NUM (1 << 4)
#define PARA_ERROR_ITEM (1 << 5)
#define PARA_ERROR_ITEM_MATCH (1 << 6)
#define PARA_ERROR_WRITE_STR_NUM_MATCH (1 << 7)
#define PARA_ERROR_READ_STR_NUM_MATCH (1 << 8)
#define PARA_ERROR_WRITE_PARA_NUM_MATCH (1 << 9)
#define PARA_ERROR_READ_PARA_NUM_MATCH (1 << 10)

#define PARA_READ_ERROR_CODE (1 << 0)
#define PARA_READ_ERROR_TYPE (1 << 1)
#define PARA_READ_ERROR_INDEX (1 << 2)
#define PARA_READ_ERROR_NOT_NEED (1 << 4)

#define PARA_WR_ERROR_NO_CODE (1 << PARA_WRITE_ERROR_NO_CODE)
#define PARA_WR_ERROR_FORBID_MODIFY (1 << PARA_WRITE_ERROR_FORBID_MODIFY)
#define PARA_WR_ERROR_TYTE (1 << PARA_WRITE_ERROR_TYTE)
#define PARA_WR_ERROR_LOSS_FRAME (1 << PARA_WRITE_ERROR_LOSS_FRAME)
#define PARA_WR_ERROR_FRAME (1 << PARA_WRITE_ERROR_FRAME)
#define PARA_WR_ERROR_LIMIT (1 << PARA_WRITE_ERROR_LIMIT)
#define PARA_WR_ERROR_MODE_FORBID_MODIFY (1 << PARA_WRITE_ERROR_MODE_FORBID_MODIFY)
#define PARA_WR_ERROR_MODE_FORBID_SAVE (1 << PARA_WRITE_ERROR_MODE_FORBID_SAVE)
#define PARA_WR_ERROR_SAVE (1 << PARA_WRITE_ERROR_SAVE)
#define PARA_WR_ERROR_NO_CMD (1 << PARA_WRITE_ERROR_NO_CMD)
#define PARA_WR_ERROR_CMD_MATCH (1 << 15)
#define PARA_WR_ERROR_CODE_MATCH (1 << 14)

#define PARA_UPDATE_ERROR_CMD (1 << 0)
#define PARA_UPDATE_ERROR_FRESET (1 << 1)
#define PARA_UPDATE_ERROR_CHSET_NOCODE (1 << 2)
#define PARA_UPDATE_ERROR_CHSET_FRE (1 << 3)
#define PARA_UPDATE_ERROR_CHSET_LOSE (1 << 4)
#define PARA_UPDATE_ERROR_START (1 << 5)
#define PARA_UPDATE_ERROR_CHSET_TYPE (1 << 6)

#endif // OTA_THREAD_H
