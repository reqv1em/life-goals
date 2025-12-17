#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QList>
#include "user.h"
#include "reminder.h"

class DataManager {
public:
    static DataManager& instance();

    void addUser(User* user);
    void removeUser(User* user);
    QList<User*> getUsers() const { return m_users; }
    User* getUserById(int id) const;
    User* getUserByName(const QString& name) const;
    User* getCurrentUser() const { return m_currentUser; }
    void setCurrentUser(User* user) { m_currentUser = user; }

    void addReminder(Reminder* reminder);
    void removeReminder(Reminder* reminder);
    QList<Reminder*> getReminders() const { return m_reminders; }
    QList<Reminder*> getRemindersForGoal(int goalId) const;
    QList<Reminder*> getPendingReminders() const;

    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);
    QString getDefaultFilePath() const;

    bool exportToText(const QString& filename) const;

private:
    DataManager();
    ~DataManager();
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    QList<User*> m_users;
    QList<Reminder*> m_reminders;
    User* m_currentUser;
};

#endif
