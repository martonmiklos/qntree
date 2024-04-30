#include "wizardpageinventreesyncstatus.h"
#include "ui_wizardpageinventreesyncstatus.h"

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
