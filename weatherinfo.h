#ifndef WEATHERINFO_H
#define WEATHERINFO_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class WeatherInfo : public QObject {
    Q_OBJECT
public:
    explicit WeatherInfo(QObject *parent = nullptr);
    void requestWeatherData(const QString &city);
signals:
    void weatherDataReady(const QString &info);
private slots:
    void onNetworkReply(QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;
    QString apiKey;
};

#endif // WEATHERINFO_H 