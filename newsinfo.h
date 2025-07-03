#ifndef NEWSINFO_H
#define NEWSINFO_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class NewsInfo : public QObject {
    Q_OBJECT
public:
    explicit NewsInfo(QObject *parent = nullptr);
    void requestNewsData(const QString &city);
signals:
    void newsDataReady(const QString &info);
private slots:
    void onNetworkReply(QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;
    QString apiKey;
};

#endif // NEWSINFO_H 