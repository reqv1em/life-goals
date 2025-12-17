#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>
#include <QDateTimeEdit>
#include <QTextEdit>
#include "reminder.h"

class ReminderDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReminderDialog(int goalId, QWidget* parent = nullptr);
    explicit ReminderDialog(Reminder* reminder, QWidget* parent = nullptr);

    QDateTime getDateTime() const;
    QString getMessage() const;
    int getGoalId() const { return m_goalId; }

private:
    void setupUi();

    QDateTimeEdit* m_dateTimeEdit;
    QTextEdit* m_messageEdit;
    int m_goalId;
    Reminder* m_reminder;
};

#endif
