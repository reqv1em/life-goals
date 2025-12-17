#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QListWidget>
#include <QPushButton>
#include "user.h"

class SearchDialog : public QDialog {
    Q_OBJECT

public:
    explicit SearchDialog(User* user, QWidget* parent = nullptr);

    int getSelectedGoalId() const { return m_selectedGoalId; }

private slots:
    void performSearch();
    void onResultDoubleClicked(QListWidgetItem* item);
    void onGoToGoal();
    void onResultSelectionChanged();

private:
    void setupUi();

    User* m_user;
    int m_selectedGoalId;

    QRadioButton* m_byNameRadio;
    QRadioButton* m_bySphereRadio;
    QRadioButton* m_byDeadlineRadio;
    QLineEdit* m_nameEdit;
    QComboBox* m_sphereCombo;
    QDateEdit* m_fromDate;
    QDateEdit* m_toDate;
    QListWidget* m_resultsList;
    QPushButton* m_goToBtn;
};

#endif
