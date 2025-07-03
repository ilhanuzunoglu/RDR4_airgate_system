#ifndef ASSIGNROLE_DIALOG_H
#define ASSIGNROLE_DIALOG_H

#include <QDialog>

namespace Ui {
class AssignRole_Dialog;
}

class AssignRole_Dialog : public QDialog {
    Q_OBJECT
public:
    explicit AssignRole_Dialog(QWidget *parent = nullptr);
    ~AssignRole_Dialog();
    QString selectedRole() const;
    void setCardUid(const QString &uid);
private slots:
    void handleAccepted();
    void handleRejected();
private:
    Ui::AssignRole_Dialog *ui;
    QString m_selectedRole;
};

#endif // ASSIGNROLE_DIALOG_H 