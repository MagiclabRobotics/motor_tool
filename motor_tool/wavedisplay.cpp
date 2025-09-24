#include "wavedisplay.h"

waveDisplay::waveDisplay(QWidget *parent) : QCustomPlot(parent)
{
    QPen pen;
    pen.setWidth(1); // 曲线的粗细
    for (quint8 i = 0; i < CHANNAL_SEL_NUM; i++)
    {
        addGraph();
        pen.setColor(ChannelColor::Color[i]);
        graph(i)->setPen(pen); // 曲线颜色设置
    }

    // 设置背景色
    setBackground(QColor(50, 50, 50));

    setInteraction(QCP::iRangeDrag, true);                     // 鼠标单击拖动
    selectionRect()->setPen(QPen(Qt::black, 1, Qt::DashLine)); // 设置选框的样式：虚线
    selectionRect()->setBrush(QBrush(QColor(0, 0, 100, 50)));  // 设置选框的样式：半透明浅蓝
    setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    setInteraction(QCP::iRangeZoom, true); // 滚轮滑动缩放

    // 设置x/y轴文本色、轴线色、字体等
    xAxis->setTickLabelColor(Qt::white); // 刻度的颜色
    xAxis->setLabelColor(Qt::white);
    xAxis->setBasePen(QPen(Qt::white));
    xAxis->setTickPen(QPen(Qt::white));
    xAxis->setSubTickPen(QPen(Qt::white));
    QFont xFont = xAxis->labelFont();
    xFont.setPixelSize(20);
    xAxis->setLabelFont(xFont);
    xAxis->setLabel("时间 (s)");

    yAxis->setTickLabelColor(Qt::white);
    yAxis->setLabelColor(Qt::white);
    yAxis->setBasePen(QPen(Qt::white));
    yAxis->setTickPen(QPen(Qt::white));
    yAxis->setSubTickPen(QPen(Qt::white));
    QFont yFont = yAxis->labelFont();
    yFont.setPixelSize(20);
    yAxis->setLabelFont(yFont);

    clearAllChannal();
}

void waveDisplay::clearAllChannal()
{
    for (quint8 i = 0; i < CHANNAL_SEL_NUM; i++)
    {
        graph(i)->data()->clear();
        graph(i)->setVisible(false);
    }
    // 默认显示范围
    Xmax = WAVE_TIME_WINDOW;
    Xmin = 0;
    Ymax = 1;
    Ymin = 0;

    channalEnableNum = 0;

    // 设置默认坐标轴显示范围，否则只能看到默认范围
    xAxis->setRange(Xmin, Xmax);
    yAxis->setRange(Ymin, Ymax);

    replot();
}

void waveDisplay::configChannal(quint8 *indexTab, quint8 channalNum)
{
    channalEnableNum = channalNum;
    for (quint8 i = 0; i < channalNum; i++)
    {
        graph(indexTab[i])->setVisible(true); // 根据下拉选项设置通道可见，

        channalIndexTab[i] = indexTab[i]; // 告诉上位机选择的通道
        // graph(indexTab[i])->data()->clear();
    }
}

void waveDisplay::addData(double timeStamp, double *paraTab)
{
    if (channalEnableNum == 0)
        return;

    uint8_t flag = 0;

    if (timeStamp > Xmax)
    {
        Xmax = timeStamp;
        Xmin = timeStamp - WAVE_TIME_WINDOW;
        flag++;
    }

    for (quint8 i = 0; i < channalEnableNum; i++)
    {
        graph(channalIndexTab[i])->addData(timeStamp, paraTab[i]);

        if (paraTab[i] > Ymax)
        {
            Ymax = paraTab[i];
            flag++;
        }
        else if (paraTab[i] < Ymin)
        {
            Ymin = paraTab[i];
            flag++;
        }
    }

    if (flag)
    {
        xAxis->setRange(Xmin, Xmax);
        yAxis->setRange(Ymin, Ymax);
    }
    replot();
}

void waveDisplay::setData(QVector<QCPGraphData> *paraTab)
{
    QSharedPointer<QCPGraphDataContainer> dataContainer;

    for (quint8 i = 0; i < channalEnableNum; i++)
    {
        dataContainer = graph(channalIndexTab[i])->data();
        dataContainer->set(paraTab[channalIndexTab[i]], true);

        if (i == 0)
            graph(channalIndexTab[i])->rescaleAxes();
        else
            graph(channalIndexTab[i])->rescaleAxes(true);
    }

    static int count = 0;
    count++;
    if(count > channalEnableNum)
    {
        count = 0;
        replot();
    }
}
