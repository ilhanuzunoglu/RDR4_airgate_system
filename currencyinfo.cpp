#include "currencyinfo.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSslConfiguration>
#include <QSslSocket>

CurrencyInfo::CurrencyInfo(QObject *parent) : QObject(parent),
    manager(new QNetworkAccessManager(this)),
    apiKey("enter your key") // exchangerate-api.com anahtarı
{
    connect(manager, &QNetworkAccessManager::finished, this, &CurrencyInfo::onNetworkReply);
}

void CurrencyInfo::requestCurrencyData(const QString &city)
{
    Q_UNUSED(city);

    QString baseCurrency = "TRY";
    QString url = QString("https://v6.exchangerate-api.com/v6/%1/latest/%2")
            .arg(apiKey)
            .arg(baseCurrency);

    QUrl qurl(url);
    QNetworkRequest request(qurl);

    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    manager->get(request);
}

void CurrencyInfo::onNetworkReply(QNetworkReply *reply)
{
    if(reply->error()) {
        emit currencyDataReady("API Hatası: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if(jsonDoc.isNull() || !jsonDoc.isObject()) {
        emit currencyDataReady("Geçersiz API cevabı.");
        reply->deleteLater();
        return;
    }

    QJsonObject obj = jsonDoc.object();

    if(obj["result"].toString() != "success") {
        emit currencyDataReady("API başarısız.");
        reply->deleteLater();
        return;
    }

    QJsonObject rates = obj["conversion_rates"].toObject();

    double usd = rates["USD"].toDouble();
    double eur = rates["EUR"].toDouble();

    QString info = QString("1 %1 = %2 USD\n1 %1 = %3 EUR")
            .arg("TRY")
            .arg(usd)
            .arg(eur);

    emit currencyDataReady(info);
    reply->deleteLater();
} 
