#pragma once

#include "gen_src/client/CompanyApi.h"
#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "gen_src/client/AttachmentApi.h"
#include "gen_src/client/CurrencyApi.h"
#include "gen_src/client/Attachment.h"

#include "supplier/supplierpart.h"

#include "wizard/inventreepartimportwizardpage.h"
#include "wizard/models/supplierattachmentsmodel.h"
#include "wizard/wizardpageselectpartcreationmode.h"

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
class WizardPageSummary;
class InvenTreePartUploader;
class WizardPageInvenTreeSyncStatus;

class InvenTreePartImportWizard : public QWizard
{
    Q_OBJECT
    friend class InvenTreePartUploader;
    friend class InvenTreePartImportWizardPage;
    friend class WizardPagePartParameters;
    friend class WizardPagePartDetails;
    friend class WizardPageSummary;
    friend class WizardPageInvenTreeSyncStatus;
public:
    enum PageIndexes {
        SupplierDataEnter,
        PartCreationMode,

        NewPartSupplierDataReview,
        ParameterMapping,
        StockAndPricing,
        Attachments,

        Summary,
        UploadPage
    };
    explicit InvenTreePartImportWizard(InvenTree::PartApi *api,
                                       InvenTree::StockApi *stockApi,
                                       InvenTree::CurrencyApi *currencyApi,
                                       InvenTree::CompanyApi *companyApi, InvenTree::AttachmentApi *attachmentApi,
                                       QWidget *parent = nullptr);
    ~InvenTreePartImportWizard();
    void setSelectedPart(::SupplierPart & part);

    InvenTree::PartApi *partApi() const;
    InvenTree::StockApi *stockApi() const;
    InvenTree::CurrencyApi *currencyApi() const;
    InvenTree::AttachmentApi *attachmentApi() const;

    void initNewInvenTreePart(InvenTree::Part *part);
    void initNewManufacturerPart(int partPk, InvenTree::ManufacturerPart *mfrPart);
    void initNewSupplierPart(int partPk, int mfrPart, InvenTree::SupplierPart *part);
    void initStockItems(int partPk, QList<InvenTree::StockItem> *items, int *defaultLocationId = nullptr);
    //void initParameterList(int partPk, QList<InvenTree::PartParameter> *params);
    void initPriceBreaks(int partPk, QList<InvenTree::SupplierPriceBreak> *breaks);
    void initAttachments(int partPk, QList<InvenTree::Attachment> *as);
    int currentSupplierDbId() const;

    InvenTree::CompanyApi *companyApi() const;

protected:
    Ui::InvenTreePartImportWizard *ui;
    ::SupplierPart m_selectedPart;

    WizardPageSupplierDataEnter *m_startPage = nullptr;
    WizardPageSelectPartCreationMode *m_partSelectionModePage = nullptr;
    WizardPagePartDetails *m_partDetailsPage = nullptr;
    WizardPagePartParameters *m_partParametersPage  = nullptr;
    WizardPageStockAndPricing *m_stockAndPricingPage = nullptr;
    WizardPageAttachments *m_attachmentsPage = nullptr;
    WizardPageSummary *m_summaryPage = nullptr;
    WizardPageInvenTreeSyncStatus *m_uploadPage = nullptr;

    QList<InvenTreePartImportWizardPage*> m_wizardPages;

    InvenTree::PartApi *m_partApi = nullptr;
    InvenTree::StockApi *m_stockApi = nullptr;
    InvenTree::CurrencyApi *m_currencyApi = nullptr;
    InvenTree::CompanyApi *m_companyApi = nullptr;
    InvenTree::AttachmentApi *m_attachmentApi = nullptr;

    QSettings m_settings;
    QNetworkAccessManager *m_networkAccessManager = nullptr;
    InvenTreePartUploader *m_uploader = nullptr;

private slots:
    void on_InvenTreePartImportWizard_currentIdChanged(int newPageId);
};

