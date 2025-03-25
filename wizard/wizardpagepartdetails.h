#pragma once

#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include "supplier/supplierpart.h"
#include "InvenTree_dialogs/dialogselectinventreecompany.h"
#include <QWizardPage>

namespace Ui {
class WizardPagePartDetails;
}

class WizardPagePartDetails : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPagePartDetails(InvenTree::PartApi *api, InvenTreePartImportWizard *parent = nullptr);
    ~WizardPagePartDetails();

    void setSelectedPart(SupplierPart *part) override;
    void saveMapping();

    void setSupplierUuid(const QString &newSupplierUuid);

    int invenTreeTargetCategoryPk() const;

private:
    Ui::WizardPagePartDetails *ui;

    InvenTree::PartApi *m_partApi = nullptr;
    int m_invenTreeTargetCategoryPk = -1;
    int m_invenTreeManufacturerPk = -1;
    QString m_invenTreeManufacturerName;
    QString m_supplierUuid;

    bool validatePage() override;
    bool isComplete() const override;

private slots:
    void categoryDetailsRetrived(InvenTree::Category category);
    void on_toolButtonEditInventTreeCategory_clicked();
    void on_labelPartImage_customContextMenuRequested(const QPoint &pos);
    void on_toolButtonSelectInvenTreeManufacturer_clicked();
    void on_comboBoxCreateOrUseExistingPart_currentIndexChanged(int index);
    void on_toolButtonSelectExistingPart_clicked();
};
