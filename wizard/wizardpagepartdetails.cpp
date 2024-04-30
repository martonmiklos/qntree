#include "wizardpagepartdetails.h"
#include "InvenTree_dialogs/dialogselectinventreecategory.h"
#include "gen_src/client/PartApi.h"
#include "supplier/supplierpart.h"
#include "ui_wizardpagepartdetails.h"
#include "db/config_db.h"

#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>

WizardPagePartDetails::WizardPagePartDetails(InvenTree::PartApi *partApi, InvenTree::StockApi *stockApi, InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent),
    ui(new Ui::WizardPagePartDetails),
    m_partApi(partApi),
    m_stockApi(stockApi)
{
    ui->setupUi(this);
}

WizardPagePartDetails::~WizardPagePartDetails()
{
    delete ui;
}

void WizardPagePartDetails::updateCategoryMapping()
{
    m_invenTreeTargetCategoryPk = -1;
    ui->labelInvenTreeCategory->clear();
    ui->labelInvenTreeCategory->setToolTip(QString());
    emit completeChanged();

    auto supplier = ConfigDb::instance()->suppliers()
                        ->query()
                        ->where(Suppliers::uuidField() == m_supplierUuid)
                        ->first();
    if (supplier) {
        auto supplierId = supplier->id();
        auto savedCategoryBinding = ConfigDb::instance()->supplier_category_map()
                                        ->query()
                                        ->where(SupplierCategoryMap::supplier_idField() == supplierId
                                                && SupplierCategoryMap::supplier_category_idField() == m_selectedPart->categoryId())
                                        ->first();
        if (savedCategoryBinding) {
            connect(m_partApi, &InvenTree::PartApi::partCategoryRetrieveSignal, this, &WizardPagePartDetails::categoryDetailsRetrived);
            m_partApi->partCategoryRetrieve(savedCategoryBinding->inventree_category_id());
        }
    }
}

void WizardPagePartDetails::setSelectedPart(SupplierPart *part)
{
    InvenTreePartImportWizardPage::setSelectedPart(part);
    ui->lineEditIPN->setText(m_selectedPart->name());
    ui->labelSupplierCategory->setText(m_selectedPart->categoryName());
    ui->textEditDescription->setText(m_selectedPart->description());
    ui->lineEditUnit->setText(m_selectedPart->unit());
    ui->labelPartImage->setPixmap(QPixmap::fromImage(m_selectedPart->image()));
}

void WizardPagePartDetails::categoryDetailsRetrived(InvenTree::Category category)
{
    ui->labelInvenTreeCategory->setText(category.getName());
    ui->labelInvenTreeCategory->setToolTip(category.getPathstring());
    m_invenTreeTargetCategoryPk = category.getPk();
    disconnect(m_partApi, &InvenTree::PartApi::partCategoryRetrieveSignal, this, &WizardPagePartDetails::categoryDetailsRetrived);
    emit completeChanged();
}

void WizardPagePartDetails::saveMapping()
{
    if (ui->checkBoxSaveCategoryBinding->isChecked() && m_invenTreeTargetCategoryPk != -1) {
        auto map = Nut::create<SupplierCategoryMap>();
        auto supplier = ConfigDb::instance()->suppliers()
                            ->query()
                            ->where(Suppliers::uuidField() == m_supplierUuid)
                            ->first();
        if (supplier)
            map->setSupplier_id(supplier->id());
        map->setInventree_category_id(m_invenTreeTargetCategoryPk);
        map->setSupplier_category_id(m_selectedPart->categoryId());
        map->setSupplier_category_name(ui->labelSupplierCategory->text());

        if (!map->save(ConfigDb::instance())) {
            QMessageBox::warning(this, tr("Error"), tr("Unable to save InvenTree - Supplier catergory relationship"));
        }
    }
}

void WizardPagePartDetails::setSupplierUuid(const QString &newSupplierUuid)
{
    m_supplierUuid = newSupplierUuid;
}

void WizardPagePartDetails::on_toolButtonEditInventTreeCategory_clicked()
{
    auto dlg = new DialogSelectInvenTreeCategory(m_partApi, m_invenTreeTargetCategoryPk, this);
    connect(dlg, &DialogSelectInvenTreeCategory::categorySelected, this, [=](int pk, const QString &categoryName, const QString &categoryPath) {
        ui->labelInvenTreeCategory->setText(categoryName);
        ui->labelInvenTreeCategory->setToolTip(categoryPath);
        m_invenTreeTargetCategoryPk = pk;
        emit completeChanged();
        dlg->close();
    });
    dlg->show();
}

bool WizardPagePartDetails::validatePage()
{
    return m_invenTreeTargetCategoryPk != -1;
}

bool WizardPagePartDetails::isComplete() const
{
    return m_invenTreeTargetCategoryPk != -1;
}

void WizardPagePartDetails::on_labelPartImage_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu = QMenu();
    auto editImageAction = contextMenu.addAction(tr("Edit image"));
    auto selectLocalImageAction = contextMenu.addAction(tr("Select local image"));

    auto selectedAction = contextMenu.exec(ui->labelPartImage->mapToGlobal(pos));
    if (!selectedAction)
        return;

    if (selectedAction == selectLocalImageAction) {
        //QFileDialog::getOpenFileName(this, tr("Select image"), m_sett)
    } else if (selectedAction == editImageAction) {
        // TODO save image, open with editor
    }
}

