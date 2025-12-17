#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include "user.h"
#include "goalitem.h"
#include "progressobserver.h"

class MainWindow : public QMainWindow, public ProgressObserver {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void onProgressChanged(GoalItem* item) override;

private slots:
    void onAddUser();
    void onEditUser();
    void onDeleteUser();
    void onUserChanged(int index);

    void onAddGoal();
    void onEditGoal();
    void onDeleteGoal();

    void onAddSubgoal();
    void onEditSubgoal();
    void onDeleteSubgoal();

    void onAddReminder();
    void checkReminders();

    void onSearch();
    void onExport();

    void onTreeItemSelected();

    void onSave();
    void onLoad();

private:
    void setupUi();
    void setupMenus();
    void setupToolBar();
    void refreshUserCombo();
    void refreshGoalTree();
    void updateStatusBar();
    GoalItem* getSelectedGoalItem();
    void selectGoalById(int goalId);

    QComboBox* m_userCombo;
    QTreeWidget* m_goalTree;
    QProgressBar* m_totalProgress;
    QLabel* m_statusLabel;
    QTimer* m_reminderTimer;

    QAction* m_addGoalAction;
    QAction* m_editGoalAction;
    QAction* m_deleteGoalAction;
    QAction* m_addSubgoalAction;
    QAction* m_addReminderAction;
};

#endif
