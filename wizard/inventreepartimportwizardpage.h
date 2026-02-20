#pragma once

#include <QWizardPage>

class InvenTreePartImportWizard;

class InvenTreePartImportWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent);
    virtual void update() {}
    virtual void saveSettings() {}
    virtual QString summary() const {return QString();}

protected:
    InvenTreePartImportWizard *m_wizard = nullptr;
};
