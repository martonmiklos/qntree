#include "wizardpagesummary.h"
#include "inventreepartimportwizard.h"
#include "ui_wizardpagesummary.h"
#include "wizard/wizardpageattachments.h"
#include "wizardpagepartdetails.h"
#include "wizardpagestockandpricing.h"

WizardPageSummary::WizardPageSummary(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageSummary)
{
    ui->setupUi(this);
}

void WizardPageSummary::updateSummary()
{
    QString summary;
    summary.append(m_wizard->m_partDetailsPage->summary());
    summary.append(m_wizard->m_attachmentsPage->summary());
    summary.append(m_wizard->m_stockAndPricingPage->summary());
    ui->labelSummary->setText(summary);
}

WizardPageSummary::~WizardPageSummary()
{
    delete ui;
}
