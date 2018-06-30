#ifndef CSVREADERTHREAD_H
#define CSVREADERTHREAD_H

#include <QThread>
#include <QFile>
#include <QStringList>
#include "rcLib.hpp"

class CsvReaderThread : public QThread
{
    Q_OBJECT
    void run() override;
public:
    CsvReaderThread(QFile *file);

signals:
    void sampleReady(QStringList items);

private:
    QFile *file;
};

#endif // CSVREADERTHREAD_H
