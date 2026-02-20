#pragma once

#include "gen_src/client/PartApi.h"
#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include <QWizardPage>

namespace Ui {
class WizardPagePartDetails;
}

class WizardPagePartDetails : public InvenTreePartImportWizardPage
{
    Q_OBJECT
public:
    friend class InvenTreePartImportWizard;

    explicit WizardPagePartDetails(InvenTree::PartApi *api, InvenTreePartImportWizard *parent = nullptr);
    ~WizardPagePartDetails();

    void update() override;
    void saveMapping();

    void setSupplierUuid(const QString &newSupplierUuid);

    int invenTreeTargetCategoryPk() const;

    QString summary() const override;

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
    void checkCategorySelection(InvenTree::Category &category);
    void categoryDetailsRetrived(InvenTree::Category category);
    void on_toolButtonEditInventTreeCategory_clicked();
    void on_labelPartImage_customContextMenuRequested(const QPoint &pos);
    void on_toolButtonSelectInvenTreeManufacturer_clicked();
};
