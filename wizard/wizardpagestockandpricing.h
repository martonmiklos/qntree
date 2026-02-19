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
    friend class WizardPageSupplierDataEnter; // if the scanned part data contains qty information it will pass it
    friend class WizardPagePartDetails; // if the part category selected and the category has default location it will be set if qty is also coming from the user input
    explicit WizardPageStockAndPricing(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageStockAndPricing();
    bool isComplete() const override;
    void update() override;
    void setSelectedPart(SupplierPart *newSelectedPart) override;
    QString summary() const override;

    void resetStockLines(qreal initialQty = 0);

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
