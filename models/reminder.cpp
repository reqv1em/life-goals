#include "reminder.h"

int Reminder::s_nextId = 1;

Reminder::Reminder(int goalId, const QDateTime& dateTime, const QString& message)
    : m_id(s_nextId++), m_goalId(goalId), m_dateTime(dateTime),
      m_message(message), m_active(true), m_triggered(false) {
}

bool Reminder::shouldTrigger() const {
    if (!m_active || m_triggered) {
        return false;
    }
    return QDateTime::currentDateTime() >= m_dateTime;
}

QJsonObject Reminder::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["goalId"] = m_goalId;
    json["dateTime"] = m_dateTime.toString(Qt::ISODate);
    json["message"] = m_message;
    json["active"] = m_active;
    json["triggered"] = m_triggered;
    return json;
}

void Reminder::fromJson(const QJsonObject& json) {
    m_id = json["id"].toInt();
    if (m_id >= s_nextId) {
        s_nextId = m_id + 1;
    }
    m_goalId = json["goalId"].toInt();
    m_dateTime = QDateTime::fromString(json["dateTime"].toString(), Qt::ISODate);
    m_message = json["message"].toString();
    m_active = json["active"].toBool();
    m_triggered = json["triggered"].toBool();
}
