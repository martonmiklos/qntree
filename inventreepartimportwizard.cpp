#include "inventreepartimportwizard.h"
#include "ui_inventreepartimportwizard.h"

#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/wizardpagestockandpricing.h"
#include "wizard/wizardpagepartdetails.h"
#include "wizard/wizardpageinventreesyncstatus.h"
#include "wizard/wizardpageattachments.h"
#include "wizard/wizardpagepartparameters.h"
#include "wizard/inventreepartuploader.h"


#include <QMessageBox>

InvenTreePartImportWizard::InvenTreePartImportWizard(InvenTree::PartApi *api,
                                                     InvenTree::StockApi *stockApi,
                                                     InvenTree::CurrencyApi *currencyApi,
                                                     InvenTree::CompanyApi *companyApi,
                                                     QWidget *parent)
    : QWizard(parent),
    ui(new Ui::InvenTreePartImportWizard),
    m_partApi(api),
    m_stockApi(stockApi),
    m_currencyApi(currencyApi),
    m_companyApi(companyApi)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_startPage = new WizardPageSupplierDataEnter(this);
    setPage(SupplierDataEnter, m_startPage);
    m_wizardPages.append(m_startPage);

    m_partDetailsPage = new WizardPagePartDetails(m_partApi, this);
    setPage(SupplierDataReview, m_partDetailsPage);
    m_wizardPages.append(m_partDetailsPage);

    m_partParametersPage = new WizardPagePartParameters(this);
    setPage(ParameterMapping, m_partParametersPage);
    m_wizardPages.append(m_partParametersPage);

    m_stockAndPricingPage = new WizardPageStockAndPricing(this);
    setPage(StockAndPricing, m_stockAndPricingPage);
    m_wizardPages.append(m_stockAndPricingPage);

    m_attachmentsPage = new WizardPageAttachments(this);
    setPage(Attachments, m_attachmentsPage);
    m_wizardPages.append(m_attachmentsPage);

    m_uploadPage = new WizardPageInvenTreeSyncStatus(this);
    setPage(UploadPage, m_uploadPage);
    m_wizardPages.append(m_uploadPage);

    m_settings.beginGroup("InvenTreePartImportWizard");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();

    m_networkAccessManager = new QNetworkAccessManager(this);

    m_uploader = new InvenTreePartUploader(this);
    connect(m_uploader, &InvenTreePartUploader::stateChanged, m_uploadPage, &WizardPageInvenTreeSyncStatus::stateChanged);
    connect(m_uploader, &InvenTreePartUploader::stateFailed, m_uploadPage, &WizardPageInvenTreeSyncStatus::stateError);
}

InvenTreePartImportWizard::~InvenTreePartImportWizard()
{
    for (auto page : std::as_const(m_wizardPages)) {
        page->saveSettings();
    }
    m_settings.beginGroup("InvenTreePartImportWizard");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();
    delete ui;
}

void InvenTreePartImportWizard::setSelectedPart(SupplierPart &part)
{
    m_selectedPart = part;
    for (auto page : std::as_const(m_wizardPages))
        page->setSelectedPart(&part);
}

InvenTree::PartApi *InvenTreePartImportWizard::partApi() const
{
    return m_partApi;
}

InvenTree::StockApi *InvenTreePartImportWizard::stockApi() const
{
    return m_stockApi;
}

InvenTree::CurrencyApi *InvenTreePartImportWizard::currencyApi() const
{
    return m_currencyApi;
}

void InvenTreePartImportWizard::initNewInvenTreePart(InvenTree::Part *part)
{
    part->setActive(true);
    part->setCategory(m_partDetailsPage->invenTreeTargetCategoryPk());
    part->setDescription(m_selectedPart.description());
    part->setName(m_selectedPart.name());
    //part->setImage();
    //part->setDefaultSupplier(m_startPage->selectedSupplier()->invenTreeId());
}

int InvenTreePartImportWizard::currentSupplierDbId() const
{
    return m_startPage->selectedSupplier()->getId();
}

InvenTree::CompanyApi *InvenTreePartImportWizard::companyApi() const
{
    return m_companyApi;
}

void InvenTreePartImportWizard::on_InvenTreePartImportWizard_currentIdChanged(int newId)
{
    if (newId == SupplierDataReview) {
        m_partDetailsPage->setSupplierUuid(m_startPage->selectedSupplier()->uid());
    } else if (newId == ParameterMapping) {
        m_partDetailsPage->saveMapping();
        // update the unit suffix
        m_stockAndPricingPage->update();
    } else if (newId == UploadPage) {
        m_uploader->start();
    }
}



