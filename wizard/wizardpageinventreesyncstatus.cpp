#include "wizardpageinventreesyncstatus.h"
#include "ui_wizardpageinventreesyncstatus.h"


QMap<InvenTreePartUploader::State, QLabel*> WizardPageInvenTreeSyncStatus::m_resultLabelMap = {
    {},
};

WizardPageInvenTreeSyncStatus::WizardPageInvenTreeSyncStatus(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageInvenTreeSyncStatus)
{
    ui->setupUi(this);
}

WizardPageInvenTreeSyncStatus::~WizardPageInvenTreeSyncStatus()
{
    delete ui;
}

bool WizardPageInvenTreeSyncStatus::isComplete() const
{
    return m_completed;
}

void WizardPageInvenTreeSyncStatus::stateChanged(InvenTreePartUploader::State old, InvenTreePartUploader::State newState)
{
    switch (old) {
    case InvenTreePartUploader::CreatePart:
        ui->labelPartStatusSprite->setText(tr("Done"));
        break;
    case InvenTreePartUploader::CreateParameters:
        ui->labelAttachmentsSprite->setText(tr("Done"));
        break;
    case InvenTreePartUploader::SetupSuppliersAndPricing:
        ui->labelPricingInfoStatusSprite->setText(tr("Done"));
        break;
    case InvenTreePartUploader::UploadFiles:
        ui->labelAttachmentsSprite->setText(tr("Done"));
        break;
    case InvenTreePartUploader::Finished:
        break;
    case InvenTreePartUploader::Idle:
        ui->labelPartStatusSprite->setText("-");
        ui->labelAttributesStatusSprite->setText("-");
        ui->labelPricingInfoStatusSprite->setText("-");
        ui->labelStockItemsStatusSprite->setText("-");
        ui->labelAttachmentsSprite->setText("-");
        m_completed = false;
        emit completeChanged();
        break;
    case InvenTreePartUploader::CreateManufacturerPart:
        ui->labelMfrPartCreated->setText(tr("Done"));
        break;
    case InvenTreePartUploader::CreateSupplierPart:
        ui->labelSupplierPartCreated->setText(tr("Done"));
        break;
    case InvenTreePartUploader::SetDefaultSupplierPart:
        ui->labelSetSupplierAsDefault->setText(tr("Done"));
        break;
    }

    if (newState == InvenTreePartUploader::Idle) {
        m_completed = true;
        emit completeChanged();
    }
}

void WizardPageInvenTreeSyncStatus::stateError(InvenTreePartUploader::State old, const QString &error)
{
    // TODO
    /*switch (old) {
    case InvenTreePartUploader::CreatePart:
        ui->labelPartStatusSprite->setText(error);
        break;
    case InvenTreePartUploader::CreateParameters:
        ui->labelAttachmentsSprite->setText(error);
        break;
    case InvenTreePartUploader::SetupSuppliersAndPricing:
        ui->labelPricingInfoStatusSprite->setText(error);
        break;
    case InvenTreePartUploader::UploadFiles:
        ui->labelAttachmentsSprite->setText(error);
        break;
    case InvenTreePartUploader::Finished:
        break;
    case InvenTreePartUploader::Idle:
        break;
    case InvenTreePartUploader::CreateManufacturerPart:
    case InvenTreePartUploader::CreateSupplierPart:
    case InvenTreePartUploader::SetDefaultSupplierPart:
        break;
    }*/
    ui->labelError->setText(error);
}
