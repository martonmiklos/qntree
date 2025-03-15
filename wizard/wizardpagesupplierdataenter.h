#pragma once

#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
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

    AbstractSupplier *selectedSupplier() const;

private slots:
    void on_lineEditPartNumber_textChanged(const QString &arg1);
    void on_comboBoxSupplier_currentIndexChanged(int index);

private:
    Ui::WizardPageSupplierDataEnter *ui;
    AbstractSupplier *m_selectedSupplier = nullptr;

    bool m_partDataRetrived = false;
    bool m_partDataError = false;
    bool m_currenciesRetrived = false;
};


