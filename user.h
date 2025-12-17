#ifndef USER_H
#define USER_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include "goalitem.h"

class User {
public:
    User(const QString& name = "");
    ~User();

    int getId() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    void addGoal(Goal* goal);
    void removeGoal(Goal* goal);
    QList<Goal*> getGoals() const { return m_goals; }
    Goal* getGoalById(int id) const;

    QList<Goal*> searchByDeadline(const QDate& from, const QDate& to) const;
    QList<Goal*> searchBySphere(LifeSphere sphere) const;
    QList<Goal*> searchByName(const QString& name) const;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    int m_id;
    QString m_name;
    QList<Goal*> m_goals;

    static int s_nextId;
};

#endif
