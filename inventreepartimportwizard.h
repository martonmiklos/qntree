#pragma once

#include "gen_src/client/PartApi.h"
#include "supplier/supplierpart.h"
#include "wizard/models/propertymappingmodel.h"
#include <QWizard>
#include <QSettings>

namespace Ui {
class InvenTreePartImportWizard;
}

class InvenTreePartImportWizard;
class WizardPageSupplierDataEnter;
class WizardPageStockAndPricing;

class InvenTreePartImportWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent);
    virtual void update() = 0;
protected:
    InvenTreePartImportWizard *m_wizard = nullptr;
};

class InvenTreePartImportWizard : public QWizard
{
    Q_OBJECT
    friend class WizardPageStockAndPricing;

public:
    enum PageIndexes {
        SupplierDataEnter,
        SupplierData,
        InvenTreeData,
        PropertyMapping,
        Stocks
    };
    explicit InvenTreePartImportWizard(InvenTree::PartApi *api, QWidget *parent = nullptr);
    ~InvenTreePartImportWizard();
    void setSelectedPart(SupplierPart & part);
private slots:
    void on_InvenTreePartImportWizard_currentIdChanged(int id);
    void on_tableViewPropertyMapping_customContextMenuRequested(const QPoint &pos);
    void on_tableViewAttachmentMapping_customContextMenuRequested(const QPoint &pos);
    void on_toolButtonEditInventTreeCategory_clicked();

private:
    Ui::InvenTreePartImportWizard *ui;
    SupplierPart m_selectedPart;
    WizardPageSupplierDataEnter *m_startPage;
    WizardPageStockAndPricing *m_stockAndPricingPage;
    PropertyMappingModel *m_propertyModel = nullptr;
    InvenTree::PartApi *m_api = nullptr;
    int m_invenTreeTargetCategoryPk = 0;
    QSettings m_settings;
};
