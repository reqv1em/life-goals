#include "userdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

UserDialog::UserDialog(QWidget* parent)
    : QDialog(parent), m_user(nullptr) {
    setWindowTitle("Add User");
    setupUi();
}

UserDialog::UserDialog(User* user, QWidget* parent)
    : QDialog(parent), m_user(user) {
    setWindowTitle("Edit User");
    setupUi();
    if (user) {
        m_nameEdit->setText(user->getName());
    }
}

void UserDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Enter user name...");
    formLayout->addRow("Name:", m_nameEdit);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setMinimumWidth(300);
}

QString UserDialog::getName() const {
    return m_nameEdit->text();
}
