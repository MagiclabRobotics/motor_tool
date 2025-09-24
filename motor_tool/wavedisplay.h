#ifndef WAVEDISPLAY_H
#define WAVEDISPLAY_H

#include <QWidget>
#include "qcustomplot.h"
#include "otaThread.h"

namespace ChannelColor
{
    const static QColor Color[CHANNAL_SEL_NUM] = {
        QColor(255, 0, 0),   // Channel1Color (255,  0,  0)
        QColor(0, 255, 0),   // Channel2Color (  0,128,  0)
        QColor(255, 140, 0), // Channel3Color (  0,  0,255)
        QColor(255, 215, 0), // Channel4Color (205,133, 63)
        QColor(255, 0, 255), // Channel5Color (255, 99, 71)
        QColor(0, 255, 255), // Channel6Color (127,255,  0)
        QColor(0, 0, 255),   // Channel7Color ( 64,224,208)
        QColor(128, 0, 128), // Channel8Color (255,215,  0)
    };
}

class waveDisplay : public QCustomPlot
{
    Q_OBJECT
public:
    waveDisplay(QWidget *parent);

    void clearAllChannal();

    void configChannal(quint8 *indexTab, quint8 channalNum);

    void addData(double timeStamp, double *paraTab);
    void setData(QVector<QCPGraphData> *paraTab);

private:
    quint8 channalIndexTab[CHANNAL_SEL_NUM];
    quint8 channalEnableNum;
    double Xmax = 10, Xmin = 0;
    double Ymax = 100, Ymin = 0;
};

#endif // WAVEDISPLAY_H
