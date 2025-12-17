#include "reminderdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

ReminderDialog::ReminderDialog(int goalId, QWidget* parent)
    : QDialog(parent), m_goalId(goalId), m_reminder(nullptr) {
    setWindowTitle("Add Reminder");
    setupUi();
}

ReminderDialog::ReminderDialog(Reminder* reminder, QWidget* parent)
    : QDialog(parent), m_goalId(reminder ? reminder->getGoalId() : 0), m_reminder(reminder) {
    setWindowTitle("Edit Reminder");
    setupUi();
    if (reminder) {
        m_dateTimeEdit->setDateTime(reminder->getDateTime());
        m_messageEdit->setText(reminder->getMessage());
    }
}

void ReminderDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    m_dateTimeEdit = new QDateTimeEdit(this);
    m_dateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    m_dateTimeEdit->setCalendarPopup(true);
    formLayout->addRow("Date/Time:", m_dateTimeEdit);

    m_messageEdit = new QTextEdit(this);
    m_messageEdit->setMaximumHeight(80);
    m_messageEdit->setPlaceholderText("Reminder message...");
    formLayout->addRow("Message:", m_messageEdit);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setMinimumWidth(350);
}

QDateTime ReminderDialog::getDateTime() const {
    return m_dateTimeEdit->dateTime();
}

QString ReminderDialog::getMessage() const {
    return m_messageEdit->toPlainText();
}
