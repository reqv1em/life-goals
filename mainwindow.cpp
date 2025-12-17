#include "mainwindow.h"
#include "datamanager.h"
#include "userdialog.h"
#include "goaldialog.h"
#include "subgoaldialog.h"
#include "reminderdialog.h"
#include "searchdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QApplication>
#include <QTreeWidgetItemIterator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Life Goals Manager");
    setupUi();
    setupMenus();
    setupToolBar();

    DataManager::instance().loadFromFile(DataManager::instance().getDefaultFilePath());
    refreshUserCombo();

    m_reminderTimer = new QTimer(this);
    connect(m_reminderTimer, &QTimer::timeout, this, &MainWindow::checkReminders);
    m_reminderTimer->start(60000);

    resize(900, 600);
}

MainWindow::~MainWindow() {
    DataManager::instance().saveToFile(DataManager::instance().getDefaultFilePath());
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QGroupBox* userGroup = new QGroupBox("User", this);
    QHBoxLayout* userLayout = new QHBoxLayout(userGroup);

    m_userCombo = new QComboBox(this);
    m_userCombo->setMinimumWidth(200);
    connect(m_userCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onUserChanged);
    userLayout->addWidget(m_userCombo);

    QPushButton* addUserBtn = new QPushButton("Add", this);
    connect(addUserBtn, &QPushButton::clicked, this, &MainWindow::onAddUser);
    userLayout->addWidget(addUserBtn);

    QPushButton* editUserBtn = new QPushButton("Edit", this);
    connect(editUserBtn, &QPushButton::clicked, this, &MainWindow::onEditUser);
    userLayout->addWidget(editUserBtn);

    QPushButton* deleteUserBtn = new QPushButton("Delete", this);
    connect(deleteUserBtn, &QPushButton::clicked, this, &MainWindow::onDeleteUser);
    userLayout->addWidget(deleteUserBtn);

    userLayout->addStretch();
    mainLayout->addWidget(userGroup);

    QGroupBox* goalsGroup = new QGroupBox("Goals and Subgoals", this);
    QVBoxLayout* goalsLayout = new QVBoxLayout(goalsGroup);

    m_goalTree = new QTreeWidget(this);
    m_goalTree->setHeaderLabels({"Name", "Sphere", "Progress", "Deadline", "Status"});
    m_goalTree->setColumnWidth(0, 250);
    m_goalTree->setColumnWidth(1, 100);
    m_goalTree->setColumnWidth(2, 80);
    m_goalTree->setColumnWidth(3, 100);
    m_goalTree->header()->setStretchLastSection(true);
    m_goalTree->setAlternatingRowColors(true);
    connect(m_goalTree, &QTreeWidget::itemSelectionChanged,
            this, &MainWindow::onTreeItemSelected);
    connect(m_goalTree, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::onEditGoal);
    goalsLayout->addWidget(m_goalTree);

    QHBoxLayout* goalBtnLayout = new QHBoxLayout();

    QPushButton* addGoalBtn = new QPushButton("Add Goal", this);
    connect(addGoalBtn, &QPushButton::clicked, this, &MainWindow::onAddGoal);
    goalBtnLayout->addWidget(addGoalBtn);

    QPushButton* addSubgoalBtn = new QPushButton("Add Subgoal", this);
    connect(addSubgoalBtn, &QPushButton::clicked, this, &MainWindow::onAddSubgoal);
    goalBtnLayout->addWidget(addSubgoalBtn);

    QPushButton* editBtn = new QPushButton("Edit", this);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditGoal);
    goalBtnLayout->addWidget(editBtn);

    QPushButton* deleteBtn = new QPushButton("Delete", this);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteGoal);
    goalBtnLayout->addWidget(deleteBtn);

    QPushButton* reminderBtn = new QPushButton("Add Reminder", this);
    connect(reminderBtn, &QPushButton::clicked, this, &MainWindow::onAddReminder);
    goalBtnLayout->addWidget(reminderBtn);

    goalBtnLayout->addStretch();
    goalsLayout->addLayout(goalBtnLayout);

    mainLayout->addWidget(goalsGroup);

    QGroupBox* progressGroup = new QGroupBox("Overall Progress", this);
    QHBoxLayout* progressLayout = new QHBoxLayout(progressGroup);
    m_totalProgress = new QProgressBar(this);
    m_totalProgress->setRange(0, 100);
    m_totalProgress->setValue(0);
    progressLayout->addWidget(m_totalProgress);
    mainLayout->addWidget(progressGroup);

    setCentralWidget(centralWidget);

    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::setupMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* saveAction = fileMenu->addAction("&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);

    QAction* loadAction = fileMenu->addAction("&Load");
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoad);

    fileMenu->addSeparator();

    QAction* exportAction = fileMenu->addAction("&Export to Text");
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExport);

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    QMenu* searchMenu = menuBar()->addMenu("&Search");
    QAction* searchAction = searchMenu->addAction("&Search Goals");
    searchAction->setShortcut(QKeySequence::Find);
    connect(searchAction, &QAction::triggered, this, &MainWindow::onSearch);

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About Life Goals Manager",
            "Life Goals Manager v1.0\n\n"
            "A desktop application for managing life goals and achievements.\n\n"
            "Patterns used:\n"
            "- Composite: Goals contain subgoals\n"
            "- Observer: Progress updates propagate automatically");
    });
}

void MainWindow::setupToolBar() {
    QToolBar* toolBar = addToolBar("Main");

    m_addGoalAction = toolBar->addAction("Add Goal");
    connect(m_addGoalAction, &QAction::triggered, this, &MainWindow::onAddGoal);

    m_addSubgoalAction = toolBar->addAction("Add Subgoal");
    connect(m_addSubgoalAction, &QAction::triggered, this, &MainWindow::onAddSubgoal);

    toolBar->addSeparator();

    QAction* searchAction = toolBar->addAction("Search");
    connect(searchAction, &QAction::triggered, this, &MainWindow::onSearch);

    QAction* exportAction = toolBar->addAction("Export");
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExport);
}

void MainWindow::refreshUserCombo() {
    m_userCombo->blockSignals(true);
    m_userCombo->clear();

    QList<User*> users = DataManager::instance().getUsers();
    for (User* user : users) {
        m_userCombo->addItem(user->getName(), user->getId());
    }

    User* current = DataManager::instance().getCurrentUser();
    if (current) {
        int idx = m_userCombo->findData(current->getId());
        if (idx >= 0) {
            m_userCombo->setCurrentIndex(idx);
        }
    }

    m_userCombo->blockSignals(false);
    refreshGoalTree();
}

void MainWindow::refreshGoalTree() {
    m_goalTree->clear();

    User* user = DataManager::instance().getCurrentUser();
    if (!user) {
        m_totalProgress->setValue(0);
        return;
    }

    int totalProgress = 0;
    int goalCount = 0;

    for (Goal* goal : user->getGoals()) {
        goal->addObserver(this);

        QTreeWidgetItem* goalItem = new QTreeWidgetItem(m_goalTree);
        goalItem->setText(0, goal->getName());
        goalItem->setText(1, lifeSphereToString(goal->getSphere()));
        goalItem->setText(2, QString("%1%").arg(goal->getProgress()));
        goalItem->setText(3, goal->getDeadline().toString("dd.MM.yyyy"));
        goalItem->setText(4, goal->isCompleted() ? "Completed" : "In Progress");
        goalItem->setData(0, Qt::UserRole, goal->getId());
        goalItem->setData(0, Qt::UserRole + 1, "Goal");

        if (goal->isCompleted()) {
            for (int col = 0; col < 5; ++col) {
                goalItem->setBackground(col, QColor(200, 255, 200));
            }
        } else if (goal->getDeadline().isValid() && goal->getDeadline() < QDate::currentDate()) {
            for (int col = 0; col < 5; ++col) {
                goalItem->setBackground(col, QColor(255, 200, 200));
            }
        }

        for (GoalItem* sub : goal->getChildren()) {
            sub->addObserver(this);

            QTreeWidgetItem* subItem = new QTreeWidgetItem(goalItem);
            subItem->setText(0, sub->getName());
            subItem->setText(1, "-");
            subItem->setText(2, QString("%1%").arg(sub->getProgress()));
            subItem->setText(3, sub->getDeadline().toString("dd.MM.yyyy"));
            subItem->setText(4, sub->isCompleted() ? "Completed" : "In Progress");
            subItem->setData(0, Qt::UserRole, sub->getId());
            subItem->setData(0, Qt::UserRole + 1, "Subgoal");

            if (sub->isCompleted()) {
                for (int col = 0; col < 5; ++col) {
                    subItem->setBackground(col, QColor(200, 255, 200));
                }
            } else if (sub->getDeadline().isValid() && sub->getDeadline() < QDate::currentDate()) {
                for (int col = 0; col < 5; ++col) {
                    subItem->setBackground(col, QColor(255, 200, 200));
                }
            }
        }

        totalProgress += goal->getProgress();
        goalCount++;
    }

    m_goalTree->expandAll();

    if (goalCount > 0) {
        m_totalProgress->setValue(totalProgress / goalCount);
    } else {
        m_totalProgress->setValue(0);
    }

    updateStatusBar();
}

void MainWindow::updateStatusBar() {
    User* user = DataManager::instance().getCurrentUser();
    if (user) {
        int total = user->getGoals().size();
        int completed = 0;
        for (Goal* g : user->getGoals()) {
            if (g->isCompleted()) completed++;
        }
        m_statusLabel->setText(QString("Goals: %1 | Completed: %2 | User: %3")
            .arg(total).arg(completed).arg(user->getName()));
    } else {
        m_statusLabel->setText("No user selected");
    }
}

void MainWindow::onProgressChanged(GoalItem* item) {
    Q_UNUSED(item);
    refreshGoalTree();
}

GoalItem* MainWindow::getSelectedGoalItem() {
    QList<QTreeWidgetItem*> selected = m_goalTree->selectedItems();
    if (selected.isEmpty()) return nullptr;

    QTreeWidgetItem* item = selected.first();
    int id = item->data(0, Qt::UserRole).toInt();
    QString type = item->data(0, Qt::UserRole + 1).toString();

    User* user = DataManager::instance().getCurrentUser();
    if (!user) return nullptr;

    for (Goal* goal : user->getGoals()) {
        if (type == "Goal" && goal->getId() == id) {
            return goal;
        }
        if (type == "Subgoal") {
            for (GoalItem* sub : goal->getChildren()) {
                if (sub->getId() == id) {
                    return sub;
                }
            }
        }
    }
    return nullptr;
}

void MainWindow::onAddUser() {
    UserDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName().trimmed();
        if (!name.isEmpty()) {
            User* user = new User(name);
            DataManager::instance().addUser(user);
            DataManager::instance().setCurrentUser(user);
            refreshUserCombo();
        }
    }
}

void MainWindow::onEditUser() {
    User* user = DataManager::instance().getCurrentUser();
    if (!user) {
        QMessageBox::warning(this, "Warning", "No user selected");
        return;
    }

    UserDialog dialog(user, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName().trimmed();
        if (!name.isEmpty()) {
            user->setName(name);
            refreshUserCombo();
        }
    }
}

void MainWindow::onDeleteUser() {
    User* user = DataManager::instance().getCurrentUser();
    if (!user) {
        QMessageBox::warning(this, "Warning", "No user selected");
        return;
    }

    if (QMessageBox::question(this, "Confirm",
        QString("Delete user '%1' and all their goals?").arg(user->getName()))
        == QMessageBox::Yes) {
        DataManager::instance().removeUser(user);
        if (!DataManager::instance().getUsers().isEmpty()) {
            DataManager::instance().setCurrentUser(DataManager::instance().getUsers().first());
        }
        refreshUserCombo();
    }
}

void MainWindow::onUserChanged(int index) {
    if (index < 0) return;

    int userId = m_userCombo->itemData(index).toInt();
    User* user = DataManager::instance().getUserById(userId);
    DataManager::instance().setCurrentUser(user);
    refreshGoalTree();
}

void MainWindow::onAddGoal() {
    User* user = DataManager::instance().getCurrentUser();
    if (!user) {
        QMessageBox::warning(this, "Warning", "Please select or create a user first");
        return;
    }

    GoalDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName().trimmed();
        if (!name.isEmpty()) {
            Goal* goal = new Goal(name, dialog.getDescription(), dialog.getSphere());
            goal->setDeadline(dialog.getDeadline());
            goal->setProgress(dialog.getProgress());
            user->addGoal(goal);
            refreshGoalTree();
        }
    }
}

void MainWindow::onEditGoal() {
    GoalItem* item = getSelectedGoalItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a goal or subgoal");
        return;
    }

    if (item->getType() == "Goal") {
        Goal* goal = static_cast<Goal*>(item);
        GoalDialog dialog(goal, this);
        if (dialog.exec() == QDialog::Accepted) {
            goal->setName(dialog.getName());
            goal->setDescription(dialog.getDescription());
            goal->setDeadline(dialog.getDeadline());
            goal->setSphere(dialog.getSphere());
            if (goal->childCount() == 0) {
                goal->setProgress(dialog.getProgress());
            }
            refreshGoalTree();
        }
    } else {
        onEditSubgoal();
    }
}

void MainWindow::onDeleteGoal() {
    GoalItem* item = getSelectedGoalItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a goal or subgoal");
        return;
    }

    if (QMessageBox::question(this, "Confirm",
        QString("Delete '%1'?").arg(item->getName())) == QMessageBox::Yes) {

        User* user = DataManager::instance().getCurrentUser();
        if (item->getType() == "Goal") {
            user->removeGoal(static_cast<Goal*>(item));
        } else {
            GoalItem* parent = item->getParent();
            if (parent) {
                parent->remove(item);
                delete item;
            }
        }
        refreshGoalTree();
    }
}

void MainWindow::onAddSubgoal() {
    GoalItem* item = getSelectedGoalItem();
    Goal* goal = nullptr;

    if (item && item->getType() == "Goal") {
        goal = static_cast<Goal*>(item);
    } else if (item && item->getParent()) {
        goal = static_cast<Goal*>(item->getParent());
    }

    if (!goal) {
        QMessageBox::warning(this, "Warning", "Please select a goal first");
        return;
    }

    SubgoalDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName().trimmed();
        if (!name.isEmpty()) {
            Subgoal* subgoal = new Subgoal(name, dialog.getDescription());
            subgoal->setDeadline(dialog.getDeadline());
            subgoal->setProgress(dialog.getProgress());
            goal->add(subgoal);
            refreshGoalTree();
        }
    }
}

void MainWindow::onEditSubgoal() {
    GoalItem* item = getSelectedGoalItem();
    if (!item || item->getType() != "Subgoal") {
        QMessageBox::warning(this, "Warning", "Please select a subgoal");
        return;
    }

    Subgoal* subgoal = static_cast<Subgoal*>(item);
    SubgoalDialog dialog(subgoal, this);
    if (dialog.exec() == QDialog::Accepted) {
        subgoal->setName(dialog.getName());
        subgoal->setDescription(dialog.getDescription());
        subgoal->setDeadline(dialog.getDeadline());
        subgoal->setProgress(dialog.getProgress());
        refreshGoalTree();
    }
}

void MainWindow::onDeleteSubgoal() {
    onDeleteGoal();
}

void MainWindow::onAddReminder() {
    GoalItem* item = getSelectedGoalItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a goal or subgoal");
        return;
    }

    ReminderDialog dialog(item->getId(), this);
    if (dialog.exec() == QDialog::Accepted) {
        Reminder* reminder = new Reminder(item->getId(),
            dialog.getDateTime(), dialog.getMessage());
        DataManager::instance().addReminder(reminder);
        QMessageBox::information(this, "Success", "Reminder added!");
    }
}

void MainWindow::checkReminders() {
    QList<Reminder*> pending = DataManager::instance().getPendingReminders();
    for (Reminder* r : pending) {
        r->setTriggered(true);
        QMessageBox::information(this, "Reminder",
            QString("Goal Reminder:\n\n%1").arg(r->getMessage()));
    }
}

void MainWindow::onTreeItemSelected() {
}

void MainWindow::onSearch() {
    User* user = DataManager::instance().getCurrentUser();
    if (!user) {
        QMessageBox::warning(this, "Warning", "Please select a user first");
        return;
    }

    SearchDialog dialog(user, this);
    if (dialog.exec() == QDialog::Accepted) {
        int goalId = dialog.getSelectedGoalId();
        if (goalId > 0) {
            selectGoalById(goalId);
        }
    }
}

void MainWindow::selectGoalById(int goalId) {
    QTreeWidgetItemIterator it(m_goalTree);
    while (*it) {
        int id = (*it)->data(0, Qt::UserRole).toInt();
        if (id == goalId) {
            m_goalTree->clearSelection();
            m_goalTree->setCurrentItem(*it);
            (*it)->setSelected(true);
            m_goalTree->scrollToItem(*it);
            return;
        }
        ++it;
    }
}

void MainWindow::onExport() {
    QString filename = QFileDialog::getSaveFileName(this,
        "Export Goals", "", "Text Files (*.txt)");
    if (!filename.isEmpty()) {
        if (DataManager::instance().exportToText(filename)) {
            QMessageBox::information(this, "Success", "Data exported successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to export data");
        }
    }
}

void MainWindow::onSave() {
    QString filename = QFileDialog::getSaveFileName(this,
        "Save Data", "", "JSON Files (*.json)");
    if (!filename.isEmpty()) {
        if (DataManager::instance().saveToFile(filename)) {
            QMessageBox::information(this, "Success", "Data saved successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to save data");
        }
    }
}

void MainWindow::onLoad() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Load Data", "", "JSON Files (*.json)");
    if (!filename.isEmpty()) {
        if (DataManager::instance().loadFromFile(filename)) {
            refreshUserCombo();
            QMessageBox::information(this, "Success", "Data loaded successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to load data");
        }
    }
}
