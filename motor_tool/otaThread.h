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

// #define PuduMotorAreaNet  //fzh20221031

#define BAUD_TAB_SIZE (12)

#define STR_NAME_LEN_MAX (18)
#define STR_NAME_LEN (16)

#define WAVE_TIME_WINDOW (10) // s
#define WAVE_SHOW_FRE (20)    // HZ

#define CHANNAL_SEL_NUM (8) // 示波器通道数量

#define CAN_ID_MASTER (0X00)        // 控制主机地址 - SPIE
#define CAN_ID_MOTOR_DEFAULT (0X7F) // 电机默认地址 - 未配置id
#define CAN_ID_BROADCAST (0XFE)     // 广播地址     - 默认接收地址
#define CAN_ID_DEBUG_UI (0XFD)      // 调试地址     - 上位机地址
#define CAN_FRAME_BYTE_LEN (17 + 2)
#define CAN_SCOPE_FRE_MAX (2600) // 示波器最大频率
extern float P_MIN;
extern float P_MAX;
extern float V_MIN;
extern float V_MAX;
extern float KP_MIN;
extern float KP_MAX;
extern float KD_MIN;
extern float KD_MAX;
extern float KI_MIN;
extern float KI_MAX;
extern float T_MIN;
extern float T_MAX;
extern quint8 motor_mode_set;
extern bool is_waterproof_motor;
enum motMode
{
    MOTOR_RESET_200,                  //复位
    MOTOR_PARALL_CLOSE_LOOP_HIGHSIDE_3, //并行闭环
    MOTOR_CASCADE_CLOSE_LOOP_HIGHSIDE_4, //串级闭环
    MOTOR_TORQU_STEP_248,         //扭矩
    MOTOR_OPENLOOP_253,          //开环控制
    MOTOR_FREQUENCY_SWEEP_17    //< 扫频模式（模式值17）
};
//#define P_MIN -100.0f
//#define P_MAX  100.0f
//#define V_MIN -40.0f
//#define V_MAX 40.0f
//#define KP_MIN 0.0f
//#define KP_MAX 10.0f
//#define KD_MIN 0.0f
//#define KD_MAX 5.0f
//#define T_MIN -600.0f
//#define T_MAX 600.0f
#define MAX_POS_VAL 65535
enum canATmode
{
    AT_CANMODE,
    AT_FRAMEFORMAT,
    AT_FILTER,
    AT_BAUD,
    AT_USARTPARAM,
    AT_CONFIG_END,
    AT_SET,
    AT_AT,
};

enum canComStep
{
    STEP_STOP,
    STEP_INIT,
    STEP_INCONFIG,
    STEP_CONFIG,
    STEP_IN_OTA,
    STEP_SEND_INFO,
    STEP_SEND_BIN,
    STEP_SEND_END,
    STEP_GET_DEV_INFO,
    STEP_SET_CAN_ID,
    STEP_SET_RST_MODE,
    STEP_SET_MOTOR_MODE,
    STEP_SET_BRAKE_MODE,
    STEP_SET_CALI_ENCODER_MODE,
    STEP_SET_ZERO_MECH_MODE,
    STEP_MOTOR_CTRL_SINGLE,
    STEP_MOTOR_CTRL_CONTINUOUS,
    STEP_ASK_PARA_STR_INFO,
    STEP_READ_PARA_STR_START,
    STEP_READ_PARA_STR,
    STEP_WRITE_PARA_STR_START,
    STEP_WRITE_PARA_STR,
    STEP_SAVE_PARA_STR,
    STEP_RECOVER_FAC,
    STEP_SCOPE_SET_FRE,
    STEP_SCOPE_SET_CHANNAL,
    STEP_SCOPE_START,
    STEP_SCOPE_STOP,
    STEP_SCOPE_SET_MODE,
    STEP_SCOPE_CONFIG,

};

enum canComMode
{
    CANCOM_ANNOUNCE_DEVID = 0, // 通告设备ID

    CANCOM_MOTOR_CTRL,     // MOTOR-电机控制
    CANCOM_MOTOR_FEEDBACK, // MOTOR-电机反馈
    CANCOM_MOTOR_IN,       // MOTOR-进入电机模式3
    CANCOM_MOTOR_RESET,    // MOTOR-复位模式
    CANCOM_MOTOR_CALI,     // MOTOR-高速编码器标定
    CANCOM_MOTOR_ZERO,     // MOTOR-设置机械零位
    CANCOM_MOTOR_ID,       // MOTOR-设置ID
    CANCOM_PARA_WRITE,     // 参数-写入
    CANCOM_PARA_READ,      // 参数-读取
    CANCOM_PARA_UPDATE,    // 参数-更新上传
    CANCOM_OTA_START,      // OTA-启动
    CANCOM_OTA_INFO,       // OTA-升级文件描述
    CANCOM_OTA_ING,        // OTA-升级中
    CANCOM_OTA_END,        // OTA-升级完成
    CANCOM_CALI_ING,       // 编码器标定中
    CANCOM_CALI_RST,       // 编码器标定结果
    CANCOM_HALLCALI_ING,   // 低速HALL标定中
    CANCOM_HALLCALI_RST,   // 低速HALL标定结果
    CANCOM_PARA_STR_INFO,  // 参数-字符串信息
    CANCOM_MOTOR_BRAKE,    // MOTOR-进入刹车模式
    CANCOM_MOTOR_OPENLOOP, //电机开环
    CANCOM_MOTOR_CASCADE,  //电机模式4
    CANCOM_MOTOR_TOQCALI, //248 扭矩步进
    CANCOM_MOTOR_SWEEP,   //电机扫频
    CANCOM_FAULT_WARN,     // 故障和警告信息
    CANCOM_SCOPE_SET_MODE, // 示波器设置模式

    CANCOM_MODE_TOTAL, // 未开发功能
};
enum stateType{
    REST_MODE = 0,          //复位模式
    CALIBRATION_MODE,       //编码器标定模式
    PARALL_MODE,            //PD控制模式
    BRAKE_MODE,             //刹车模式
    ZERO_MODE,              //设置机械零位模式
    ERROR_MODE,             //故障状态
    OPENLOOP_MODE,          //开环运行模式
    CASCADE_MODE,           //级联控制模式
    TOQCALI_MODE,           //扭矩步进模式
    CURSWEEP_MODE           //电流环形扫频模式
};
struct exCanIdInfo
{
    quint32 id : 8;
    quint32 data : 16;
    enum canComMode mode : 5;
    quint32 res : 3;
};

struct stdCanIdInfo
{
    quint32 id : 11;
    quint32 res : 21;
};

enum motorMode  //电机模式
{
    MT_MODE_RESET = 0,
    MT_MODE_CALI,
    MT_MODE_MOTOR,
    MT_MODE_BRAKE,
    MT_MODE_ZORE,
    MT_MODE_ERR,
    MT_MODE_MOTOR_253,
    MT_MODE_MOTOR_4,
    MT_MODE_MOTOR_248,
    MT_MODE_MOTOR_17,  //扫频

};

enum frameType
{
    FRAME_STD,
    FRAME_EXD
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

struct canPack
{
    struct exCanIdInfo exId;
    quint8 len;
    quint8 data[8];
};
struct canPack_Std
{
    struct stdCanIdInfo stdId;
    quint8 len;
    quint8 data[8];
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
    OTA_STA_CONNECT_OVER_TIME,
    OTA_STA_CONNECT_SUCCESS,
    OTA_STA_INCONFIG_OVER_TIME,
    OTA_STA_INCONFIG_SUCCESS,
    OTA_STA_CONFIG_OVER_TIME,
    OTA_STA_CONFIG_SUCCESS,
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
    OTA_STA_SWEEP_RESULT,//扫频结果数据

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

enum paraWriteCmdType
{
    WRITE_CMD_MODIFY_ONLY = 0,
    WRITE_CMD_MODIFY_SAVE,
    WRITE_CMD_SAVE_ONLY,
    WRITE_CMD_RECOVER_FAC,
    WRITE_CMD_TOTAL,
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

#define MCU_NUM (14)

struct mcuInfo
{
    quint64 id[MCU_NUM];
    quint8 canId[MCU_NUM];
    quint8 point;
    quint8 usePos;
    bool changeFlag;
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

    enum paraStrItemType itemIndex;
    quint16 posIndex;

    quint16 codeNeed;
    enum paraType typeNeed;

    enum paraWriteCmdType writeCmd;
    enum paraWriteCmdType writeCmdBack;
    enum paraWriteErrorCode writeErrorCode;

    quint8 echoNum;
    quint8 meaTimerNum;

    quint8 wLen;
    quint8 wIndex;
    quint8 wOffset;
};

class otaThread : public QThread
{
    Q_OBJECT
public:
    otaThread();

    void setCmd(enum canComStep mode);

    void selCh_index2code(quint16 index, quint16 *pCode, enum paraType *pType);

    void delay_ms(uint ms);

    enum otaStatus otaStatus;

    struct paraInfoType paraInfo;
    quint32 paraErrorCode;

    struct canPack rxFrame;
    QByteArray RxData;

    QByteArray BinData;

    quint32 binSize;
    quint32 PackNumber;
    quint16 PackCnt;

    quint64 mcuId;
    quint8 mcuCanId;
    quint8 devCanId;

    quint8 configCanId;
    quint8 subScanCount;
    quint8 mainScanCount;
    quint8 scanId;

    struct mcuInfo mcuBuf;

    enum frameType canFrameType; /* 2022/11/23 */

    struct hallTabInfoType hallInfo[6];
    qint16 hallRaw[6];
    qint16 hallRdLval;
    quint8 hallError;
    quint8 hallWarn;
    quint16 hallCount;

    float posSet;
    float velSet;
    float kpSet;
    float kdSet;
    float kiSet;
    float torqueSet;

    int posIntSet;
    int velIntSet;
    int kpIntSet;
    int kdIntSet;
    int kiIntSet;
    int torqueIntSet;

    quint16 caliI;
    quint16 caliInd;
    qint32 caliLut;

    quint32 faultSta;
    quint32 warnSta;
    qint32 frequency;
    qint32 caliRaw;
    float amplitude;//幅值
    float phase;  //相位
    float currentAngle;
    float rotorAngle;
    float caliOffset;
    float caliMSE;
    uint8_t calerrorcode;

    quint8 canIdGet;
    float posGet;
    float velGet;
    float torqueGet;

    qint32 posIntGet;
    qint32 velIntGet;
    qint32 torqueIntGet;

    float chTickUs;
    quint8 chSelNum;
    quint8 chSelPos;
    quint8 Burst_mode;
    quint16 chEchoFreHz;
    quint16 chEchoFreHzMax;
    quint16 chEchoFreHzMin;
    quint16 chEchoDevFreMax;
    quint16 chEchoDevFreMin;
    quint8 chSelIndex[CHANNAL_SEL_NUM];
    quint16 chSelCode[CHANNAL_SEL_NUM];
    quint8 chRxdByte[128];
    quint16 chTickBak;
    quint16 chNeedByteTotal;
    double chTimeStamp;
    // double  chParaTab[CHANNAL_SEL_NUM];
    enum paraType chTypeNeed[CHANNAL_SEL_NUM];
    enum paraType chTypeDev[CHANNAL_SEL_NUM];
    bool scopeIngFlag;

    quint32 scopeShowNum;
    quint32 scopeShowCount;
    quint32 scopeWindowNum;
    quint32 scopeWindowCount;
    bool scopeWindowRemoveFlag;

    QVector<QCPGraphData> mData[CHANNAL_SEL_NUM];

    struct motorStatus mtStatus;
    quint8 SelfMode; // 自定义模式

    const static qint8 paraByteNumTab[ParaType_TOTAL];
    const static qint32 baudTab[BAUD_TAB_SIZE];

    QFile waveFile;
    bool isWaveFileOpen;

//    qreal P_MIN = -100.0f;
//    qreal P_MAX = 100.0f;
//    qreal V_MIN = -40.0f;
//    qreal V_MAX = 40.0f;
//    qreal KP_MIN = 0.0f;
//    qreal KP_MAX = 10.0f;
//    qreal KD_MIN = 0.0f;
//    qreal KD_MAX = 5.0f;
//    qreal T_MIN = -600.0f;
//    qreal T_MAX = 600.0f;

protected:
    void run();

signals:
    void sendState(); // emit when each packge send done  //fzh20221031,   sender()是QObject类的方法. 如果在由信号激活的插槽中调用该函数，返回指向发送信号的对象的指针，否则返回0，该指针仅在从该对象的线程上下文调用此函数的槽执行期间有效。
    void WriteToSerial(QByteArray, qint16);
    void scopeToShow(QVector<QCPGraphData> *);

private slots:
    void getSerialData(QByteArray);

private:
    int float_to_uint(float x, float x_min, float x_max, int bits);
    float uint_to_float(int x_int, float x_min, float x_max, int bits);
    void analysisRxdDatas(QByteArray);
    void txdPack(struct canPack *pack);
    void CanConfig(enum canATmode mode);

    void rxdPack_ANNOUNCE_DEVID();
    void rxdPack_MOTOR_FEEDBACK();
    void rxdPack_CALI_ING();
    void rxdPack_CALI_RST();
    void rxdPack_HALLCALI_ING();
    void rxdPack_HALLCALI_RST();
    void rxdPack_MOTOR_CALI();
    //添加扫频数据处理函数
    void rxdPack_MOTOR_SWEEP();
    void rxdPack_PARA_STR_INFO();
    void rxdPack_PARA_READ();
    void rxdPack_PARA_Write();
    void rxdPack_PARA_UPDATE();
    void rxdPack_FAULT_WARN();

    void scope_dataProcess();

    volatile enum canComStep runStep;
    bool hasAcked;
    quint32 UseTimeMs;

    quint8 indexBak;
    enum canATmode configStep;

    enum canParaItemType paraItemBak;
    enum paraStrItemType paraTypeBak;
    quint16 paraPointBak;
    // QByteArray paraStr;
    char p_buf[STR_NAME_LEN_MAX];

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
