#include "subgoaldialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

SubgoalDialog::SubgoalDialog(QWidget* parent)
    : QDialog(parent), m_subgoal(nullptr) {
    setWindowTitle("Add Subgoal");
    setupUi();
}

SubgoalDialog::SubgoalDialog(Subgoal* subgoal, QWidget* parent)
    : QDialog(parent), m_subgoal(subgoal) {
    setWindowTitle("Edit Subgoal");
    setupUi();
    if (subgoal) {
        populateFromSubgoal(subgoal);
    }
}

void SubgoalDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Enter subgoal name...");
    formLayout->addRow("Name:", m_nameEdit);

    m_descEdit = new QTextEdit(this);
    m_descEdit->setMaximumHeight(80);
    m_descEdit->setPlaceholderText("Enter description...");
    formLayout->addRow("Description:", m_descEdit);

    m_deadlineEdit = new QDateEdit(this);
    m_deadlineEdit->setDate(QDate::currentDate().addDays(14));
    m_deadlineEdit->setCalendarPopup(true);
    formLayout->addRow("Deadline:", m_deadlineEdit);

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

    setMinimumWidth(350);
}

void SubgoalDialog::populateFromSubgoal(Subgoal* subgoal) {
    m_nameEdit->setText(subgoal->getName());
    m_descEdit->setText(subgoal->getDescription());
    m_deadlineEdit->setDate(subgoal->getDeadline());
    m_progressSpin->setValue(subgoal->getProgress());
}

QString SubgoalDialog::getName() const {
    return m_nameEdit->text();
}

QString SubgoalDialog::getDescription() const {
    return m_descEdit->toPlainText();
}

QDate SubgoalDialog::getDeadline() const {
    return m_deadlineEdit->date();
}

int SubgoalDialog::getProgress() const {
    return m_progressSpin->value();
}
