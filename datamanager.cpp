#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>

DataManager& DataManager::instance() {
    static DataManager instance;
    return instance;
}

DataManager::DataManager() : m_currentUser(nullptr) {
}

DataManager::~DataManager() {
    qDeleteAll(m_users);
    qDeleteAll(m_reminders);
}

void DataManager::addUser(User* user) {
    if (user && !m_users.contains(user)) {
        m_users.append(user);
    }
}

void DataManager::removeUser(User* user) {
    if (user) {
        if (m_currentUser == user) {
            m_currentUser = nullptr;
        }
        m_users.removeOne(user);
        delete user;
    }
}

User* DataManager::getUserById(int id) const {
    for (User* user : m_users) {
        if (user->getId() == id) {
            return user;
        }
    }
    return nullptr;
}

User* DataManager::getUserByName(const QString& name) const {
    for (User* user : m_users) {
        if (user->getName() == name) {
            return user;
        }
    }
    return nullptr;
}

void DataManager::addReminder(Reminder* reminder) {
    if (reminder && !m_reminders.contains(reminder)) {
        m_reminders.append(reminder);
    }
}

void DataManager::removeReminder(Reminder* reminder) {
    if (reminder) {
        m_reminders.removeOne(reminder);
        delete reminder;
    }
}

QList<Reminder*> DataManager::getRemindersForGoal(int goalId) const {
    QList<Reminder*> result;
    for (Reminder* r : m_reminders) {
        if (r->getGoalId() == goalId) {
            result.append(r);
        }
    }
    return result;
}

QList<Reminder*> DataManager::getPendingReminders() const {
    QList<Reminder*> result;
    for (Reminder* r : m_reminders) {
        if (r->shouldTrigger()) {
            result.append(r);
        }
    }
    return result;
}

QString DataManager::getDefaultFilePath() const {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dataPath + "/life-goals-data.json";
}

bool DataManager::saveToFile(const QString& filename) {
    QJsonObject root;

    QJsonArray usersArray;
    for (User* user : m_users) {
        usersArray.append(user->toJson());
    }
    root["users"] = usersArray;

    root["currentUserId"] = m_currentUser ? m_currentUser->getId() : -1;

    QJsonArray remindersArray;
    for (Reminder* r : m_reminders) {
        remindersArray.append(r->toJson());
    }
    root["reminders"] = remindersArray;

    QJsonDocument doc(root);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

bool DataManager::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonObject root = doc.object();

    qDeleteAll(m_users);
    m_users.clear();
    qDeleteAll(m_reminders);
    m_reminders.clear();
    m_currentUser = nullptr;

    QJsonArray usersArray = root["users"].toArray();
    for (const QJsonValue& val : usersArray) {
        User* user = new User();
        user->fromJson(val.toObject());
        m_users.append(user);
    }

    int currentUserId = root["currentUserId"].toInt(-1);
    if (currentUserId >= 0) {
        m_currentUser = getUserById(currentUserId);
    }

    QJsonArray remindersArray = root["reminders"].toArray();
    for (const QJsonValue& val : remindersArray) {
        Reminder* r = new Reminder();
        r->fromJson(val.toObject());
        m_reminders.append(r);
    }

    return true;
}

bool DataManager::exportToText(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    out << "=== Life Goals Export ===" << "\n\n";

    for (User* user : m_users) {
        out << "User: " << user->getName() << "\n";
        out << QString("-").repeated(40) << "\n";

        for (Goal* goal : user->getGoals()) {
            out << "  Goal: " << goal->getName() << "\n";
            out << "  Sphere: " << lifeSphereToString(goal->getSphere()) << "\n";
            out << "  Description: " << goal->getDescription() << "\n";
            out << "  Deadline: " << goal->getDeadline().toString("dd.MM.yyyy") << "\n";
            out << "  Progress: " << goal->getProgress() << "%" << "\n";

            if (goal->childCount() > 0) {
                out << "  Subgoals:" << "\n";
                for (GoalItem* sub : goal->getChildren()) {
                    out << "    - " << sub->getName() << " (" << sub->getProgress() << "%)" << "\n";
                }
            }
            out << "\n";
        }
        out << "\n";
    }

    file.close();
    return true;
}
