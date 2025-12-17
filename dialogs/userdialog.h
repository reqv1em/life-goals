#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "user.h"

class UserDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserDialog(QWidget* parent = nullptr);
    explicit UserDialog(User* user, QWidget* parent = nullptr);

    QString getName() const;

private:
    void setupUi();

    QLineEdit* m_nameEdit;
    User* m_user;
};

#endif
