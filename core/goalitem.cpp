#include "goalitem.h"
#include "progressobserver.h"

int GoalItem::s_nextId = 1;

QString lifeSphereToString(LifeSphere sphere) {
    switch (sphere) {
        case LifeSphere::Work: return "Work";
        case LifeSphere::Health: return "Health";
        case LifeSphere::Education: return "Education";
        case LifeSphere::Finance: return "Finance";
        case LifeSphere::Family: return "Family";
        case LifeSphere::Hobby: return "Hobby";
        default: return "Other";
    }
}

LifeSphere stringToLifeSphere(const QString& str) {
    if (str == "Work") return LifeSphere::Work;
    if (str == "Health") return LifeSphere::Health;
    if (str == "Education") return LifeSphere::Education;
    if (str == "Finance") return LifeSphere::Finance;
    if (str == "Family") return LifeSphere::Family;
    if (str == "Hobby") return LifeSphere::Hobby;
    return LifeSphere::Other;
}

GoalItem::GoalItem(const QString& name, const QString& description)
    : m_id(s_nextId++), m_name(name), m_description(description),
      m_completed(false), m_parent(nullptr) {
}

GoalItem::~GoalItem() {
    qDeleteAll(m_children);
}

void GoalItem::add(GoalItem* item) {
    Q_UNUSED(item);
}

void GoalItem::remove(GoalItem* item) {
    Q_UNUSED(item);
}

GoalItem* GoalItem::getChild(int index) {
    if (index >= 0 && index < m_children.size()) {
        return m_children.at(index);
    }
    return nullptr;
}

int GoalItem::childCount() const {
    return m_children.size();
}

QList<GoalItem*> GoalItem::getChildren() const {
    return m_children;
}

void GoalItem::setCompleted(bool completed) {
    m_completed = completed;
    if (completed) {
        setProgress(100);
    }
    notifyObservers();
}

void GoalItem::addObserver(ProgressObserver* observer) {
    if (!m_observers.contains(observer)) {
        m_observers.append(observer);
    }
}

void GoalItem::removeObserver(ProgressObserver* observer) {
    m_observers.removeOne(observer);
}

void GoalItem::notifyObservers() {
    for (ProgressObserver* obs : m_observers) {
        obs->onProgressChanged(this);
    }
    if (m_parent) {
        m_parent->notifyObservers();
    }
}

QJsonObject GoalItem::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["description"] = m_description;
    json["deadline"] = m_deadline.toString(Qt::ISODate);
    json["completed"] = m_completed;
    json["type"] = getType();
    return json;
}

void GoalItem::fromJson(const QJsonObject& json) {
    m_id = json["id"].toInt();
    if (m_id >= s_nextId) {
        s_nextId = m_id + 1;
    }
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_deadline = QDate::fromString(json["deadline"].toString(), Qt::ISODate);
    m_completed = json["completed"].toBool();
}

Subgoal::Subgoal(const QString& name, const QString& description)
    : GoalItem(name, description), m_progress(0) {
}

Subgoal::~Subgoal() {
}

void Subgoal::setProgress(int progress) {
    m_progress = qBound(0, progress, 100);
    m_completed = (m_progress == 100);
    notifyObservers();
}

QJsonObject Subgoal::toJson() const {
    QJsonObject json = GoalItem::toJson();
    json["progress"] = m_progress;
    return json;
}

void Subgoal::fromJson(const QJsonObject& json) {
    GoalItem::fromJson(json);
    m_progress = json["progress"].toInt();
}

Goal::Goal(const QString& name, const QString& description, LifeSphere sphere)
    : GoalItem(name, description), m_sphere(sphere), m_directProgress(0) {
}

Goal::~Goal() {
}

void Goal::add(GoalItem* item) {
    if (item && !m_children.contains(item)) {
        m_children.append(item);
        item->setParent(this);
    }
}

void Goal::remove(GoalItem* item) {
    if (item) {
        m_children.removeOne(item);
        item->setParent(nullptr);
    }
}

int Goal::getProgress() const {
    if (m_children.isEmpty()) {
        return m_directProgress;
    }

    int totalProgress = 0;
    for (GoalItem* child : m_children) {
        totalProgress += child->getProgress();
    }
    return m_children.isEmpty() ? 0 : totalProgress / m_children.size();
}

void Goal::setProgress(int progress) {
    m_directProgress = qBound(0, progress, 100);
    m_completed = (m_directProgress == 100);
    notifyObservers();
}

QJsonObject Goal::toJson() const {
    QJsonObject json = GoalItem::toJson();
    json["sphere"] = lifeSphereToString(m_sphere);
    json["directProgress"] = m_directProgress;

    QJsonArray childrenArray;
    for (GoalItem* child : m_children) {
        childrenArray.append(child->toJson());
    }
    json["subgoals"] = childrenArray;

    return json;
}

void Goal::fromJson(const QJsonObject& json) {
    GoalItem::fromJson(json);
    m_sphere = stringToLifeSphere(json["sphere"].toString());
    m_directProgress = json["directProgress"].toInt();

    QJsonArray subgoals = json["subgoals"].toArray();
    for (const QJsonValue& val : subgoals) {
        Subgoal* subgoal = new Subgoal("");
        subgoal->fromJson(val.toObject());
        add(subgoal);
    }
}
