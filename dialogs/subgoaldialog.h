#ifndef SUBGOALDIALOG_H
#define SUBGOALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateEdit>
#include <QSpinBox>
#include "goalitem.h"

class SubgoalDialog : public QDialog {
    Q_OBJECT

public:
    explicit SubgoalDialog(QWidget* parent = nullptr);
    explicit SubgoalDialog(Subgoal* subgoal, QWidget* parent = nullptr);

    QString getName() const;
    QString getDescription() const;
    QDate getDeadline() const;
    int getProgress() const;

private:
    void setupUi();
    void populateFromSubgoal(Subgoal* subgoal);

    QLineEdit* m_nameEdit;
    QTextEdit* m_descEdit;
    QDateEdit* m_deadlineEdit;
    QSpinBox* m_progressSpin;
    Subgoal* m_subgoal;
};

#endif
