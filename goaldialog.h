#ifndef GOALDIALOG_H
#define GOALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "goalitem.h"

class GoalDialog : public QDialog {
    Q_OBJECT

public:
    explicit GoalDialog(QWidget* parent = nullptr);
    explicit GoalDialog(Goal* goal, QWidget* parent = nullptr);

    QString getName() const;
    QString getDescription() const;
    QDate getDeadline() const;
    LifeSphere getSphere() const;
    int getProgress() const;

private:
    void setupUi();
    void populateFromGoal(Goal* goal);

    QLineEdit* m_nameEdit;
    QTextEdit* m_descEdit;
    QDateEdit* m_deadlineEdit;
    QComboBox* m_sphereCombo;
    QSpinBox* m_progressSpin;
    Goal* m_goal;
};

#endif
