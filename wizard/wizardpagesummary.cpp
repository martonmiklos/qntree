#include "wizardpagesummary.h"
#include "ui_wizardpagesummary.h"

WizardPageSummary::WizardPageSummary(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::WizardPageSummary)
{
    ui->setupUi(this);
}

WizardPageSummary::~WizardPageSummary()
{
    delete ui;
}
