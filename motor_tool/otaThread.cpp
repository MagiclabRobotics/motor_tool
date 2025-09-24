#include "otaThread.h"
float P_MIN = -12.5f;
float P_MAX = 12.5f;
float V_MIN = -25.0f;
float V_MAX = 25.0f;
float KP_MIN = 0.0f;
float KP_MAX = 500.0f;
float KD_MIN = 0.0f;
float KD_MAX = 5.0f;
float KI_MIN = 0.0f;
float KI_MAX = 5.0f;
float T_MIN = -135.0f;
float T_MAX = 135.0f;
const qint32 otaThread::baudTab[BAUD_TAB_SIZE] = {
    921600,
    9600,
    115200,
    1200,
    2400,
    4800,
    19200,
    38400,
    57600,
    230400,
    460800,
    1152000,
};
const qint16 otaThread::paraNumTab[PARA_STR_TOTAL] = {WRITE_STRING_NUM, READ_STRING_NUM, SET_PARA_NUM, STATUS_PARA_NUM};
const qint8 otaThread::paraByteNumTab[ParaType_TOTAL] = {1, 1, 2, 2, 4, 4, 4, 8, 8, 8, STR_NAME_LEN};
quint8 motor_mode_set = 0;
bool is_waterproof_motor = false;
otaThread::otaThread() // 构造函数。
{
    runStep = STEP_STOP;

    RxData.clear();
    BinData.clear();

    PackNumber = 0;
    PackCnt = 0;

    canFrameType = FRAME_EXD;

    mcuBuf.point = 0;
    mcuBuf.changeFlag = false;

    posSet = 0.0f;
    velSet = 0.0f;
    kpSet = 0.0f;
    kdSet = 0.0f;
    kiSet = 0.0f;
    torqueSet = 0.0f;

    paraErrorCode = 0;
    scopeIngFlag = false;
    isWaveFileOpen = false;
}

// 设置can模块
void otaThread::CanConfig(enum canATmode mode)
{
    QByteArray Pack("\0");

    QString str;

    switch (mode)
    {

    case AT_CANMODE:
        str = "AT+CAN_MODE=0\r\n";
        break;
    case AT_FRAMEFORMAT:
        str = "AT+CAN_FRAMEFORMAT=1,0,1,0\r\n";
        break;
    case AT_BAUD:
        str = "AT+CAN_BAUD=1000000\r\n";
        break;
    case AT_USARTPARAM:
        str = "AT+USART_PARAM=" + QString::number(baudTab[0], 10) + ",0,0,0\r\n";
        break;
    case AT_FILTER:
        str = "AT+CAN_FILTER0=1,0,0,0\r\n";
        break;
    case AT_SET:
        str = "AT+CG\r\n";
        break;
    case AT_AT:
    default:
        str = "AT+AT\r\n";
        break;
    }

    Pack = str.toLatin1();

    emit WriteToSerial(Pack, (qint16)Pack.size());
}

int otaThread::float_to_uint(float x, float x_min, float x_max, int bits)
{
    /// Converts a float to an unsigned int, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

float otaThread::uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}


/* 发送数据包 */
void otaThread::txdPack(struct canPack *pack)
{
    QByteArray Pack("\0");
    QByteArray Pack_Std1("\0");
    QByteArray Pack_Std2("\0");

    if (canFrameType == FRAME_STD)
    {
        if (pack->len < 8)
        {
            pack->len = 8; /* 强制将所有数据域长度都转为 8byte */
        }
        else if (pack->len > 8)
        {
            pack->len = 8;
        }
        pack->exId.res = 0;

        quint32 addr;
        memcpy(&addr, &(pack->exId), 4);
        /*
         * 维特智能 --> （USB - CAN）模块
         * frame    | 字符AT    帧类型    帧ID     数据长度    帧数据     回车换行字符
           size     | 2Byte    1Byte    4Byte    1Byte     1-8Byte    2Byte
        ------------------------------------------------------------------------
         * 帧ID结构  | bit31-bit21    bit20-bit3    bit2    bit1    bit0
           标准帧    |    帧ID            全0         0       0       0
           扩展帧    | 帧ID前11bit     帧ID后18bit     1       0       0
        */
        quint8 id_byte1 = (pack->exId.id) >> 3;        /* 取ID的高5位放在Byte1的低5位 */
        quint8 id_byte2 = (pack->exId.id & 0x07) << 5; /* 取ID的低3位放在Byte2的高3位 */

        // 将扩展正修改为两个标准帧进行发送
        // 第一帧
        Pack_Std1.resize(9 + 8);
        Pack_Std1[0] = 'A'; /* AT */
        Pack_Std1[1] = 'T';
        // frame ID
        Pack_Std1[2] = (quint8)id_byte1 | (0b001 << 5);
        Pack_Std1[3] = (quint8)id_byte2;
        Pack_Std1[4] = (quint8)0;
        Pack_Std1[5] = (quint8)0;

        Pack_Std1[6] = pack->len;

        for (quint8 i = 0; i < 8; i++)
            Pack_Std1[7 + i] = pack->data[i];

        Pack_Std1[7 + 8] = '\r';
        Pack_Std1[8 + 8] = '\n';

        emit WriteToSerial(Pack_Std1, (qint16)Pack_Std1.size());

        // 第二帧
        Pack_Std2.resize(9 + 8);
        Pack_Std2[0] = 'A';
        Pack_Std2[1] = 'T';
        //
        Pack_Std2[2] = (quint8)id_byte1 | (0b100 << 5);
        Pack_Std2[3] = (quint8)id_byte2;
        Pack_Std2[4] = (quint8)0;
        Pack_Std2[5] = (quint8)0;

        Pack_Std2[6] = pack->len;

        Pack_Std2[7] = (quint8)((addr & 0X0000FF00) >> 8); /* 主机ID   0XFD */
        Pack_Std2[8] = (quint8)((addr & 0X00FF0000) >> 16);
        Pack_Std2[9] = (quint8)((addr & 0XFF000000) >> 24); /* 通信类型 */

        for (quint8 i = 3; i < 8; i++)
            Pack_Std2[7 + i] = 0;

        Pack_Std2[7 + 8] = '\r';
        Pack_Std2[8 + 8] = '\n';

        emit WriteToSerial(Pack_Std2, (qint16)Pack_Std2.size());
    }
    else
    {

        if (pack->len < 1)
        {
            pack->len = 1;
            pack->data[0] = 0;
        }
        else if (pack->len > 8)
        {
            pack->len = 8;
        }
        pack->exId.res = 0;

        Pack.resize(9 + pack->len);
        /* start */
        Pack[0] = 'A';
        Pack[1] = 'T';

        quint32 addr;
        memcpy(&addr, &(pack->exId), 4);
        addr = (addr << 3) | (0X00000004);
        Pack[2] = (quint8)((addr & 0XFF000000) >> 24);
        Pack[3] = (quint8)((addr & 0X00FF0000) >> 16);
        Pack[4] = (quint8)((addr & 0X0000FF00) >> 8);
        Pack[5] = (quint8)((addr & 0X000000FF) >> 0);
        /* datalen */
        Pack[6] = pack->len;
        /* data1 */
        for (quint8 i = 0; i < pack->len; i++)
            Pack[7 + i] = pack->data[i];
        /* end */
        Pack[7 + pack->len] = '\r';
        Pack[8 + pack->len] = '\n';

        emit WriteToSerial(Pack, (qint16)Pack.size());
    }
}

void otaThread::getSerialData(QByteArray temp)
{
    int pos;
    QByteArray rxPack;

    RxData.append(temp); // fzh20221031，b.append(a) 将a的值复制到b上的末节点；

    pos = RxData.indexOf("\r\n");

    while (pos >= 0)
    {
        rxPack = RxData.left(pos + 2); // fzh20221031，left(int n)是获取字符串左边n个字符，right(int n)是获取字符串右边n个字符。第二个参数是一个索引，表示函数的目标字符串是该索引之前的字符串
        RxData.remove(0, pos + 2);     // fzh20221031，

        analysisRxdDatas(rxPack);
        // qDebug()<<"rxPack:"<<rxPack<<"  hex:"<<rxPack.toHex();
        // qDebug() << "rxHex:" << rxPack.toHex();

        pos = RxData.indexOf("\r\n"); // fzh20221031，查询"\r\n"字符串之前还有多少剩余数据，只要还有剩余数据，则继续接收数据
    }
}

// 设置运行模式
void otaThread::setCmd(enum canComStep mode)
{
    // 每次进入初始化
    struct canPack pack;
    memset(&pack, 0, sizeof(struct canPack));
    pack.len = 8;
    pack.exId.id = devCanId;

    UseTimeMs = 0;
    hasAcked = false;
    quint32 addr;

    switch (mode)
    {
    case STEP_SEND_BIN: // OTA升级，这部分是传输代码部分

        pack.exId.data = PackCnt;
        pack.exId.mode = CANCOM_OTA_ING;

        addr = PackCnt * 8;
        for (uint8_t i = 0; i < 8; i++, addr++)
        {
            if (addr < binSize)
                pack.data[i] = BinData[PackCnt * 8 + i];
            else
                pack.data[i] = 0XFF;
        }
        txdPack(&pack);

        break;

    case STEP_GET_DEV_INFO:
        subScanCount = 0;
        mainScanCount = 0;
        scanId = 0;
        break;

    case STEP_SEND_END:
        pack.exId.data = 0;
        pack.exId.mode = CANCOM_OTA_END;
        memcpy(&(pack.data[0]), &PackNumber, 4);
        txdPack(&pack);
        break;

    case STEP_STOP:
        RxData.clear();
        break;

    case STEP_INIT:
        RxData.clear();
        CanConfig(AT_AT);
        break;

    case STEP_INCONFIG:
        RxData.clear();
        CanConfig(AT_SET);
        configStep = AT_CANMODE;
        break;

    case STEP_CONFIG:
        RxData.clear();
        CanConfig(configStep);
        break;

    case STEP_IN_OTA: // OTA升级的第一步
        pack.exId.data = CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_OTA_START;
        memcpy(&(pack.data[0]), &(mcuBuf.id[mcuBuf.usePos]), 8); // 这里储存的是单片机的64位识别码
        txdPack(&pack);
        break;

    case STEP_SEND_INFO: // 发送数据包的大小信息
        binSize = BinData.size();

        if (binSize == 0)
        {
            mode = STEP_STOP;
            otaStatus = OTA_STA_BINFILE_EMPTY;
            emit sendState();
        }
        else if (binSize > 0X80000)
        {
            mode = STEP_STOP;
            otaStatus = OTA_STA_BINFILE_SIZE_OVER_LIMIT;
            emit sendState();
        }
        else
        {
            PackNumber = binSize / 8;

            if (binSize % 8)
            {
                PackNumber += 1;
            }
            PackCnt = 0;

            memcpy(&(pack.data[0]), &binSize, 4);
            memcpy(&(pack.data[4]), &PackNumber, 4);

            pack.exId.data = CAN_ID_DEBUG_UI;
            pack.exId.mode = CANCOM_OTA_INFO;
            txdPack(&pack);
        }
        break;

    case STEP_SET_CAN_ID:
        pack.exId.data = (((quint16)configCanId) << 8) | CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_MOTOR_ID;
        memcpy(&(pack.data[0]), &(mcuBuf.id[mcuBuf.usePos]), 8);

        txdPack(&pack);
        break;

    case STEP_SET_RST_MODE:
        pack.exId.data = CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_MOTOR_RESET;

        txdPack(&pack);
        break;

    case STEP_SET_MOTOR_MODE:
        pack.exId.data = CAN_ID_DEBUG_UI;
        if(motor_mode_set == MOTOR_RESET_200){
            pack.exId.mode = CANCOM_MOTOR_RESET;
        }
        else if(motor_mode_set == MOTOR_PARALL_CLOSE_LOOP_HIGHSIDE_3){
            pack.exId.mode = CANCOM_MOTOR_IN;
        }
        else if(motor_mode_set == MOTOR_CASCADE_CLOSE_LOOP_HIGHSIDE_4){
            pack.exId.mode = CANCOM_MOTOR_CASCADE;
        }
        else if(motor_mode_set == MOTOR_TORQU_STEP_248){
            pack.exId.mode = CANCOM_MOTOR_TOQCALI;
        }
        else if(motor_mode_set == MOTOR_OPENLOOP_253){
            pack.exId.mode = CANCOM_MOTOR_OPENLOOP;
        }
        else if(motor_mode_set == MOTOR_FREQUENCY_SWEEP_17) {
               pack.exId.mode = CANCOM_MOTOR_SWEEP ; // 新增扫频命令
           }
        else{
            pack.exId.mode = CANCOM_MOTOR_RESET;
        }

        txdPack(&pack);
        break;

    case STEP_SET_BRAKE_MODE:
        pack.exId.data = CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_MOTOR_BRAKE;

        txdPack(&pack);
        break;

    case STEP_SET_CALI_ENCODER_MODE:
        pack.exId.data = CAN_ID_DEBUG_UI;   // 主机的ID
        pack.exId.mode = CANCOM_MOTOR_CALI; // 模式

        txdPack(&pack);
        break;

    case STEP_SET_ZERO_MECH_MODE:
        pack.exId.data = CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_MOTOR_ZERO;

        txdPack(&pack);
        break;

    case STEP_MOTOR_CTRL_SINGLE:

        pack.exId.id = devCanId;
        pack.len = 8;
        pack.exId.mode = CANCOM_MOTOR_CTRL;

        posIntSet = float_to_uint(posSet, P_MIN, P_MAX, 16);
        velIntSet = float_to_uint(velSet, V_MIN, V_MAX, 16);
        if(is_waterproof_motor){
            kpIntSet = float_to_uint(kpSet, KP_MIN, KP_MAX, 8);
            kdIntSet = float_to_uint(kdSet, KD_MIN, KD_MAX, 8);
            kiIntSet = float_to_uint(kiSet, KI_MIN, KI_MAX, 8);
            torqueIntSet = (int)(torqueSet * 10);//float_to_uint(torqueSet, T_MIN, T_MAX, 16);
        }
        else{
            kpIntSet = float_to_uint(kpSet, KP_MIN, KP_MAX, 16);
            kdIntSet = float_to_uint(kdSet, KD_MIN, KD_MAX, 16);
            kiIntSet = float_to_uint(kiSet, KI_MIN, KI_MAX, 16);
            torqueIntSet = float_to_uint(torqueSet, T_MIN, T_MAX, 16);
        }


        qDebug() << "posIntSet: " << posIntSet << "velIntSet: " << velIntSet
                 << "kpIntSet: " << kpIntSet << "torqueIntSet: " << torqueIntSet << endl;

        pack.exId.data = torqueIntSet;

        pack.data[0] = posIntSet >> 8;     // H
        pack.data[1] = posIntSet & 0X00FF; // L
        pack.data[2] = velIntSet >> 8;
        pack.data[3] = velIntSet & 0X00FF;
        if(is_waterproof_motor){
            pack.data[4] = kpIntSet;
            pack.data[5] = kdIntSet;
            pack.data[6] = kiIntSet;
            pack.data[7] = 0;
        }
        else{
            pack.data[4] = kpIntSet >> 8;
            pack.data[5] = kpIntSet & 0X00FF;
            pack.data[6] = kdIntSet >> 8;
            pack.data[7] = kdIntSet & 0X00FF;
        }

        txdPack(&pack);
        break;

    case STEP_MOTOR_CTRL_CONTINUOUS:
        break;

    case STEP_ASK_PARA_STR_INFO:
        memset(paraInfo.numTab, 0, 2 * PARA_STR_TOTAL);
        paraErrorCode = 0;
        pack.exId.data = CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_PARA_STR_INFO;
        memcpy(&(pack.data[0]), &(mcuBuf.id[mcuBuf.usePos]), 8);
        txdPack(&pack);
        break;

    case STEP_READ_PARA_STR_START:
        paraInfo.posIndex = 0;
        paraInfo.itemIndex = (enum paraStrItemType)0;
        mode = STEP_READ_PARA_STR;

    case STEP_READ_PARA_STR:
        paraErrorCode = 0;
        indexBak = 0;
        paraInfo.codeNeed = (((quint16)paraInfo.itemIndex) << 12) | (paraInfo.posIndex & 0X0FFF);
        pack.exId.data = CAN_ID_DEBUG_UI;
        pack.exId.mode = CANCOM_PARA_READ;
        memcpy(&(pack.data[0]), &(paraInfo.codeNeed), 2);
        txdPack(&pack);
        break;

    case STEP_WRITE_PARA_STR_START:
        paraInfo.writeCmd = WRITE_CMD_MODIFY_ONLY;
        paraInfo.itemIndex = WRITE_STR;
        for (paraInfo.posIndex = 0; paraInfo.posIndex < paraInfo.numTab[WRITE_STR]; paraInfo.posIndex++)
        {
            if (paraInfo.writeStr[paraInfo.posIndex].modifyFlag)
                break;
        }

        if (paraInfo.posIndex >= paraInfo.numTab[WRITE_STR])
        {
            paraInfo.itemIndex = WRITE_PARA;

            for (paraInfo.posIndex = 0; paraInfo.posIndex < paraInfo.numTab[WRITE_PARA]; paraInfo.posIndex++)
            {
                if (paraInfo.writePara[paraInfo.posIndex].modifyFlag)
                    break;
            }
        }
        mode = STEP_WRITE_PARA_STR;
    case STEP_WRITE_PARA_STR:
        paraErrorCode = 0;
        paraInfo.writeErrorCode = PARA_WRITE_ERROR_NONE;

        if (paraInfo.posIndex >= paraInfo.numTab[paraInfo.itemIndex])
        {
            paraInfo.writeErrorCode = PARA_WRITE_ERROR_NO_CODE;
        }
        else
        {
            switch (paraInfo.itemIndex)
            {
            case WRITE_STR:
                paraInfo.typeNeed = ParaType_STRING;
                memcpy(p_buf, paraInfo.writeStr[paraInfo.posIndex].strEdit, paraByteNumTab[paraInfo.typeNeed]);
                break;
            case WRITE_PARA:
                paraInfo.typeNeed = paraInfo.writePara[paraInfo.posIndex].type;
                memcpy(p_buf, paraInfo.writePara[paraInfo.posIndex].valM, paraByteNumTab[paraInfo.typeNeed]);
                break;
            default:
                paraInfo.writeErrorCode = PARA_WRITE_ERROR_FORBID_MODIFY;
                break;
            }
        }
        if (paraInfo.writeErrorCode != PARA_WRITE_ERROR_NONE)
        {
            mode = STEP_STOP;
            paraErrorCode |= (1 << paraInfo.writeErrorCode);
            otaStatus = OTA_STA_WRITE_PARA_ERROR;
            emit sendState();
        }
        else
        {
            paraInfo.codeNeed = (((quint16)paraInfo.itemIndex) << 12) | (paraInfo.posIndex & 0X0FFF);
            pack.exId.data = CAN_ID_DEBUG_UI | (((quint16)paraInfo.writeCmd) << 8);
            pack.exId.mode = CANCOM_PARA_WRITE;
            memcpy(&(pack.data[0]), &(paraInfo.codeNeed), 2);
            pack.data[2] = paraInfo.typeNeed;

            paraInfo.wLen = paraByteNumTab[paraInfo.typeNeed];
            paraInfo.wIndex = 0;
            paraInfo.wOffset = 0;

            pack.data[3] = paraInfo.wIndex;
            quint8 txLen = (paraInfo.wLen >= 4) ? 4 : paraInfo.wLen;
            paraInfo.wLen -= txLen;
            memcpy(&(pack.data[4]), &p_buf[paraInfo.wOffset], txLen);
            paraInfo.wOffset += txLen;
            txdPack(&pack);
        }
        break;
    case STEP_SAVE_PARA_STR:
        mode = STEP_WRITE_PARA_STR;
        paraInfo.writeCmd = WRITE_CMD_SAVE_ONLY;
        paraErrorCode = 0;
        paraInfo.writeErrorCode = PARA_WRITE_ERROR_NONE;

        pack.exId.data = CAN_ID_DEBUG_UI | (((quint16)paraInfo.writeCmd) << 8);
        pack.exId.mode = CANCOM_PARA_WRITE;
        txdPack(&pack);
        break;
    case STEP_RECOVER_FAC:
        mode = STEP_STOP;
        paraInfo.writeCmd = WRITE_CMD_RECOVER_FAC;
        paraErrorCode = 0;
        paraInfo.writeErrorCode = PARA_WRITE_ERROR_NONE;

        pack.exId.data = CAN_ID_DEBUG_UI | (((quint16)paraInfo.writeCmd) << 8);
        pack.exId.mode = CANCOM_PARA_WRITE;
        txdPack(&pack);
        break;
    case STEP_SCOPE_SET_MODE:
        mode = STEP_SCOPE_CONFIG;
        pack.exId.data = CAN_ID_DEBUG_UI | ((quint16)(Burst_mode & 0X01) << 8);
        pack.exId.mode = CANCOM_SCOPE_SET_MODE; // 新开的模式，放在最后

        txdPack(&pack);
        break;
    case STEP_SCOPE_SET_FRE:
        mode = STEP_SCOPE_CONFIG;
        pack.exId.data = CAN_ID_DEBUG_UI | ((quint16)0 << 8);
        pack.exId.mode = CANCOM_PARA_UPDATE;
        memcpy(&(pack.data[0]), &chEchoFreHz, 2);
        pack.data[2] = 17;
        pack.data[3] = 0;
        memcpy(&(pack.data[4]), &baudTab[0], 4);
        txdPack(&pack);
        break;
    case STEP_SCOPE_SET_CHANNAL:
        mode = STEP_SCOPE_CONFIG;
        pack.exId.data = CAN_ID_DEBUG_UI | ((quint16)1 << 8) | ((quint16)((chSelPos / 4) & 0X03) << 10) | (((quint16)(chSelNum - 1) & 0X0F) << 12);
        pack.exId.mode = CANCOM_PARA_UPDATE;
        memcpy(&(pack.data[0]), &chSelCode[chSelIndex[chSelPos]], 2);
        chSelPos++;
        memcpy(&(pack.data[2]), &chSelCode[chSelIndex[chSelPos]], 2);
        chSelPos++;
        memcpy(&(pack.data[4]), &chSelCode[chSelIndex[chSelPos]], 2);
        chSelPos++;
        memcpy(&(pack.data[6]), &chSelCode[chSelIndex[chSelPos]], 2);
        chSelPos++;
        txdPack(&pack);
        break;
    case STEP_SCOPE_START:
        // mode = STEP_SCOPE_CONFIG;

        pack.exId.data = CAN_ID_DEBUG_UI | ((quint16)2 << 8); // 在这种模式下，会解析第十位，查看是否处于触发模式
        pack.exId.mode = CANCOM_PARA_UPDATE;
        txdPack(&pack);
        break;
    case STEP_SCOPE_STOP:
        // mode = STEP_SCOPE_CONFIG;
        pack.exId.data = CAN_ID_DEBUG_UI | ((quint16)3 << 8);
        pack.exId.mode = CANCOM_PARA_UPDATE;
        txdPack(&pack);
        break;
    default:
        mode = STEP_STOP;
        break;
    }

    runStep = mode;
}

// 解析接收数据
#define CanStdFrameNum 2            // 扩展帧和标准帧数量的对应关系
struct canPack_Std rxMsg_StdSingle; // 用于存储单次接收到的帧，进行诊断，防止标准帧反序或者缺失
struct canPack_Std rxMsg_Std[CanStdFrameNum];
struct canPack_Std txMsg_Std[CanStdFrameNum];
volatile uint8_t CANrxMsg_Cnt; // 用于记录连续两次接收

void otaThread::analysisRxdDatas(QByteArray pack)
{
    bool isAFrame = false;

    if (canFrameType == FRAME_EXD)//扩展数据帧处理
    {
        if ((pack.startsWith("AT")) && (pack.size() == ((quint8)pack[6] + 9)))//包头是AT，验证数据长度=pack[6]+9)))
        {
            /* 地址解析*/
            uint32_t addr = pack[5] & 0x000000FF;
            addr |= ((pack[4] << 8) & 0x0000FF00);
            addr |= ((pack[3] << 16) & 0x00FF0000);
            addr |= ((pack[2] << 24) & 0xFF000000);//拼接32位地址

            addr = addr >> 3;  //右移3位得到最终地址
            //数据填充
            memcpy(&(rxFrame.exId), &addr, 4);
            rxFrame.len = pack[6];

            for (quint8 i = 0; i < rxFrame.len; i++)
                rxFrame.data[i] = pack[7 + i];


//            qDebug()<<"Rxid:"<<QString::number(rxFrame.exId.id,16)
//                    <<"  idData:"<<QString::number(rxFrame.exId.data,16)
//                    <<"  idMode:"<<QString::number(rxFrame.exId.mode,16)
//                    <<"  rxLen:"<<QString::number(rxFrame.len,16)
//                    <<"data:"<<" "<<QString::number(rxFrame.data[0],16)
//                             <<" "<<QString::number(rxFrame.data[1],16)
//                             <<" "<<QString::number(rxFrame.data[2],16)
//                             <<" "<<QString::number(rxFrame.data[3],16)
//                             <<" "<<QString::number(rxFrame.data[4],16)
//                             <<" "<<QString::number(rxFrame.data[5],16)
//                             <<" "<<QString::number(rxFrame.data[6],16)
//                             <<" "<<QString::number(rxFrame.data[7],16);

//模式分发
            switch (rxFrame.exId.mode)
            {
            case CANCOM_ANNOUNCE_DEVID:
                rxdPack_ANNOUNCE_DEVID();
                break;
            case CANCOM_MOTOR_FEEDBACK:
                rxdPack_MOTOR_FEEDBACK();
                break;
            case CANCOM_CALI_ING:
                rxdPack_CALI_ING();
                break;
            case CANCOM_CALI_RST:
                rxdPack_CALI_RST();
                break;
            case CANCOM_HALLCALI_ING:
                rxdPack_HALLCALI_ING();
                break;
            case CANCOM_HALLCALI_RST:
                rxdPack_HALLCALI_RST();
                break;
            case CANCOM_MOTOR_CALI:
                rxdPack_MOTOR_CALI();
                break;
            case CANCOM_MOTOR_SWEEP:
                rxdPack_MOTOR_SWEEP();
                break;

            case CANCOM_PARA_STR_INFO:
                rxdPack_PARA_STR_INFO();
                break;
            case CANCOM_PARA_READ:
                rxdPack_PARA_READ();
                break;
            case CANCOM_PARA_WRITE:
                rxdPack_PARA_Write();
                break;
            case CANCOM_PARA_UPDATE:
                rxdPack_PARA_UPDATE();
                break;
            case CANCOM_FAULT_WARN:
                rxdPack_FAULT_WARN();
                break;
                // case CANCOM_SCOPE_SET_MODE :break;
            default:
                break;
            }

            if (rxFrame.exId.id == CAN_ID_DEBUG_UI)
                isAFrame = true;
        }

        // 以下部分是处理应答的
        switch (runStep)
        {

        case STEP_CONFIG:
        case STEP_INCONFIG:
        case STEP_INIT:
            if (pack.contains("OK"))
                hasAcked = true;
            break;
        case STEP_IN_OTA:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_START) && (rxFrame.exId.data == devCanId))
                hasAcked = true;
            break;
        case STEP_SEND_INFO:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_INFO) && (rxFrame.exId.data == devCanId))
            {
                hasAcked = true;
                otaStatus = OTA_STA_OTA_INFO_SUCCESS;
                emit sendState();
                setCmd(STEP_SEND_BIN);
            }
            break;
        case STEP_SEND_BIN:

            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_ING))
            {
                hasAcked = true;
                if (rxFrame.exId.data == devCanId)
                {
                    PackCnt++;
                    otaStatus = OTA_STA_SEND_BIN_OK;
                }
                else if (rxFrame.exId.data == ((quint16)devCanId | 0X0F00))
                {
                    memcpy(&PackCnt, rxFrame.data, 2);
                    otaStatus = OTA_STA_SEND_BIN_RETRY;
                }

                if (PackCnt >= PackNumber)
                {
                    otaStatus = OTA_STA_SEND_BIN_DONE;
                    setCmd(STEP_SEND_END);
                }
                else
                {
                    setCmd(STEP_SEND_BIN);
                }

                emit sendState();
            }
            break;

        case STEP_SEND_END:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_END))
            {
                hasAcked = true;
                if (rxFrame.exId.data == devCanId)
                {
                    otaStatus = OTA_STA_SUCCESS;
                }
                else if (rxFrame.exId.data == ((quint16)devCanId | 0X0F00))
                {
                    memcpy(&PackCnt, rxFrame.data, 2);
                    otaStatus = OTA_STA_FAILED;
                }
                emit sendState();
            }
            break;

        case STEP_SET_CAN_ID:
            if (rxFrame.exId.mode == CANCOM_ANNOUNCE_DEVID)
                hasAcked = true;
            break;

        case STEP_SET_RST_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_RESET))
                hasAcked = true;
            break;

        case STEP_SET_MOTOR_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && ((mtStatus.mode == MT_MODE_MOTOR) ||(mtStatus.mode == MT_MODE_MOTOR_4) ||(mtStatus.mode == MT_MODE_MOTOR_248) ||(mtStatus.mode == MT_MODE_MOTOR_253)||(mtStatus.mode == MT_MODE_MOTOR_17)))
                hasAcked = true;
            break;

        case STEP_SET_BRAKE_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_BRAKE))
                hasAcked = true;
            break;

        case STEP_SET_CALI_ENCODER_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_CALI))
                hasAcked = true;
            break;

        case STEP_SET_ZERO_MECH_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (posGet < 0.01f) && (posGet > -0.01f))
                hasAcked = true;
            break;

        case STEP_ASK_PARA_STR_INFO:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_STR_INFO))
                hasAcked = true;
            break;

        case STEP_READ_PARA_STR:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_READ))
                hasAcked = true;
            break;

        case STEP_WRITE_PARA_STR:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_WRITE))
                hasAcked = true;
            break;

        case STEP_SCOPE_CONFIG:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_UPDATE))
                hasAcked = true;
            break;

        default:
            break;
        }
    }
    else
    {
        if ((pack.startsWith("AT")) && (pack.size() == ((quint8)pack[6] + 9)))
        {
            // （1）初步对接收到的数据进行保存和处理
            uint32_t addr = pack[5] & 0x000000FF;
            addr |= ((pack[4] << 8) & 0x0000FF00);
            addr |= ((pack[3] << 16) & 0x00FF0000);
            addr |= ((pack[2] << 24) & 0xFF000000);

            addr = addr >> 21; // 扩展帧29its，而标准帧是11bits

            memcpy(&(rxMsg_StdSingle.stdId), &addr, 4);
            rxMsg_StdSingle.len = pack[6];

            for (quint8 i = 0; i < rxMsg_StdSingle.len; i++)
                rxMsg_StdSingle.data[i] = pack[7 + i];

            // （2）进行帧排序，并组合成扩展帧
            // 判断帧序号，以及是否反序号或者漏帧。通过停止标志位和剩余帧数判断。0b100表示接收完成；0b001表示没有完成
            // if(((rxMsg_StdSingle.rx_sfid >> 8) & 0b111) == 0b001)    //接收第一帧
            if (((rxMsg_StdSingle.stdId.id >> 8) & 7) == 1) // 接收第一帧
            {
                memcpy(&rxMsg_Std[0], &rxMsg_StdSingle, sizeof(rxMsg_StdSingle));
                CANrxMsg_Cnt++;
                if (CANrxMsg_Cnt == CanStdFrameNum) // 连续接收到了2帧第一帧，帧接收出现乱序
                {
                    // 报错，漏帧
                    // return 2;
                }
            }
            // else if(((rxMsg_StdSingle.rx_sfid >> 8) & 0b111) == 0b100)  //接收完成，第二帧
            else if (((rxMsg_StdSingle.stdId.id >> 8) & 7) == 4) // 接收完成，第二帧
            {
                memcpy(&rxMsg_Std[1], &rxMsg_StdSingle, sizeof(rxMsg_StdSingle));
                CANrxMsg_Cnt++;
                if (CANrxMsg_Cnt < CanStdFrameNum) // 先接收到了第二针，帧接收出现乱序
                {
                    // 报错，乱序
                    // return 3;
                }

                if ((rxMsg_Std[0].stdId.id & 0xff) != (rxMsg_Std[1].stdId.id & 0xff)) // 2帧的ID号不一致
                {
                    // 报错，ID号不一致
                    // return 5;
                }

                // 接收完成，组合成扩展帧
                {
                    //
                    rxFrame.exId.id = (rxMsg_Std[0].stdId.id & 0xff);
                    rxFrame.exId.data = rxMsg_Std[1].data[0] | rxMsg_Std[1].data[1] << 8;
                    rxFrame.exId.mode = (enum canComMode)(rxMsg_Std[1].data[2] & 31); // 必须强制类型转化，否则数据类型报错
                    rxFrame.len = 8;
                    memcpy(&rxFrame.data, &rxMsg_Std[0].data[0], rxFrame.len);

                    CANrxMsg_Cnt = 0;
                }
            }
            else
            {
                // 报错，帧数据错误
                // return 4;
            }

            // return 0;  //正常返回

            // 组合完成后对扩展帧的mod进行处理
            if (CANrxMsg_Cnt == 0) // 组合完成判断
            {
                switch (rxFrame.exId.mode)
                {
                case CANCOM_ANNOUNCE_DEVID:
                    rxdPack_ANNOUNCE_DEVID();
                    break;
                case CANCOM_MOTOR_FEEDBACK:
                    rxdPack_MOTOR_FEEDBACK();
                    break;
                case CANCOM_CALI_ING:
                    rxdPack_CALI_ING();
                    break;
                case CANCOM_CALI_RST:
                    rxdPack_CALI_RST();
                    break;
                case CANCOM_HALLCALI_ING:
                    rxdPack_HALLCALI_ING();
                    break;
                case CANCOM_HALLCALI_RST:
                    rxdPack_HALLCALI_RST();
                    break;
                case CANCOM_MOTOR_CALI:
                    rxdPack_MOTOR_CALI();
                    break;
                case CANCOM_MOTOR_SWEEP:
                    rxdPack_MOTOR_SWEEP();
                    break;
                case CANCOM_PARA_STR_INFO:
                    rxdPack_PARA_STR_INFO();
                    break;
                case CANCOM_PARA_READ:
                    rxdPack_PARA_READ();
                    break;
                case CANCOM_PARA_WRITE:
                    rxdPack_PARA_Write();
                    break;
                case CANCOM_PARA_UPDATE:
                    rxdPack_PARA_UPDATE();
                    break;
                case CANCOM_FAULT_WARN:
                    rxdPack_FAULT_WARN();
                    break;
                    // case CANCOM_SCOPE_SET_MODE :break;
                default:
                    break;
                }

                if (rxFrame.exId.id == CAN_ID_DEBUG_UI)
                    isAFrame = true;
            }
        }

        // 以下部分是处理应答的
        switch (runStep)
        {

        case STEP_CONFIG:
        case STEP_INCONFIG:
        case STEP_INIT:
            if (pack.contains("OK"))
                hasAcked = true;
            break;
        case STEP_IN_OTA:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_START) && (rxFrame.exId.data == devCanId))
                hasAcked = true;
            break;
        case STEP_SEND_INFO:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_INFO) && (rxFrame.exId.data == devCanId))
            {
                hasAcked = true;
                otaStatus = OTA_STA_OTA_INFO_SUCCESS;
                emit sendState();
                setCmd(STEP_SEND_BIN);
            }
            break;
        case STEP_SEND_BIN:

            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_ING))
            {
                hasAcked = true;
                if (rxFrame.exId.data == devCanId)
                {
                    PackCnt++;
                    otaStatus = OTA_STA_SEND_BIN_OK;
                }
                else if (rxFrame.exId.data == ((quint16)devCanId | 0X0F00))
                {
                    memcpy(&PackCnt, rxFrame.data, 2);
                    otaStatus = OTA_STA_SEND_BIN_RETRY;
                }

                if (PackCnt >= PackNumber)
                {
                    otaStatus = OTA_STA_SEND_BIN_DONE;
                    setCmd(STEP_SEND_END);
                }
                else
                {
                    setCmd(STEP_SEND_BIN);
                }

                emit sendState();
            }
            break;

        case STEP_SEND_END:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_END))
            {
                hasAcked = true;
                if (rxFrame.exId.data == devCanId)
                {
                    otaStatus = OTA_STA_SUCCESS;
                }
                else if (rxFrame.exId.data == ((quint16)devCanId | 0X0F00))
                {
                    memcpy(&PackCnt, rxFrame.data, 2);
                    otaStatus = OTA_STA_FAILED;
                }
                emit sendState();
            }
            break;

        case STEP_SET_CAN_ID:
            if (rxFrame.exId.mode == CANCOM_ANNOUNCE_DEVID)
                hasAcked = true;
            break;

        case STEP_SET_RST_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_RESET))
                hasAcked = true;
            break;

        case STEP_SET_MOTOR_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && ((mtStatus.mode == MT_MODE_MOTOR) ||(mtStatus.mode == MT_MODE_MOTOR_4) ||(mtStatus.mode == MT_MODE_MOTOR_248) ||(mtStatus.mode == MT_MODE_MOTOR_253)||(mtStatus.mode==MT_MODE_MOTOR_17)))
                hasAcked = true;
            break;

        case STEP_SET_BRAKE_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_BRAKE))
                hasAcked = true;
            break;

        case STEP_SET_CALI_ENCODER_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_CALI))
                hasAcked = true;
            break;

        case STEP_SET_ZERO_MECH_MODE:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (posGet < 0.01f) && (posGet > -0.01f))
                hasAcked = true;
            break;

        case STEP_ASK_PARA_STR_INFO:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_STR_INFO))
                hasAcked = true;
            break;

        case STEP_READ_PARA_STR:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_READ))
                hasAcked = true;
            break;

        case STEP_WRITE_PARA_STR:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_WRITE))
                hasAcked = true;
            break;

        case STEP_SCOPE_CONFIG:
            if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_UPDATE))
                hasAcked = true;
            break;

        default:
            break;
        }
    }
}

// run function
void otaThread::run()
{
    struct canPack pack;
    // qDebug("otaThread run!");

    while (true)
    {
        switch (runStep)
        {

        case STEP_INIT:
            msleep(20);
            UseTimeMs += 20;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_CONNECT_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 500)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_CONNECT_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_INCONFIG:
            msleep(20);
            UseTimeMs += 20;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_INCONFIG_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 200)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_INCONFIG_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_CONFIG:
            msleep(20);
            UseTimeMs += 20;

            if (hasAcked)
            {
                configStep = (enum canATmode)((quint8)configStep + 1);

                if (configStep == AT_CONFIG_END)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_CONFIG_SUCCESS;
                    emit sendState();
                }
                else
                    setCmd(STEP_CONFIG);
            }
            else if (UseTimeMs > 500)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_CONFIG_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_IN_OTA:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_SEND_INFO;
                otaStatus = OTA_STA_IN_OTA_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 8000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_IN_OTA_MODE_OVER_TIME;
                emit sendState();
            }
            break;
        case STEP_SEND_INFO:

            msleep(100);
            UseTimeMs += 100;

            if ((hasAcked == false) && (UseTimeMs > 2000))
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_OTA_INFO_OVER_TIME;
                emit sendState();
            }

            break;

        case STEP_SET_CAN_ID:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_SET_ID_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 2000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_SET_ID_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_SET_RST_MODE:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_RST_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 2000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_RST_MODE_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_SET_CALI_ENCODER_MODE:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_CALI_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 2000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_CALI_MODE_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_SET_ZERO_MECH_MODE:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_ZERO_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 2000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_ZERO_MODE_OVER_TIME;
                emit sendState();
            }
            break;
        case STEP_SET_MOTOR_MODE:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_MOTOR_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 2000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_MOTOR_MODE_OVER_TIME;
                emit sendState();
            }
            break;
        case STEP_SET_BRAKE_MODE:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_BRAKE_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 2000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_BRAKE_MODE_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_GET_DEV_INFO:
#define SUB_SCAN_MAX (16)
#define MAIN_SCAN_MAX (128 / SUB_SCAN_MAX)

            if ((subScanCount % SUB_SCAN_MAX) == 0)
            {
                subScanCount = 0;
                mainScanCount++;
            }
            scanId = subScanCount * MAIN_SCAN_MAX + mainScanCount;
            subScanCount++;

            // qDebug()<<"scanId"<<scanId<<" sub:"<<subScanCount<<" main:"<<mainScanCount;

            if (scanId > CAN_ID_MOTOR_DEFAULT)
            {
                runStep = STEP_STOP;
            }
            else
            {

                pack.exId.id = scanId;
                pack.exId.data = CAN_ID_DEBUG_UI;       /* 0xFD */
                pack.exId.mode = CANCOM_ANNOUNCE_DEVID; /* 0 */
                pack.len = 8;
                for (quint8 i = 0; i < pack.len; ++i)
                    pack.data[i] = 0;
                txdPack(&pack);
                msleep(4);
            }
            break;
        case STEP_ASK_PARA_STR_INFO:

            msleep(50);
            UseTimeMs += 50;

            if (hasAcked)
            {
                hasAcked = false;
                if (paraItemBak == PARA_DONE)
                    runStep = STEP_STOP;
                else
                    UseTimeMs = 0;
            }
            else if (UseTimeMs > 1000)
            {
                runStep = STEP_STOP;
                if (paraErrorCode)
                    otaStatus = OTA_STA_GET_PARA_INFO_ERROR;
                else
                    otaStatus = OTA_STA_GET_PARA_INFO_OVER_TIME;
                emit sendState();
            }
            break;
        case STEP_READ_PARA_STR:

            msleep(50);
            UseTimeMs += 50;

            if (hasAcked)
            {
                hasAcked = false;
                UseTimeMs = 0;
            }
            else if (UseTimeMs > 1000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_READ_PARA_OVER_TIME;
                emit sendState();
            }
            break;
        case STEP_SCOPE_CONFIG:
            msleep(50);
            UseTimeMs += 50;

            if (hasAcked)
            {
                hasAcked = false;
                UseTimeMs = 0;
            }
            else if (UseTimeMs > 1000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_PARA_UPDATE_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_WRITE_PARA_STR:

            msleep(50);
            UseTimeMs += 100;

            if (hasAcked)
            {
                hasAcked = false;
                UseTimeMs = 0;
            }
            else if (UseTimeMs > 1000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_WRITE_PARA_OVER_TIME;
                emit sendState();
            }
            break;

        case STEP_MOTOR_CTRL_CONTINUOUS:
            msleep(100);

            pack.exId.id = devCanId;
            pack.len = 8;
            pack.exId.mode = CANCOM_MOTOR_CTRL;

            if(is_waterproof_motor){
                kpIntSet = float_to_uint(kpSet, KP_MIN, KP_MAX, 8);
                kdIntSet = float_to_uint(kdSet, KD_MIN, KD_MAX, 8);
                kiIntSet = float_to_uint(kiSet, KI_MIN, KI_MAX, 8);
                torqueIntSet = (int)(torqueSet * 10);//float_to_uint(torqueSet, T_MIN, T_MAX, 16);
            }
            else{
                kpIntSet = float_to_uint(kpSet, KP_MIN, KP_MAX, 16);
                kdIntSet = float_to_uint(kdSet, KD_MIN, KD_MAX, 16);
                kiIntSet = float_to_uint(kiSet, KI_MIN, KI_MAX, 16);
                torqueIntSet = float_to_uint(torqueSet, T_MIN, T_MAX, 16);
            }

            pack.exId.data = torqueIntSet;

            pack.data[0] = posIntSet >> 8;
            pack.data[1] = posIntSet & 0X00FF;
            pack.data[2] = velIntSet >> 8;
            pack.data[3] = velIntSet & 0X00FF;
            pack.data[4] = kpIntSet & 0X00FF;
            pack.data[5] = kdIntSet & 0X00FF;
            pack.data[6] = kiIntSet & 0X00FF;

            txdPack(&pack);

            break;

        case STEP_SCOPE_STOP:

            msleep(2);

            if (scopeIngFlag)
            {
                // for(quint64 i = 0XFFFFFFFFFFFFFF;i>0;i--)
                //     for(quint32 j = 0XFFF;j>0;j--);

                setCmd(STEP_SCOPE_STOP);
            }
            else
                runStep = STEP_STOP;

            break;

        default:
            UseTimeMs = 0;
            break;
        }
    }
}

void otaThread::rxdPack_ANNOUNCE_DEVID()
{
    memcpy(&mcuId, &rxFrame.data[0], 8);
    mcuCanId = (quint8)rxFrame.exId.data;

    quint8 i;
    for (i = 0; i < mcuBuf.point; i++)
    {
        if (mcuId == mcuBuf.id[i])
        {
            if (mcuBuf.canId[i] != mcuCanId)
            {
                mcuBuf.canId[i] = mcuCanId;
                mcuBuf.changeFlag = true;
            }
            break;
        }
    }

    if ((i == mcuBuf.point) && (mcuBuf.point < MCU_NUM))
    {
        mcuBuf.id[mcuBuf.point] = mcuId;
        mcuBuf.canId[mcuBuf.point] = mcuCanId;
        mcuBuf.point++;
        mcuBuf.changeFlag = true;
    }

    // qDebug()<<"mcuId:"<<QString::number(mcuId,16);
    otaStatus = OTA_STA_DETECT_MCU;
    emit sendState();
}

void otaThread::rxdPack_MOTOR_FEEDBACK()
{
    if (canFrameType == FRAME_EXD)
    {
        if (rxFrame.len != 6)
            return;
    }
    else
    {
        if (rxFrame.len != 8)
            return;
    }

    canIdGet = rxFrame.exId.data & 0X00FF;

    if(is_waterproof_motor){
        switch((rxFrame.exId.data & 0x700) >> 8){
        case 0:
            mtStatus.underVoltFault = false;
            mtStatus.overCurFault = false;
            mtStatus.overTempFault = false;
            mtStatus.encoderFault = false;
            mtStatus.hallFault = false;
            mtStatus.noCaliFault = false;
            break;
        case 1:
            mtStatus.underVoltFault = true;
            break;
        case 2:
            mtStatus.overCurFault = true;
            break;
        case 3:
            mtStatus.overTempFault = true;
            break;
        case 4:
            mtStatus.encoderFault = true;
            break;
        case 5:
            mtStatus.hallFault = true;
            break;
        case 6:
            mtStatus.noCaliFault = true;
            break;
        default:
            break;
        }
        mtStatus.mode = (enum motorMode)((rxFrame.exId.data & 0XF800) >> 11);
    }
    else{
        mtStatus.underVoltFault = (rxFrame.exId.data & 0X0100) ? true : false;
        mtStatus.overCurFault = (rxFrame.exId.data & 0X0200) ? true : false;
        mtStatus.overTempFault = (rxFrame.exId.data & 0X0400) ? true : false;
        mtStatus.encoderFault = (rxFrame.exId.data & 0X0800) ? true : false;
        mtStatus.hallFault = (rxFrame.exId.data & 0X1000) ? true : false;
        mtStatus.noCaliFault = (rxFrame.exId.data & 0X2000) ? true : false;

        mtStatus.mode = (enum motorMode)((rxFrame.exId.data & 0XC000) >> 14);
    }

    // if((rxFrame.exId.data&0XC000)==0X4000)
    //     mtStatus.mode  = MT_MODE_CALI;
    // else if((rxFrame.exId.data&0XC000)==0X8000)
    //     mtStatus.mode  = MT_MODE_MOTOR;
    // else
    //     mtStatus.mode  = MT_MODE_RESET;

    posIntGet = ((int)rxFrame.data[0] << 8) | rxFrame.data[1];
    velIntGet = ((int)rxFrame.data[2] << 8) | rxFrame.data[3];
    torqueIntGet = ((int)rxFrame.data[4] << 8) | rxFrame.data[5];

    posGet = uint_to_float(posIntGet, P_MIN, P_MAX, 16);
    velGet = uint_to_float(velIntGet, V_MIN, V_MAX, 16);
    torqueGet = uint_to_float(torqueIntGet, T_MIN, T_MAX, 16);

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

void otaThread::rxdPack_FAULT_WARN()
{
    if (rxFrame.len != 8)
        return;
    canIdGet = rxFrame.exId.data & 0X00FF;

    memcpy(&faultSta, &rxFrame.data[0], 4);
    memcpy(&warnSta, &rxFrame.data[4], 4);

    otaStatus = OTA_STA_MOTOR_FAULTWARN;

    emit sendState();
}

void otaThread::rxdPack_CALI_ING()
{
    memcpy(&currentAngle, &rxFrame.data[0], 4);
    memcpy(&rotorAngle, &rxFrame.data[4], 4);
    caliRaw = rxFrame.exId.data;

    otaStatus = OTA_STA_ENCODER_CALI_ING;
    emit sendState();
}

void otaThread::rxdPack_CALI_RST()
{
    canIdGet = rxFrame.exId.data & 0X00FF;
    memcpy(&caliI, &rxFrame.data[0], 2);
    memcpy(&caliInd, &rxFrame.data[2], 2);
    memcpy(&caliLut, &rxFrame.data[4], 4);

    otaStatus = OTA_STA_ENCODER_CALI_RESULT;
    emit sendState();
}

void otaThread::rxdPack_HALLCALI_ING()
{
    quint8 tempU8;
    quint16 tempU16;
    canIdGet = rxFrame.exId.data & 0X00FF;
    tempU8 = (rxFrame.exId.data & 0XFF00) >> 8;

    memcpy(&tempU16, &rxFrame.data[0], 2);

    if (tempU8 == 0)
    {
        hallCount = tempU16;
        memcpy(&hallRaw[0], &rxFrame.data[2], 6);
    }
    else if ((tempU8 == 1) && (tempU16 == hallCount))
    {
        memcpy(&hallRaw[3], &rxFrame.data[2], 6);
        otaStatus = OTA_STA_HALL_CALI_ING;
        emit sendState();
    }
}

void otaThread::rxdPack_HALLCALI_RST()
{
    quint8 tempU8;
    canIdGet = rxFrame.exId.data & 0X00FF;
    tempU8 = (rxFrame.exId.data & 0XFF00) >> 8;

    if (tempU8 == 1)
    {
        memcpy(&hallCount, &rxFrame.data[0], 2);
        memcpy(&hallRdLval, &rxFrame.data[2], 2);
        otaStatus = OTA_STA_HALL_CALI_RESULT1;
        emit sendState();
    }
    else if ((tempU8 == 2) && (rxFrame.data[1] < 6)) //&&(rxFrame.data[1]>=0)
    {
        if (rxFrame.data[0] == 0)
        {
            hallCount = rxFrame.data[1];
            memcpy(&hallInfo[hallCount], &rxFrame.data[2], 6);
        }
        else if ((rxFrame.data[0] == 1) && (rxFrame.data[1] == hallCount))
        {
            memcpy(&hallInfo[hallCount].maxPos, &rxFrame.data[2], 6);
            otaStatus = OTA_STA_HALL_CALI_RESULT2;
            emit sendState();
        }
    }
}

void otaThread::rxdPack_MOTOR_CALI()
{
    quint8 tempU8;
    canIdGet = rxFrame.exId.data & 0X00FF;
    tempU8 = (rxFrame.exId.data & 0XFF00) >> 8;
    if (tempU8 == 0)
    {
        calerrorcode = 0;
        memcpy(&caliOffset, &rxFrame.data[0], 4);
        memcpy(&caliMSE, &rxFrame.data[4], 4);
        otaStatus = OTA_STA_ENCODER_CALI_DONE;
        emit sendState();
    }
    else if (tempU8 == 1)
    {
        for (quint8 i = 0; i < 6; i++)
        {
            hallInfo[i].fault = (rxFrame.data[i] & 0XF0) >> 4;
            hallInfo[i].warn = rxFrame.data[i] & 0X0F;
        }
        hallError = rxFrame.data[6];
        hallWarn = rxFrame.data[7];
        otaStatus = OTA_STA_HALL_CALI_DONE;
        emit sendState();
    }
}

void otaThread::rxdPack_MOTOR_SWEEP()
{
        memcpy(&amplitude, &rxFrame.data[0], 4);
        memcpy(&phase, &rxFrame.data[4], 4);
         frequency = rxFrame.exId.data;   //频率索引
        // 发出信号通知界面更新
        otaStatus = OTA_STA_SWEEP_RESULT;
        qDebug() << "123" << endl;
        emit sendState();
}
void otaThread::rxdPack_PARA_STR_INFO()
{
    quint16 tempU16;
    canIdGet = rxFrame.exId.data & 0X00FF;
    enum canParaItemType item = (enum canParaItemType)((rxFrame.exId.data & 0XFF00) >> 8);
    memcpy(&tempU16, &rxFrame.data[0], 2);
    enum paraStrItemType type = (enum paraStrItemType)((tempU16 & 0XF000) >> 12);
    quint16 point = (quint16)(tempU16 & 0X0FFF);
    quint16 paraCode = 0;

    qint16 maxS16, minS16;
    qint8 ratio;
    float coef = 1.0f;

    if (paraPointBak != point)
        paraCode |= PARA_ERROR_POINT;
    else if (paraTypeBak != type)
        paraCode |= PARA_ERROR_TYPE;
    else if (point >= paraNumTab[type])
        paraCode |= PARA_ERROR_NUM;

    switch (item)
    {
    case PARA_NAME0:
        paraCode = 0;
        memset(&p_buf[0], 0, 18);
        memcpy(&p_buf[0], &rxFrame.data[2], 6);
        break;
    case PARA_NAME1:
        memcpy(&p_buf[6], &rxFrame.data[2], 6);
        if (paraItemBak != PARA_NAME0)
            paraCode |= PARA_ERROR_ITEM;
        break;
    case PARA_NAME2:
        memcpy(&p_buf[12], &rxFrame.data[2], 6);
        p_buf[STR_NAME_LEN] = 0;
        if (paraItemBak != PARA_NAME1)
            paraCode |= PARA_ERROR_ITEM;
        else if (point < paraNumTab[type])
        {
            switch (type)
            {
            case WRITE_STR:
                paraInfo.writeStr[point].name = QString(QLatin1String(p_buf));
                break;
            case READ_STR:
                paraInfo.readStr[point].name = QString(QLatin1String(p_buf));
                break;
            case WRITE_PARA:
                paraInfo.writePara[point].name = QString(QLatin1String(p_buf));
                break;
            case READ_PARA:
                paraInfo.readPara[point].name = QString(QLatin1String(p_buf));
                break;
            default:
                paraCode |= PARA_ERROR_ITEM_MATCH;
                break;
            }
        }
        break;
    case PARA_ATTR:
        paraCode = 0;
        if (point < paraNumTab[type])
        {
            switch (type)
            {
            case WRITE_PARA:
                paraInfo.writePara[point].type = (enum paraType)(rxFrame.data[7] & 0X1F);
                paraInfo.writePara[point].wPerm = (rxFrame.data[7] & 0XE0) >> 5;
                memcpy(&minS16, &rxFrame.data[2], 2);
                memcpy(&maxS16, &rxFrame.data[4], 2);
                ratio = (qint8)rxFrame.data[6];
                paraInfo.writePara[point].prec = (ratio < 0) ? (-ratio) : 0;

                coef = 1.0f;
                while (ratio != 0)
                {
                    if (ratio > 0)
                    {
                        ratio--;
                        coef *= 10.0f;
                    }
                    else
                    {
                        ratio++;
                        coef /= 10.0f;
                    }
                }

                paraInfo.writePara[point].max = coef * maxS16;
                paraInfo.writePara[point].min = coef * minS16;
                break;
            case READ_PARA:
                paraInfo.readPara[point].type = (enum paraType)(rxFrame.data[7] & 0X1F);
                break;
            default:
                paraCode |= PARA_ERROR_ITEM_MATCH;
                break;
            }
        }
        break;
    case PARA_VAL0:
        paraCode = 0;
        if (point < paraNumTab[type])
        {
            switch (type)
            {
            case WRITE_PARA:
                paraInfo.writePara[point].type = (enum paraType)rxFrame.data[3];
                memcpy(&(paraInfo.writePara[point].valR[0]), &rxFrame.data[4], 4);
                break;
            case READ_PARA:
                paraInfo.readPara[point].type = (enum paraType)rxFrame.data[3];
                memcpy(&(paraInfo.readPara[point].valR[0]), &rxFrame.data[4], 4);
                break;
            default:
                paraCode |= PARA_ERROR_ITEM_MATCH;
                break;
            }
        }
        break;
    case PARA_VAL1:
        if (point < paraNumTab[type])
        {
            switch (type)
            {
            case WRITE_PARA:
                memcpy(&(paraInfo.writePara[point].valR[4]), &rxFrame.data[4], 4);
                break;
            case READ_PARA:
                memcpy(&(paraInfo.readPara[point].valR[4]), &rxFrame.data[4], 4);
                break;
            default:
                paraCode |= PARA_ERROR_ITEM_MATCH;
                break;
            }
        }
        break;
    case PARA_STR0:
        paraCode = 0;
        memset(&p_buf[0], 0, 18);
        memcpy(&p_buf[0], &rxFrame.data[2], 6);
        break;
    case PARA_STR1:
        memcpy(&p_buf[6], &rxFrame.data[2], 6);
        if (paraItemBak != PARA_STR0)
            paraCode |= PARA_ERROR_ITEM;
        break;
    case PARA_STR2:
        memcpy(&p_buf[12], &rxFrame.data[2], 6);
        p_buf[STR_NAME_LEN] = 0;
        if (paraItemBak != PARA_STR1)
            paraCode |= PARA_ERROR_ITEM;
        else if (point < paraNumTab[type])
        {
            switch (type)
            {
            case WRITE_STR:
                paraInfo.writeStr[point].str = QString(QLatin1String(p_buf));
                break;
            case READ_STR:
                paraInfo.readStr[point].str = QString(QLatin1String(p_buf));
                break;
            default:
                paraCode |= PARA_ERROR_ITEM_MATCH;
                break;
            }
        }
        break;
    case PARA_DONE:
        paraCode = 0;
        paraInfo.echoNum = rxFrame.data[0];
        paraInfo.meaTimerNum = rxFrame.data[1];
        tempU16 = rxFrame.data[2];
        if (tempU16 != paraInfo.numTab[WRITE_STR])
            paraErrorCode |= PARA_ERROR_WRITE_STR_NUM_MATCH;
        tempU16 = rxFrame.data[3];
        if (tempU16 != paraInfo.numTab[READ_STR])
            paraErrorCode |= PARA_ERROR_READ_STR_NUM_MATCH;
        memcpy(&tempU16, &rxFrame.data[4], 2);
        if (tempU16 != paraInfo.numTab[WRITE_PARA])
            paraErrorCode |= PARA_ERROR_WRITE_PARA_NUM_MATCH;
        memcpy(&tempU16, &rxFrame.data[6], 2);
        if (tempU16 != paraInfo.numTab[READ_PARA])
            paraErrorCode |= PARA_ERROR_READ_PARA_NUM_MATCH;

        if (paraErrorCode)
            otaStatus = OTA_STA_GET_PARA_INFO_ERROR;
        else
            otaStatus = OTA_STA_GET_PARA_INFO_SUCCESS;
        emit sendState();

        hasAcked = true;
        paraItemBak = item;
        return;
    default:
        paraCode |= PARA_ERROR_NO_TYPE;
        break;
    }

    if (point > paraInfo.numTab[type])
    {
        paraCode |= PARA_ERROR_POINT_CONT;
    }
    else if (point == paraInfo.numTab[type])
    {
        paraInfo.numTab[type]++;
    }

    if (paraCode)
    {
        // qDebug()<<"error:"<<QString::number(paraCode,16)
        //         <<" type:"<<QString::number(type,10)<<" typeBak:"<<QString::number(paraTypeBak,10)
        //         <<" item:"<<QString::number(item,10)<<" itemBak:"<<QString::number(paraItemBak,10)
        //         <<" point:"<<QString::number(point,10)<<" pointBak:"<<QString::number(paraPointBak,10)
        //           ;
        paraErrorCode |= paraCode;
    }

    paraPointBak = point;
    paraItemBak = item;
    paraTypeBak = type;
}

// 读取参数
void otaThread::rxdPack_PARA_READ()
{
    if (runStep != STEP_READ_PARA_STR)
        return;

    quint16 code;
    quint16 errorCode = (((rxFrame.exId.data & 0XFF00) >> 8) == 1) ? PARA_READ_ERROR_CODE : 0;
    enum paraType type = (enum paraType)rxFrame.data[2];

    quint8 indexMax = (paraByteNumTab[type] / 4) + ((paraByteNumTab[type] % 4) ? 1 : 0);
    quint8 index = rxFrame.data[3];
    memcpy(&code, &rxFrame.data[0], 2);

    if (code != paraInfo.codeNeed)
        errorCode |= PARA_READ_ERROR_NOT_NEED;
    else if (
        (((paraInfo.itemIndex == WRITE_STR) || (paraInfo.itemIndex == READ_STR)) && (type != ParaType_STRING)) || ((paraInfo.itemIndex == WRITE_PARA) && (type != paraInfo.writePara[paraInfo.posIndex].type)) || ((paraInfo.itemIndex == READ_PARA) && (type != paraInfo.readPara[paraInfo.posIndex].type)))
        errorCode |= PARA_READ_ERROR_TYPE;
    else if ((index != indexBak) || (index >= indexMax))
        errorCode |= PARA_READ_ERROR_INDEX;
    else
    {
        memcpy(&p_buf[index * 4], &rxFrame.data[4], 4);
        indexBak++;

        if (indexBak >= indexMax)
        {
            p_buf[STR_NAME_LEN] = 0;
            switch (paraInfo.itemIndex)
            {
            case WRITE_STR:
                paraInfo.writeStr[paraInfo.posIndex].str = QString(QLatin1String(p_buf));
                break;
            case READ_STR:
                paraInfo.readStr[paraInfo.posIndex].str = QString(QLatin1String(p_buf));
                break;
            case WRITE_PARA:
                memcpy(&(paraInfo.writePara[paraInfo.posIndex].valR[0]), p_buf, paraByteNumTab[type]);
                break;
            case READ_PARA:
                memcpy(&(paraInfo.readPara[paraInfo.posIndex].valR[0]), p_buf, paraByteNumTab[type]);
                break;
            default:
                errorCode |= PARA_READ_ERROR_CODE;
                break;
            }
        }
        else
            return;
    }

    if (errorCode)
    {
        paraErrorCode = errorCode;
        otaStatus = OTA_STA_READ_PARA_ERROR;
    }
    else
    {
        otaStatus = OTA_STA_READ_PARA_SUCCESS;
    }

    runStep = STEP_STOP;

    emit sendState();
}

// 写参数
void otaThread::rxdPack_PARA_Write()
{
    paraInfo.writeCmdBack = (enum paraWriteCmdType)((rxFrame.exId.data & 0X0300) >> 8);
    paraInfo.writeErrorCode = (enum paraWriteErrorCode)((rxFrame.exId.data & 0XFC00) >> 10);

    quint16 code;

    memcpy(&code, &rxFrame.data[0], 2);

    if (paraInfo.writeCmdBack != paraInfo.writeCmd)
    {
        paraErrorCode |= PARA_WR_ERROR_CMD_MATCH;
    }
    else if ((paraInfo.codeNeed != code) && (paraInfo.writeCmdBack != WRITE_CMD_SAVE_ONLY))
    {
        paraErrorCode |= PARA_WR_ERROR_CODE_MATCH;
    }
    else if (rxFrame.data[2] != paraInfo.typeNeed)
    {
        paraErrorCode |= PARA_WR_ERROR_TYTE;
    }
    else if (rxFrame.data[3] != paraInfo.wIndex)
    {
        paraErrorCode |= PARA_WR_ERROR_FRAME;
    }
    else if (paraInfo.writeErrorCode)
    {
        paraErrorCode |= (1 << paraInfo.writeErrorCode);
    }

    if (paraErrorCode)
    {
        otaStatus = OTA_STA_WRITE_PARA_ERROR;
    }
    else if (paraInfo.writeCmd == WRITE_CMD_SAVE_ONLY)
    {
        otaStatus = OTA_STA_SAVE_PARA_SUCCESS;
    }
    else if (paraInfo.wLen)
    {
        paraInfo.wIndex++;

        struct canPack pack;
        memset(&pack, 0, sizeof(struct canPack));
        pack.len = 8;
        pack.exId.id = devCanId;

        pack.exId.data = CAN_ID_DEBUG_UI | (((quint16)paraInfo.writeCmd) << 8);
        pack.exId.mode = CANCOM_PARA_WRITE;
        memcpy(&(pack.data[0]), &(paraInfo.codeNeed), 2);
        pack.data[2] = paraInfo.typeNeed;
        pack.data[3] = paraInfo.wIndex;
        quint8 txLen = (paraInfo.wLen >= 4) ? 4 : paraInfo.wLen;
        paraInfo.wLen -= txLen;
        memcpy(&(pack.data[4]), &p_buf[paraInfo.wOffset], txLen);
        paraInfo.wOffset += txLen;
        txdPack(&pack);
        return;
    }
    else
    {
        otaStatus = OTA_STA_WRITE_PARA_SUCCESS;
    }

    runStep = STEP_STOP;

    emit sendState();
}

// 根据编码索引找编号
// uart2can.selCh_index2code(indexTab[i],&uart2can.chSelCode[i],&uart2can.chTypeNeed[i]);
void otaThread::selCh_index2code(quint16 index, quint16 *pCode, enum paraType *pType)
{
    if (index < 1) // 下拉框的选择索引
    {
        *pCode = 0;
        *pType = ParaType_U8;
        return;
    }

    index = index + paraInfo.echoNum; // 传入的是下拉框的索引。必须加上一个偏置

    if (index < paraInfo.numTab[WRITE_PARA]) // 这应该是读写的都可以用来输出，所以这个偏执应该是总数据区的长度
    {
        *pCode = (((quint16)WRITE_PARA << 12) | index); // 输出可写的参数
        *pType = paraInfo.writePara[index].type;
    }
    else if (index < (paraInfo.numTab[WRITE_PARA] + paraInfo.numTab[READ_PARA]))
    {
        *pCode = (((quint16)READ_PARA << 12) | (index - paraInfo.numTab[WRITE_PARA])); // 输出只读参数
        *pType = paraInfo.readPara[index - paraInfo.numTab[WRITE_PARA]].type;
    }
    else // 否则给零
    {
        *pCode = 0;
        *pType = ParaType_U8;
    }
}

// 参数上传
void otaThread::rxdPack_PARA_UPDATE()
{
    quint8 cmd = (quint8)((rxFrame.exId.data & 0X0300) >> 8);  // 可能是模式两位有四种模式
    quint8 ack = (quint8)((rxFrame.exId.data & 0X0C00) >> 10); // 这是错误代码。10bit
    quint8 pos = (quint8)((rxFrame.exId.data & 0XF000) >> 12); // 这个是帧的序号。
    quint16 tempU16;

    quint8 i = 0;
    paraErrorCode = 0;

    runStep = STEP_STOP;

    switch (cmd)
    {
    case 0:
        memcpy(&chTickUs, &rxFrame.data[0], 4);        // 这个是时间是PWM周期
        memcpy(&chEchoDevFreMax, &rxFrame.data[4], 2); // 固定值最大可设置频率
        memcpy(&chEchoDevFreMin, &rxFrame.data[6], 2); // 固定值 1
        if (ack)                                       // 错误代码
        {
            paraErrorCode |= PARA_UPDATE_ERROR_FRESET;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
        }
        else
        {
            otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_FRESET;
        }
        break;
    case 1:
        chTickBak = 0;
        chTimeStamp = 0.0f;
        switch (ack)
        {
        case 0:
            if (chSelPos >= chSelNum)
            {
                for (i = 0; i < chSelNum; i++)
                    chTypeDev[chSelIndex[i]] = (enum paraType)((i % 2) ? ((rxFrame.data[i / 2] & 0XF0) >> 4) : (rxFrame.data[i / 2] & 0X0F)); // 发上来的是拼接的数据，这里还原，根据

                otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_CHSET;

                for (i = 0; i < CHANNAL_SEL_NUM; i++)
                    mData[i].clear();

                scopeShowNum = (quint32)(chEchoFreHz/ WAVE_SHOW_FRE)*2;  //減緩示波器ui刷新速率；

                if (scopeShowNum == 0)
                    scopeShowNum = 1;
                scopeShowCount = 0;
                scopeWindowNum = (quint32)(WAVE_TIME_WINDOW * chEchoFreHz*10);
                scopeWindowCount = 0;
                scopeWindowRemoveFlag = false;

                // qDebug()<<"scopeShowNum"<<QString::number(scopeShowNum,10)
                //  <<"  scopeWindowNum"<<QString::number(scopeWindowNum,10);

                // qDebug()<<"通道数量："<<QString::number(chSelNum,10);
                for (i = 0; i < chSelNum; i++)
                {
                    // qDebug()<<"needType:"<<QString::number(chTypeNeed[chSelIndex[i]],10)
                    //         <<"devType:"<<QString::number(chTypeDev[chSelIndex[i]],10);
                    if (chTypeDev[chSelIndex[i]] != chTypeNeed[chSelIndex[i]])
                    {
                        paraErrorCode |= PARA_UPDATE_ERROR_CHSET_TYPE;
                        otaStatus = OTA_STA_PARA_UPDATE_ERROR;
                    }
                }
            }
            else
            {
                setCmd(STEP_SCOPE_SET_CHANNAL);
                return;
            }
            break;
            // 下面是报错可以暂时不理会
        case 1:
            paraErrorCode |= PARA_UPDATE_ERROR_CHSET_NOCODE;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        case 2:
            paraErrorCode |= PARA_UPDATE_ERROR_CHSET_FRE;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        case 3:
        default:
            paraErrorCode |= PARA_UPDATE_ERROR_CHSET_LOSE;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        }
        break;
    case 2:
        scopeIngFlag = true; //
        if (ack)
        {
            paraErrorCode |= PARA_UPDATE_ERROR_START;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
        }
        else
        {
            tempU16 = pos * 8;                                // 起始地址
            memcpy(&chRxdByte[tempU16], &rxFrame.data[0], 8); // 接收到的字节全部放在这里

            tempU16 += 8; // 结束地址

            // qDebug()<<"pos:"<<QString::number(pos,10)<<" need"<<QString::number(chNeedByteTotal,10);

            if (tempU16 >= chNeedByteTotal)
            {
                // qDebug()<<"need:"<<QString::number(chNeedByteTotal,10);
                scope_dataProcess(); // 这里是把数据分配到每一个通道
            }

            return;
        }
        break;
    case 3:
        scopeIngFlag = false;
        otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_END;
        break;
    default:
        paraErrorCode |= PARA_UPDATE_ERROR_CMD;
        otaStatus = OTA_STA_PARA_UPDATE_ERROR;
        break;
    }
    emit sendState();
}

void otaThread::scope_dataProcess()
{
    quint16 tick;
    memcpy(&tick, &chRxdByte[0], 2);

    if (Burst_mode) {
        if (tick > chTickBak) {
            chTimeStamp += (tick - chTickBak) / 1000000.0f;
        } else {
            chTimeStamp += (65536 + tick - chTickBak) / 10000000000.0f;
        }
        chTickBak = tick;
    } else {
        if (tick > chTickBak) {
            // 修正：乘以2以解决时间慢1倍的问题
            chTimeStamp += 2 * (tick - chTickBak) * chTickUs / 10000000000.0f;
        } else {
            // 修正：乘以2以解决时间慢1倍的问题
            chTimeStamp += 2 * (65536 + tick - chTickBak) * chTickUs / 10000000000.0f;
        }
        chTickBak = tick;
    }


    quint16 pos = 2;

    double val;
    enum paraType type;

    QCPGraphData newPoint;
    newPoint.key = chTimeStamp;

    QString str = ""; // QString::asprintf("%14.6f",chTimeStamp);

    for (quint8 i = 0; i < chSelNum; i++)
    {
        QString pStr;
        type = chTypeNeed[chSelIndex[i]];
        switch (type)
        {
        case ParaType_U8:
            quint8 tempU8;
            memcpy(&tempU8, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempU8 * 1.0f;
            pStr = QString::asprintf("%4d", tempU8);
            break;
        case ParaType_S8:
            qint8 tempS8;
            memcpy(&tempS8, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempS8 * 1.0f;
            pStr = QString::asprintf("%5d", tempS8);
            break;
        case ParaType_U16:
            quint16 tempU16;
            memcpy(&tempU16, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempU16 * 1.0f;
            pStr = QString::asprintf("%6d", tempU16);
            break;
        case ParaType_S16:
            qint16 tempS16;
            memcpy(&tempS16, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempS16 * 1.0f;
            pStr = QString::asprintf("%7d", tempS16);
            break;
        case ParaType_U32:
            quint32 tempU32;
            memcpy(&tempU32, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempU32 * 1.0f;
            pStr = QString::asprintf("%10d", tempU32);
            break;
        case ParaType_S32:
            qint32 tempS32;
            memcpy(&tempS32, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempS32 * 1.0f;
            pStr = QString::asprintf("%11d", tempS32);
            break;
        case ParaType_FLOAT:
            float tempSf;
            memcpy(&tempSf, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempSf;
            pStr = QString::asprintf("%12.6f", tempSf);
            break;
        case ParaType_U64:
            quint64 tempU64;
            memcpy(&tempU64, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempU64 * 1.0f;
            pStr = QString::asprintf("%15l", tempU64);
            break;
        case ParaType_S64:
            qint64 tempS64;
            memcpy(&tempS64, &chRxdByte[pos], paraByteNumTab[type]);
            val = tempS64 * 1.0f;
            pStr = QString::asprintf("%16l", tempS64);
            break;
        case ParaType_DOUBLE:
            memcpy(&val, &chRxdByte[pos], paraByteNumTab[type]);
            pStr = QString::asprintf("%16.8f", val);
            break;
        default:
            val = 0.0f;
            pStr = "null";
            break;
        }

        str += pStr;

        // chParaTab[chSelIndex[i]] = val;
        pos += paraByteNumTab[type];

        newPoint.value = val;

        if (mData[chSelIndex[i]].size() == 1)
            mData[chSelIndex[i]].replace(0, newPoint);

        mData[chSelIndex[i]].append(newPoint);

        if (scopeWindowRemoveFlag)
            mData[chSelIndex[i]].removeFirst();
    }

    if (isWaveFileOpen)
    {
        str = QString::asprintf("%14.6f", chTimeStamp) + str + "\r\n";
        waveFile.write(str.toUtf8());
    }

    if (scopeWindowRemoveFlag == false)
    {
        if (++scopeWindowCount >= scopeWindowNum)
            scopeWindowRemoveFlag = true;
    }

    if (++scopeShowCount >= scopeShowNum)
    {
        scopeShowCount = 0;

        emit scopeToShow(mData);
    }
}
