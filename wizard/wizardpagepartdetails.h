#pragma once

#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "supplier/supplierpart.h"
#include <QWizardPage>

namespace Ui {
class WizardPagePartDetails;
}

class WizardPagePartDetails : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardPagePartDetails(InvenTree::PartApi *partApi, InvenTree::StockApi *stockApi, QWidget *parent = nullptr);
    ~WizardPagePartDetails();

    void setSelectedPart(SupplierPart &part);
    void saveMapping();

    void setSupplierUuid(const QString &newSupplierUuid);

    void updateCategoryMapping();

private:
    Ui::WizardPagePartDetails *ui;

    InvenTree::PartApi *m_partApi;
    InvenTree::StockApi *m_stockApi;
    ::SupplierPart m_selectedPart;
    int m_invenTreeTargetCategoryPk = -1;
    QString m_supplierUuid;

    bool validatePage();
    bool isComplete() const;

private slots:
    void categoryDetailsRetrived(InvenTree::Category category);
    void on_toolButtonEditInventTreeCategory_clicked();

};
