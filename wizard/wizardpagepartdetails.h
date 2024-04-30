#pragma once

#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include "supplier/supplierpart.h"
#include <QWizardPage>

namespace Ui {
class WizardPagePartDetails;
}

class WizardPagePartDetails : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPagePartDetails(InvenTree::PartApi *partApi, InvenTree::StockApi *stockApi, InvenTreePartImportWizard *parent = nullptr);
    ~WizardPagePartDetails();

    void setSelectedPart(SupplierPart *part) override;
    void saveMapping();

    void setSupplierUuid(const QString &newSupplierUuid);

    void updateCategoryMapping();

private:
    Ui::WizardPagePartDetails *ui;

    InvenTree::PartApi *m_partApi;
    InvenTree::StockApi *m_stockApi;
    int m_invenTreeTargetCategoryPk = -1;
    QString m_supplierUuid;

    bool validatePage() override;
    bool isComplete() const override;

private slots:
    void categoryDetailsRetrived(InvenTree::Category category);
    void on_toolButtonEditInventTreeCategory_clicked();
    void on_labelPartImage_customContextMenuRequested(const QPoint &pos);
};
