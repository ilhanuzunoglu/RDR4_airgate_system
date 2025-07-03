#include "newsinfo.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSslConfiguration>
#include <QSslSocket>

NewsInfo::NewsInfo(QObject *parent) : QObject(parent),
    manager(new QNetworkAccessManager(this)),
    apiKey("enter your key") // newsapi.org anahtarı
{
    connect(manager, &QNetworkAccessManager::finished, this, &NewsInfo::onNetworkReply);
}

void NewsInfo::requestNewsData(const QString &city)
{
    QString url = QString("https://newsapi.org/v2/everything?q=%1&apiKey=%2&language=tr&pageSize=5")
            .arg(city)
            .arg(apiKey);

    QUrl qurl(url);
    QNetworkRequest request(qurl);

    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    manager->get(request);
}

void NewsInfo::onNetworkReply(QNetworkReply *reply)
{
    if(reply->error()) {
        emit newsDataReady("API Hatası: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if(jsonDoc.isNull() || !jsonDoc.isObject()) {
        emit newsDataReady("Geçersiz API cevabı.");
        reply->deleteLater();
        return;
    }

    QJsonObject obj = jsonDoc.object();
    QJsonArray articles = obj["articles"].toArray();

    if(articles.isEmpty()) {
        emit newsDataReady("Haber bulunamadı.");
        reply->deleteLater();
        return;
    }

    QString result;
    for (int i = 0; i < articles.size(); ++i) {
        QJsonObject article = articles.at(i).toObject();
        QString title = article["title"].toString();
        QString source = article["source"].toObject()["name"].toString();
        QString url = article["url"].toString();

        result += QString("%1 - %2\n%3\n\n").arg(title).arg(source).arg(url);
    }

    emit newsDataReady(result);
    reply->deleteLater();
} 
