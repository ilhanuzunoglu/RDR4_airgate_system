#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"
#include "cardreader.h"
#include "rolepage_pilot.h"
#include "rolepage_hostess.h"
#include "rolepage_passenger.h"
#include "assignrole_dialog.h"
#include <QStackedWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onCardRead(const QString &uid);
    void onRoleAssigned();
    void showHomePage();

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    Database *db;
    CardReader *cardReader;
    RolePage_Pilot *pilotPage;
    RolePage_Hostess *hostessPage;
    RolePage_Passenger *passengerPage;
    AssignRole_Dialog *assignRoleDialog;
    QString lastUid;
    void setupPages();
    void showRolePage(const QString &role, const QString &uid);
};

#endif // MAINWINDOW_H
