#include "inventreepartimportwizardpage.h"


#include "inventreepartimportwizard.h"

#include <QWizardPage>

InvenTreePartImportWizardPage::InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent) :
    QWizardPage(parent)
{
    m_wizard = parent;
}
