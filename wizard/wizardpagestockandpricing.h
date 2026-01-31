#pragma once

#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include "supplier/supplierpart.h"
#include "wizard/models/pricebreaksmodel.h"

class StockLineWidget;

namespace Ui {
class WizardPageStockAndPricing;
}

class WizardPageStockAndPricing : public InvenTreePartImportWizardPage
{
    Q_OBJECT
public:
    friend class InvenTreePartImportWizard;
    explicit WizardPageStockAndPricing(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageStockAndPricing();
    bool isComplete() const override;
    void update() override;
    void setSelectedPart(SupplierPart *newSelectedPart) override;
    QString summary() const override;

private:
    qreal totalQuantity() const;

    Ui::WizardPageStockAndPricing *ui;
    PricebreaksModel *m_priceBreakModel = nullptr;
    bool m_priceWasEdited = false;
    void updatePriceFromPriceBreaks();

    QList<StockLineWidget*> m_stockLines;
    void addNewLineWidget();

private slots:
    void quantityChanged();
    void on_doubleSpinBoxUnitPrice_editingFinished();
    void on_doubleSpinBoxUnitPrice_valueChanged(double arg1);
};
