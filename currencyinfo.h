#ifndef CURRENCYINFO_H
#define CURRENCYINFO_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class CurrencyInfo : public QObject {
    Q_OBJECT
public:
    explicit CurrencyInfo(QObject *parent = nullptr);
    void requestCurrencyData(const QString &city);
signals:
    void currencyDataReady(const QString &info);
private slots:
    void onNetworkReply(QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;
    QString apiKey;
};

#endif // CURRENCYINFO_H 