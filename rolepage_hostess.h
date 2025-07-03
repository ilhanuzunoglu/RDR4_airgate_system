#ifndef ROLEPAGE_HOSTESS_H
#define ROLEPAGE_HOSTESS_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QComboBox>

namespace Ui {
class RolePage_Hostess;
}

class RolePage_Hostess : public QWidget {
    Q_OBJECT
public:
    explicit RolePage_Hostess(QWidget *parent = nullptr);
    ~RolePage_Hostess();
    void setHostessInfo(const QString &uid);
private slots:
    void onCityChanged(int index);
    void onFlightReply(QNetworkReply* reply);
    void onBackClicked();
private:
    Ui::RolePage_Hostess *ui;
    QNetworkAccessManager *networkManager;
    QMap<QString, QString> cityToIata;
    void setupCities();
    void updateFlightInfo(const QString &city);
};

#endif // ROLEPAGE_HOSTESS_H 