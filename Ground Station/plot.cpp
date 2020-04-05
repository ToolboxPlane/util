#include "plot.h"
#include <QDebug>

Plot::Plot(QWidget *parent) : QWidget(parent)
{

}

void Plot::addValue(double y, int type)
{
    this->addValue(currT, y, type);
    if(type == 0) {
        currT++;
    }
}

void Plot::addValue(int t, double y, int type)
{
    if(y < min) {
        min = y;
    } else if (y > max) {
        max = y;
    }

    if(data.size() <= type) {
        data.resize(type+1);
    }

    if(data[type].size() <= t) {
        data[type].resize(t+1);
    }

    data[type][t] = y;

    this->update();
}

void Plot::setXSteps(int steps)
{
    xSteps = steps;
}


void Plot::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QSize size = this->size();

    int xAxisPos = yToPlot(0);

    painter.drawLine(0, xAxisPos, size.width(), xAxisPos);
    painter.drawLine(0, 0, 0, size.height());

    double step = (this->max - this->min) / xNumbers;
    for(int c = 0; c < xNumbers; c++) {
        painter.drawText(0, yToPlot(step * c + min), QString::number(c*step+min));
    }

    for(int c = 1; c <= xSteps; c++) {
        int x = c * size.width() / xSteps;
        painter.drawLine(x, xAxisPos+3, x, xAxisPos-3);
    }

    for(int t = 0; t < data.length(); t++) {
        painter.setPen(QPen(colors[t % colors.size()], 2));
        for(int s = 1; s < xSteps; s++) {
            int index = data[t].size() - xSteps + s;
            if(index > 0) {
                int lastX = (s-1) * size.width() / xSteps;
                int x = s * size.width() / xSteps;

                painter.drawLine(lastX,yToPlot(data[t][index-1]) , x, yToPlot(data[t][index]));
            }
        }
    }
}

int Plot::yToPlot(double y)
{
    QSize size = this->size();
    return size.height() - (int)((size.height()-1) * (y-min)/ (max - min)) - 1;
}
