#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QTimeZone>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stackedWidget(new QStackedWidget(this)),
    db(new Database(this)),
    cardReader(new CardReader(this)),
    pilotPage(new RolePage_Pilot(this)),
    hostessPage(new RolePage_Hostess(this)),
    passengerPage(new RolePage_Passenger(this)),
    assignRoleDialog(new AssignRole_Dialog(this))
{
    ui->setupUi(this);
    setFixedSize(480, 272);
    setupPages();
    setCentralWidget(stackedWidget);

    if (!db->open()) {
        qCritical() << "[MainWindow] Veritabanı açılamadı, uygulama kapatılıyor.";
        QMessageBox::critical(this, "Veritabanı Hatası", "Veritabanı açılamadı! Uygulama kapatılıyor.");
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);
        return;
    }
    if (!db->ensureTable()) {
        qCritical() << "[MainWindow] Tablo oluşturulamadı, uygulama kapatılıyor.";
        QMessageBox::critical(this, "Veritabanı Hatası", "Tablo oluşturulamadı! Uygulama kapatılıyor.");
        QTimer::singleShot(0, qApp, &QCoreApplication::quit);
        return;
    }

    connect(cardReader, &CardReader::cardRead, this, &MainWindow::onCardRead);
    connect(assignRoleDialog, &QDialog::accepted, this, &MainWindow::onRoleAssigned);
    connect(assignRoleDialog, &QDialog::rejected, this, &MainWindow::showHomePage);

    cardReader->start();

    // Şehir ve timezone eşlemesi
    QMap<QString, QString> cityToTz = {
        {"İstanbul", "Europe/Istanbul"},
        {"Ankara", "Europe/Istanbul"},
        {"İzmir", "Europe/Istanbul"},
        {"Berlin", "Europe/Berlin"},
        {"Paris", "Europe/Paris"},
        {"Londra", "Europe/London"},
        {"New York", "America/New_York"},
        {"Tokyo", "Asia/Tokyo"}
    };
    if (ui->comboCityMain) {
        ui->comboCityMain->clear();
        for (const QString &city : cityToTz.keys())
            ui->comboCityMain->addItem(city);
        ui->comboCityMain->setCurrentText("İstanbul");
    }
    // Timer ile saat/tarih güncelle
    QMap<QString, int> cityToOffset = {
        {"İstanbul", 3}, // GMT+3
        {"Ankara", 3},
        {"İzmir", 3},
        {"Berlin", 2}, // GMT+2
        {"Paris", 2},
        {"Londra", 1}, // GMT+1
        {"New York", -4}, // GMT-4
        {"Tokyo", 9} // GMT+9
    };
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, cityToOffset]() {
        QString city = ui->comboCityMain->currentText();
        int offset = cityToOffset.value(city, 3); // Varsayılan İstanbul
        QDateTime now = QDateTime::currentDateTime().addSecs((offset - QDateTime::currentDateTime().utcOffset()/3600) * 3600);
        if (ui->labelTimeMain) ui->labelTimeMain->setText("Saat: " + now.time().toString("HH:mm:ss"));
        if (ui->labelDateMain) ui->labelDateMain->setText("Tarih: " + now.date().toString("yyyy-MM-dd"));
    });
    timer->start(1000);
    connect(ui->comboCityMain, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, cityToOffset]() {
        QString city = ui->comboCityMain->currentText();
        int offset = cityToOffset.value(city, 3);
        QDateTime now = QDateTime::currentDateTime().addSecs((offset - QDateTime::currentDateTime().utcOffset()/3600) * 3600);
        if (ui->labelTimeMain) ui->labelTimeMain->setText("Saat: " + now.time().toString("HH:mm:ss"));
        if (ui->labelDateMain) ui->labelDateMain->setText("Tarih: " + now.date().toString("yyyy-MM-dd"));
    });
}

MainWindow::~MainWindow()
{
    db->close();
    delete ui;
}

void MainWindow::setupPages() {
    stackedWidget->addWidget(ui->centralWidget); // index 0: Home
    stackedWidget->addWidget(pilotPage); // index 1: Pilot
    stackedWidget->addWidget(hostessPage); // index 2: Hostess
    stackedWidget->addWidget(passengerPage); // index 3: Passenger
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::onCardRead(const QString &uid) {
    lastUid = uid;
    if (cardReader) cardReader->stopPoll();
    QString role = db->getRoleForCard(uid);
    if (role == "pilot") {
        pilotPage->setPilotInfo(uid);
        stackedWidget->setCurrentWidget(pilotPage);
    } else if (role == "hostess") {
        hostessPage->setHostessInfo(uid);
        stackedWidget->setCurrentWidget(hostessPage);
    } else if (role == "passenger") {
        passengerPage->setPassengerInfo(uid);
        stackedWidget->setCurrentWidget(passengerPage);
    } else {
        assignRoleDialog->setCardUid(uid);
        assignRoleDialog->exec();
    }
}

void MainWindow::onRoleAssigned() {
    QString role = assignRoleDialog->selectedRole();
    if (!role.isEmpty()) {
        db->assignRoleToCard(lastUid, role);
        if (role == "pilot") {
            pilotPage->setPilotInfo(lastUid);
            stackedWidget->setCurrentWidget(pilotPage);
        } else if (role == "hostess") {
            hostessPage->setHostessInfo(lastUid);
            stackedWidget->setCurrentWidget(hostessPage);
        } else if (role == "passenger") {
            passengerPage->setPassengerInfo(lastUid);
            stackedWidget->setCurrentWidget(passengerPage);
        }
    } else {
        showHomePage();
    }
}

void MainWindow::showRolePage(const QString &role, const QString &uid) {
    Q_UNUSED(uid);
    if (role == "pilot")
        stackedWidget->setCurrentIndex(1);
    else if (role == "hostess")
        stackedWidget->setCurrentIndex(2);
    else if (role == "passenger")
        stackedWidget->setCurrentIndex(3);
    // Sayfada "Ana ekrana dön" butonu eklenebilir, burada showHomePage() çağrılır.
}

void MainWindow::showHomePage() {
    stackedWidget->setCurrentIndex(0);
    if (cardReader) {
        cardReader->restartPoll();
        qDebug() << "[MainWindow] Ana menüye dönüldü, poll tekrar başlatıldı.";
    }
}
