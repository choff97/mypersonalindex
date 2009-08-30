#include "frmPortfolio_UI.h"
#include "functions.h"
#include "mpiDoubleValidator.h"

void frmPortfolio_UI::setupUI(QDialog *dialog)
{
    dialog->setWindowFlags(dialog->windowFlags() & (~Qt::WindowContextHelpButtonHint));

    btnOkCancel = new QDialogButtonBox(dialog);
    btnOkCancel->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    vlayout = new QVBoxLayout(dialog);
    vlayout->setSpacing(15);

    layout = new QFormLayout();
    layout->setVerticalSpacing(8);

    desc = new QLabel("&Description:", dialog);
    layout->setWidget(0, QFormLayout::LabelRole, desc);

    txtDesc = new QLineEdit(dialog);
    layout->setWidget(0, QFormLayout::FieldRole, txtDesc);

    startValue = new QLabel("&Index Start Value:", dialog);
    layout->setWidget(1, QFormLayout::LabelRole, startValue);

    txtStartValue = new QLineEdit("100", dialog);
    dvalidator = new mpiDoubleValidator(1, 1000000, 4, dialog);
    dvalidator->setNotation(QDoubleValidator::StandardNotation);
    txtStartValue->setValidator(dvalidator);
    layout->setWidget(1, QFormLayout::FieldRole, txtStartValue);

    aaThreshold = new QLabel("&AA Threshold:", dialog);
    layout->setWidget(2, QFormLayout::LabelRole, aaThreshold);

    sbAAThreshold = new QSpinBox(dialog);
    sbAAThreshold->setMaximum(100);
    sbAAThreshold->setSuffix("%");
    sbAAThreshold->setValue(5);
    layout->setWidget(2, QFormLayout::FieldRole, sbAAThreshold);

    costBasis = new QLabel("&Cost Basis Method:", dialog);
    layout->setWidget(3, QFormLayout::LabelRole, costBasis);

    cmbCostBasis = new QComboBox(dialog);
    cmbCostBasis->addItem("FIFO");
    cmbCostBasis->addItem("LIFO");
    cmbCostBasis->addItem("Average");
    layout->setWidget(3, QFormLayout::FieldRole, cmbCostBasis);

    startDate = new QLabel("&Start Date:", dialog);
    layout->setWidget(4, QFormLayout::LabelRole, startDate);

    dateStartDate = functions::createDateEdit(dialog);
    dateStartDate->setDate(QDate::currentDate());
    layout->setWidget(4, QFormLayout::FieldRole, dateStartDate);

    chkIncludeDiv = new QCheckBox("Include &Dividends", dialog);
    chkIncludeDiv->setChecked(true);
    layout->setWidget(5, QFormLayout::FieldRole, chkIncludeDiv);

    vlayout->addLayout(layout);
    vlayout->addWidget(btnOkCancel);

    desc->setBuddy(txtDesc);
    startValue->setBuddy(txtStartValue);
    aaThreshold->setBuddy(sbAAThreshold);
    costBasis->setBuddy(cmbCostBasis);
    startDate->setBuddy(dateStartDate);
}
