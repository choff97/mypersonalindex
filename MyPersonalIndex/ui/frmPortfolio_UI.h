#ifndef FRMPORTFOLIO_UI_H
#define FRMPORTFOLIO_UI_H

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDialog>
#include "mpiValidator.h"
#include "mpiDateEdit.h"
#include "costBasis.h"

class frmPortfolio_UI
{
public:
    QVBoxLayout *mainLayout;
    QDialogButtonBox *btnOkCancel;
    QFormLayout *layout;
    QLabel *desc;
    QLineEdit *txtDesc;
    QLabel *startValue;
    QLineEdit *txtStartValue;
    mpiIntValidator *ivalidator;
    QLabel *aaThreshold;
    QSpinBox *sbAAThreshold;
    QLabel *aaThresholdValue;
    QComboBox *cmbAAThresholdValue;
    QLabel *costBasis;
    QComboBox *cmbCostBasis;
    QLabel *startDate;
    mpiDateEdit *dateStartDate;
    QCheckBox *chkIncludeDiv;

    void setupUI(QDialog* dialog);
};

#endif // FRMPORTFOLIO_UI_H
