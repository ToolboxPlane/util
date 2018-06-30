#include "csvreaderthread.h"

#include <QTextStream>
#include <QDateTime>

void CsvReaderThread::run()
{
    QTextStream *recordingStream = new QTextStream(file);
    long startTimeRecording = recordingStream->readLine().toLong();
    QString header = recordingStream->readLine();

    long startTimeReplay = QDateTime::currentMSecsSinceEpoch();

    while(!recordingStream->atEnd()) {
        QStringList items = recordingStream->readLine().split(';');
        long time = items.at(0).toLong();
        long deltaRecording = time - startTimeRecording;
        long deltaReplay = QDateTime::currentMSecsSinceEpoch() - startTimeReplay;
        if(deltaReplay < deltaRecording) {
            QThread::msleep(deltaRecording - deltaReplay);
        }
        emit sampleReady(items);
    }

    file->close();
}

CsvReaderThread::CsvReaderThread(QFile *file)
{
    this->file = file;
}
