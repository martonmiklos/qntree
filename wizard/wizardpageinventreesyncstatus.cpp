#include "wizardpageinventreesyncstatus.h"
#include "ui_wizardpageinventreesyncstatus.h"

#include "ui_wizardpagesummary.h"
#include "wizardpagesummary.h"

#include <QDesktopServices>

WizardPageInvenTreeSyncStatus::WizardPageInvenTreeSyncStatus(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageInvenTreeSyncStatus)
{
    ui->setupUi(this);
    m_resultLabelMap = {
        {InvenTreePartUploader::CreatePart, ui->labelPartStatusSprite},
        {InvenTreePartUploader::CreateManufacturerPart, ui->labelMfrPartCreated},
        {InvenTreePartUploader::CreateSupplierPart, ui->labelSupplierPartCreated},
        {InvenTreePartUploader::SetDefaultSupplierPart, ui->labelSetSupplierAsDefault},
        {InvenTreePartUploader::UploadPartImage, ui->labelPartImage},
        {InvenTreePartUploader::CreateParameters, ui->labelAttributesStatusSprite},
        {InvenTreePartUploader::AddStockItems, ui->labelStockItemsStatusSprite},
        {InvenTreePartUploader::SetupSuppliersAndPricing, ui->labelPricingInfoStatusSprite},
        {InvenTreePartUploader::UploadFiles, ui->labelAttachmentsSprite},
    };
}

WizardPageInvenTreeSyncStatus::~WizardPageInvenTreeSyncStatus()
{
    delete ui;
}

bool WizardPageInvenTreeSyncStatus::isComplete() const
{
    return m_completed;
}

void WizardPageInvenTreeSyncStatus::reset()
{
    ui->labelError->clear();
    auto keys = m_resultLabelMap.keys();
    for (auto key : keys) {
        if (m_resultLabelMap.contains(key))
            m_resultLabelMap[key]->setText("-");
    }
}

void WizardPageInvenTreeSyncStatus::stateChanged(InvenTreePartUploader::State old, InvenTreePartUploader::State newState)
{
    if (!m_resultLabelMap.contains(old))
        return;
    auto label = m_resultLabelMap[old];
    if (label)
        label->setText(tr("Done"));

    if (newState == InvenTreePartUploader::Finished) {
        if (m_wizard->m_summaryPage->ui->checkBoxOpenInInvenTree->isChecked()) {
            QDesktopServices::openUrl(QUrl("http://localhost:8000/web/part/" + QString::number(m_wizard->m_uploader->partPk()) + "/details")); // FIXME
        }
        m_completed = true;
        emit completeChanged();
    }
}

void WizardPageInvenTreeSyncStatus::stateError(InvenTreePartUploader::State old, const QString &error)
{
    if (!m_resultLabelMap.contains(old))
        return;

    auto label = m_resultLabelMap[old];
    if (label)
        label->setText(tr("Error"));
    ui->labelError->setText(error);
}
