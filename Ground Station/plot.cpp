#include "plot.h"
#include <QDebug>

PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent)
{

}

void PlotWidget::addValue(double y, int type)
{
    this->addValue(currT, y, type);
    if(type == 0) {
        currT++;
    }
}

void PlotWidget::addValue(int t, double y, int type)
{
    if(data.size() <= type) {
        data.resize(type+1);
    }

    if(data[type].size() <= t) {
        data[type].resize(t+1);
    }

    data[type][t] = y;

    this->update();
}

void PlotWidget::setXSteps(int steps)
{
    xSteps = steps;
}

void PlotWidget::clear()
{
    data.clear();
    currT = 0;
}


void PlotWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QSize size = this->size();

    painter.fillRect(QRect(QPoint(0,0), size), Qt::black);
    painter.setPen(Qt::white);

    double min = std::numeric_limits<double>::infinity();
    double max = -std::numeric_limits<double>::infinity();

    for(int t = 0; t < data.length(); t++) {
        for(int s = 0; s < xSteps; s++) {
            int index = data[t].size() - xSteps + s;
            if (index >= 0) {
                min = std::min(min, data[t][index]);
                max = std::max(max, data[t][index]);
            }
        }
    }

    if (data.empty() || data[0].empty()) {
        min = 0;
        max = 1;
    }


    if (max <= min) {
        min -= 1;
        max += 1;
    } else {
        auto diff = max - min;
        min -= 0.1 * diff;
        max += 0.1 * diff;
    }

    int xAxisPos = yToPlot(0, min, max);

    painter.drawLine(0, xAxisPos, size.width(), xAxisPos);
    painter.drawLine(0, 0, 0, size.height());


    double step = (max - min) / xNumbers;

    for(int c = 0; c < xNumbers; c++) {
        painter.drawText(0, yToPlot(step * c + min, min, max), QString::number(c*step+min));
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

                painter.drawLine(lastX,yToPlot(data[t][index-1], min, max) , x, yToPlot(data[t][index], min, max));
            }
        }
    }
}

int PlotWidget::yToPlot(double y, double drawMin, double drawMax)
{
    QSize size = this->size();
    return size.height() - static_cast<int>((size.height()-1) * (y-drawMin)/ (drawMax - drawMin)) - 1;
}
