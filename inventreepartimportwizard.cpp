#include "inventreepartimportwizard.h"
#include "ui_inventreepartimportwizard.h"

#include "ui_wizardpagepartdetails.h"
#include "ui_wizardpagesupplierdataenter.h"
#include "wizard/wizardpagesummary.h"
#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/wizardpagestockandpricing.h"
#include "wizard/wizardpagepartdetails.h"
#include "wizard/wizardpageinventreesyncstatus.h"
#include "wizard/wizardpageattachments.h"
#include "wizard/wizardpagepartparameters.h"
#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/inventreepartuploader.h"
#include "supplier/supplierregistry.h"
#include "wizard/stocklinewidget.h"
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

    m_partSelectionModePage = new WizardPageSelectPartCreationMode(this);
    setPage(PartCreationMode, m_partSelectionModePage);
    m_wizardPages.append(m_partSelectionModePage);

    m_partDetailsPage = new WizardPagePartDetails(m_partApi, this);
    setPage(NewPartSupplierDataReview, m_partDetailsPage);
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

    m_summaryPage = new WizardPageSummary(this);
    setPage(Summary, m_summaryPage);
    m_wizardPages.append(m_summaryPage);

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
    /*        part = Part.create(inventree_api, {
            'name': name,
            'description': description,
            'category': category_id,
            'keywords': keywords,
            'revision': revision,
            'IPN': ipn,
            'active': True,
            'virtual': False,
            'component': True,
            'purchaseable': True,
        })*/
    part->setName(m_selectedPart.name());
    part->setDescription(m_selectedPart.description());
    part->setCategory(m_partDetailsPage->invenTreeTargetCategoryPk());
    part->setKeywords(m_partDetailsPage->ui->lineEditKeyWords->text());
    part->setRevision(m_partDetailsPage->ui->lineEditRevision->text());
    // TODO IPN???
    part->setActive(false); // FIXME!!
    part->setComponent(true);
    part->setPurchaseable(true);
}

void InvenTreePartImportWizard::initNewManufacturerPart(int partPk, InvenTree::ManufacturerPart *mfrPart)
{/*
    part_id=part_pk,
    manufacturer_name=manufacturer_name,
    manufacturer_mpn=manufacturer_mpn,
    datasheet=inventree_part['datasheet'],
    description=inventree_part['description'],
*/
    mfrPart->setPart(partPk);
    mfrPart->setManufacturer(m_partDetailsPage->m_invenTreeManufacturerPk);
    mfrPart->setMpn(m_selectedPart.mpn());
    mfrPart->setDescription(m_selectedPart.description());
}

void InvenTreePartImportWizard::initNewSupplierPart(int partPk, int mfrPart, InvenTree::SupplierPart *supplierPart)
{
    supplierPart->setManufacturerPart(mfrPart);
    supplierPart->setPart(partPk);
    supplierPart->setSku(m_partDetailsPage->ui->lineEditSupplierPn->text());
    supplierPart->setMpn(m_partDetailsPage->ui->lineEditMpn->text());
    supplierPart->setDescription(m_selectedPart.description());
    supplierPart->setLink(m_selectedPart.supplierLink());
    supplierPart->setSupplier(SupplierRegistry::instance()->getSupplierByUId(m_startPage->ui->comboBoxSupplier->currentData().toString())->invenTreeId());
}

void InvenTreePartImportWizard::initStockItems(int partPk, QList<InvenTree::StockItem> *items, int *defaultLocationId)
{
    for (auto sl : m_stockAndPricingPage->m_stockLines) {
        if (sl->create()) {
            InvenTree::StockItem siOut;
            siOut.setPart(partPk);
            siOut.setLocation(sl->selectedLocationPk());
            siOut.setQuantity(sl->quantity());
            siOut.setNotes(sl->m_notes);
            items->append(siOut);

            if (sl->isDefaultLocation() && defaultLocationId)
                *defaultLocationId = sl->selectedLocationPk();
        }
    }
}

void InvenTreePartImportWizard::initParameterList(int partPk, QList<InvenTree::PartParameter> *params)
{

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
    if (newId == NewPartSupplierDataReview) {
        m_partDetailsPage->setSupplierUuid(m_startPage->selectedSupplier()->uid());
    } else if (newId == ParameterMapping) {
        m_partDetailsPage->saveMapping();
        // update the unit suffix
        m_stockAndPricingPage->update();
    } else if (newId == Summary) {
        m_summaryPage->updateSummary();
    } else if (newId == UploadPage) {
        m_uploadPage->reset();
        m_uploader->start();
    }
}



