#include "inventreepartimportwizard.h"
#include "ui_inventreepartimportwizard.h"

#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/wizardpagestockandpricing.h"
#include "wizard/wizardpagepartdetails.h"
#include "wizard/wizardpageinventreesyncstatus.h"
#include "wizard/wizardpageattachments.h"
#include "wizard/wizardpagepartparameters.h"


#include <QMessageBox>

InvenTreePartImportWizard::InvenTreePartImportWizard(InvenTree::PartApi *api,
                                                     InvenTree::StockApi *stockApi,
                                                     QWidget *parent)
    : QWizard(parent),
    ui(new Ui::InvenTreePartImportWizard),
    m_partApi(api),
    m_stockApi(stockApi)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_startPage = new WizardPageSupplierDataEnter(this);
    setPage(SupplierDataEnter, m_startPage);
    m_wizardPages.append(m_startPage);

    m_partDetailsPage = new WizardPagePartDetails(m_partApi, stockApi, this);
    setPage(SupplierDataReview, m_partDetailsPage);
    m_wizardPages.append(m_partDetailsPage);

    m_partParametersPage = new WizardPagePartParameters(this);
    setPage(ParameterMapping, m_partParametersPage);
    m_wizardPages.append(m_partParametersPage);

    m_stockAndPricingPage = new WizardPageStockAndPricing(stockApi, this);
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
}

InvenTreePartImportWizard::~InvenTreePartImportWizard()
{
    m_settings.beginGroup("InvenTreePartImportWizard");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();
    delete ui;
}

void InvenTreePartImportWizard::setSelectedPart(SupplierPart &part)
{
    m_selectedPart = part;
    for (auto page : m_wizardPages)
        page->setSelectedPart(&part);
}

InvenTree::PartApi *InvenTreePartImportWizard::partApi() const
{
    return m_partApi;
}

void InvenTreePartImportWizard::on_InvenTreePartImportWizard_currentIdChanged(int newId)
{
    if (newId == SupplierDataReview) {
        m_partDetailsPage->setSupplierUuid(m_startPage->selectedSupplier()->uid());
        m_partDetailsPage->updateCategoryMapping();
    } else if (newId == ParameterMapping) {
        m_partDetailsPage->saveMapping();
        // update the unit suffix
        m_stockAndPricingPage->update();
    }
}



