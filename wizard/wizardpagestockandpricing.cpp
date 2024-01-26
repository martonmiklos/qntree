#include "wizardpagestockandpricing.h"
#include "ui_wizardpagestockandpricing.h"

WizardPageStockAndPricing::WizardPageStockAndPricing(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageStockAndPricing)
{
    ui->setupUi(this);
    m_stockWidgets
        << ui->checkBoxMakeStockLocatioDefault
        << ui->textEditStockNotes
        << ui->doubleSpinBoxStockQuantity
        << ui->doubleSpinBoxUnitPrice
        << ui->comboBoxUnitPrice;
}

WizardPageStockAndPricing::~WizardPageStockAndPricing()
{
    delete ui;
}

bool WizardPageStockAndPricing::isComplete() const
{
    return ui->checkBoxCreateStock->isChecked() && !qFuzzyIsNull(ui->doubleSpinBoxStockQuantity->value());
}

void WizardPageStockAndPricing::update()
{
    ui->doubleSpinBoxStockQuantity->setSuffix(" " + m_wizard->m_selectedPart.unit());
}

void WizardPageStockAndPricing::on_doubleSpinBoxUnitPrice_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    emit completeChanged();
}


void WizardPageStockAndPricing::on_checkBoxCreateStock_toggled(bool checked)
{
    for (auto widget : qAsConst(m_stockWidgets)) {
        widget->setEnabled(checked);
    }
    emit completeChanged();
}

