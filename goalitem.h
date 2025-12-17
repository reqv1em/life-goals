#ifndef GOALITEM_H
#define GOALITEM_H

#include <QString>
#include <QList>
#include <QDate>
#include <QJsonObject>
#include <QJsonArray>

class ProgressObserver;

enum class LifeSphere {
    Work,
    Health,
    Education,
    Finance,
    Family,
    Hobby,
    Other
};

QString lifeSphereToString(LifeSphere sphere);
LifeSphere stringToLifeSphere(const QString& str);

class GoalItem {
public:
    GoalItem(const QString& name, const QString& description = "");
    virtual ~GoalItem();

    virtual void add(GoalItem* item);
    virtual void remove(GoalItem* item);
    virtual GoalItem* getChild(int index);
    virtual int childCount() const;
    virtual QList<GoalItem*> getChildren() const;

    virtual int getProgress() const = 0;
    virtual void setProgress(int progress) = 0;
    virtual bool isLeaf() const = 0;
    virtual QString getType() const = 0;

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString getDescription() const { return m_description; }
    void setDescription(const QString& desc) { m_description = desc; }

    QDate getDeadline() const { return m_deadline; }
    void setDeadline(const QDate& date) { m_deadline = date; }

    bool isCompleted() const { return m_completed; }
    void setCompleted(bool completed);

    GoalItem* getParent() const { return m_parent; }
    void setParent(GoalItem* parent) { m_parent = parent; }

    int getId() const { return m_id; }
    void setId(int id) { m_id = id; }

    void addObserver(ProgressObserver* observer);
    void removeObserver(ProgressObserver* observer);
    void notifyObservers();

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);

protected:
    int m_id;
    QString m_name;
    QString m_description;
    QDate m_deadline;
    bool m_completed;
    GoalItem* m_parent;
    QList<GoalItem*> m_children;
    QList<ProgressObserver*> m_observers;

    static int s_nextId;
};

class Subgoal : public GoalItem {
public:
    Subgoal(const QString& name, const QString& description = "");
    ~Subgoal() override;

    int getProgress() const override { return m_progress; }
    void setProgress(int progress) override;
    bool isLeaf() const override { return true; }
    QString getType() const override { return "Subgoal"; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

private:
    int m_progress;
};

class Goal : public GoalItem {
public:
    Goal(const QString& name, const QString& description = "", LifeSphere sphere = LifeSphere::Other);
    ~Goal() override;

    void add(GoalItem* item) override;
    void remove(GoalItem* item) override;

    int getProgress() const override;
    void setProgress(int progress) override;
    bool isLeaf() const override { return false; }
    QString getType() const override { return "Goal"; }

    LifeSphere getSphere() const { return m_sphere; }
    void setSphere(LifeSphere sphere) { m_sphere = sphere; }

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

private:
    LifeSphere m_sphere;
    int m_directProgress;
};

#endif
