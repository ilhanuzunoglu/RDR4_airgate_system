#include "rolepage_hostess.h"
#include "ui_rolepage_hostess.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

RolePage_Hostess::RolePage_Hostess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RolePage_Hostess),
    networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setupCities();
    connect(ui->comboCity, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RolePage_Hostess::onCityChanged);
    connect(networkManager, &QNetworkAccessManager::finished, this, &RolePage_Hostess::onFlightReply);
    connect(ui->btnBack, &QPushButton::clicked, this, &RolePage_Hostess::onBackClicked);
    ui->comboCity->setCurrentIndex(0);
    updateFlightInfo(ui->comboCity->currentText());
}

RolePage_Hostess::~RolePage_Hostess() {
    delete ui;
}

void RolePage_Hostess::setHostessInfo(const QString &uid) {
    Q_UNUSED(uid);
    // Artık labelHostessInfo yok, UID gösterimi istenirse yeni bir label eklenebilir.
}

void RolePage_Hostess::setupCities() {
    cityToIata = {
        {"İstanbul", "IST"}, {"Ankara", "ESB"}, {"İzmir", "ADB"},
        {"Berlin", "BER"}, {"Paris", "CDG"}, {"Londra", "LHR"},
        {"New York", "JFK"}, {"Tokyo", "HND"}
    };
    if (ui->comboCity) ui->comboCity->clear();
    for (const QString &city : cityToIata.keys())
        ui->comboCity->addItem(city);
}

void RolePage_Hostess::onCityChanged(int index) {
    Q_UNUSED(index);
    QString city = ui->comboCity->currentText();
    updateFlightInfo(city);
    // Şehre göre default bilgiler
    if (city == "İstanbul") {
        if (ui->labelAircraftInfo) ui->labelAircraftInfo->setText("Model: Airbus A320\nTescil: TC-ABC\nKapasite: 180\nÜretici: Airbus\nSeri No: 12345\nUçak Yaşı: 6 yıl");
        if (ui->labelSeatsInfo) ui->labelSeatsInfo->setText("Toplam Koltuk: 180\nDolu: 150\nBoş: 30\nEngelli Koltuk: 2\nKoltuk Planı: 1A 1B 1C ...");
        if (ui->labelMenuInfo) ui->labelMenuInfo->setText("Yemekler: Tavuk, Makarna, Vejetaryen\nİçecekler: Su, Meyve Suyu, Çay, Kahve\nAlerjenler: Gluten, Süt, Fındık\nÖzel Sipariş: Vejetaryen (2), Çocuk (1)");
    } else if (city == "Paris") {
        if (ui->labelAircraftInfo) ui->labelAircraftInfo->setText("Model: Boeing 737\nTescil: FR-XYZ\nKapasite: 160\nÜretici: Boeing\nSeri No: 54321\nUçak Yaşı: 4 yıl");
        if (ui->labelSeatsInfo) ui->labelSeatsInfo->setText("Toplam Koltuk: 160\nDolu: 120\nBoş: 40\nEngelli Koltuk: 1\nKoltuk Planı: 2A 2B 2C ...");
        if (ui->labelMenuInfo) ui->labelMenuInfo->setText("Yemekler: Balık, Makarna, Vegan\nİçecekler: Su, Şarap, Çay, Kahve\nAlerjenler: Balık, Süt\nÖzel Sipariş: Vegan (1), Çocuk (2)");
    } else if (city == "Berlin") {
        if (ui->labelAircraftInfo) ui->labelAircraftInfo->setText("Model: Embraer 190\nTescil: DE-EMB\nKapasite: 110\nÜretici: Embraer\nSeri No: 67890\nUçak Yaşı: 2 yıl");
        if (ui->labelSeatsInfo) ui->labelSeatsInfo->setText("Toplam Koltuk: 110\nDolu: 80\nBoş: 30\nEngelli Koltuk: 1\nKoltuk Planı: 3A 3B 3C ...");
        if (ui->labelMenuInfo) ui->labelMenuInfo->setText("Yemekler: Sosis, Salata, Vejetaryen\nİçecekler: Su, Bira, Çay\nAlerjenler: Süt, Gluten\nÖzel Sipariş: Vejetaryen (1)");
    } else {
        if (ui->labelAircraftInfo) ui->labelAircraftInfo->setText("Model: Airbus A321\nTescil: DEF-123\nKapasite: 200\nÜretici: Airbus\nSeri No: 24680\nUçak Yaşı: 3 yıl");
        if (ui->labelSeatsInfo) ui->labelSeatsInfo->setText("Toplam Koltuk: 200\nDolu: 170\nBoş: 30\nEngelli Koltuk: 2\nKoltuk Planı: 4A 4B 4C ...");
        if (ui->labelMenuInfo) ui->labelMenuInfo->setText("Yemekler: Et, Tavuk, Vejetaryen\nİçecekler: Su, Meyve Suyu, Çay\nAlerjenler: Süt, Fındık\nÖzel Sipariş: Vejetaryen (2)");
    }
}

void RolePage_Hostess::updateFlightInfo(const QString &city) {
    QString iata = cityToIata.value(city, "IST");
    QString apiKey = "abf69b83dac7991586a8d886f84081ce";
    QString url = QString("http://api.aviationstack.com/v1/flights?access_key=%1&arr_iata=%2&limit=1").arg(apiKey).arg(iata);
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void RolePage_Hostess::onFlightReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonArray flights = doc.object()["data"].toArray();
        if (!flights.isEmpty()) {
            QJsonObject flight = flights[0].toObject();
            QString dep = flight["departure"].toObject()["scheduled"].toString().replace("T", " ").left(16);
            QString arr = flight["arrival"].toObject()["scheduled"].toString().replace("T", " ").left(16);
            QString depAirport = flight["departure"].toObject()["iata"].toString();
            QString arrAirport = flight["arrival"].toObject()["iata"].toString();
            QString route = QString("%1 (%2) → %3 (%4)")
                .arg(flight["departure"].toObject()["airport"].toString())
                .arg(depAirport)
                .arg(flight["arrival"].toObject()["airport"].toString())
                .arg(arrAirport);
            QString times = QString("Kalkış: %1\nVarış: %2").arg(dep).arg(arr);
            if (ui->labelTimesInfo) ui->labelTimesInfo->setText(times);
            if (ui->labelRouteInfo) ui->labelRouteInfo->setText(route);
        }
    }
    reply->deleteLater();
}

void RolePage_Hostess::onBackClicked() {
    QWidget *parentW = parentWidget();
    while (parentW && !parentW->inherits("QMainWindow"))
        parentW = parentW->parentWidget();
    if (parentW) {
        QMetaObject::invokeMethod(parentW, "showHomePage");
    }
} 