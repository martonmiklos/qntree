#pragma once

#include "supplier/supplierpart.h"
#include <QWizardPage>

class InvenTreePartImportWizard;

class InvenTreePartImportWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent);
    virtual void update() {}
    virtual void setSelectedPart(::SupplierPart *newSelectedPart);

protected:
    InvenTreePartImportWizard *m_wizard = nullptr;
    ::SupplierPart *m_selectedPart = nullptr;
};
