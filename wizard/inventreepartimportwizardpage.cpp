#include "inventreepartimportwizardpage.h"


#include "inventreepartimportwizard.h"

#include <QWizardPage>

InvenTreePartImportWizardPage::InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent) :
    QWizardPage(parent)
{
    m_wizard = parent;
}

void InvenTreePartImportWizardPage::setSelectedPart(::SupplierPart *newSelectedPart)
{
    m_selectedPart = newSelectedPart;
}
