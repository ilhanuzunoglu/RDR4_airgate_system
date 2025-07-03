#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Database::Database(QObject *parent) : QObject(parent) {}

bool Database::open() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("airgate_cards.db");
    bool ok = db.open();
    if (!ok) {
        qCritical() << "[Database] Veritabanı açılamadı:" << db.lastError().text();
    } else {
        qDebug() << "[Database] Veritabanı açıldı:" << db.databaseName();
    }
    return ok;
}

void Database::close() {
    if (db.isOpen()) db.close();
}

bool Database::ensureTable() {
    QSqlQuery query;
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS cards (uid TEXT PRIMARY KEY, role TEXT)");
    if (!ok) {
        qCritical() << "[Database] Tablo oluşturulamadı:" << query.lastError().text();
    } else {
        qDebug() << "[Database] Tablo kontrolü/oluşturma başarılı.";
    }
    return ok;
}

QString Database::getRoleForCard(const QString &uid) {
    QSqlQuery query;
    query.prepare("SELECT role FROM cards WHERE uid = ?");
    query.addBindValue(uid);
    if (query.exec() && query.next()) {
        qDebug() << "[Database] Kart bulundu, rol:" << query.value(0).toString();
        return query.value(0).toString();
    }
    qDebug() << "[Database] Kart bulunamadı, UID:" << uid;
    return QString();
}

bool Database::assignRoleToCard(const QString &uid, const QString &role) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO cards (uid, role) VALUES (?, ?)");
    query.addBindValue(uid);
    query.addBindValue(role);
    bool ok = query.exec();
    if (!ok) {
        qCritical() << "[Database] Kart role atanamadı:" << query.lastError().text();
    } else {
        qDebug() << "[Database] Kart role atandı:" << uid << role;
    }
    return ok;
} 