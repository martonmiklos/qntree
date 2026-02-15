#pragma once

#include "wizard/inventreepartimportwizardpage.h"
#include <QWizardPage>
#include <QSettings>

class PropertyMappingModel;

namespace Ui {
class WizardPagePartParameters;
}

class WizardPagePartParameters : public InvenTreePartImportWizardPage
{
    Q_OBJECT
public:
    friend class InvenTreePartImportWizard;
    explicit WizardPagePartParameters(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPagePartParameters();
    void saveSettings() override;

    void setSelectedPart(SupplierPart *newSelectedPart) override;
    //void initParameterList(int partPk, QList<InvenTree::PartParameter> *params);
private slots:
    void on_tableViewPropertyMapping_customContextMenuRequested(const QPoint &pos);

private:
    Ui::WizardPagePartParameters *ui;
    PropertyMappingModel *m_propertyModel = nullptr;
};
