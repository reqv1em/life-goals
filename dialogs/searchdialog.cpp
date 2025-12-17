#include "searchdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QWhatsThis>

SearchDialog::SearchDialog(User* user, QWidget* parent)
    : QDialog(parent), m_user(user), m_selectedGoalId(-1) {
    setWindowTitle("Search Goals");
    setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint);
    setupUi();
}

void SearchDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Search type group
    QGroupBox* typeGroup = new QGroupBox("Search By", this);
    QHBoxLayout* typeLayout = new QHBoxLayout(typeGroup);

    m_byNameRadio = new QRadioButton("Name", this);
    m_byNameRadio->setChecked(true);
    m_byNameRadio->setWhatsThis("Search goals by name. Enter part of the goal name in the text field below.");
    m_bySphereRadio = new QRadioButton("Sphere", this);
    m_bySphereRadio->setWhatsThis("Search goals by life sphere (Work, Health, Education, etc.).");
    m_byDeadlineRadio = new QRadioButton("Deadline", this);
    m_byDeadlineRadio->setWhatsThis("Search goals by deadline date range.");

    typeGroup->setWhatsThis("Select search criteria type:\n- Name: search by goal name\n- Sphere: filter by life area\n- Deadline: find goals within date range");

    typeLayout->addWidget(m_byNameRadio);
    typeLayout->addWidget(m_bySphereRadio);
    typeLayout->addWidget(m_byDeadlineRadio);
    mainLayout->addWidget(typeGroup);

    // Search criteria group
    QGroupBox* criteriaGroup = new QGroupBox("Criteria", this);
    QVBoxLayout* criteriaLayout = new QVBoxLayout(criteriaGroup);

    // Name search
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Name:", this));
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Enter search text...");
    m_nameEdit->setWhatsThis("Enter part of the goal name to search.\nThe search is case-insensitive.\nPress Enter to search quickly.");
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &SearchDialog::performSearch);
    nameLayout->addWidget(m_nameEdit);
    criteriaLayout->addLayout(nameLayout);

    // Sphere search
    QHBoxLayout* sphereLayout = new QHBoxLayout();
    sphereLayout->addWidget(new QLabel("Sphere:", this));
    m_sphereCombo = new QComboBox(this);
    m_sphereCombo->addItem("Work", static_cast<int>(LifeSphere::Work));
    m_sphereCombo->addItem("Health", static_cast<int>(LifeSphere::Health));
    m_sphereCombo->addItem("Education", static_cast<int>(LifeSphere::Education));
    m_sphereCombo->addItem("Finance", static_cast<int>(LifeSphere::Finance));
    m_sphereCombo->addItem("Family", static_cast<int>(LifeSphere::Family));
    m_sphereCombo->addItem("Hobby", static_cast<int>(LifeSphere::Hobby));
    m_sphereCombo->addItem("Other", static_cast<int>(LifeSphere::Other));
    m_sphereCombo->setWhatsThis("Select a life sphere to filter goals.\nOnly goals from the selected sphere will be shown.");
    sphereLayout->addWidget(m_sphereCombo);
    criteriaLayout->addLayout(sphereLayout);

    // Date range search
    QHBoxLayout* dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("From:", this));
    m_fromDate = new QDateEdit(this);
    m_fromDate->setDate(QDate::currentDate());
    m_fromDate->setCalendarPopup(true);
    m_fromDate->setWhatsThis("Start date for deadline search.\nGoals with deadlines on or after this date will be included.");
    dateLayout->addWidget(m_fromDate);
    dateLayout->addWidget(new QLabel("To:", this));
    m_toDate = new QDateEdit(this);
    m_toDate->setDate(QDate::currentDate().addMonths(6));
    m_toDate->setCalendarPopup(true);
    m_toDate->setWhatsThis("End date for deadline search.\nGoals with deadlines on or before this date will be included.");
    dateLayout->addWidget(m_toDate);
    criteriaLayout->addLayout(dateLayout);

    criteriaGroup->setWhatsThis("Enter search criteria based on selected search type.\nUse the appropriate field for your search.");
    mainLayout->addWidget(criteriaGroup);

    // Search button
    QPushButton* searchBtn = new QPushButton("Search", this);
    searchBtn->setWhatsThis("Click to perform the search with selected criteria.");
    connect(searchBtn, &QPushButton::clicked, this, &SearchDialog::performSearch);
    mainLayout->addWidget(searchBtn);

    // Results
    QGroupBox* resultsGroup = new QGroupBox("Results (double-click to go to goal)", this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);
    m_resultsList = new QListWidget(this);
    m_resultsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsList->setWhatsThis("Search results are displayed here.\n\n"
        "Color coding:\n"
        "- Green: completed goals\n"
        "- Red: overdue goals\n"
        "- White: active goals\n\n"
        "Double-click a result to go to that goal in the main window.");
    connect(m_resultsList, &QListWidget::itemDoubleClicked,
            this, &SearchDialog::onResultDoubleClicked);
    connect(m_resultsList, &QListWidget::itemSelectionChanged,
            this, &SearchDialog::onResultSelectionChanged);
    resultsLayout->addWidget(m_resultsList);
    resultsGroup->setWhatsThis("Shows found goals matching your search criteria.\nDouble-click or use 'Go to Goal' button to navigate.");
    mainLayout->addWidget(resultsGroup);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();

    m_goToBtn = new QPushButton("Go to Goal", this);
    m_goToBtn->setEnabled(false);
    m_goToBtn->setWhatsThis("Select a goal from the results and click this button\nto close the search dialog and highlight the goal\nin the main window.");
    connect(m_goToBtn, &QPushButton::clicked, this, &SearchDialog::onGoToGoal);
    btnLayout->addWidget(m_goToBtn);

    btnLayout->addStretch();

    QPushButton* closeBtn = new QPushButton("Close", this);
    closeBtn->setWhatsThis("Close this search dialog without selecting a goal.");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);

    // WhatsThis for the dialog itself
    setWhatsThis("Search Goals Dialog\n\n"
        "Use this dialog to find goals by:\n"
        "1. Name - partial text match\n"
        "2. Sphere - filter by life area\n"
        "3. Deadline - date range\n\n"
        "Click '?' then click any element for help.");

    setMinimumSize(500, 550);
}

void SearchDialog::performSearch() {
    m_resultsList->clear();
    m_goToBtn->setEnabled(false);

    if (!m_user) {
        m_resultsList->addItem("No user selected");
        return;
    }

    QList<Goal*> results;

    if (m_byNameRadio->isChecked()) {
        results = m_user->searchByName(m_nameEdit->text());
    } else if (m_bySphereRadio->isChecked()) {
        LifeSphere sphere = static_cast<LifeSphere>(m_sphereCombo->currentData().toInt());
        results = m_user->searchBySphere(sphere);
    } else if (m_byDeadlineRadio->isChecked()) {
        results = m_user->searchByDeadline(m_fromDate->date(), m_toDate->date());
    }

    if (results.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("No results found");
        item->setData(Qt::UserRole, -1);
        m_resultsList->addItem(item);
    } else {
        for (Goal* goal : results) {
            QString text = QString("%1 [%2] - %3% - Deadline: %4")
                .arg(goal->getName())
                .arg(lifeSphereToString(goal->getSphere()))
                .arg(goal->getProgress())
                .arg(goal->getDeadline().toString("dd.MM.yyyy"));

            QListWidgetItem* item = new QListWidgetItem(text);
            item->setData(Qt::UserRole, goal->getId()); // Store goal ID

            // Color code by status
            if (goal->isCompleted()) {
                item->setBackground(QColor(200, 255, 200)); // Green for completed
            } else if (goal->getDeadline().isValid() &&
                       goal->getDeadline() < QDate::currentDate()) {
                item->setBackground(QColor(255, 200, 200)); // Red for overdue
            }

            m_resultsList->addItem(item);
        }
    }
}

void SearchDialog::onResultDoubleClicked(QListWidgetItem* item) {
    if (!item) return;

    int goalId = item->data(Qt::UserRole).toInt();
    if (goalId > 0) {
        m_selectedGoalId = goalId;
        accept(); // Close dialog with Accepted result
    }
}

void SearchDialog::onGoToGoal() {
    QListWidgetItem* item = m_resultsList->currentItem();
    if (item) {
        onResultDoubleClicked(item);
    }
}

void SearchDialog::onResultSelectionChanged() {
    QListWidgetItem* item = m_resultsList->currentItem();
    if (item) {
        int goalId = item->data(Qt::UserRole).toInt();
        m_goToBtn->setEnabled(goalId > 0);
    } else {
        m_goToBtn->setEnabled(false);
    }
}
