#pragma once

#include "gen_src/client/StockApi.h"
#include "inventreepartimportwizard.h"
#include "supplier/supplierpart.h"
#include "wizard/models/pricebreaksmodel.h"

namespace Ui {
class WizardPageStockAndPricing;
}

class WizardPageStockAndPricing : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageStockAndPricing(InvenTree::StockApi *api, InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageStockAndPricing();
    bool isComplete() const override;
    void update() override;
    void setPart(::SupplierPart *part);

    int selectedLocationPk() const;

private slots:
    void on_doubleSpinBoxUnitPrice_valueChanged(double arg1);
    void on_checkBoxCreateStock_toggled(bool checked);
    void on_toolButtonChangeTargetLocation_clicked();
    void on_doubleSpinBoxStockQuantity_valueChanged(double arg1);
    void on_doubleSpinBoxUnitPrice_editingFinished();

private:
    Ui::WizardPageStockAndPricing *ui;
    QList<QWidget*> m_stockWidgets;
    PricebreaksModel *m_priceBreakModel = nullptr;
    int m_selectedLocationPk = -1;
    InvenTree::StockApi *m_stockApi = nullptr;
    bool m_priceWasEdited = false;
    void updatePriceFromPriceBreaks();
};
