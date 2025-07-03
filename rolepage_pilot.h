#ifndef ROLEPAGE_PILOT_H
#define ROLEPAGE_PILOT_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QPixmap>

namespace Ui {
class RolePage_Pilot;
}

class RolePage_Pilot : public QWidget {
    Q_OBJECT
public:
    explicit RolePage_Pilot(QWidget *parent = nullptr);
    ~RolePage_Pilot();
    void setPilotInfo(const QString &uid);
private slots:
    void onCityChanged(int index);
    void onFlightReply(QNetworkReply* reply);
    void onPlaneReply(QNetworkReply* reply);
    void onAirportReply(QNetworkReply* reply);
    void onFlightBoxClicked();
    void onMapImageDownloaded(QNetworkReply* reply);
    void onBackClicked();
private:
    Ui::RolePage_Pilot *ui;
    QNetworkAccessManager *networkManager;
    QNetworkAccessManager *mapManager;
    QString currentUid;
    QString currentFlightIata;
    QString currentPlaneReg;
    QString currentDepIata;
    QString currentArrIata;
    double currentLat = 0.0;
    double currentLon = 0.0;
    QMap<QString, QString> cityToIata;
    QMap<QString, QString> cityToApiName;
    void setupCities();
    void updateAllData();
    void updateFlightInfo(const QString &city);
    void updatePlaneInfo(const QString &regNum, const QString &icao24 = QString(), const QString &fallback = QString());
    void updateAirportInfo(const QString &depIata, const QString &arrIata);
    void updateMap(double lat, double lon);
    void clearFlightBoxes();
};

#endif // ROLEPAGE_PILOT_H 