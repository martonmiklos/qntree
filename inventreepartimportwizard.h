#pragma once

#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "supplier/supplierpart.h"
#include "wizard/inventreepartimportwizardpage.h"
#include "wizard/models/supplierattachmentsmodel.h"
#include <QWizard>
#include <QSettings>

namespace Ui {
class InvenTreePartImportWizard;
}

class InvenTreePartImportWizard;
class WizardPageSupplierDataEnter;
class WizardPageStockAndPricing;
class QNetworkAccessManager;
class WizardPagePartDetails;
class WizardPageInvenTreeSyncStatus;
class WizardPagePartParameters;
class WizardPageAttachments;

class InvenTreePartImportWizard : public QWizard
{
    Q_OBJECT
    friend class InvenTreePartImportWizardPage;
public:
    enum PageIndexes {
        SupplierDataEnter,
        SupplierDataReview,
        ParameterMapping,
        StockAndPricing,
        Attachments,
        UploadPage
    };
    explicit InvenTreePartImportWizard(InvenTree::PartApi *api,
                                       InvenTree::StockApi *stockApi,
                                       QWidget *parent = nullptr);
    ~InvenTreePartImportWizard();
    void setSelectedPart(::SupplierPart & part);

    InvenTree::PartApi *partApi() const;

protected:
    Ui::InvenTreePartImportWizard *ui;
    ::SupplierPart m_selectedPart;
    WizardPageSupplierDataEnter *m_startPage = nullptr;
    WizardPageStockAndPricing *m_stockAndPricingPage = nullptr;
    WizardPagePartDetails *m_partDetailsPage = nullptr;
    WizardPageInvenTreeSyncStatus *m_uploadPage = nullptr;
    WizardPageAttachments *m_attachmentsPage = nullptr;
    WizardPagePartParameters *m_partParametersPage  = nullptr;

    QList<InvenTreePartImportWizardPage*> m_wizardPages;

    InvenTree::PartApi *m_partApi = nullptr;
    InvenTree::StockApi *m_stockApi = nullptr;
    QSettings m_settings;
    QNetworkAccessManager *m_networkAccessManager = nullptr;

private slots:
    void on_InvenTreePartImportWizard_currentIdChanged(int newPageId);
};
