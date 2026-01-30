#pragma once

#include "wizard/inventreepartimportwizardpage.h"

namespace Ui {
class WizardPageSummary;
}

class WizardPageSummary : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageSummary(InvenTreePartImportWizard *parent = nullptr);
    void updateSummary();
    ~WizardPageSummary();

private:
    Ui::WizardPageSummary *ui;
};
