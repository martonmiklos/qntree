#pragma once

#include "wizard/inventreepartimportwizardpage.h"
#include <QWizardPage>

namespace Ui {
class WizardPageSelectPartCreationMode;
}

class WizardPageSelectPartCreationMode : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageSelectPartCreationMode(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageSelectPartCreationMode();
    int nextId() const override;

private slots:
    void on_toolButtonSelectPartForSupplierPartSelection_clicked();

private:
    Ui::WizardPageSelectPartCreationMode *ui;
};
