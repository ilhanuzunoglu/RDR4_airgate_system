#include "rolepage_pilot.h"
#include "ui_rolepage_pilot.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QNetworkRequest>
#include <QUrl>
#include <QPixmap>

RolePage_Pilot::RolePage_Pilot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RolePage_Pilot),
    networkManager(new QNetworkAccessManager(this)),
    mapManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setupCities();
    connect(ui->comboCity, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RolePage_Pilot::onCityChanged);
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        QUrl url = reply->url();
        if (url.toString().contains("flights")) onFlightReply(reply);
        else if (url.toString().contains("airplanes")) onPlaneReply(reply);
        else if (url.toString().contains("airports")) onAirportReply(reply);
        reply->deleteLater();
    });
    connect(mapManager, &QNetworkAccessManager::finished, this, &RolePage_Pilot::onMapImageDownloaded);
    connect(ui->btnBack, &QPushButton::clicked, this, &RolePage_Pilot::onBackClicked);
    ui->comboCity->setCurrentIndex(0);
    updateAllData();
}

RolePage_Pilot::~RolePage_Pilot() {
    delete ui;
}

void RolePage_Pilot::setupCities() {
    cityToIata = {
        {"İstanbul", "IST"}, {"Ankara", "ESB"}, {"İzmir", "ADB"},
        {"Berlin", "BER"}, {"Paris", "CDG"}, {"Londra", "LHR"},
        {"New York", "JFK"}, {"Tokyo", "HND"}
    };
    cityToApiName = {
        {"İstanbul", "Istanbul"}, {"Ankara", "Ankara"}, {"İzmir", "Izmir"},
        {"Berlin", "Berlin"}, {"Paris", "Paris"}, {"Londra", "London"},
        {"New York", "New York"}, {"Tokyo", "Tokyo"}
    };
    ui->comboCity->clear();
    for (const QString &city : cityToIata.keys())
        ui->comboCity->addItem(city);
}

void RolePage_Pilot::setPilotInfo(const QString &uid) {
    currentUid = uid;
    ui->comboCity->setCurrentIndex(0);
    updateAllData();
}

void RolePage_Pilot::onCityChanged(int index) {
    Q_UNUSED(index);
    updateAllData();
}

void RolePage_Pilot::updateAllData() {
    QString city = ui->comboCity->currentText();
    updateFlightInfo(city);
}

void RolePage_Pilot::clearFlightBoxes() {
    QVBoxLayout *layout = ui->scrollAreaWidgetFlights->findChild<QVBoxLayout*>("layoutFlights");
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
    }
}

void RolePage_Pilot::updateFlightInfo(const QString &city) {
    clearFlightBoxes();
    QString iata = cityToIata.value(city, "IST");
    QString apiKey = "abf69b83dac7991586a8d886f84081ce";
    QString url = QString("http://api.aviationstack.com/v1/flights?access_key=%1&arr_iata=%2&limit=8").arg(apiKey).arg(iata);
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void RolePage_Pilot::onFlightReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QVBoxLayout *layout = ui->scrollAreaWidgetFlights->findChild<QVBoxLayout*>("layoutFlights");
    clearFlightBoxes();
    if (!doc.isNull() && doc.isObject()) {
        QJsonArray flights = doc.object()["data"].toArray();
        qDebug() << "[Flight API] Gelen veri:" << data;
        qDebug() << "[Flight API] Uçuş sayısı:" << flights.size();
        if (!flights.isEmpty()) {
            for (int i = 0; i < flights.size(); ++i) {
                QJsonObject flight = flights[i].toObject();
                QString flightNo = flight["flight"].toObject()["iata"].toString();
                QString airline = flight["airline"].toObject()["name"].toString();
                QString dep = flight["departure"].toObject()["scheduled"].toString().replace("T", " ").left(16);
                QString arr = flight["arrival"].toObject()["scheduled"].toString().replace("T", " ").left(16);
                QString depAirport = flight["departure"].toObject()["iata"].toString();
                QString arrAirport = flight["arrival"].toObject()["iata"].toString();
                QString status = flight["flight_status"].toString();
                QString gate = flight["departure"].toObject()["gate"].toString();
                QString terminal = flight["departure"].toObject()["terminal"].toString();
                QString delay = flight["departure"].toObject()["delay"].toString();
                QString reg = flight["aircraft"].toObject()["registration"].toString();
                QString icao24 = flight["aircraft"].toObject()["icao24"].toString();
                double lat = flight["live"].toObject()["latitude"].toDouble();
                double lon = flight["live"].toObject()["longitude"].toDouble();
                // Düz metin, çok satırlı ve okunaklı format
                QString infoText = QString(
                    "Uçuş: %1\nHavayolu: %2\nKalkış: %3 (%4)\nVarış: %5 (%6)\nDurum: %7\nKapı: %8  Terminal: %9  Gecikme: %10")
                    .arg(flightNo)
                    .arg(airline)
                    .arg(dep).arg(depAirport)
                    .arg(arr).arg(arrAirport)
                    .arg(status.isEmpty() ? "Bilinmiyor" : status)
                    .arg(gate.isEmpty() ? "-" : gate)
                    .arg(terminal.isEmpty() ? "-" : terminal)
                    .arg(delay.isEmpty() ? "-" : delay);
                QPushButton *box = new QPushButton;
                box->setText(infoText);
                box->setProperty("class", "flightBox");
                box->setCursor(Qt::PointingHandCursor);
                box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                box->setMinimumHeight(60);
                box->setProperty("flightIndex", i);
                // Temel teknik bilgileri kutuya property olarak ekle
                QString techInfo = QString("Uçak Modeli: %1\nICAO24: %2\nTescil: %3\nHavayolu: %4\nFlight No: %5\nAircraft ICAO: %6")
                    .arg(flight["aircraft"].toObject()["model"].toString().isEmpty() ? "-" : flight["aircraft"].toObject()["model"].toString())
                    .arg(icao24.isEmpty() ? "-" : icao24)
                    .arg(reg.isEmpty() ? "-" : reg)
                    .arg(airline.isEmpty() ? "-" : airline)
                    .arg(flightNo.isEmpty() ? "-" : flightNo)
                    .arg(flight["aircraft"].toObject()["icao"].toString().isEmpty() ? "-" : flight["aircraft"].toObject()["icao"].toString());
                box->setProperty("techInfo", techInfo);
                connect(box, &QPushButton::clicked, this, &RolePage_Pilot::onFlightBoxClicked);
                layout->addWidget(box);
                // İlk kutu için teknik ve harita sekmelerini otomatik güncelle
                if (i == 0) {
                    updatePlaneInfo(reg, icao24, box->property("techInfo").toString());
                    updateAirportInfo(depAirport, arrAirport);
                    if (lat != 0.0 && lon != 0.0) updateMap(lat, lon);
                    else { ui->labelLiveInfo->clear(); ui->labelLiveInfo->setText("Canlı konum verisi yok."); }
                }
            }
        } else {
            QLabel *label = new QLabel("API'den canlı uçuş bilgisi bulunamadı.");
            label->setStyleSheet("font-size: 18px;");
            label->setWordWrap(true);
            label->setTextFormat(Qt::RichText);
            layout->addWidget(label);
        }
    } else {
        QLabel *label = new QLabel("Uçuş bilgisi alınamadı (API hatası).");
        label->setStyleSheet("font-size: 18px;");
        label->setWordWrap(true);
        label->setTextFormat(Qt::RichText);
        layout->addWidget(label);
    }
    layout->addStretch();
}

void RolePage_Pilot::onFlightBoxClicked() {
    QPushButton *box = qobject_cast<QPushButton*>(sender());
    if (!box) return;
    int index = box->property("flightIndex").toInt();
    QString techFallback = box->property("techInfo").toString();
    QString city = ui->comboCity->currentText();
    QString iata = cityToIata.value(city, "IST");
    QString apiKey = "abf69b83dac7991586a8d886f84081ce";
    QString url = QString("http://api.aviationstack.com/v1/flights?access_key=%1&arr_iata=%2&limit=8").arg(apiKey).arg(iata);
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, [this, reply, index, techFallback]() {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull() && doc.isObject()) {
            QJsonArray flights = doc.object()["data"].toArray();
            if (index < flights.size()) {
                QJsonObject flight = flights[index].toObject();
                QString reg = flight["aircraft"].toObject()["registration"].toString();
                QString icao24 = flight["aircraft"].toObject()["icao24"].toString();
                QString depAirport = flight["departure"].toObject()["iata"].toString();
                QString arrAirport = flight["arrival"].toObject()["iata"].toString();
                double lat = flight["live"].toObject()["latitude"].toDouble();
                double lon = flight["live"].toObject()["longitude"].toDouble();
                updatePlaneInfo(reg, icao24, techFallback);
                updateAirportInfo(depAirport, arrAirport);
                if (lat != 0.0 && lon != 0.0) updateMap(lat, lon);
                else { ui->labelLiveInfo->clear(); ui->labelLiveInfo->setText("Canlı konum verisi yok."); }
            }
        }
        reply->deleteLater();
    });
}

void RolePage_Pilot::updatePlaneInfo(const QString &regNum, const QString &icao24, const QString &fallback) {
    // Önce teknik bilgi kutularını temizle
    QVBoxLayout *layout = ui->scrollAreaWidgetTech->findChild<QVBoxLayout*>("layoutTechBoxes");
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
    }
    auto addTechBox = [layout](const QString &info) {
        QLabel *label = new QLabel(info);
        label->setStyleSheet("font-size: 15px; background:#f7fafd; border:2px solid #2a4d69; border-radius:10px; padding:10px; margin-bottom:10px;");
        label->setWordWrap(true);
        layout->addWidget(label);
    };
    auto fillTechInfo = [addTechBox](const QJsonObject &plane, const QString &fallback) {
        QString reg = plane.value("registration_number").toString();
        QString model = plane.value("model").toString();
        QString manufacturer = plane.value("manufacturer").toString();
        QString icao24 = plane.value("icao24").toString();
        QString serial = plane.value("serial_number").toString();
        QString type = plane.value("plane_type").toString();
        QString year = plane.value("production_line").toString();
        QString info = QString("Tescil: %1\nModel: %2\nÜretici: %3\nICAO24: %4\nSeri No: %5\nTip: %6\nÜretim Yılı: %7")
            .arg(reg.isEmpty() ? "-" : reg)
            .arg(model.isEmpty() ? "-" : model)
            .arg(manufacturer.isEmpty() ? "-" : manufacturer)
            .arg(icao24.isEmpty() ? "-" : icao24)
            .arg(serial.isEmpty() ? "-" : serial)
            .arg(type.isEmpty() ? "-" : type)
            .arg(year.isEmpty() ? "-" : year);
        addTechBox(info);
        if (!fallback.isEmpty()) addTechBox(fallback);
    };
    if (!regNum.isEmpty()) {
        QString apiKey = "abf69b83dac7991586a8d886f84081ce";
        QString url = QString("http://api.aviationstack.com/v1/airplanes?access_key=%1&registration_number=%2").arg(apiKey).arg(regNum);
        QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
        connect(reply, &QNetworkReply::finished, [this, reply, fallback, fillTechInfo, addTechBox]() {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (!doc.isNull() && doc.isObject()) {
                QJsonArray planes = doc.object()["data"].toArray();
                if (!planes.isEmpty()) {
                    for (const QJsonValue &val : planes) {
                        fillTechInfo(val.toObject(), "");
                    }
                } else {
                    if (!fallback.isEmpty()) addTechBox(fallback);
                    else addTechBox("Teknik bilgi bulunamadı.");
                }
            } else {
                if (!fallback.isEmpty()) addTechBox(fallback);
                else addTechBox("Teknik bilgi alınamadı (API hatası).");
            }
            reply->deleteLater();
        });
    } else if (!icao24.isEmpty()) {
        QString apiKey = "abf69b83dac7991586a8d886f84081ce";
        QString url = QString("http://api.aviationstack.com/v1/airplanes?access_key=%1&icao24=%2").arg(apiKey).arg(icao24);
        QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
        connect(reply, &QNetworkReply::finished, [this, reply, fallback, fillTechInfo, addTechBox]() {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (!doc.isNull() && doc.isObject()) {
                QJsonArray planes = doc.object()["data"].toArray();
                if (!planes.isEmpty()) {
                    for (const QJsonValue &val : planes) {
                        fillTechInfo(val.toObject(), "");
                    }
                } else {
                    if (!fallback.isEmpty()) addTechBox(fallback);
                    else addTechBox("Teknik bilgi bulunamadı.");
                }
            } else {
                if (!fallback.isEmpty()) addTechBox(fallback);
                else addTechBox("Teknik bilgi alınamadı (API hatası).");
            }
            reply->deleteLater();
        });
    } else {
        if (!fallback.isEmpty()) addTechBox(fallback);
        else addTechBox("Teknik bilgi bulunamadı.");
    }
}

void RolePage_Pilot::onPlaneReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonArray planes = doc.object()["data"].toArray();
        if (!planes.isEmpty()) {
            QJsonObject plane = planes[0].toObject();
            QString reg = plane["registration_number"].toString();
            QString model = plane["model"].toString();
            QString manufacturer = plane["manufacturer"].toString();
            QString icao24 = plane["icao24"].toString();
            QString serial = plane["serial_number"].toString();
            QString type = plane["plane_type"].toString();
            QString year = plane["production_line"].toString();
            QString info = QString("<b>Tescil:</b> %1<br><b>Model:</b> %2<br><b>Üretici:</b> %3<br><b>ICAO24:</b> %4<br><b>Seri No:</b> %5<br><b>Tip:</b> %6<br><b>Üretim Yılı:</b> %7")
                .arg(reg)
                .arg(model)
                .arg(manufacturer)
                .arg(icao24)
                .arg(serial)
                .arg(type)
                .arg(year);
        } else {
            // Teknik bilgi bulunamadı
        }
    } else {
        // Teknik bilgi alınamadı (API hatası)
    }
}

void RolePage_Pilot::updateAirportInfo(const QString &depIata, const QString &arrIata) {
    if (depIata.isEmpty() && arrIata.isEmpty()) {
        ui->labelLiveInfo->setText("Havalimanı bilgisi bulunamadı.");
        return;
    }
    QString apiKey = "abf69b83dac7991586a8d886f84081ce";
    QString url = QString("http://api.aviationstack.com/v1/airports?access_key=%1&iata_code=%2").arg(apiKey).arg(depIata);
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void RolePage_Pilot::onAirportReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonArray airports = doc.object()["data"].toArray();
        if (!airports.isEmpty()) {
            QJsonObject airport = airports[0].toObject();
            QString name = airport["airport_name"].toString();
            QString city = airport["city_name"].toString();
            QString country = airport["country_name"].toString();
            QString iata = airport["iata_code"].toString();
            QString icao = airport["icao_code"].toString();
            QString lat = airport["latitude"].toString();
            QString lon = airport["longitude"].toString();
            QString info = QString("<b>Havalimanı:</b> %1 (%2/%3)<br><b>Şehir:</b> %4<br><b>Ülke:</b> %5<br><b>Koordinatlar:</b> %6, %7")
                .arg(name)
                .arg(iata)
                .arg(icao)
                .arg(city)
                .arg(country)
                .arg(lat)
                .arg(lon);
            ui->labelLiveInfo->setText(info);
        } else {
            ui->labelLiveInfo->setText("Havalimanı bilgisi bulunamadı.");
        }
    } else {
        ui->labelLiveInfo->setText("Havalimanı bilgisi alınamadı (API hatası).");
    }
}

void RolePage_Pilot::updateMap(double lat, double lon) {
    QString url = QString("https://static-maps.yandex.ru/1.x/?ll=%1,%2&z=10&size=480,272&l=map&pt=%1,%2,pm2rdm")
                    .arg(lon, 0, 'f', 6).arg(lat, 0, 'f', 6);
    qDebug() << "[Harita URL]" << url;
    QUrl qurl(url);
    QNetworkRequest req(qurl);
    mapManager->get(req);
}

void RolePage_Pilot::onMapImageDownloaded(QNetworkReply* reply) {
    QByteArray imgData = reply->readAll();
    QPixmap pix;
    pix.loadFromData(imgData);
    ui->labelLiveInfo->clear();
    if (!pix.isNull()) {
        ui->labelLiveInfo->setPixmap(pix.scaled(460, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        ui->labelLiveInfo->setText("Harita yüklenemedi.");
    }
}

void RolePage_Pilot::onBackClicked() {
    QWidget *parentW = parentWidget();
    while (parentW && !parentW->inherits("QMainWindow"))
        parentW = parentW->parentWidget();
    if (parentW) {
        QMetaObject::invokeMethod(parentW, "showHomePage");
    }
} 