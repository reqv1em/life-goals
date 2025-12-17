#include "user.h"

int User::s_nextId = 1;

User::User(const QString& name)
    : m_id(s_nextId++), m_name(name) {
}

User::~User() {
    qDeleteAll(m_goals);
}

void User::addGoal(Goal* goal) {
    if (goal && !m_goals.contains(goal)) {
        m_goals.append(goal);
    }
}

void User::removeGoal(Goal* goal) {
    if (goal) {
        m_goals.removeOne(goal);
        delete goal;
    }
}

Goal* User::getGoalById(int id) const {
    for (Goal* goal : m_goals) {
        if (goal->getId() == id) {
            return goal;
        }
    }
    return nullptr;
}

QList<Goal*> User::searchByDeadline(const QDate& from, const QDate& to) const {
    QList<Goal*> result;
    for (Goal* goal : m_goals) {
        QDate deadline = goal->getDeadline();
        if (deadline.isValid() && deadline >= from && deadline <= to) {
            result.append(goal);
        }
    }
    return result;
}

QList<Goal*> User::searchBySphere(LifeSphere sphere) const {
    QList<Goal*> result;
    for (Goal* goal : m_goals) {
        if (goal->getSphere() == sphere) {
            result.append(goal);
        }
    }
    return result;
}

QList<Goal*> User::searchByName(const QString& name) const {
    QList<Goal*> result;
    for (Goal* goal : m_goals) {
        if (goal->getName().contains(name, Qt::CaseInsensitive)) {
            result.append(goal);
        }
    }
    return result;
}

QJsonObject User::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;

    QJsonArray goalsArray;
    for (Goal* goal : m_goals) {
        goalsArray.append(goal->toJson());
    }
    json["goals"] = goalsArray;

    return json;
}

void User::fromJson(const QJsonObject& json) {
    m_id = json["id"].toInt();
    if (m_id >= s_nextId) {
        s_nextId = m_id + 1;
    }
    m_name = json["name"].toString();

    qDeleteAll(m_goals);
    m_goals.clear();

    QJsonArray goals = json["goals"].toArray();
    for (const QJsonValue& val : goals) {
        Goal* goal = new Goal("");
        goal->fromJson(val.toObject());
        m_goals.append(goal);
    }
}
