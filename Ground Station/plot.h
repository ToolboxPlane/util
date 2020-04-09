#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QPainter>

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = 0);
    void addValue(double y, int type);
    void addValue(int t, double y, int type);
    void setXSteps(int steps = 10);
    void clear();

protected:
    void paintEvent(QPaintEvent *event);

private:
    int yToPlot(double y, double drawMin, double drawMax);

    int xSteps = 100;
    int yNumbers = 10;
    int xNumbers = 10;
    int currT = 0;
    QVector<QVector<double> > data;
    const QVector<QColor> colors = {Qt::blue, Qt::green, Qt::red, Qt::yellow};
};

#endif // PLOT_H
