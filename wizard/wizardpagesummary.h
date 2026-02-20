#pragma once

#include "wizard/inventreepartimportwizardpage.h"

namespace Ui {
class WizardPageSummary;
}

class WizardPageInvenTreeSyncStatus;

class WizardPageSummary : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    friend class WizardPageInvenTreeSyncStatus;
    explicit WizardPageSummary(InvenTreePartImportWizard *parent = nullptr);
    void update() override;
    ~WizardPageSummary();

private:
    Ui::WizardPageSummary *ui;
};
