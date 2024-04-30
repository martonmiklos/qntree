#pragma once

#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include <QWizardPage>

namespace Ui {
class WizardPageInvenTreeSyncStatus;
}

class WizardPageInvenTreeSyncStatus : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageInvenTreeSyncStatus(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageInvenTreeSyncStatus();

private:
    Ui::WizardPageInvenTreeSyncStatus *ui;
};
