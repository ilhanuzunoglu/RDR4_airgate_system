#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class Database : public QObject {
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    bool open();
    void close();
    bool ensureTable();
    QString getRoleForCard(const QString &uid);
    bool assignRoleToCard(const QString &uid, const QString &role);
private:
    QSqlDatabase db;
};

#endif // DATABASE_H 