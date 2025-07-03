#include "weatherinfo.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSslConfiguration>
#include <QSslSocket>

WeatherInfo::WeatherInfo(QObject *parent) : QObject(parent),
    manager(new QNetworkAccessManager(this)),
    apiKey("enter your key") // OpenWeatherMap API anahtarı
{
    connect(manager, &QNetworkAccessManager::finished, this, &WeatherInfo::onNetworkReply);
}

void WeatherInfo::requestWeatherData(const QString &city)
{
    QString url = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric&lang=tr")
            .arg(city)
            .arg(apiKey);

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    manager->get(request);
}

void WeatherInfo::onNetworkReply(QNetworkReply *reply)
{
    if(reply->error()) {
        emit weatherDataReady("API Hatası: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if(jsonDoc.isNull() || !jsonDoc.isObject()) {
        emit weatherDataReady("Geçersiz API cevabı.");
        reply->deleteLater();
        return;
    }

    QJsonObject obj = jsonDoc.object();

    QString cityName = obj["name"].toString();
    double temp = obj["main"].toObject()["temp"].toDouble();
    int humidity = obj["main"].toObject()["humidity"].toInt();
    QString description = obj["weather"].toArray().at(0).toObject()["description"].toString();

    QString info = QString("%1 için Hava Durumu:\nSıcaklık: %2 °C\nNem: %3%\nDurum: %4")
            .arg(cityName)
            .arg(temp)
            .arg(humidity)
            .arg(description);

    emit weatherDataReady(info);
    reply->deleteLater();
} 
