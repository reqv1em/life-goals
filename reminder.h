#ifndef REMINDER_H
#define REMINDER_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Reminder {
public:
    Reminder(int goalId = 0, const QDateTime& dateTime = QDateTime(),
             const QString& message = "");

    int getId() const { return m_id; }
    void setId(int id) { m_id = id; }

    int getGoalId() const { return m_goalId; }
    void setGoalId(int id) { m_goalId = id; }

    QDateTime getDateTime() const { return m_dateTime; }
    void setDateTime(const QDateTime& dt) { m_dateTime = dt; }

    QString getMessage() const { return m_message; }
    void setMessage(const QString& msg) { m_message = msg; }

    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    bool isTriggered() const { return m_triggered; }
    void setTriggered(bool triggered) { m_triggered = triggered; }

    bool shouldTrigger() const;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    int m_id;
    int m_goalId;
    QDateTime m_dateTime;
    QString m_message;
    bool m_active;
    bool m_triggered;

    static int s_nextId;
};

#endif
