#pragma once

#include "gen_src/client/CompanyApi.h"
#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "gen_src/client/CurrencyApi.h"
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
class InvenTreePartUploader;

class InvenTreePartImportWizard : public QWizard
{
    Q_OBJECT
    friend class InvenTreePartImportWizardPage;
    friend class WizardPagePartParameters;
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
                                       InvenTree::CurrencyApi *currencyApi,
                                       InvenTree::CompanyApi *companyApi,
                                       QWidget *parent = nullptr);
    ~InvenTreePartImportWizard();
    void setSelectedPart(::SupplierPart & part);

    InvenTree::PartApi *partApi() const;
    InvenTree::StockApi *stockApi() const;
    InvenTree::CurrencyApi *currencyApi() const;

    void initNewInvenTreePart(InvenTree::Part *part);

    int currentSupplierDbId() const;

    InvenTree::CompanyApi *companyApi() const;

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
    InvenTree::CurrencyApi *m_currencyApi = nullptr;
    InvenTree::CompanyApi *m_companyApi = nullptr;

    QSettings m_settings;
    QNetworkAccessManager *m_networkAccessManager = nullptr;
    InvenTreePartUploader *m_uploader = nullptr;

private slots:
    void on_InvenTreePartImportWizard_currentIdChanged(int newPageId);
};
