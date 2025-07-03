#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QTimer>

class CardReader : public QObject {
    Q_OBJECT
public:
    explicit CardReader(QObject *parent = nullptr);
    void start();
    void stopPoll();
    void restartPoll();
signals:
    void cardRead(const QString &uid);
private slots:
    void handleReadyRead();
    void sendPoll();
private:
    QSerialPort serial;
    QTimer pollTimer;
    bool cardDetected = false;
};

#endif // CARDREADER_H 