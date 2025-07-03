#include "assignrole_dialog.h"
#include "ui_assignrole_dialog.h"

AssignRole_Dialog::AssignRole_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AssignRole_Dialog)
{
    ui->setupUi(this);
    m_selectedRole = "";
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AssignRole_Dialog::handleAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AssignRole_Dialog::handleRejected);
}

AssignRole_Dialog::~AssignRole_Dialog() {
    delete ui;
}

QString AssignRole_Dialog::selectedRole() const {
    return m_selectedRole;
}

void AssignRole_Dialog::setCardUid(const QString &uid) {
    ui->labelUid->setText("Kart UID: " + uid);
}

void AssignRole_Dialog::handleAccepted() {
    if (ui->radioPilot->isChecked())
        m_selectedRole = "pilot";
    else if (ui->radioHostess->isChecked())
        m_selectedRole = "hostess";
    else if (ui->radioPassenger->isChecked())
        m_selectedRole = "passenger";
    else
        m_selectedRole = "";
    accept();
}

void AssignRole_Dialog::handleRejected() {
    m_selectedRole = "";
    reject();
} 