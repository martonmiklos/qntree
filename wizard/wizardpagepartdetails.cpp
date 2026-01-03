#include "wizardpagepartdetails.h"
#include "InvenTree_dialogs/dialogselectinventreecategory.h"
#include "InvenTree_dialogs/dialogselectinventreepart.h"
#include "gen_src/client/PartApi.h"
#include "supplier/supplierpart.h"
#include "ui_wizardpagepartdetails.h"
#include "db/config_db.h"

#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>

WizardPagePartDetails::WizardPagePartDetails(InvenTree::PartApi *api, InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent),
    ui(new Ui::WizardPagePartDetails),
    m_partApi(api)
{
    ui->setupUi(this);
    ui->toolButtonSelectExistingPart->setVisible(false);
}

WizardPagePartDetails::~WizardPagePartDetails()
{
    delete ui;
}

void WizardPagePartDetails::setSelectedPart(SupplierPart *part)
{
    InvenTreePartImportWizardPage::setSelectedPart(part);
    ui->lineEditIPN->setText(m_selectedPart->name());
    ui->labelSupplierCategory->setText(m_selectedPart->categoryName());
    ui->textEditDescription->setText(m_selectedPart->description());
    ui->lineEditUnit->setText(m_selectedPart->unit());
    ui->labelPartImage->setPixmap(QPixmap::fromImage(m_selectedPart->image()));
    ui->labelSupplierManufacturerName->setText(part->manufacturer());

    m_invenTreeTargetCategoryPk = -1;
    ui->labelInvenTreeSelectedCategory->clear();
    ui->labelInvenTreeSelectedCategory->setToolTip(QString());
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
            connect(m_wizard->partApi(), &InvenTree::PartApi::partCategoryRetrieveSignal, this, &WizardPagePartDetails::categoryDetailsRetrived);
            m_wizard->partApi()->partCategoryRetrieve(savedCategoryBinding->inventree_category_id());
        }
    }

    auto existingManufacturerLink = ConfigDb::instance()->manufacturer_map()
                                        ->query()
                                        ->where(ManufacturerMap::manufacturer_nameField() == m_selectedPart->manufacturer() &&
                                                ManufacturerMap::supplier_idField() == m_wizard->currentSupplierDbId())
                                        ->first();
    if (existingManufacturerLink) {
        connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyRetrieveSignal, this, [=](InvenTree::Company summary) {
            ui->labelInvenTreeManufacturerName->setText(summary.getName());
            m_invenTreeManufacturerPk = summary.getPk();
            disconnect(m_wizard->companyApi(), nullptr, this, nullptr);
        });

        connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyRetrieveSignalError,
                this, [=](InvenTree::Company summary, QNetworkReply::NetworkError error_type, const QString &error_str) {
                    Q_UNUSED(summary)
                    Q_UNUSED(error_type)
                    ui->labelInvenTreeManufacturerName->setText(error_str);
                    m_invenTreeManufacturerPk = -1;
                    disconnect(m_wizard->companyApi(), nullptr, this, nullptr);
                });
        m_wizard->companyApi()->companyRetrieve(QString::number(existingManufacturerLink->inventree_company_pk()));
    } else {
        ui->labelInvenTreeManufacturerName->setText(tr("%1 (creating)").arg(m_selectedPart->manufacturer()));
        m_invenTreeManufacturerPk = -1;
    }

    auto categoryMap = ConfigDb::instance()->supplier_category_map()
                           ->query()
                           ->where(SupplierCategoryMap::supplier_category_idField() == m_selectedPart->categoryId() &&
                                   SupplierCategoryMap::supplier_idField() == m_wizard->currentSupplierDbId())
                           ->first();
    if (categoryMap) {
        connect(m_wizard->partApi(), &InvenTree::PartApi::partCategoryRetrieveSignal, this, [=](InvenTree::Category summary) {
            ui->labelInvenTreeSelectedCategory->setText(summary.getName());
            m_invenTreeTargetCategoryPk = summary.getPk();
            disconnect(m_wizard->partApi(), nullptr, this, nullptr);
            emit completeChanged();
        });
        m_wizard->partApi()->partCategoryRetrieve(categoryMap->inventree_category_id());
    }
}

void WizardPagePartDetails::categoryDetailsRetrived(InvenTree::Category category)
{
    ui->labelInvenTreeSelectedCategory->setText(category.getName());
    ui->labelInvenTreeSelectedCategory->setToolTip(category.getPathstring());
    m_invenTreeTargetCategoryPk = category.getPk();
    disconnect(m_wizard->partApi(), &InvenTree::PartApi::partCategoryRetrieveSignal, this, &WizardPagePartDetails::categoryDetailsRetrived);
    emit completeChanged();
}

void WizardPagePartDetails::saveMapping()
{
    if (ui->checkBoxSaveCategoryBinding->isChecked() && m_invenTreeTargetCategoryPk != -1) {
        auto categoryMap = ConfigDb::instance()->supplier_category_map()
            ->query()
            ->where(SupplierCategoryMap::supplier_category_idField() == m_selectedPart->categoryId() &&
                    SupplierCategoryMap::supplier_idField() == m_wizard->currentSupplierDbId())
            ->first();
        if (categoryMap) {
            categoryMap->setInventree_category_id(m_invenTreeTargetCategoryPk);
            categoryMap->save(ConfigDb::instance());
        } else {
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
}

void WizardPagePartDetails::setSupplierUuid(const QString &newSupplierUuid)
{
    m_supplierUuid = newSupplierUuid;
}

int WizardPagePartDetails::invenTreeTargetCategoryPk() const
{
    return m_invenTreeTargetCategoryPk;
}

void WizardPagePartDetails::on_toolButtonEditInventTreeCategory_clicked()
{
    if (ui->comboBoxCreateOrUseExistingPart->currentIndex() == 1) {
        QMessageBox::information(this, tr("Existing part selected"), tr("The part will be added as a supplier part to an existing part.<br>"
                                                                        "The category will be determined by the selected part"));
        return;
    }
    auto dlg = new DialogSelectInvenTreeCategory(m_wizard->partApi(), m_invenTreeTargetCategoryPk, this);
    connect(dlg, &DialogSelectInvenTreeCategory::categorySelected, this, [=](int pk, const QString &categoryName, const QString &categoryPath) {
        ui->labelInvenTreeSelectedCategory->setText(categoryName);
        ui->labelInvenTreeSelectedCategory->setToolTip(categoryPath);
        m_invenTreeTargetCategoryPk = pk;
        emit completeChanged();
        dlg->close();
    });
    dlg->show();
}

bool WizardPagePartDetails::validatePage()
{
    return m_invenTreeTargetCategoryPk != -1 || m_selectedPart->existingPk() != 0;
}

bool WizardPagePartDetails::isComplete() const
{
    return m_invenTreeTargetCategoryPk != -1 || m_selectedPart->existingPk() != 0;
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

void WizardPagePartDetails::on_toolButtonSelectInvenTreeManufacturer_clicked()
{
    auto companySelectDialog = new DialogSelectInvenTreeCompany(m_wizard->companyApi(), this);
    connect(companySelectDialog, &DialogSelectInvenTreeCompany::companySelected, this, [=](int pk, const QString &name) {
        m_invenTreeManufacturerPk = pk;
        ui->labelInvenTreeManufacturerName->setText(name);
    });

    companySelectDialog->listOnlyManufacturers(true);
    companySelectDialog->show();
    companySelectDialog->update();
}


void WizardPagePartDetails::on_comboBoxCreateOrUseExistingPart_currentIndexChanged(int index)
{
    static QString shadowPn;
    if (index == 0) {
        // Create new part
        ui->toolButtonSelectExistingPart->setVisible(false);
        m_selectedPart->setExistingPk(0);
        ui->lineEditIPN->setText(shadowPn);
    } else {
        // Add to existing as supplier part
        ui->toolButtonSelectExistingPart->setVisible(true);
        shadowPn = ui->lineEditIPN->text();
        ui->lineEditIPN->clear();
    }
}


void WizardPagePartDetails::on_toolButtonSelectExistingPart_clicked()
{
    DialogSelectInvenTreePart *partSelectDlg = new DialogSelectInvenTreePart(m_partApi, this);
    connect(partSelectDlg, &DialogSelectInvenTreePart::partSelected, this, [=](InvenTree::Part part) {
        ui->lineEditIPN->setText(part.getIpn());
        m_selectedPart->setExistingPk(part.getPk());
        ui->labelInvenTreeSelectedCategory->setText(part.getCategoryName());
        emit completeChanged();
    });
    partSelectDlg->show();
}

