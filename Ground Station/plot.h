#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QPainter>

class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = 0);
    void addValue(double y, int type);
    void addValue(int t, double y, int type);
    void setXSteps(int steps = 10);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int yToPlot(double y);

    int xSteps = 10;
    int currT = 0;
    double min, max = 0;
    QVector<QVector<double> > data;
    const QVector<QColor> colors = {Qt::blue, Qt::green, Qt::red, Qt::yellow};
};

#endif // PLOT_H
