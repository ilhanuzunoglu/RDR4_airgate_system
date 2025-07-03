#ifndef ROLEPAGE_PASSENGER_H
#define ROLEPAGE_PASSENGER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMap>
#include <QLabel>
#include "weatherinfo.h"
#include "newsinfo.h"
#include "currencyinfo.h"

namespace Ui {
class RolePage_Passenger;
}

class RolePage_Passenger : public QWidget {
    Q_OBJECT
public:
    explicit RolePage_Passenger(QWidget *parent = nullptr);
    ~RolePage_Passenger();
    void setPassengerInfo(const QString &uid);
private slots:
    void onCityChanged(int index);
    void onWeatherDataReady(const QString &info);
    void onNewsDataReady(const QString &info);
    void onCurrencyDataReady(const QString &info);
    void onFlightReply(QNetworkReply* reply);
    void onBackClicked();
private:
    Ui::RolePage_Passenger *ui;
    QString currentUid;
    QNetworkAccessManager *networkManager;
    WeatherInfo *weatherInfo;
    NewsInfo *newsInfo;
    CurrencyInfo *currencyInfo;
    QMap<QString, QString> cityToIata; // Şehir -> IATA kodu
    QMap<QString, QString> cityToCountry; // Şehir -> Ülke kodu
    QMap<QString, QString> cityToApiName; // Şehir -> API'de kullanılacak isim
    void setupCities();
    void updateAllData();
    void updateFlightInfo(const QString &city);
    void updateWeather(const QString &city);
    void updateCurrency(const QString &city);
    void updateNews(const QString &city);
};

#endif // ROLEPAGE_PASSENGER_H 