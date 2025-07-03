#include "rolepage_passenger.h"
#include "ui_rolepage_passenger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QLabel>
#include "weatherinfo.h"
#include "newsinfo.h"
#include "currencyinfo.h"
#include <QScrollBar>

RolePage_Passenger::RolePage_Passenger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RolePage_Passenger),
    networkManager(new QNetworkAccessManager(this)),
    weatherInfo(new WeatherInfo(this)),
    newsInfo(new NewsInfo(this)),
    currencyInfo(new CurrencyInfo(this))
{
    ui->setupUi(this);
    setupCities();
    connect(ui->comboCity, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RolePage_Passenger::onCityChanged);
    connect(ui->btnBack, &QPushButton::clicked, this, &RolePage_Passenger::onBackClicked);
    connect(weatherInfo, &WeatherInfo::weatherDataReady, this, &RolePage_Passenger::onWeatherDataReady);
    connect(newsInfo, &NewsInfo::newsDataReady, this, &RolePage_Passenger::onNewsDataReady);
    connect(currencyInfo, &CurrencyInfo::currencyDataReady, this, &RolePage_Passenger::onCurrencyDataReady);
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        QUrl url = reply->url();
        if (url.toString().contains("aviationstack")) onFlightReply(reply);
        reply->deleteLater();
    });
}

RolePage_Passenger::~RolePage_Passenger() {
    delete ui;
}

void RolePage_Passenger::setupCities() {
    // Şehir, IATA, Ülke kodu eşleştirmeleri
    cityToIata = {
        {"İstanbul", "IST"}, {"Ankara", "ESB"}, {"İzmir", "ADB"},
        {"Berlin", "BER"}, {"Paris", "CDG"}, {"Londra", "LHR"},
        {"New York", "JFK"}, {"Tokyo", "HND"}
    };
    cityToCountry = {
        {"İstanbul", "TR"}, {"Ankara", "TR"}, {"İzmir", "TR"},
        {"Berlin", "DE"}, {"Paris", "FR"}, {"Londra", "GB"},
        {"New York", "US"}, {"Tokyo", "JP"}
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

void RolePage_Passenger::setPassengerInfo(const QString &uid) {
    currentUid = uid;
    ui->comboCity->setCurrentIndex(0);
    updateAllData();
}

void RolePage_Passenger::onCityChanged(int index) {
    Q_UNUSED(index);
    updateAllData();
}

void RolePage_Passenger::updateAllData() {
    QString city = ui->comboCity->currentText();
    updateFlightInfo(city);
    updateWeather(city);
    updateCurrency(city);
    updateNews(city);
}

void RolePage_Passenger::updateFlightInfo(const QString &city) {
    QString iata = cityToIata.value(city, "IST");
    QString apiKey = "enter your key";
    QString url = QString("http://api.aviationstack.com/v1/flights?access_key=%1&arr_iata=%2&limit=8").arg(apiKey).arg(iata);
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void RolePage_Passenger::updateWeather(const QString &city) {
    QString apiCity = cityToApiName.value(city, city);
    weatherInfo->requestWeatherData(apiCity);
}

void RolePage_Passenger::updateCurrency(const QString &city) {
    currencyInfo->requestCurrencyData(city);
}

void RolePage_Passenger::updateNews(const QString &city) {
    QString apiCity = cityToApiName.value(city, city);
    newsInfo->requestNewsData(apiCity);
}

void RolePage_Passenger::onWeatherDataReady(const QString &info) {
    ui->labelWeather->setText(info);
    qDebug() << "[WeatherInfo]" << info;
}

void RolePage_Passenger::onCurrencyDataReady(const QString &info) {
    ui->labelCurrency->setText(info);
    qDebug() << "[CurrencyInfo]" << info;
}

void RolePage_Passenger::onNewsDataReady(const QString &info) {
    QVBoxLayout *layout = ui->scrollAreaWidgetNews->findChild<QVBoxLayout*>("layoutNews");
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
    }
    QStringList newsList = info.split("\n\n", QString::SkipEmptyParts);
    for (const QString &news : newsList) {
        QLabel *label = new QLabel(news);
        label->setStyleSheet("font-size: 18px; margin-bottom: 8px;");
        label->setWordWrap(true);
        layout->addWidget(label);
    }
    qDebug() << "[NewsInfo]" << info;
}

void RolePage_Passenger::onFlightReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    // Debug: Kaç uçuş geldiğini terminale yaz
    if (!doc.isNull() && doc.isObject()) {
        QJsonArray flights = doc.object()["data"].toArray();
        qDebug() << "[Flight API] Gelen veri:" << data;
        qDebug() << "[Flight API] Uçuş sayısı:" << flights.size();
    }

    // Eski widget'ı sil
    QWidget *oldWidget = ui->scrollAreaFlights->widget();
    if (oldWidget) oldWidget->deleteLater();

    // Yeni widget ve layout oluştur
    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    if (!doc.isNull() && doc.isObject()) {
        QJsonArray flights = doc.object()["data"].toArray();
        if (!flights.isEmpty()) {
            int maxFlights = 8;
            for (int i = 0; i < flights.size() && i < maxFlights; ++i) {
                QJsonObject flight = flights[i].toObject();
                QString flightNo = flight["flight"].toObject()["iata"].toString();
                QString airline = flight["airline"].toObject()["name"].toString();
                QString dep = flight["departure"].toObject()["scheduled"].toString().replace("T", " ").left(16);
                QString arr = flight["arrival"].toObject()["scheduled"].toString().replace("T", " ").left(16);
                QString depAirport = flight["departure"].toObject()["iata"].toString();
                QString arrAirport = flight["arrival"].toObject()["iata"].toString();
                QString gate = flight["departure"].toObject()["gate"].toString();
                QString terminal = flight["departure"].toObject()["terminal"].toString();
                QString status = flight["flight_status"].toString();
                QString seat = "12A";
                QString info = QString(
                    "<div style='background:#f7fafd; border:2px solid #2a4d69; border-radius:10px; padding:10px; margin-bottom:10px;'>"
                    "<span style='font-size:22px; font-weight:bold; color:#2a4d69;'>%1</span> <span style='font-size:16px; color:#888;'>%2</span><br>"
                    "<b>Kalkış:</b> %3 (%4) &nbsp; <b>Varış:</b> %5 (%6)<br>"
                    "<b>Koltuk:</b> %7 &nbsp; <b>Kapı:</b> %8 &nbsp; <b>Terminal:</b> %9<br>"
                    "<b>Durum:</b> %10"
                    "</div>")
                    .arg(flightNo)
                    .arg(airline)
                    .arg(dep).arg(depAirport)
                    .arg(arr).arg(arrAirport)
                    .arg(seat)
                    .arg(gate.isEmpty() ? "-" : gate)
                    .arg(terminal.isEmpty() ? "-" : terminal)
                    .arg(status.isEmpty() ? "Bilinmiyor" : status);
                QLabel *label = new QLabel(info);
                label->setTextFormat(Qt::RichText);
                label->setWordWrap(true);
                label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                layout->addWidget(label);
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
    container->setLayout(layout);
    ui->scrollAreaFlights->setWidget(container);
    ui->scrollAreaFlights->setWidgetResizable(true);
}

void RolePage_Passenger::onBackClicked() {
    // Ana menüye dönmek için ana pencereye sinyal gönder
    QWidget *parentW = parentWidget();
    while (parentW && !parentW->inherits("QMainWindow"))
        parentW = parentW->parentWidget();
    if (parentW) {
        QMetaObject::invokeMethod(parentW, "showHomePage");
    }
} 
