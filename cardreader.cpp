#include "cardreader.h"
#include <QDebug>
#include <QTimer>

CardReader::CardReader(QObject *parent) : QObject(parent) {
    serial.setPortName("/dev/ttyS2");
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    connect(&serial, &QSerialPort::readyRead, this, &CardReader::handleReadyRead);
    connect(&pollTimer, &QTimer::timeout, this, &CardReader::sendPoll);
    pollTimer.setInterval(300); // 300ms aralıkla poll
}

void CardReader::start() {
    if (!serial.isOpen()) {
        if (!serial.open(QIODevice::ReadWrite)) {
            qWarning() << "[CardReader] Port açma hatası:" << serial.errorString();
            return;
        } else {
            qDebug() << "[CardReader] Port açıldı:" << serial.portName();
        }
    }
    cardDetected = false;
    pollTimer.start();
    qDebug() << "[CardReader] Poll döngüsü başlatıldı.";
}

void CardReader::stopPoll() {
    pollTimer.stop();
    qDebug() << "[CardReader] Poll döngüsü durduruldu.";
}

void CardReader::restartPoll() {
    cardDetected = false;
    if (serial.isOpen()) serial.clear(); // buffer temizliği
    pollTimer.start();
    qDebug() << "[CardReader] Poll döngüsü tekrar başlatıldı.";
}

void CardReader::sendPoll() {
    if (!cardDetected && serial.isOpen()) {
        QByteArray pollCmd = QByteArray::fromHex("020A003EDF7E01009603");
        serial.write(pollCmd);
        qDebug() << "[CardReader] Poll komutu gönderildi.";
    }
}

static QString parseUidFromData(const QByteArray &data) {
    // DF 0D ile başlayan alanı bul, ardından gelen byte uzunluk, sonra UID
    for (int i = 0; i < data.size() - 4; ++i) {
        if ((quint8)data[i] == 0xDF && (quint8)data[i+1] == 0x0D) {
            int uidLen = (quint8)data[i+2];
            if (i + 3 + uidLen <= data.size()) {
                QByteArray uidBytes = data.mid(i+3, uidLen);
                return uidBytes.toHex(' ').toUpper();
            }
        }
    }
    return QString();
}

void CardReader::handleReadyRead() {
    QByteArray data = serial.readAll();
    qDebug() << "[CardReader] Veri okundu:" << data.toHex(' ').toUpper();
    // Sadece gerçek kart verisi geldiğinde poll dursun ve cardRead tetiklensin
    if (!cardDetected && data.size() > 4 && (quint8)data[0] == 0x02 && (quint8)data[4] == 0xFF && (quint8)data[5] == 0x01) {
        QString uid = parseUidFromData(data);
        if (!uid.isEmpty()) {
            cardDetected = true;
            pollTimer.stop();
            qDebug() << "[CardReader] Kart algılandı, poll durdu. UID:" << uid;
            emit cardRead(uid);
        } else {
            qDebug() << "[CardReader] UID bulunamadı, poll devam ediyor.";
        }
    }
} 