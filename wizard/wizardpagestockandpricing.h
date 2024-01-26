#pragma once

#include "inventreepartimportwizard.h"

namespace Ui {
class WizardPageStockAndPricing;
}

class WizardPageStockAndPricing : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageStockAndPricing(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageStockAndPricing();
    bool isComplete() const override;
    void update() override;

private slots:
    void on_doubleSpinBoxUnitPrice_valueChanged(double arg1);
    void on_checkBoxCreateStock_toggled(bool checked);

private:
    Ui::WizardPageStockAndPricing *ui;
    QList<QWidget*> m_stockWidgets;
};
