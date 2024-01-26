#pragma once

#include "inventreepartimportwizard.h"
#include "supplier/abstractsupplier.h"

namespace Ui {
class WizardPageSupplierDataEnter;
}

class WizardPageSupplierDataEnter : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageSupplierDataEnter(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageSupplierDataEnter();
    bool isComplete() const override;
    bool validatePage() override;
    void update() override;

private slots:
    void on_lineEditPartNumber_textChanged(const QString &arg1);

    void on_comboBoxSupplier_currentIndexChanged(int index);

private:
    Ui::WizardPageSupplierDataEnter *ui;
    InvenTreePartImportWizard *m_wizard = nullptr;
    AbstractSupplier *m_selectedSupplier = nullptr;

    bool partDataRetrived = false;
    bool partDataError = false;
};


