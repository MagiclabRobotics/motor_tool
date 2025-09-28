#include "otaThread.h"
const qint16 otaThread::paraNumTab[PARA_STR_TOTAL] = {WRITE_STRING_NUM, READ_STRING_NUM, SET_PARA_NUM, STATUS_PARA_NUM}; // 各类型参数允许的最大数量
const qint8 otaThread::paraByteNumTab[ParaType_TOTAL] = {1, 1, 2, 2, 4, 4, 4, 8, 8, 8, STR_NAME_LEN};                    // 各类型参数的字节数

otaThread::otaThread() // 构造函数。
{
    runStep = STEP_STOP;

    RxData.clear();
    BinData.clear();

    PackNumber = 0;
    PackCnt = 0;

    mcuBuf.point = 0;
    mcuBuf.changeFlag = false;

    pos_tarSet = 0.0f;
    vel_tarSet = 0.0f;
    pos_kpSet = 0.0f;
    pos_kiSet = 0.0f;
    tor_tarSet = 0.0f;

    paraErrorCode = 0;
    scopeIngFlag = false;
    isWaveFileOpen = false;
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

/* 串口发送数据包 */
void otaThread::uartPack(struct uartPack *rawpack)
{
    QByteArray Pack("\0");

    Pack.resize(7 + rawpack->len);

    Pack[0] = 0xaa;
    Pack[1] = 0x55;
    Pack[2] = rawpack->len + 2;
    Pack[3] = rawpack->frame_id;
    Pack[4] = rawpack->mot_id;
    for (int i = 0; i < rawpack->len; i++)
    {
        Pack[5 + i] = rawpack->data[i];
    }

    int max = rawpack->len + 5;
    quint16 chkSum = 0;
    for (int i = 2; i < max; i++)
    {
        chkSum += (quint8)(Pack[i]);
    }
    Pack[rawpack->len + 5] = chkSum & 0xff;
    Pack[rawpack->len + 6] = ((chkSum & 0xff00) >> 8);
    // qDebug() << "chk:" << chkSum;

    emit WriteToSerial(Pack, (qint16)Pack.size());
}

// void otaThread::getSerialData(QByteArray temp)
// {
//     int pos;
//     QByteArray rxPack;

//     RxData.append(temp); // fzh20221031，b.append(a) 将a的值复制到b上的末节点；

//     pos = RxData.indexOf("\r\n");

//     while (pos >= 0)
//     {
//         rxPack = RxData.left(pos + 2); // fzh20221031，left(int n)是获取字符串左边n个字符，right(int n)是获取字符串右边n个字符。第二个参数是一个索引，表示函数的目标字符串是该索引之前的字符串
//         RxData.remove(0, pos + 2);     // fzh20221031，

//         analysisRxdDatas(rxPack);
//         // qDebug()<<"rxPack:"<<rxPack<<"  hex:"<<rxPack.toHex();
//         qDebug() << "rxHex:" << rxPack.toHex();

//         pos = RxData.indexOf("\r\n"); // fzh20221031，查询"\r\n"字符串之前还有多少剩余数据，只要还有剩余数据，则继续接收数据
//     }
// }

void otaThread::getSerialData(QByteArray temp)
{
    bool chk = false;

    RxData.append(temp);
    // qDebug() << "rxPart:" << temp.toHex();
    // qDebug() << "rxAll:" << RxData.toHex();

    while (1)
    {
        if (RxData.size() < 5) // 2 head + len + 2 crc
        {                      // 此时数据长度肯定不够
            return;
        }

        // 检查帧头，并删除之前数据
        for (int i = 0; i < (RxData.size() - 1); i++)
        {
            if (((static_cast<quint8>(RxData[i]) == (quint8)REC_HEAD1))         //
                && ((static_cast<quint8>(RxData[i + 1]) == (quint8)REC_HEAD2))) //
            {
                RxData.remove(0, i);
                chk = true;
                break;
            }
        }
        if (chk == false)
        { // 找不到正确帧头时，删到只剩两个数据
            RxData.remove(0, RxData.size() - 2);
            return;
        }

        if (RxData.size() < 5)
        { // 此时数据长度肯定不够
            return;
        }

        // 此时帧头正确
        int frame_len = RxData[2] + 5;
        if (((static_cast<quint8>(RxData[0]) == (quint8)REC_HEAD1))    //
            && ((static_cast<quint8>(RxData[1]) == (quint8)REC_HEAD2)) //
            && (RxData.size() >= frame_len))
        {
            // 可以提取数据了
            QByteArray frame = RxData.mid(0, frame_len);
            // qDebug() << "rxFrame:" << frame.toHex();
            analysisRxdDatas(frame);
            // 提取完后删除当前帧。
            RxData.remove(0, frame_len);

            // qDebug() << "*";
            continue;
        }
        return;
    }
    return;
}

// 设置运行模式
void otaThread::setCmd(enum canComStep mode)
{
    // 每次进入初始化
    struct uartPack uartdata;

    UseTimeMs = 0;
    hasAcked = false;
    // quint32 addr;

    // 设置指令前的预处理
    switch (mode)
    {
    case STEP_SEND_BIN: // OTA升级，这部分是传输代码部分

        // pack.exId.data = PackCnt;
        // pack.exId.mode = CANCOM_OTA_ING;

        // addr = PackCnt * 8;
        // for (uint8_t i = 0; i < 8; i++, addr++)
        // {
        //     if (addr < binSize)
        //         pack.data[i] = BinData[PackCnt * 8 + i];
        //     else
        //         pack.data[i] = 0XFF;
        // }
        // txdPack(&pack);

        break;

    case STEP_GET_DEV_INFO:
        scanId = 0;
        break;

    case STEP_SEND_END:
        // pack.exId.data = 0;
        // pack.exId.mode = CANCOM_OTA_END;
        // memcpy(&(pack.data[0]), &PackNumber, 4);
        // txdPack(&pack);
        break;

    case STEP_STOP:
        // RxData.clear();
        break;

    case STEP_IN_OTA: // OTA升级的第一步
        // pack.exId.data = CAN_ID_DEBUG_UI;
        // pack.exId.mode = CANCOM_OTA_START;
        // memcpy(&(pack.data[0]), &(mcuBuf.id[mcuBuf.usePos]), 8); // 这里储存的是单片机的64位识别码
        // txdPack(&pack);
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

            // memcpy(&(pack.data[0]), &binSize, 4);
            // memcpy(&(pack.data[4]), &PackNumber, 4);

            // pack.exId.data = CAN_ID_DEBUG_UI;
            // pack.exId.mode = CANCOM_OTA_INFO;
            // txdPack(&pack);
        }
        break;

    case STEP_SET_CAN_ID:
        // pack.exId.data = (((quint16)configCanId) << 8) | CAN_ID_DEBUG_UI;
        // pack.exId.mode = CANCOM_MOTOR_ID;
        // memcpy(&(pack.data[0]), &(mcuBuf.id[mcuBuf.usePos]), 8);

        // txdPack(&pack);
        break;

    case STEP_SET_RST_MODE:
        // pack.exId.data = CAN_ID_DEBUG_UI;
        // pack.exId.mode = CANCOM_MOTOR_RESET;
        // txdPack(&pack);
        uartdata.frame_id = UART_FRAME_MOTOR_RESET;
        uartdata.mot_id = devCanId;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;

    case STEP_SET_MOTOR_MODE:
        // pack.exId.data = CAN_ID_DEBUG_UI;
        // pack.exId.mode = CANCOM_MOTOR_IN;
        // txdPack(&pack);
        uartdata.frame_id = UART_FRAME_CTRL_SET_MODE;
        uartdata.mot_id = devCanId;
        uartdata.len = 1;
        {
            uartdata.data[0] = MotCtrlMode;
        }
        uartPack(&uartdata);
        break;

    case STEP_SET_CUSTOME_CMD:
        uartdata.frame_id = UART_FRAME_CUSTOME_CMD;
        uartdata.mot_id = devCanId;
        uartdata.len = 33;
        {
            uartdata.data[0] = cust_cmd;
            union typeConvert t;
            for (int i = 0; i < 8; i++)
            {
                t.f[0] = cust_cmd_para[i];
                for (int j = 0; j < 4; j++)
                {
                    uartdata.data[1 + 4 * i + j] = t.u8[j];
                }
            }
        }
        uartPack(&uartdata);
        break;

    case STEP_SET_BRAKE_MODE:
        uartdata.frame_id = UART_FRAME_MOTOR_BRAKE;
        uartdata.mot_id = devCanId;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;

    case STEP_SET_CALI_ENCODER_MODE:
        uartdata.frame_id = UART_FRAME_CALI_BEGIN;
        uartdata.mot_id = devCanId;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;

    case STEP_SET_ZERO_MECH_MODE:
        uartdata.frame_id = UART_FRAME_MOTOR_ZERO_MECH;
        uartdata.mot_id = devCanId;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;

    case STEP_MOTOR_CTRL_SINGLE:
        // qDebug() << "pos_tarSet: " << pos_tarSet //
        //         << "vel_tarSet: " << vel_tarSet //
        //         << "pos_kpSet: " << pos_kpSet //
        //         << "pos_kiSet: " << pos_kiSet //
        //         << "tor_tarSet: " << tor_tarSet << endl;

        union typeConvert t;
        uartdata.frame_id = UART_FRAME_CTRL_SET_PARA;
        uartdata.mot_id = devCanId;
        uartdata.len = 36;
        t.f[0] = pos_tarSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[i] = t.u8[i];
        }
        t.f[0] = pos_kpSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[4 + i] = t.u8[i];
        }
        t.f[0] = pos_kiSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[8 + i] = t.u8[i];
        }

        t.f[0] = vel_tarSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[12 + i] = t.u8[i];
        }
        t.f[0] = vel_kpSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[16 + i] = t.u8[i];
        }
        t.f[0] = vel_kiSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[20 + i] = t.u8[i];
        }

        t.f[0] = tor_tarSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[24 + i] = t.u8[i];
        }
        t.f[0] = tor_kpSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[28 + i] = t.u8[i];
        }
        t.f[0] = tor_kiSet;
        for (int i = 0; i < 4; i++)
        {
            uartdata.data[32 + i] = t.u8[i];
        }

        uartPack(&uartdata);
        break;

    case STEP_MOTOR_CTRL_CONTINUOUS:
        break;

    case STEP_ASK_PARA_STR_INFO:
        memset(paraInfo.numTab, 0, 2 * PARA_STR_TOTAL);
        paraErrorCode = 0;

        uartdata.frame_id = UART_FRAME_PARA_STR_INFO;
        uartdata.mot_id = devCanId;
        // uartdata.mot_id = 0xaa;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;

    case STEP_READ_PARA_STR_START:
        paraInfo.posIndex = 0;
        paraInfo.itemIndex = (enum paraStrItemType)0;
        err_cnt = 0;
        mode = STEP_READ_PARA_STR;

    case STEP_READ_PARA_STR:
        paraErrorCode = 0;
        // indexBak = 0;
        paraInfo.codeNeed = (((quint16)paraInfo.itemIndex) << 12) | (paraInfo.posIndex & 0X0FFF);

        uartdata.frame_id = UART_FRAME_PARA_READ;
        uartdata.mot_id = devCanId;
        uartdata.len = 2;
        uartdata.data[0] = (paraInfo.codeNeed & 0xff00) >> 8;
        uartdata.data[1] = paraInfo.codeNeed & 0x00ff;
        uartPack(&uartdata);
        break;

    case STEP_WRITE_PARA_STR_START:
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
            // paraInfo.wLen = paraByteNumTab[paraInfo.typeNeed];

            uartdata.frame_id = UART_FRAME_PARA_WRITE;
            uartdata.mot_id = devCanId;
            uartdata.len = 19;
            uartdata.data[0] = (paraInfo.codeNeed & 0xff00) >> 8;
            uartdata.data[1] = paraInfo.codeNeed & 0x00ff;
            uartdata.data[2] = paraInfo.typeNeed;
            for (int i = 0; i < 16; i++)
            {
                uartdata.data[3 + i] = p_buf[i];
            }
            uartPack(&uartdata);
        }
        break;
    case STEP_SAVE_PARA_STR:
        paraErrorCode = 0;
        paraInfo.writeErrorCode = PARA_WRITE_ERROR_NONE;
        {
            uartdata.frame_id = UART_FRAME_PARA_SAVE;
            uartdata.mot_id = devCanId;
            uartdata.len = 0;
            uartPack(&uartdata);
        }
        break;
    case STEP_RECOVER_FAC:
        mode = STEP_STOP;
        paraErrorCode = 0;
        paraInfo.writeErrorCode = PARA_WRITE_ERROR_NONE;
        {
            uartdata.frame_id = UART_FRAME_RECOVER_FAC;
            uartdata.mot_id = devCanId;
            uartdata.len = 0;
            uartPack(&uartdata);
        }
        break;
    case STEP_SCOPE_SET_MODE:
        // mode = STEP_SCOPE_CONFIG;
        // pack.exId.data = CAN_ID_DEBUG_UI | ((quint16)(Burst_mode & 0X01) << 8);
        // pack.exId.mode = CANCOM_SCOPE_SET_MODE; // 新开的模式，放在最后

        // txdPack(&pack);
        break;
    case STEP_SCOPE_SET_FRE:
        UseTimeMs = 0;
        mode = STEP_SCOPE_CONFIG;

        uartdata.frame_id = UART_FRAME_PARA_UPDATE_FRE;
        uartdata.mot_id = devCanId;
        uartdata.len = 19;
        uartdata.data[0] = chSelNum;
        for (int i = 0; i < CHANNAL_SEL_NUM; i++)
        {
            uartdata.data[1 + 2 * i] = chSelCode[chSelIndex[i]] >> 8;
            uartdata.data[2 + 2 * i] = chSelCode[chSelIndex[i]] & 0X00FF;
        }
        uartdata.data[17] = (chEchoFreHz & 0XFF00) >> 8;
        uartdata.data[18] = chEchoFreHz & 0X00FF;
        uartPack(&uartdata);
        break;
    case STEP_SCOPE_SET_CHANNAL:
        mode = STEP_SCOPE_CONFIG;

        uartdata.frame_id = UART_FRAME_PARA_UPDATE_CHA;
        uartdata.mot_id = devCanId;
        uartdata.len = 17;
        uartdata.data[0] = chSelNum;
        for (int i = 0; i < CHANNAL_SEL_NUM; i++)
        {
            uartdata.data[1 + 2 * i] = chSelCode[chSelIndex[i]] >> 8;
            uartdata.data[2 + 2 * i] = chSelCode[chSelIndex[i]] & 0X00FF;
        }
        uartPack(&uartdata);
        break;
    case STEP_SCOPE_START:
        uartdata.frame_id = UART_FRAME_PARA_UPDATE_START;
        uartdata.mot_id = devCanId;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;
    case STEP_SCOPE_STOP:
        uartdata.frame_id = UART_FRAME_PARA_UPDATE_STOP;
        uartdata.mot_id = devCanId;
        uartdata.len = 0;
        uartPack(&uartdata);
        break;
    default:
        mode = STEP_STOP;
        break;
    }

    runStep = mode;
}

// 解析接收数据
// #define CanStdFrameNum 2            // 扩展帧和标准帧数量的对应关系
// struct canPack_Std rxMsg_StdSingle; // 用于存储单次接收到的帧，进行诊断，防止标准帧反序或者缺失
// struct canPack_Std rxMsg_Std[CanStdFrameNum];
// struct canPack_Std txMsg_Std[CanStdFrameNum];
// volatile uint8_t CANrxMsg_Cnt; // 用于记录连续两次接收

// void otaThread::analysisRxdDatas(QByteArray pack)
// {
//     bool isAFrame = false;

//     if ((pack.startsWith("AT")) && (pack.size() == ((quint8)pack[6] + 9)))
//     {
//         uint32_t addr = pack[5] & 0x000000FF;
//         addr |= ((pack[4] << 8) & 0x0000FF00);
//         addr |= ((pack[3] << 16) & 0x00FF0000);
//         addr |= ((pack[2] << 24) & 0xFF000000);

//         addr = addr >> 3;

//         memcpy(&(rxFrame.exId), &addr, 4);
//         rxFrame.len = pack[6];

//         for (quint8 i = 0; i < rxFrame.len; i++)
//             rxFrame.data[i] = pack[7 + i];

//         /*
//         qDebug()<<"Rxid:"<<QString::number(rxFrame.exId.id,16)
//                 <<"  idData:"<<QString::number(rxFrame.exId.data,16)
//                 <<"  idMode:"<<QString::number(rxFrame.exId.mode,16)
//                 <<"  rxLen:"<<QString::number(rxFrame.len,16)
//                 <<"data:"<<" "<<QString::number(rxFrame.data[0],16)
//                             <<" "<<QString::number(rxFrame.data[1],16)
//                             <<" "<<QString::number(rxFrame.data[2],16)
//                             <<" "<<QString::number(rxFrame.data[3],16)
//                             <<" "<<QString::number(rxFrame.data[4],16)
//                             <<" "<<QString::number(rxFrame.data[5],16)
//                             <<" "<<QString::number(rxFrame.data[6],16)
//                             <<" "<<QString::number(rxFrame.data[7],16);
//         */

//         switch (rxFrame.exId.mode)
//         {
//         case CANCOM_ANNOUNCE_DEVID:
//             rxdPack_ANNOUNCE_DEVID();
//             break;
//         case CANCOM_MOTOR_FEEDBACK:
//             rxdPack_MOTOR_FEEDBACK();
//             break;
//         case CANCOM_CALI_ING:
//             rxdPack_CALI_ING();
//             break;
//         case CANCOM_CALI_RST:
//             rxdPack_CALI_RST();
//             break;
//         case CANCOM_HALLCALI_ING:
//             rxdPack_HALLCALI_ING();
//             break;
//         case CANCOM_HALLCALI_RST:
//             rxdPack_HALLCALI_RST();
//             break;
//         case CANCOM_MOTOR_CALI:
//             rxdPack_MOTOR_CALI();
//             break;
//         case CANCOM_PARA_STR_INFO:
//             rxdPack_PARA_STR_INFO();
//             break;
//         case CANCOM_PARA_READ:
//             rxdPack_PARA_READ();
//             break;
//         case CANCOM_PARA_WRITE:
//             rxdPack_PARA_Write();
//             break;
//         case CANCOM_PARA_UPDATE:
//             rxdPack_PARA_UPDATE();
//             break;
//         case CANCOM_FAULT_WARN:
//             rxdPack_FAULT_WARN();
//             break;
//             // case CANCOM_SCOPE_SET_MODE :break;
//         default:
//             break;
//         }

//         if (rxFrame.exId.id == CAN_ID_DEBUG_UI)
//             isAFrame = true;
//     }

//     // 以下部分是处理应答的
//     switch (runStep)
//     {

//     case STEP_CONFIG:
//     case STEP_INCONFIG:
//     case STEP_INIT:
//         if (pack.contains("OK"))
//             hasAcked = true;
//         break;
//     case STEP_IN_OTA:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_START) && (rxFrame.exId.data == devCanId))
//             hasAcked = true;
//         break;
//     case STEP_SEND_INFO:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_INFO) && (rxFrame.exId.data == devCanId))
//         {
//             hasAcked = true;
//             otaStatus = OTA_STA_OTA_INFO_SUCCESS;
//             emit sendState();
//             setCmd(STEP_SEND_BIN);
//         }
//         break;
//     case STEP_SEND_BIN:

//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_ING))
//         {
//             hasAcked = true;
//             if (rxFrame.exId.data == devCanId)
//             {
//                 PackCnt++;
//                 otaStatus = OTA_STA_SEND_BIN_OK;
//             }
//             else if (rxFrame.exId.data == ((quint16)devCanId | 0X0F00))
//             {
//                 memcpy(&PackCnt, rxFrame.data, 2);
//                 otaStatus = OTA_STA_SEND_BIN_RETRY;
//             }

//             if (PackCnt >= PackNumber)
//             {
//                 otaStatus = OTA_STA_SEND_BIN_DONE;
//                 setCmd(STEP_SEND_END);
//             }
//             else
//             {
//                 setCmd(STEP_SEND_BIN);
//             }

//             emit sendState();
//         }
//         break;

//     case STEP_SEND_END:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_OTA_END))
//         {
//             hasAcked = true;
//             if (rxFrame.exId.data == devCanId)
//             {
//                 otaStatus = OTA_STA_SUCCESS;
//             }
//             else if (rxFrame.exId.data == ((quint16)devCanId | 0X0F00))
//             {
//                 memcpy(&PackCnt, rxFrame.data, 2);
//                 otaStatus = OTA_STA_FAILED;
//             }
//             emit sendState();
//         }
//         break;

//     case STEP_SET_CAN_ID:
//         if (rxFrame.exId.mode == CANCOM_ANNOUNCE_DEVID)
//             hasAcked = true;
//         break;

//     case STEP_SET_RST_MODE:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_RESET))
//             hasAcked = true;
//         break;

//     case STEP_SET_MOTOR_MODE:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_MOTOR))
//             hasAcked = true;
//         break;

//     case STEP_SET_BRAKE_MODE:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_BRAKE))
//             hasAcked = true;
//         break;

//     case STEP_SET_CALI_ENCODER_MODE:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (mtStatus.mode == MT_MODE_CALI))
//             hasAcked = true;
//         break;

//     case STEP_SET_ZERO_MECH_MODE:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_MOTOR_FEEDBACK) && (posGet < 0.01f) && (posGet > -0.01f))
//             hasAcked = true;
//         break;

//     case STEP_ASK_PARA_STR_INFO:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_STR_INFO))
//             hasAcked = true;
//         break;

//     case STEP_READ_PARA_STR:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_READ))
//             hasAcked = true;
//         break;

//     case STEP_WRITE_PARA_STR:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_WRITE))
//             hasAcked = true;
//         break;

//     case STEP_SCOPE_CONFIG:
//         if ((isAFrame == true) && (rxFrame.exId.mode == CANCOM_PARA_UPDATE))
//             hasAcked = true;
//         break;

//     default:
//         break;
//     }
// }

void otaThread::analysisRxdDatas(QByteArray pack)
{
    // bool isAFrame = false;
    // 先校验
    if (pack.length() < 5)
    {
        return;
    }
    quint8 t = pack[0];
    if (t != REC_HEAD1)
    {
        return;
    }
    t = pack[1];
    if (t != REC_HEAD2)
    {
        return;
    }
    int len = pack[2];
    if (pack.length() < (len + 5))
    {
        return;
    }

    QByteArray datas = pack.mid(3, len);

    if (datas.length() < 2)
    {
        return;
    }
    int frame_id = datas[0];
    int mot_id = datas[1];

    switch (frame_id)
    {
    case UART_FRAME_ANNOUNCE_DEVID:
        rxdPack_ANNOUNCE_DEVID(datas);
        break;
    case UART_FRAME_CUSTOME_STR:
        rxdPack_CUSTOME_STR(datas);
        break;
    case UART_FRAME_CUSTOME_DATA:
        rxdPack_CUSTOME_DAT(datas);
        break;
    case UART_FRAME_MOTOR_RESET:
        rxdPack_MOTOR_RESET(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SET_RST_MODE))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_MOTOR_RESET" << mcuCanId << devCanId << runStep << faultGet;
        break;
    case UART_FRAME_MOTOR_BRAKE:
        rxdPack_MOTOR_BRAKE(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SET_BRAKE_MODE))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_MOTOR_BRAKE" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_CUSTOME_CMD:
        rxdPack_MOTOR_Custom_Cmd(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SET_CUSTOME_CMD))
        {
            hasAcked = true;
        }
        qDebug() << "STEP_SET_CUSTOME_CMD" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_MOTOR_ZERO_MECH:
        rxdPack_MOTOR_Zero_Mech(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SET_ZERO_MECH_MODE))
        {
            hasAcked = true;
        }
        qDebug() << "STEP_SET_ZERO_MECH_MODE" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_MOTOR_STATE:
        // hasAcked = true;
        rxdPack_MOTOR_STATE(datas);
        break;
    case UART_FRAME_CALI_GET:
        rxdPack_CALI_GET(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SET_CALI_ENCODER_MODE))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_CALI_GET" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_CALI_BEGIN:
        rxdPack_CALI_BEGIN(datas);
        break;
    case UART_FRAME_CTRL_SET_MODE:
        rxdPack_MOTOR_CTRL_SET(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SET_MOTOR_MODE))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_MOTOR_CTRL_SET" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_CTRL_SET_PARA:
        rxdPack_MOTOR_CTRL_PARA(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_MOTOR_CTRL_SINGLE))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_MOTOR_CTRL_SET" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_PARA_SAVE:
        rxdPack_MOTOR_PARA_SAVE(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SAVE_PARA_STR))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_MOTOR_CTRL_SET" << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_CALI_ING:
        rxdPack_CALI_ING(datas);
        break;
    case UART_FRAME_CALI_RST:
        rxdPack_CALI_RST(datas);
        break;
    case UART_FRAME_CALI_DONE:
        rxdPack_CALI_DONE(datas);
        break;
    case UART_FRAME_PARA_STR_INFO:
        hasAcked = true;
        rxdPack_PARA_STR_INFO(datas);
        break;
    case UART_FRAME_PARA_NUM_INFO:
        hasAcked = true;
        rxdPack_PARA_DATA_INFO(datas);
        break;
    case UART_FRAME_PARA_DONE:
        rxdPack_PARA_INFO_DONE(datas);
        break;
    case UART_FRAME_PARA_UPDATE_FRE:
        rxdPack_PARA_UPDATE_FRE(datas);
        break;
    case UART_FRAME_PARA_UPDATE_START:
        rxdPack_PARA_UPDATE_START(datas);
        if ((mcuCanId == devCanId) //
            && (runStep == STEP_SCOPE_START))
        {
            hasAcked = true;
        }
        qDebug() << "rxdPack_PARA_UPDATE_START " << mcuCanId << devCanId << runStep;
        break;
    case UART_FRAME_PARA_UPDATE_STOP:
        rxdPack_PARA_UPDATE_STOP(datas);
        break;
    case UART_FRAME_PARA_READ:
        rxdPack_PARA_READ_com(datas);
        break;
    case UART_FRAME_PARA_WRITE:
        rxdPack_PARA_Write_com(datas);
        break;
    default:
        break;
    }
    // if ((pack.startsWith("AT")) && (pack.size() == ((quint8)pack[6] + 9)))
    // {
    //     uint32_t addr = pack[5] & 0x000000FF;
    //     addr |= ((pack[4] << 8) & 0x0000FF00);
    //     addr |= ((pack[3] << 16) & 0x00FF0000);
    //     addr |= ((pack[2] << 24) & 0xFF000000);

    //     addr = addr >> 3;

    //     memcpy(&(rxFrame.exId), &addr, 4);
    //     rxFrame.len = pack[6];

    //     for (quint8 i = 0; i < rxFrame.len; i++)
    //         rxFrame.data[i] = pack[7 + i];

    //     /*
    //     qDebug()<<"Rxid:"<<QString::number(rxFrame.exId.id,16)
    //             <<"  idData:"<<QString::number(rxFrame.exId.data,16)
    //             <<"  idMode:"<<QString::number(rxFrame.exId.mode,16)
    //             <<"  rxLen:"<<QString::number(rxFrame.len,16)
    //             <<"data:"<<" "<<QString::number(rxFrame.data[0],16)
    //                         <<" "<<QString::number(rxFrame.data[1],16)
    //                         <<" "<<QString::number(rxFrame.data[2],16)
    //                         <<" "<<QString::number(rxFrame.data[3],16)
    //                         <<" "<<QString::number(rxFrame.data[4],16)
    //                         <<" "<<QString::number(rxFrame.data[5],16)
    //                         <<" "<<QString::number(rxFrame.data[6],16)
    //                         <<" "<<QString::number(rxFrame.data[7],16);
    //     */

    //     switch (rxFrame.exId.mode)
    //     {
    //     case CANCOM_ANNOUNCE_DEVID:
    //         rxdPack_ANNOUNCE_DEVID();
    //         break;
    //     case CANCOM_MOTOR_FEEDBACK:
    //         rxdPack_MOTOR_FEEDBACK();
    //         break;
    //     case CANCOM_CALI_ING:
    //         rxdPack_CALI_ING();
    //         break;
    //     case CANCOM_CALI_RST:
    //         rxdPack_CALI_RST();
    //         break;
    //     case CANCOM_HALLCALI_ING:
    //         rxdPack_HALLCALI_ING();
    //         break;
    //     case CANCOM_HALLCALI_RST:
    //         rxdPack_HALLCALI_RST();
    //         break;
    //     case CANCOM_MOTOR_CALI:
    //         rxdPack_MOTOR_CALI();
    //         break;
    //     case CANCOM_PARA_STR_INFO:
    //         rxdPack_PARA_STR_INFO();
    //         break;
    //     case CANCOM_PARA_READ:
    //         rxdPack_PARA_READ();
    //         break;
    //     case CANCOM_PARA_WRITE:
    //         rxdPack_PARA_Write();
    //         break;
    //     case CANCOM_PARA_UPDATE:
    //         rxdPack_PARA_UPDATE();
    //         break;
    //     case CANCOM_FAULT_WARN:
    //         rxdPack_FAULT_WARN();
    //         break;
    //         // case CANCOM_SCOPE_SET_MODE :break;
    //     default:
    //         break;
    //     }
    // }
}

// run function
void otaThread::run()
{
    // struct canPack pack;
    struct uartPack uartdata;
    // qDebug("otaThread run!");

    while (true)
    {
        switch (runStep)
        {
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

            // case STEP_SET_CAN_ID:
            //     msleep(100);
            //     UseTimeMs += 100;

            //     if (hasAcked)
            //     {
            //         runStep = STEP_STOP;
            //         otaStatus = OTA_STA_SET_ID_SUCCESS;
            //         emit sendState();
            //     }
            //     else if (UseTimeMs > 2000)
            //     {
            //         runStep = STEP_STOP;
            //         otaStatus = OTA_STA_SET_ID_OVER_TIME;
            //         emit sendState();
            //     }
            //     break;

        case STEP_SET_RST_MODE:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_RST_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs >= 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_RST_MODE_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    setCmd(STEP_SET_RST_MODE);
                    qDebug() << "set rst mode again:" << unAckedCnt;
                }
            }
            break;

        case STEP_SET_CALI_ENCODER_MODE:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                // otaStatus = OTA_STA_CALI_MODE_SUCCESS;
                // emit sendState();
            }
            else if (UseTimeMs >= 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_CALI_MODE_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    setCmd(STEP_SET_CALI_ENCODER_MODE);
                    qDebug() << "STEP_SET_CALI_ENCODER_MODE:" << unAckedCnt;
                }
            }
            break;

        case STEP_SET_ZERO_MECH_MODE:
            msleep(100);
            UseTimeMs += 100;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                // otaStatus = OTA_STA_ZERO_MODE_SUCCESS;
                // emit sendState();
            }
            else if (UseTimeMs > 400)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_ZERO_MODE_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    setCmd(STEP_SET_ZERO_MECH_MODE);
                    qDebug() << "STEP_SET_ZERO_MECH_MODE:" << unAckedCnt;
                }
            }
            break;
        case STEP_SET_MOTOR_MODE:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked) // need
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_MOTOR_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs >= 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_MOTOR_MODE_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    setCmd(STEP_SET_MOTOR_MODE);
                    qDebug() << "STEP_SET_MOTOR_MODE:" << unAckedCnt;
                }
            }
            break;
        case STEP_MOTOR_CTRL_SINGLE:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked) // need
            {
                runStep = STEP_STOP;
                // otaStatus = OTA_STA_MOTOR_MODE_SUCCESS;
                // emit sendState();
            }
            else if (UseTimeMs >= 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                    // otaStatus = OTA_STA_MOTOR_MODE_OVER_TIME;
                    // emit sendState();
                }
                else
                {
                    setCmd(STEP_MOTOR_CTRL_SINGLE);
                    qDebug() << "STEP_MOTOR_CTRL_SINGLE:" << unAckedCnt;
                }
            }
            break;
        case STEP_SET_BRAKE_MODE:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_BRAKE_MODE_SUCCESS;
                emit sendState();
            }
            else if (UseTimeMs > 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_BRAKE_MODE_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    setCmd(STEP_SET_BRAKE_MODE);
                }
                qDebug() << "set brake mode again:" << unAckedCnt;
            }
            break;
        case STEP_SET_CUSTOME_CMD:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked)
            {
                runStep = STEP_STOP;
            }
            else if (UseTimeMs > 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                }
                else
                {
                    setCmd(STEP_SET_CUSTOME_CMD);
                }
                qDebug() << "set custome cmd again:" << unAckedCnt;
            }
            break;
        case STEP_SAVE_PARA_STR:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked)
            {
                runStep = STEP_STOP;
            }
            else if (UseTimeMs > 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                }
                else
                {
                    setCmd(STEP_SAVE_PARA_STR);
                }
                qDebug() << "STEP_SAVE_PARA_STR:" << unAckedCnt;
            }
            break;
        case STEP_SCOPE_START:
            msleep(10);
            UseTimeMs += 10;

            if (hasAcked)
            {
                runStep = STEP_STOP;
            }
            else if (UseTimeMs > 40)
            {
                unAckedCnt++;
                if (unAckedCnt > 5)
                {
                    runStep = STEP_STOP;
                }
                else
                {
                    setCmd(STEP_SCOPE_START);
                }
                qDebug() << "STEP_SCOPE_START:" << unAckedCnt;
            }
            break;
        case STEP_GET_DEV_INFO:
            // qDebug() << "scanId" << scanId;

            if (scanId > MOT_ID_MAX)
            {
                runStep = STEP_STOP;
            }
            else
            {
                uartdata.frame_id = UART_FRAME_ANNOUNCE_DEVID;
                uartdata.mot_id = scanId;
                uartdata.len = 0;
                uartPack(&uartdata);
                msleep(4);
            }
            scanId++;
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
            else if (UseTimeMs >= 300)
            {
                if (err_cnt > 5)
                { // 连续5次失败才算超时
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_READ_PARA_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    err_cnt++;
                    setCmd(STEP_READ_PARA_STR);
                }
            }
            break;
        case STEP_SCOPE_CONFIG:
            msleep(50);
            UseTimeMs += 50;
            // qDebug() << "osta:" << otaStatus << " t:" << UseTimeMs << " rs:" << runStep;

            if (hasAcked) // need
            {
                hasAcked = false;
                UseTimeMs = 0;
            }
            else if (UseTimeMs > 1000)
            {
                runStep = STEP_STOP;
                otaStatus = OTA_STA_PARA_UPDATE_OVER_TIME;
                // qDebug() << "scope config over time";
                emit sendState();
            }
            break;

        case STEP_WRITE_PARA_STR:

            msleep(50);
            UseTimeMs += 50;

            if (hasAcked)
            {
                hasAcked = false;
                UseTimeMs = 0;
            }
            else if (UseTimeMs >= 300)
            {
                if (err_cnt > 5)
                {
                    runStep = STEP_STOP;
                    otaStatus = OTA_STA_WRITE_PARA_OVER_TIME;
                    emit sendState();
                }
                else
                {
                    err_cnt++;
                    setCmd(STEP_WRITE_PARA_STR);
                }
            }
            break;

        case STEP_MOTOR_CTRL_CONTINUOUS:
            msleep(50);
            ContinueMode = 1;

            union typeConvert t;
            uartdata.frame_id = UART_FRAME_CTRL_SET_PARA;
            uartdata.mot_id = devCanId;
            uartdata.len = 36;
            t.f[0] = pos_tarSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[i] = t.u8[i];
            }
            t.f[0] = pos_kpSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[4 + i] = t.u8[i];
            }
            t.f[0] = pos_kiSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[8 + i] = t.u8[i];
            }

            t.f[0] = vel_tarSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[12 + i] = t.u8[i];
            }
            t.f[0] = vel_kpSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[16 + i] = t.u8[i];
            }
            t.f[0] = vel_kiSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[20 + i] = t.u8[i];
            }

            t.f[0] = tor_tarSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[24 + i] = t.u8[i];
            }
            t.f[0] = tor_kpSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[28 + i] = t.u8[i];
            }
            t.f[0] = tor_kiSet;
            for (int i = 0; i < 4; i++)
            {
                uartdata.data[32 + i] = t.u8[i];
            }

            uartPack(&uartdata);
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
            unAckedCnt = 0;
            ContinueMode = 0;
            break;
        }
    }
}

// 解析串口收到的广播数据
void otaThread::rxdPack_CUSTOME_STR(QByteArray datas)
{
    if (datas.length() < 4)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    custome_strPlace = (enum StrPlaceTpye)datasg[2];
    uint8_t len = datasg[3];
    if (datas.length() < (len + 4))
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    custome_str = "Id "                            //
                  + QString::number(datasg[1], 16) //
                  + ":"                            //
                  + datas.mid(4, len);

    // qDebug() << "Id:" << QString::number(mcuCanId, 16) //
    //         << " get str:" << custome_str;            //

    otaStatus = OTA_STA_GET_CUSTOME_STR;

    emit sendState();
}

// 解析串口收到的标定数据
void otaThread::rxdPack_CALI_GET(QByteArray datas)
{
    if (datas.length() < 2)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
}

// 解析串口收到的广播数据
void otaThread::rxdPack_CALI_BEGIN(QByteArray datas)
{
    if (datas.length() < 2)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    custome_strPlace = STR_PALCE_NONE;
    custome_str = "Id "                            //
                  + QString::number(datasg[1], 16) //
                  + " 准备标定";

    otaStatus = OTA_STA_GET_CUSTOME_STR;
    emit sendState();
}

void otaThread::rxdPack_CALI_ING(QByteArray datas)
{
    // memcpy(&currentAngle, &rxFrame.data[0], 4);
    // memcpy(&rotorAngle, &rxFrame.data[4], 4);
    // caliRaw = rxFrame.exId.data;
    if (datas.length() < 14)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[2 + i];
    }
    currentAngle = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[6 + i];
    }
    rotorAngle = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[10 + i];
    }
    caliRaw = tc.u32[0];

    otaStatus = OTA_STA_ENCODER_CALI_ING;
    emit sendState();
}

void otaThread::rxdPack_CALI_RST(QByteArray datas)
{
    if (datas.length() < 10)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    caliI = (datasg[2] << 8) + (datasg[3]);
    caliInd = (datasg[4] << 8) + (datasg[5]);
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[6 + i];
    }
    caliLut = tc.f[0];

    otaStatus = OTA_STA_ENCODER_CALI_RESULT;
    emit sendState();
}

void otaThread::rxdPack_CALI_DONE(QByteArray datas)
{
    if (datas.length() < 14)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[2 + i];
    }
    caliOffset = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[6 + i];
    }
    variance = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[10 + i];
    }
    calerrorcode = tc.u32[0];

    otaStatus = OTA_STA_ENCODER_CALI_DONE;
    emit sendState();
}
float f[3] = {0};
float last_amp = 0;
int first_through = 0;
int set_format_style = 0;
// 解析串口收到的广播数据
void otaThread::rxdPack_CUSTOME_DAT(QByteArray datas)
{
    if (datas.length() < 4)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }


    mcuCanId = datasg[1];
    quint8 type = datasg[2];
    quint8 len = datasg[3];
    if (datas.length() < (len * 4 + 4))
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    custome_str = "扫频模式-";                            //
                  // + QString::number(datasg[1], 16) //
                  // + " Type "                       //
                  // + QString::number(datasg[2], 10) //
                  // + ":";
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            tc.u8[j] = datasg[4 + i * 4 + j];
        }
        f[i] = tc.f[0];
        float f_other_kind_dat = tc.f[0];
        if(i==1) {
            if((last_amp > f[1]) && (f[1] < -3.0f) && (first_through == 0)){
                first_through = 1;
                set_format_style = 1;

            }
            last_amp = f[1];
        }
        if(type != 17)
            custome_str += " " +  QString::number(f_other_kind_dat);
    }




    if(type == 17){

        /*custome_str += "频率: " + QString::number(f[0]) + " Hz  \t" + "幅值: " +
        QString::number(f[1]) + " dB  \t" + "相角: " + QString::number(f[2]) + "°";*/
        custome_str += QString::asprintf("频率: %.0f Hz  \t幅值: %.5f dB  \t相角: %.3f°",
                                         f[0]*25, f[1], f[2]);
    }
    custome_strPlace = STR_PALCE_NONE;
    // qDebug() << custome_str;

    otaStatus = OTA_STA_GET_CUSTOME_STR;

    emit sendState();
}

// 解析串口收到的状态数据
void otaThread::rxdPack_MOTOR_STATE(QByteArray datas)
{
    if (datas.length() < 18)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    // qDebug() << "mcuId:" << QString::number(mcuId, 16) << " mcuCanId:" << QString::number(mcuCanId, 16);

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;
    // mtStatus.underVoltFault = (rxFrame.exId.data & 0X0100) ? true : false;
    // mtStatus.overCurFault = (rxFrame.exId.data & 0X0200) ? true : false;
    // mtStatus.overTempFault = (rxFrame.exId.data & 0X0400) ? true : false;
    // mtStatus.encoderFault = (rxFrame.exId.data & 0X0800) ? true : false;
    // mtStatus.hallFault = (rxFrame.exId.data & 0X1000) ? true : false;
    // mtStatus.noCaliFault = (rxFrame.exId.data & 0X2000) ? true : false;
    // mtStatus.mode = (enum motorMode)((rxFrame.exId.data & 0XC000) >> 14);

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    // qDebug() << "Id:" << QString::number(mcuCanId, 16)     //
    //          << " fault:" << QString::number(faultGet, 16) //
    //          << " pos:" << posGet                          //
    //          << " vel:" << velGet                          //
    //          << " tor:" << torqueGet                       //
    //          << " mode:" << QString::number(modeGet, 16);

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的复位数据
void otaThread::rxdPack_MOTOR_RESET(QByteArray datas)
{
    if (datas.length() < 18)
    {
        mcuCanId = 0xff;
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的数据 - 设置模式
void otaThread::rxdPack_MOTOR_CTRL_SET(QByteArray datas)
{
    if (datas.length() < 18)
    {
        mcuCanId = 0xff;
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的数据 - 设置参数
void otaThread::rxdPack_MOTOR_CTRL_PARA(QByteArray datas)
{
    if (datas.length() < 18)
    {
        mcuCanId = 0xff;
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的数据 - 保存参数
void otaThread::rxdPack_MOTOR_PARA_SAVE(QByteArray datas)
{
    if (datas.length() < 18)
    {
        mcuCanId = 0xff;
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的刹车数据
void otaThread::rxdPack_MOTOR_BRAKE(QByteArray datas)
{
    if (datas.length() < 18)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的数据 - 自定义控制
void otaThread::rxdPack_MOTOR_Custom_Cmd(QByteArray datas)
{
    if (datas.length() < 18)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的数据 - 机械标零
void otaThread::rxdPack_MOTOR_Zero_Mech(QByteArray datas)
{
    if (datas.length() < 18)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];
    faultGet = datasg[2];
    modeGet = ((datasg[3] << 8) + datasg[4]);

    mtStatus.underVoltFault = false;
    mtStatus.overCurFault = false;
    mtStatus.overTempFault = false;

    mtStatus.encoderFault = false;
    mtStatus.hallFault = false;
    mtStatus.noCaliFault = false;

    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[5 + i];
    }
    posGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[9 + i];
    }
    velGet = tc.f[0];
    for (int i = 0; i < 4; i++)
    {
        tc.u8[i] = datasg[13 + i];
    }
    torqueGet = tc.f[0];

    otaStatus = OTA_STA_MOTOR_FEEDBACK;

    emit sendState();
}

// 解析串口收到的广播数据
void otaThread::rxdPack_ANNOUNCE_DEVID(QByteArray datas)
{
    if (datas.length() < 18)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }

    quint8 i;

    mcuCanId = datas[1];
    mcuId = 0;
    for (i = 0; i < 8; i++)
    {
        mcuId = (mcuId << 8) + datas[i + 2];
    }
    // memcpy(&mcuId, &datas.data[2], 8);

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

    // qDebug() << "mcuId:" << QString::number(mcuId, 16) << " mcuCanId:" << QString::number(mcuCanId, 16);
    otaStatus = OTA_STA_DETECT_MCU;
    emit sendState();
}

// 解析串口收到的字符串节点数据
void otaThread::rxdPack_PARA_STR_INFO(QByteArray datas)
{
    if (datas.length() < 37)
    {
        qDebug() << "len too short: " << datas.length() << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    quint16 paraCode = 0;

    mcuCanId = datasg[1];
    quint8 t = datasg[2];
    enum paraStrItemType type = (enum paraStrItemType)(t);
    quint16 point = (datasg[3] << 8) + datasg[4];
    QByteArray name = datas.mid(5, 16);
    QByteArray strs = datas.mid(21, 16);
    // qDebug() << "mode:" << type << " index:" << point;
    // qDebug() << "name:" << name.toHex();
    // qDebug() << "strs:" << strs.toHex();

    if (point < paraNumTab[type])
    {
        switch (type)
        {
        case WRITE_STR:
            paraInfo.writeStr[point].name = QString(name);
            paraInfo.writeStr[point].str = QString(strs);
            break;
        case READ_STR:
            paraInfo.readStr[point].name = QString(name);
            paraInfo.readStr[point].str = QString(strs);
            break;
        // case WRITE_PARA:
        //     paraInfo.writePara[point].name = QString(name);
        //     break;
        // case READ_PARA:
        //     paraInfo.readPara[point].name = QString(name);
        //     break;
        default:
            paraCode |= PARA_ERROR_ITEM_MATCH;
            break;
        }
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
        paraErrorCode |= paraCode;
    }

    // paraPointBak = point;
    // paraItemBak = item;
    // paraTypeBak = type;
}

// 解析串口收到的数值节点数据
void otaThread::rxdPack_PARA_DATA_INFO(QByteArray datas)
{
    if (datas.length() < 36)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    quint16 paraCode = 0;

    mcuCanId = datasg[1];
    quint8 t = datasg[2];
    enum paraStrItemType type = (enum paraStrItemType)(t);
    quint16 point = (datasg[3] << 8) + datasg[4];
    QByteArray name = datas.mid(5, 16);
    qDebug() << "mode:" << type << " index:" << point;
    qDebug() << "name:" << name.toHex();

    if (point < paraNumTab[type])
    {
        switch (type)
        {
        case WRITE_PARA:
        {
            paraInfo.writePara[point].name = QString(name);
            t = datasg[26];
            paraInfo.writePara[point].type = (enum paraType)(t & 0X1F);
            paraInfo.writePara[point].wPerm = (t & 0XE0) >> 5;
            qint16 maxS16, minS16;
            qint8 ratio;
            float coef = 1.0f;
            minS16 = (datasg[22] << 8) + datasg[21];
            maxS16 = (datasg[24] << 8) + datasg[23];
            ratio = datasg[25];
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

            for (int i = 0; i < 4; i++)
            {
                paraInfo.writePara[point].valR[i] = datasg[28 + i];
            }
            if (paraInfo.writePara[point].type >= ParaType_U64)
            {
                for (int i = 4; i < 8; i++)
                {
                    paraInfo.writePara[point].valR[i] = datasg[28 + i];
                }
            }
        }
        break;
        case READ_PARA:
        {
            paraInfo.readPara[point].name = QString(name);
            t = datasg[27];
            paraInfo.readPara[point].type = (enum paraType)(t & 0X1F);

            for (int i = 0; i < 4; i++)
            {
                paraInfo.readPara[point].valR[i] = datasg[28 + i];
            }
            if (paraInfo.readPara[point].type >= ParaType_U64)
            {
                for (int i = 4; i < 8; i++)
                {
                    paraInfo.readPara[point].valR[i] = datasg[28 + i];
                }
            }
        }
        break;
        default:
            paraCode |= PARA_ERROR_ITEM_MATCH;
            break;
        }
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
        paraErrorCode |= paraCode;
    }

    // paraPointBak = point;
    // paraItemBak = item;
    // paraTypeBak = type;
}

void otaThread::rxdPack_PARA_INFO_DONE(QByteArray datas)
{
    if (datas.length() < 10)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    quint16 tempU16;
    quint16 paraCode = 0;
    paraInfo.echoNum = datasg[2];
    paraInfo.meaTimerNum = datasg[3];
    tempU16 = datasg[4];
    if (tempU16 != paraInfo.numTab[WRITE_STR])
        paraErrorCode |= PARA_ERROR_WRITE_STR_NUM_MATCH;
    tempU16 = datasg[5];
    if (tempU16 != paraInfo.numTab[READ_STR])
        paraErrorCode |= PARA_ERROR_READ_STR_NUM_MATCH;
    tempU16 = (datasg[6] << 8) + datasg[7];
    if (tempU16 != paraInfo.numTab[WRITE_PARA])
        paraErrorCode |= PARA_ERROR_WRITE_PARA_NUM_MATCH;
    tempU16 = (datasg[8] << 8) + datasg[9];
    if (tempU16 != paraInfo.numTab[READ_PARA])
        paraErrorCode |= PARA_ERROR_READ_PARA_NUM_MATCH;

    if (paraErrorCode)
        otaStatus = OTA_STA_GET_PARA_INFO_ERROR;
    else
        otaStatus = OTA_STA_GET_PARA_INFO_SUCCESS;
    emit sendState();

    hasAcked = true;
    paraItemBak = PARA_DONE;
}

// 参数上传 - 频率
void otaThread::rxdPack_PARA_UPDATE_FRE(QByteArray datas)
{
    if (datas.length() < 19)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    quint8 i = 0;
    quint8 ack = datasg[2]; // 这是错误代码。10bit

    paraErrorCode = 0;

    runStep = STEP_STOP;
    UseTimeMs = 0;
    hasAcked = true;

    tc.u8[0] = datasg[11];
    tc.u8[1] = datasg[12];
    tc.u8[2] = datasg[13];
    tc.u8[3] = datasg[14];
    chTickUs = tc.f[0];
    chEchoDevFreMax = (datasg[15] << 8) + datasg[16];
    chEchoDevFreMin = (datasg[17] << 8) + datasg[18];

    chTickBak = 0;
    chTimeStamp = 0.0f;

    if (ack) // 错误代码
    {
        switch (ack)
        {
        case 1:
            paraErrorCode |= PARA_UPDATE_ERROR_CHSET_NOCODE;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        case 2:
            paraErrorCode |= PARA_UPDATE_ERROR_CHSET_FRE;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        case 4:
            paraErrorCode |= PARA_UPDATE_ERROR_FRESET;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        case 3:
        default:
            paraErrorCode |= PARA_UPDATE_ERROR_CHSET_LOSE;
            otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            break;
        }
    }
    else
    { // 当前没有报错的状况
        for (i = 0; i < chSelNum; i++)
        {
            quint8 t = datasg[3 + i];
            chTypeDev[chSelIndex[i]] = (enum paraType)(t); // 发上来的是拼接的数据，这里还原，根据
        }

        otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_CHSET;

        for (i = 0; i < CHANNAL_SEL_NUM; i++)
            mData[i].clear();

        scopeShowNum = (quint32)(chEchoFreHz / WAVE_SHOW_FRE);

        if (scopeShowNum == 0)
            scopeShowNum = 1;
        scopeShowCount = 0;
        scopeWindowNum = (quint32)(WAVE_TIME_WINDOW * chEchoFreHz);
        scopeWindowCount = 0;
        scopeWindowRemoveFlag = false;

        qDebug() << "scopeShowNum" << QString::number(scopeShowNum, 10)
                 << "  scopeWindowNum" << QString::number(scopeWindowNum, 10);

        qDebug() << "通道数量：" << QString::number(chSelNum, 10);
        for (i = 0; i < chSelNum; i++)
        {
            qDebug() << "needType:" << QString::number(chTypeNeed[chSelIndex[i]], 10)
                     << "devType:" << QString::number(chTypeDev[chSelIndex[i]], 10);
            if (chTypeDev[chSelIndex[i]] != chTypeNeed[chSelIndex[i]])
            {
                paraErrorCode |= PARA_UPDATE_ERROR_CHSET_TYPE;
                otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            }
        }
    }

    emit sendState();
}

// 参数上传 - 通道
void otaThread::rxdPack_PARA_UPDATE_CHA(QByteArray datas)
{
    if (datas.length() < 11)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    quint8 ack = datasg[2]; // 这是错误代码。10bit

    quint8 i = 0;
    paraErrorCode = 0;

    runStep = STEP_STOP;
    UseTimeMs = 0;
    hasAcked = true;

    chTickBak = 0;
    chTimeStamp = 0.0f;
    switch (ack)
    {
    case 0:
    {
        for (i = 0; i < chSelNum; i++)
        {
            quint8 t = datasg[3 + i];
            chTypeDev[chSelIndex[i]] = (enum paraType)(t); // 发上来的是拼接的数据，这里还原，根据
        }

        otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_CHSET;

        for (i = 0; i < CHANNAL_SEL_NUM; i++)
            mData[i].clear();

        scopeShowNum = (quint32)(chEchoFreHz / WAVE_SHOW_FRE);

        if (scopeShowNum == 0)
            scopeShowNum = 1;
        scopeShowCount = 0;
        scopeWindowNum = (quint32)(WAVE_TIME_WINDOW * chEchoFreHz);
        scopeWindowCount = 0;
        scopeWindowRemoveFlag = false;

        qDebug() << "scopeShowNum" << QString::number(scopeShowNum, 10)
                 << "  scopeWindowNum" << QString::number(scopeWindowNum, 10);

        qDebug() << "通道数量：" << QString::number(chSelNum, 10);
        for (i = 0; i < chSelNum; i++)
        {
            qDebug() << "needType:" << QString::number(chTypeNeed[chSelIndex[i]], 10)
                     << "devType:" << QString::number(chTypeDev[chSelIndex[i]], 10);
            if (chTypeDev[chSelIndex[i]] != chTypeNeed[chSelIndex[i]])
            {
                paraErrorCode |= PARA_UPDATE_ERROR_CHSET_TYPE;
                otaStatus = OTA_STA_PARA_UPDATE_ERROR;
            }
        }
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

    emit sendState();
}

// 参数上传 - 开始
void otaThread::rxdPack_PARA_UPDATE_START(QByteArray datas)
{

    if (datas.length() < 3)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }

    quint8 ack = datas[2]; // 这是错误代码。10bit

    scopeIngFlag = true;
    if (ack)
    {
        paraErrorCode |= PARA_UPDATE_ERROR_START;
        otaStatus = OTA_STA_PARA_UPDATE_ERROR;
    }
    else
    {
        if (datas.length() < (chNeedByteTotal + 3))
        {
            qDebug() << "len too short" << __FILE__ << __LINE__;
            return;
        }
        for (int i = 0; i < chNeedByteTotal; i++)
        {
            chRxdByte[i] = (quint8)(datas[3 + i]);
        }
        {
            // qDebug()<<"need:"<<QString::number(chNeedByteTotal,10);
            scope_dataProcess(); // 这里是把数据分配到每一个通道
        }

        return;
    }

    emit sendState();
}

// 参数上传 - 停止
void otaThread::rxdPack_PARA_UPDATE_STOP(QByteArray datas)
{
    if (datas.length() < 1)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }

    scopeIngFlag = false;
    otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_END;

    emit sendState();
}

// 解析串口收到的读数据回应
void otaThread::rxdPack_PARA_READ_com(QByteArray datas)
{
    if (datas.length() < 22)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];

    quint16 errorCode = ((datasg[2]) == 1) ? PARA_READ_ERROR_CODE : 0;
    quint16 code = (datasg[3] << 8) + datasg[4];
    enum paraType type = (enum paraType)datasg[5];

    if (code != paraInfo.codeNeed)
        errorCode |= PARA_READ_ERROR_NOT_NEED;
    else if (
        (((paraInfo.itemIndex == WRITE_STR) || (paraInfo.itemIndex == READ_STR)) && (type != ParaType_STRING)) //
        || ((paraInfo.itemIndex == WRITE_PARA) && (type != paraInfo.writePara[paraInfo.posIndex].type))        //
        || ((paraInfo.itemIndex == READ_PARA) && (type != paraInfo.readPara[paraInfo.posIndex].type)))
    {
        errorCode |= PARA_READ_ERROR_TYPE;
    }
    else
    {
        QByteArray strs = datas.mid(6, 16);
        switch (paraInfo.itemIndex)
        {
        case WRITE_STR:
            paraInfo.writeStr[paraInfo.posIndex].str = QString(strs);
            break;
        case READ_STR:
            paraInfo.readStr[paraInfo.posIndex].str = QString(strs);
            break;
        case WRITE_PARA:
            memcpy(&(paraInfo.writePara[paraInfo.posIndex].valR[0]), (datasg + 6), paraByteNumTab[type]);
            break;
        case READ_PARA:
            memcpy(&(paraInfo.readPara[paraInfo.posIndex].valR[0]), (datasg + 6), paraByteNumTab[type]);
            break;
        default:
            errorCode |= PARA_READ_ERROR_CODE;
            break;
        }
    }
    if (errorCode)
    {
        paraErrorCode = errorCode;
        otaStatus = OTA_STA_READ_PARA_ERROR;
    }
    else
    {
        // msleep(2);
        otaStatus = OTA_STA_READ_PARA_SUCCESS;
    }

    runStep = STEP_STOP;

    emit sendState();
}

// 写参数
void otaThread::rxdPack_PARA_Write_com(QByteArray datas)
{
    if (datas.length() < 6)
    {
        qDebug() << "len too short" << __FILE__ << __LINE__;
        return;
    }
    quint8 datasg[datas.length()];
    for (int i = 0; i < datas.length(); i++)
    {
        datasg[i] = (quint8)datas[i];
    }

    mcuCanId = datasg[1];

    paraInfo.writeErrorCode = (enum paraWriteErrorCode)datasg[2];
    quint16 code = (datasg[3] << 8) + datasg[4];
    enum paraType type = (enum paraType)datasg[5];

    if (code != paraInfo.codeNeed)
    {
        paraErrorCode |= PARA_WR_ERROR_CODE_MATCH;
    }
    else if (type != paraInfo.typeNeed)
    {
        paraErrorCode |= PARA_WR_ERROR_TYTE;
    }
    else if (paraInfo.writeErrorCode)
    {
        paraErrorCode |= (1 << paraInfo.writeErrorCode);
    }

    // paraInfo.writeCmdBack = (enum paraWriteCmdType)((rxFrame.exId.data & 0X0300) >> 8);
    // paraInfo.writeErrorCode = (enum paraWriteErrorCode)((rxFrame.exId.data & 0XFC00) >> 10);
    // quint16 code;
    // memcpy(&code, &rxFrame.data[0], 2);
    // if (paraInfo.writeCmdBack != paraInfo.writeCmd)
    // {
    //     paraErrorCode |= PARA_WR_ERROR_CMD_MATCH;
    // }
    // else if ((paraInfo.codeNeed != code) && (paraInfo.writeCmdBack != WRITE_CMD_SAVE_ONLY))
    // {
    //     paraErrorCode |= PARA_WR_ERROR_CODE_MATCH;
    // }
    // else if (rxFrame.data[2] != paraInfo.typeNeed)
    // {
    //     paraErrorCode |= PARA_WR_ERROR_TYTE;
    // }
    // else if (rxFrame.data[3] != paraInfo.wIndex)
    // {
    //     paraErrorCode |= PARA_WR_ERROR_FRAME;
    // }
    // else if (paraInfo.writeErrorCode)
    // {
    //     paraErrorCode |= (1 << paraInfo.writeErrorCode);
    // }

    if (paraErrorCode)
    {
        otaStatus = OTA_STA_WRITE_PARA_ERROR;
    }
    else
    {
        otaStatus = OTA_STA_WRITE_PARA_SUCCESS;
    }

    runStep = STEP_STOP;

    emit sendState();
}

// void otaThread::rxdPack_MOTOR_FEEDBACK()
// {

//     canIdGet = rxFrame.exId.data & 0X00FF;

//     mtStatus.underVoltFault = (rxFrame.exId.data & 0X0100) ? true : false;
//     mtStatus.overCurFault = (rxFrame.exId.data & 0X0200) ? true : false;
//     mtStatus.overTempFault = (rxFrame.exId.data & 0X0400) ? true : false;
//     mtStatus.encoderFault = (rxFrame.exId.data & 0X0800) ? true : false;
//     mtStatus.hallFault = (rxFrame.exId.data & 0X1000) ? true : false;
//     mtStatus.noCaliFault = (rxFrame.exId.data & 0X2000) ? true : false;

//     mtStatus.mode = (enum motorMode)((rxFrame.exId.data & 0XC000) >> 14);
// if((rxFrame.exId.data&0XC000)==0X4000)
//     mtStatus.mode  = MT_MODE_CALI;
// else if((rxFrame.exId.data&0XC000)==0X8000)
//     mtStatus.mode  = MT_MODE_MOTOR;
// else
//     mtStatus.mode  = MT_MODE_RESET;

// posIntGet = ((int)rxFrame.data[0] << 8) | rxFrame.data[1];
// velIntGet = ((int)rxFrame.data[2] << 8) | rxFrame.data[3];
// torqueIntGet = ((int)rxFrame.data[4] << 8) | rxFrame.data[5];

// posGet = uint_to_float(posIntGet, P_MIN, P_MAX, 16);
// velGet = uint_to_float(velIntGet, V_MIN, V_MAX, 16);
// torqueGet = uint_to_float(torqueIntGet, T_MIN, T_MAX, 16);

//     otaStatus = OTA_STA_MOTOR_FEEDBACK;

//     emit sendState();
// }

// void otaThread::rxdPack_FAULT_WARN()
// {
//     if (rxFrame.len != 8)
//         return;
//     canIdGet = rxFrame.exId.data & 0X00FF;

//     memcpy(&faultSta, &rxFrame.data[0], 4);
//     memcpy(&warnSta, &rxFrame.data[4], 4);

//     otaStatus = OTA_STA_MOTOR_FAULTWARN;

//     emit sendState();
// }

// void otaThread::rxdPack_CALI_ING()
// {
//     memcpy(&currentAngle, &rxFrame.data[0], 4);
//     memcpy(&rotorAngle, &rxFrame.data[4], 4);
//     caliRaw = rxFrame.exId.data;

//     otaStatus = OTA_STA_ENCODER_CALI_ING;
//     emit sendState();
// }

// void otaThread::rxdPack_CALI_RST()
// {
//     canIdGet = rxFrame.exId.data & 0X00FF;
//     memcpy(&caliI, &rxFrame.data[0], 2);
//     memcpy(&caliInd, &rxFrame.data[2], 2);
//     memcpy(&caliLut, &rxFrame.data[4], 4);

//     otaStatus = OTA_STA_ENCODER_CALI_RESULT;
//     emit sendState();
// }

// void otaThread::rxdPack_HALLCALI_ING()
// {
//     quint8 tempU8;
//     quint16 tempU16;
//     canIdGet = rxFrame.exId.data & 0X00FF;
//     tempU8 = (rxFrame.exId.data & 0XFF00) >> 8;

//     memcpy(&tempU16, &rxFrame.data[0], 2);

//     if (tempU8 == 0)
//     {
//         hallCount = tempU16;
//         memcpy(&hallRaw[0], &rxFrame.data[2], 6);
//     }
//     else if ((tempU8 == 1) && (tempU16 == hallCount))
//     {
//         memcpy(&hallRaw[3], &rxFrame.data[2], 6);
//         otaStatus = OTA_STA_HALL_CALI_ING;
//         emit sendState();
//     }
// }

// void otaThread::rxdPack_HALLCALI_RST()
// {
//     quint8 tempU8;
//     canIdGet = rxFrame.exId.data & 0X00FF;
//     tempU8 = (rxFrame.exId.data & 0XFF00) >> 8;

//     if (tempU8 == 1)
//     {
//         memcpy(&hallCount, &rxFrame.data[0], 2);
//         memcpy(&hallRdLval, &rxFrame.data[2], 2);
//         otaStatus = OTA_STA_HALL_CALI_RESULT1;
//         emit sendState();
//     }
//     else if ((tempU8 == 2) && (rxFrame.data[1] < 6)) //&&(rxFrame.data[1]>=0)
//     {
//         if (rxFrame.data[0] == 0)
//         {
//             hallCount = rxFrame.data[1];
//             memcpy(&hallInfo[hallCount], &rxFrame.data[2], 6);
//         }
//         else if ((rxFrame.data[0] == 1) && (rxFrame.data[1] == hallCount))
//         {
//             memcpy(&hallInfo[hallCount].maxPos, &rxFrame.data[2], 6);
//             otaStatus = OTA_STA_HALL_CALI_RESULT2;
//             emit sendState();
//         }
//     }
// }

// void otaThread::rxdPack_MOTOR_CALI()
// {
//     quint8 tempU8;
//     canIdGet = rxFrame.exId.data & 0X00FF;
//     tempU8 = (rxFrame.exId.data & 0XFF00) >> 8;
//     if (tempU8 == 0)
//     {
//         calerrorcode = 0;
//         memcpy(&caliOffset, &rxFrame.data[0], 4);

//         otaStatus = OTA_STA_ENCODER_CALI_DONE;
//         emit sendState();
//     }
//     else if (tempU8 == 1)
//     {
//         for (quint8 i = 0; i < 6; i++)
//         {
//             hallInfo[i].fault = (rxFrame.data[i] & 0XF0) >> 4;
//             hallInfo[i].warn = rxFrame.data[i] & 0X0F;
//         }
//         hallError = rxFrame.data[6];
//         hallWarn = rxFrame.data[7];
//         otaStatus = OTA_STA_HALL_CALI_DONE;
//         emit sendState();
//     }
// }

// void otaThread::rxdPack_PARA_STR_INFO_bak()
// {
//     quint16 tempU16;
//     canIdGet = rxFrame.exId.data & 0X00FF;
//     enum canParaItemType item = (enum canParaItemType)((rxFrame.exId.data & 0XFF00) >> 8);
//     memcpy(&tempU16, &rxFrame.data[0], 2);
//     enum paraStrItemType type = (enum paraStrItemType)((tempU16 & 0XF000) >> 12);
//     quint16 point = (quint16)(tempU16 & 0X0FFF);
//     quint16 paraCode = 0;

//     qint16 maxS16, minS16;
//     qint8 ratio;
//     float coef = 1.0f;

//     if (paraPointBak != point)
//         paraCode |= PARA_ERROR_POINT;
//     else if (paraTypeBak != type)
//         paraCode |= PARA_ERROR_TYPE;
//     else if (point >= paraNumTab[type])
//         paraCode |= PARA_ERROR_NUM;

//     switch (item)
//     {
//     case PARA_NAME0:
//         paraCode = 0;
//         memset(&p_buf[0], 0, 18);
//         memcpy(&p_buf[0], &rxFrame.data[2], 6);
//         break;
//     case PARA_NAME1:
//         memcpy(&p_buf[6], &rxFrame.data[2], 6);
//         if (paraItemBak != PARA_NAME0)
//             paraCode |= PARA_ERROR_ITEM;
//         break;
//     case PARA_NAME2:
//         memcpy(&p_buf[12], &rxFrame.data[2], 6);
//         p_buf[STR_NAME_LEN] = 0;
//         if (paraItemBak != PARA_NAME1)
//             paraCode |= PARA_ERROR_ITEM;
//         else if (point < paraNumTab[type])
//         {
//             switch (type)
//             {
//             case WRITE_STR:
//                 paraInfo.writeStr[point].name = QString(QLatin1String(p_buf));
//                 break;
//             case READ_STR:
//                 paraInfo.readStr[point].name = QString(QLatin1String(p_buf));
//                 break;
//             case WRITE_PARA:
//                 paraInfo.writePara[point].name = QString(QLatin1String(p_buf));
//                 break;
//             case READ_PARA:
//                 paraInfo.readPara[point].name = QString(QLatin1String(p_buf));
//                 break;
//             default:
//                 paraCode |= PARA_ERROR_ITEM_MATCH;
//                 break;
//             }
//         }
//         break;
//     case PARA_ATTR:
//         paraCode = 0;
//         if (point < paraNumTab[type])
//         {
//             switch (type)
//             {
//             case WRITE_PARA:
//                 paraInfo.writePara[point].type = (enum paraType)(rxFrame.data[7] & 0X1F);
//                 paraInfo.writePara[point].wPerm = (rxFrame.data[7] & 0XE0) >> 5;
//                 memcpy(&minS16, &rxFrame.data[2], 2);
//                 memcpy(&maxS16, &rxFrame.data[4], 2);
//                 ratio = (qint8)rxFrame.data[6];
//                 paraInfo.writePara[point].prec = (ratio < 0) ? (-ratio) : 0;

//                 coef = 1.0f;
//                 while (ratio != 0)
//                 {
//                     if (ratio > 0)
//                     {
//                         ratio--;
//                         coef *= 10.0f;
//                     }
//                     else
//                     {
//                         ratio++;
//                         coef /= 10.0f;
//                     }
//                 }

//                 paraInfo.writePara[point].max = coef * maxS16;
//                 paraInfo.writePara[point].min = coef * minS16;
//                 break;
//             case READ_PARA:
//                 paraInfo.readPara[point].type = (enum paraType)(rxFrame.data[7] & 0X1F);
//                 break;
//             default:
//                 paraCode |= PARA_ERROR_ITEM_MATCH;
//                 break;
//             }
//         }
//         break;
//     case PARA_VAL0:
//         paraCode = 0;
//         if (point < paraNumTab[type])
//         {
//             switch (type)
//             {
//             case WRITE_PARA:
//                 paraInfo.writePara[point].type = (enum paraType)rxFrame.data[3];
//                 memcpy(&(paraInfo.writePara[point].valR[0]), &rxFrame.data[4], 4);
//                 break;
//             case READ_PARA:
//                 paraInfo.readPara[point].type = (enum paraType)rxFrame.data[3];
//                 memcpy(&(paraInfo.readPara[point].valR[0]), &rxFrame.data[4], 4);
//                 break;
//             default:
//                 paraCode |= PARA_ERROR_ITEM_MATCH;
//                 break;
//             }
//         }
//         break;
//     case PARA_VAL1:
//         if (point < paraNumTab[type])
//         {
//             switch (type)
//             {
//             case WRITE_PARA:
//                 memcpy(&(paraInfo.writePara[point].valR[4]), &rxFrame.data[4], 4);
//                 break;
//             case READ_PARA:
//                 memcpy(&(paraInfo.readPara[point].valR[4]), &rxFrame.data[4], 4);
//                 break;
//             default:
//                 paraCode |= PARA_ERROR_ITEM_MATCH;
//                 break;
//             }
//         }
//         break;
//     case PARA_STR0:
//         paraCode = 0;
//         memset(&p_buf[0], 0, 18);
//         memcpy(&p_buf[0], &rxFrame.data[2], 6);
//         break;
//     case PARA_STR1:
//         memcpy(&p_buf[6], &rxFrame.data[2], 6);
//         if (paraItemBak != PARA_STR0)
//             paraCode |= PARA_ERROR_ITEM;
//         break;
//     case PARA_STR2:
//         memcpy(&p_buf[12], &rxFrame.data[2], 6);
//         p_buf[STR_NAME_LEN] = 0;
//         if (paraItemBak != PARA_STR1)
//             paraCode |= PARA_ERROR_ITEM;
//         else if (point < paraNumTab[type])
//         {
//             switch (type)
//             {
//             case WRITE_STR:
//                 paraInfo.writeStr[point].str = QString(QLatin1String(p_buf));
//                 break;
//             case READ_STR:
//                 paraInfo.readStr[point].str = QString(QLatin1String(p_buf));
//                 break;
//             default:
//                 paraCode |= PARA_ERROR_ITEM_MATCH;
//                 break;
//             }
//         }
//         break;
//     case PARA_DONE:
//         paraCode = 0;
//         paraInfo.echoNum = rxFrame.data[0];
//         paraInfo.meaTimerNum = rxFrame.data[1];
//         tempU16 = rxFrame.data[2];
//         if (tempU16 != paraInfo.numTab[WRITE_STR])
//             paraErrorCode |= PARA_ERROR_WRITE_STR_NUM_MATCH;
//         tempU16 = rxFrame.data[3];
//         if (tempU16 != paraInfo.numTab[READ_STR])
//             paraErrorCode |= PARA_ERROR_READ_STR_NUM_MATCH;
//         memcpy(&tempU16, &rxFrame.data[4], 2);
//         if (tempU16 != paraInfo.numTab[WRITE_PARA])
//             paraErrorCode |= PARA_ERROR_WRITE_PARA_NUM_MATCH;
//         memcpy(&tempU16, &rxFrame.data[6], 2);
//         if (tempU16 != paraInfo.numTab[READ_PARA])
//             paraErrorCode |= PARA_ERROR_READ_PARA_NUM_MATCH;

//         if (paraErrorCode)
//             otaStatus = OTA_STA_GET_PARA_INFO_ERROR;
//         else
//             otaStatus = OTA_STA_GET_PARA_INFO_SUCCESS;
//         emit sendState();

//         hasAcked = true;
//         paraItemBak = item;
//         return;
//     default:
//         paraCode |= PARA_ERROR_NO_TYPE;
//         break;
//     }

//     if (point > paraInfo.numTab[type])
//     {
//         paraCode |= PARA_ERROR_POINT_CONT;
//     }
//     else if (point == paraInfo.numTab[type])
//     {
//         paraInfo.numTab[type]++;
//     }

//     if (paraCode)
//     {
//         // qDebug()<<"error:"<<QString::number(paraCode,16)
//         //         <<" type:"<<QString::number(type,10)<<" typeBak:"<<QString::number(paraTypeBak,10)
//         //         <<" item:"<<QString::number(item,10)<<" itemBak:"<<QString::number(paraItemBak,10)
//         //         <<" point:"<<QString::number(point,10)<<" pointBak:"<<QString::number(paraPointBak,10)
//         //           ;
//         paraErrorCode |= paraCode;
//     }

//     paraPointBak = point;
//     paraItemBak = item;
//     paraTypeBak = type;
// }

// 读取参数
// void otaThread::rxdPack_PARA_READ()
// {
//     if (runStep != STEP_READ_PARA_STR)
//         return;

//     quint16 code;
//     quint16 errorCode = (((rxFrame.exId.data & 0XFF00) >> 8) == 1) ? PARA_READ_ERROR_CODE : 0;
//     enum paraType type = (enum paraType)rxFrame.data[2];

//     quint8 indexMax = (paraByteNumTab[type] / 4) + ((paraByteNumTab[type] % 4) ? 1 : 0);
//     quint8 index = rxFrame.data[3];
//     memcpy(&code, &rxFrame.data[0], 2);

//     if (code != paraInfo.codeNeed)
//         errorCode |= PARA_READ_ERROR_NOT_NEED;
//     else if (
//         (((paraInfo.itemIndex == WRITE_STR) || (paraInfo.itemIndex == READ_STR)) && (type != ParaType_STRING)) || ((paraInfo.itemIndex == WRITE_PARA) && (type != paraInfo.writePara[paraInfo.posIndex].type)) || ((paraInfo.itemIndex == READ_PARA) && (type != paraInfo.readPara[paraInfo.posIndex].type)))
//         errorCode |= PARA_READ_ERROR_TYPE;
//     else if ((index != indexBak) || (index >= indexMax))
//         errorCode |= PARA_READ_ERROR_INDEX;
//     else
//     {
//         memcpy(&p_buf[index * 4], &rxFrame.data[4], 4);
//         indexBak++;

//         if (indexBak >= indexMax)
//         {
//             p_buf[STR_NAME_LEN] = 0;
//             switch (paraInfo.itemIndex)
//             {
//             case WRITE_STR:
//                 paraInfo.writeStr[paraInfo.posIndex].str = QString(QLatin1String(p_buf));
//                 break;
//             case READ_STR:
//                 paraInfo.readStr[paraInfo.posIndex].str = QString(QLatin1String(p_buf));
//                 break;
//             case WRITE_PARA:
//                 memcpy(&(paraInfo.writePara[paraInfo.posIndex].valR[0]), p_buf, paraByteNumTab[type]);
//                 break;
//             case READ_PARA:
//                 memcpy(&(paraInfo.readPara[paraInfo.posIndex].valR[0]), p_buf, paraByteNumTab[type]);
//                 break;
//             default:
//                 errorCode |= PARA_READ_ERROR_CODE;
//                 break;
//             }
//         }
//         else
//             return;
//     }

//     if (errorCode)
//     {
//         paraErrorCode = errorCode;
//         otaStatus = OTA_STA_READ_PARA_ERROR;
//     }
//     else
//     {
//         otaStatus = OTA_STA_READ_PARA_SUCCESS;
//     }

//     runStep = STEP_STOP;

//     emit sendState();
// }

// 写参数
// void otaThread::rxdPack_PARA_Write()
// {
//     paraInfo.writeCmdBack = (enum paraWriteCmdType)((rxFrame.exId.data & 0X0300) >> 8);
//     paraInfo.writeErrorCode = (enum paraWriteErrorCode)((rxFrame.exId.data & 0XFC00) >> 10);

//     quint16 code;

//     memcpy(&code, &rxFrame.data[0], 2);

//     if (paraInfo.writeCmdBack != paraInfo.writeCmd)
//     {
//         paraErrorCode |= PARA_WR_ERROR_CMD_MATCH;
//     }
//     else if ((paraInfo.codeNeed != code) && (paraInfo.writeCmdBack != WRITE_CMD_SAVE_ONLY))
//     {
//         paraErrorCode |= PARA_WR_ERROR_CODE_MATCH;
//     }
//     else if (rxFrame.data[2] != paraInfo.typeNeed)
//     {
//         paraErrorCode |= PARA_WR_ERROR_TYTE;
//     }
//     else if (rxFrame.data[3] != paraInfo.wIndex)
//     {
//         paraErrorCode |= PARA_WR_ERROR_FRAME;
//     }
//     else if (paraInfo.writeErrorCode)
//     {
//         paraErrorCode |= (1 << paraInfo.writeErrorCode);
//     }

//     if (paraErrorCode)
//     {
//         otaStatus = OTA_STA_WRITE_PARA_ERROR;
//     }
//     else if (paraInfo.writeCmd == WRITE_CMD_SAVE_ONLY)
//     {
//         otaStatus = OTA_STA_SAVE_PARA_SUCCESS;
//     }
//     else if (paraInfo.wLen)
//     {
//         paraInfo.wIndex++;

//         // struct canPack pack;
//         // memset(&pack, 0, sizeof(struct canPack));
//         // pack.len = 8;
//         // pack.exId.id = devCanId;

//         // pack.exId.data = CAN_ID_DEBUG_UI | (((quint16)paraInfo.writeCmd) << 8);
//         // pack.exId.mode = CANCOM_PARA_WRITE;
//         // memcpy(&(pack.data[0]), &(paraInfo.codeNeed), 2);
//         // pack.data[2] = paraInfo.typeNeed;
//         // pack.data[3] = paraInfo.wIndex;
//         // quint8 txLen = (paraInfo.wLen >= 4) ? 4 : paraInfo.wLen;
//         // paraInfo.wLen -= txLen;
//         // memcpy(&(pack.data[4]), &p_buf[paraInfo.wOffset], txLen);
//         // paraInfo.wOffset += txLen;
//         // txdPack(&pack);
//         return;
//     }
//     else
//     {
//         otaStatus = OTA_STA_WRITE_PARA_SUCCESS;
//     }

//     runStep = STEP_STOP;

//     emit sendState();
// }

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
// void otaThread::rxdPack_PARA_UPDATE()
// {
//     quint8 cmd = (quint8)((rxFrame.exId.data & 0X0300) >> 8);  // 可能是模式两位有四种模式
//     quint8 ack = (quint8)((rxFrame.exId.data & 0X0C00) >> 10); // 这是错误代码。10bit
//     quint8 pos = (quint8)((rxFrame.exId.data & 0XF000) >> 12); // 这个是帧的序号。
//     quint16 tempU16;

//     quint8 i = 0;
//     paraErrorCode = 0;

//     runStep = STEP_STOP;

//     switch (cmd)
//     {
//     case 0:
//         memcpy(&chTickUs, &rxFrame.data[0], 4);        // 这个是时间是PWM周期
//         memcpy(&chEchoDevFreMax, &rxFrame.data[4], 2); // 固定值最大可设置频率
//         memcpy(&chEchoDevFreMin, &rxFrame.data[6], 2); // 固定值 1
//         if (ack)                                       // 错误代码
//         {
//             paraErrorCode |= PARA_UPDATE_ERROR_FRESET;
//             otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//         }
//         else
//         {
//             otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_FRESET;
//         }
//         break;
//     case 1:
//         chTickBak = 0;
//         chTimeStamp = 0.0f;
//         switch (ack)
//         {
//         case 0:
//             if (chSelPos >= chSelNum)
//             {
//                 for (i = 0; i < chSelNum; i++)
//                     chTypeDev[chSelIndex[i]] = (enum paraType)((i % 2) ? ((rxFrame.data[i / 2] & 0XF0) >> 4) : (rxFrame.data[i / 2] & 0X0F)); // 发上来的是拼接的数据，这里还原，根据

//                 otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_CHSET;

//                 for (i = 0; i < CHANNAL_SEL_NUM; i++)
//                     mData[i].clear();

//                 scopeShowNum = (quint32)(chEchoFreHz / WAVE_SHOW_FRE);

//                 if (scopeShowNum == 0)
//                     scopeShowNum = 1;
//                 scopeShowCount = 0;
//                 scopeWindowNum = (quint32)(WAVE_TIME_WINDOW * chEchoFreHz);
//                 scopeWindowCount = 0;
//                 scopeWindowRemoveFlag = false;

//                 // qDebug()<<"scopeShowNum"<<QString::number(scopeShowNum,10)
//                 //  <<"  scopeWindowNum"<<QString::number(scopeWindowNum,10);

//                 // qDebug()<<"通道数量："<<QString::number(chSelNum,10);
//                 for (i = 0; i < chSelNum; i++)
//                 {
//                     // qDebug()<<"needType:"<<QString::number(chTypeNeed[chSelIndex[i]],10)
//                     //         <<"devType:"<<QString::number(chTypeDev[chSelIndex[i]],10);
//                     if (chTypeDev[chSelIndex[i]] != chTypeNeed[chSelIndex[i]])
//                     {
//                         paraErrorCode |= PARA_UPDATE_ERROR_CHSET_TYPE;
//                         otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//                     }
//                 }
//             }
//             else
//             {
//                 setCmd(STEP_SCOPE_SET_CHANNAL);
//                 return;
//             }
//             break;
//             // 下面是报错可以暂时不理会
//         case 1:
//             paraErrorCode |= PARA_UPDATE_ERROR_CHSET_NOCODE;
//             otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//             break;
//         case 2:
//             paraErrorCode |= PARA_UPDATE_ERROR_CHSET_FRE;
//             otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//             break;
//         case 3:
//         default:
//             paraErrorCode |= PARA_UPDATE_ERROR_CHSET_LOSE;
//             otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//             break;
//         }
//         break;
//     case 2:
//         scopeIngFlag = true; //
//         if (ack)
//         {
//             paraErrorCode |= PARA_UPDATE_ERROR_START;
//             otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//         }
//         else
//         {
//             tempU16 = pos * 8;                                // 起始地址
//             memcpy(&chRxdByte[tempU16], &rxFrame.data[0], 8); // 接收到的字节全部放在这里

//             tempU16 += 8; // 结束地址

//             // qDebug()<<"pos:"<<QString::number(pos,10)<<" need"<<QString::number(chNeedByteTotal,10);

//             if (tempU16 >= chNeedByteTotal)
//             {
//                 // qDebug()<<"need:"<<QString::number(chNeedByteTotal,10);
//                 scope_dataProcess(); // 这里是把数据分配到每一个通道
//             }

//             return;
//         }
//         break;
//     case 3:
//         scopeIngFlag = false;
//         otaStatus = OTA_STA_PARA_UPDATE_SUCCESS_END;
//         break;
//     default:
//         paraErrorCode |= PARA_UPDATE_ERROR_CMD;
//         otaStatus = OTA_STA_PARA_UPDATE_ERROR;
//         break;
//     }
//     emit sendState();
// }

void otaThread::scope_dataProcess() //
{
    quint16 tick;
    memcpy(&tick, &chRxdByte[0], 2); // 这个时间表示的是，这一次数据的时间，应该是用来做位横轴的刻度的
    if (Burst_mode)
    {
        if (tick > chTickBak){


            chTimeStamp += (tick - chTickBak) / 1000000.0f;
            // 转换成以秒为单位

        }
        else{
            chTimeStamp += ((65536 + tick - chTickBak) / 1000000.0f);
        }
        chTickBak =tick;

    }
    else {
        // 非突发模式：使用chTickUs作为时间单位（假设为微秒）
        if (tick > chTickBak) {
            chTimeStamp +=(( (tick - chTickBak) * chTickUs) / 1000000.0f);
        } else {
            chTimeStamp += ((65536 + tick - chTickBak) * chTickUs / 1000000.0f);
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
