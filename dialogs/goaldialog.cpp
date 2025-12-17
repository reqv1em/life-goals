#include "goaldialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>

GoalDialog::GoalDialog(QWidget* parent)
    : QDialog(parent), m_goal(nullptr) {
    setWindowTitle("Add Goal");
    setupUi();
}

GoalDialog::GoalDialog(Goal* goal, QWidget* parent)
    : QDialog(parent), m_goal(goal) {
    setWindowTitle("Edit Goal");
    setupUi();
    if (goal) {
        populateFromGoal(goal);
    }
}

void GoalDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Enter goal name...");
    formLayout->addRow("Name:", m_nameEdit);

    m_descEdit = new QTextEdit(this);
    m_descEdit->setMaximumHeight(100);
    m_descEdit->setPlaceholderText("Enter description...");
    formLayout->addRow("Description:", m_descEdit);

    m_deadlineEdit = new QDateEdit(this);
    m_deadlineEdit->setDate(QDate::currentDate().addMonths(1));
    m_deadlineEdit->setCalendarPopup(true);
    formLayout->addRow("Deadline:", m_deadlineEdit);

    m_sphereCombo = new QComboBox(this);
    m_sphereCombo->addItem("Work", static_cast<int>(LifeSphere::Work));
    m_sphereCombo->addItem("Health", static_cast<int>(LifeSphere::Health));
    m_sphereCombo->addItem("Education", static_cast<int>(LifeSphere::Education));
    m_sphereCombo->addItem("Finance", static_cast<int>(LifeSphere::Finance));
    m_sphereCombo->addItem("Family", static_cast<int>(LifeSphere::Family));
    m_sphereCombo->addItem("Hobby", static_cast<int>(LifeSphere::Hobby));
    m_sphereCombo->addItem("Other", static_cast<int>(LifeSphere::Other));
    formLayout->addRow("Sphere:", m_sphereCombo);

    m_progressSpin = new QSpinBox(this);
    m_progressSpin->setRange(0, 100);
    m_progressSpin->setSuffix("%");
    formLayout->addRow("Progress:", m_progressSpin);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setMinimumWidth(400);
}

void GoalDialog::populateFromGoal(Goal* goal) {
    m_nameEdit->setText(goal->getName());
    m_descEdit->setText(goal->getDescription());
    m_deadlineEdit->setDate(goal->getDeadline());

    int idx = m_sphereCombo->findData(static_cast<int>(goal->getSphere()));
    if (idx >= 0) {
        m_sphereCombo->setCurrentIndex(idx);
    }

    m_progressSpin->setValue(goal->getProgress());
}

QString GoalDialog::getName() const {
    return m_nameEdit->text();
}

QString GoalDialog::getDescription() const {
    return m_descEdit->toPlainText();
}

QDate GoalDialog::getDeadline() const {
    return m_deadlineEdit->date();
}

LifeSphere GoalDialog::getSphere() const {
    return static_cast<LifeSphere>(m_sphereCombo->currentData().toInt());
}

int GoalDialog::getProgress() const {
    return m_progressSpin->value();
}
