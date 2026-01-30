#include "wizardpageselectpartcreationmode.h"
#include "ui_wizardpageselectpartcreationmode.h"

#include "inventreepartimportwizard.h"

WizardPageSelectPartCreationMode::WizardPageSelectPartCreationMode(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageSelectPartCreationMode)
{
    ui->setupUi(this);
}

WizardPageSelectPartCreationMode::~WizardPageSelectPartCreationMode()
{
    delete ui;
}

int WizardPageSelectPartCreationMode::nextId() const
{
    if (ui->radioButtonCreateNewPart->isChecked())
        return InvenTreePartImportWizard::NewPartSupplierDataReview;
    // FIXME
    return InvenTreePartImportWizard::Summary;
}

void WizardPageSelectPartCreationMode::on_toolButtonSelectPartForSupplierPartSelection_clicked()
{

}

