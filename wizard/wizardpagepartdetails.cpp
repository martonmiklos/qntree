#include "wizardpagepartdetails.h"
#include "InvenTree_dialogs/dialogselectinventreecategory.h"
#include "InvenTree_dialogs/dialogselectinventreecompany.h"
#include "gen_src/client/PartApi.h"
#include "supplier/supplierpart.h"
#include "ui_wizardpagepartdetails.h"
#include "wizardpagestockandpricing.h"
#include "stocklinewidget.h"
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
}

WizardPagePartDetails::~WizardPagePartDetails()
{
    delete ui;
}

Nut::Row<SupplierCategoryMap> WizardPagePartDetails::findSavedCategoryBinding(int supplierId, const SupplierPart &part)
{
    const QString categoryKey = supplierCategoryMapKey(part);
    const QString categoryName = part.categoryName().trimmed();

    if (!categoryKey.isEmpty()) {
        auto categoryMap = ConfigDb::instance()->supplier_category_map()
        ->query()
            ->where(SupplierCategoryMap::supplier_idField() == supplierId
                    && SupplierCategoryMap::supplier_category_idField() == categoryKey)
            ->first();
        if (categoryMap)
            return categoryMap;
    }

    if (!categoryName.isEmpty() && categoryKey == categoryName) {
        auto categoryMap = ConfigDb::instance()->supplier_category_map()
        ->query()
            ->where(SupplierCategoryMap::supplier_idField() == supplierId
                    && SupplierCategoryMap::supplier_category_nameField() == categoryName)
            ->first();
        if (categoryMap)
            return categoryMap;
    }

    return Nut::Row<SupplierCategoryMap>();
}

QString WizardPagePartDetails::supplierCategoryMapKey(const SupplierPart &part)
{
    const QString categoryId = part.categoryId().trimmed();
    if (!categoryId.isEmpty())
        return categoryId;

    return part.categoryName().trimmed();
}

void WizardPagePartDetails::update()
{
    ui->lineEditIPN->setText(m_wizard->m_selectedPart.name());
    ui->labelSupplierCategory->setText(m_wizard->m_selectedPart.categoryName());
    ui->textEditDescription->setText(m_wizard->m_selectedPart.description());
    ui->lineEditUnit->setText(m_wizard->m_selectedPart.unit());
    ui->labelPartImage->setPixmap(QPixmap::fromImage(m_wizard->m_selectedPart.image()));
    ui->labelSupplierManufacturerName->setText(m_wizard->m_selectedPart.manufacturer());
    ui->lineEditSupplierPn->setText(m_wizard->m_selectedPart.name());
    ui->lineEditMpn->setText(m_wizard->m_selectedPart.mpn());

    m_invenTreeTargetCategoryPk = -1;
    ui->labelInvenTreeSelectedCategory->clear();
    ui->labelInvenTreeSelectedCategory->setToolTip(QString());
    emit completeChanged();

    auto supplier = ConfigDb::instance()->suppliers()
                        ->query()
                        ->where(Suppliers::uuidField() == m_supplierUuid)
                        ->first();
    if (supplier) {
        auto savedCategoryBinding = findSavedCategoryBinding(supplier->id(), m_wizard->m_selectedPart);
        if (savedCategoryBinding) {
            connect(m_wizard->partApi(), &InvenTree::PartApi::partCategoryRetrieveSignal, this, &WizardPagePartDetails::categoryDetailsRetrived);
            m_wizard->partApi()->partCategoryRetrieve(savedCategoryBinding->inventree_category_id());
        }
    } else {
        // supplier not found WTF??
        Q_ASSERT(false);
    }

    auto existingManufacturerLink = ConfigDb::instance()->manufacturer_map()
                                        ->query()
                                        ->where(ManufacturerMap::manufacturer_nameField() == m_wizard->m_selectedPart.manufacturer() &&
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
        m_wizard->companyApi()->companyRetrieve(existingManufacturerLink->inventree_company_pk());
    } else {
        // no mapping is saved yet let's look for equal names
        ui->labelInvenTreeManufacturerName->setText(tr("%1 (creating)").arg(m_wizard->m_selectedPart.manufacturer()));
        m_invenTreeManufacturerPk = -1;

        connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyListSignal, this, [=](InvenTree::PaginatedCompanyList summary) {
            disconnect(m_wizard->companyApi(), nullptr, this, nullptr);
            if (summary.getCount() == 1) {
                // single exact name match
                ui->labelInvenTreeManufacturerName->setText(tr("%1 (by name)").arg(m_wizard->m_selectedPart.manufacturer()));
                m_invenTreeManufacturerPk = summary.getResults().first().getPk();
                ui->checkBoxSaveMfrBinding->setChecked(true);
            }
        });

        m_wizard->companyApi()->companyList(
            std::numeric_limits<qint32>().max(), // limit
            true, //active
            InvenTree::OptionalParam<bool>(), //is_customer
            true, //is_manufacturer
            InvenTree::OptionalParam<bool>(), //is_supplier
            m_wizard->m_selectedPart.manufacturer(), //name
            InvenTree::OptionalParam<qint32>(0), //offset
            InvenTree::OptionalParam<QString>(), //ordering
            InvenTree::OptionalParam<QString>() //search
            );
    }
}

void WizardPagePartDetails::checkCategorySelection(InvenTree::Category &category)
{
    if (category.getDefaultLocation() != 0) {
        if (m_wizard->m_selectedPart.quantity() > 0) {
            connect(m_wizard->stockApi(),
                    &InvenTree::StockApi::stockLocationRetrieveSignal, this,
                    [=](InvenTree::Location location) {
                        disconnect(m_wizard->stockApi(), nullptr, this, nullptr);
                        m_wizard->m_stockAndPricingPage->m_stockLines.first()
                            ->selectLocation(location);
                    });

            connect(
                m_wizard->stockApi(),
                &InvenTree::StockApi::stockLocationRetrieveSignalError, this,
                [=]() { disconnect(m_wizard->stockApi(), nullptr, this, nullptr); });
            m_wizard->stockApi()->stockLocationRetrieve(
                category.getDefaultLocation());
        }
    }
}

void WizardPagePartDetails::categoryDetailsRetrived(InvenTree::Category category)
{
    ui->labelInvenTreeSelectedCategory->setText(category.getName());
    ui->labelInvenTreeSelectedCategory->setToolTip(category.getPathstring());
    m_invenTreeTargetCategoryPk = category.getPk();
    disconnect(m_wizard->partApi(),
               &InvenTree::PartApi::partCategoryRetrieveSignal, this,
               &WizardPagePartDetails::categoryDetailsRetrived);
    checkCategorySelection(category);
    emit completeChanged();
}

void WizardPagePartDetails::saveMapping()
{
    if (ui->checkBoxSaveCategoryBinding->isChecked() && m_invenTreeTargetCategoryPk != -1) {
        const QString supplierCategoryKey = supplierCategoryMapKey(m_wizard->m_selectedPart);
        auto categoryMap = findSavedCategoryBinding(m_wizard->currentSupplierDbId(), m_wizard->m_selectedPart);
        if (categoryMap) {
            categoryMap->setInventree_category_id(m_invenTreeTargetCategoryPk);
            categoryMap->setSupplier_category_id(supplierCategoryKey);
            categoryMap->setSupplier_category_name(ui->labelSupplierCategory->text());
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
            map->setSupplier_category_id(supplierCategoryKey);
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

QString WizardPagePartDetails::summary() const
{
    QString ret;
    ret.append(tr("<b>Create new part:</b>"
                  "<ul>"
                  "<li>IPN: %1</li>"
                  "<li>Description: %2</li>"
                  "<li>Category: %3</li>"
                  "<li>Keywords: %4</li>"
                  "<li>Unit: %5</li>"
                  "</ul>")
                   .arg(ui->lineEditIPN->text(),
                        ui->textEditDescription->toPlainText(),
                        ui->labelInvenTreeSelectedCategory->toolTip(),
                        ui->lineEditKeyWords->text(),
                        ui->lineEditUnit->text()));
    return ret;
}

void WizardPagePartDetails::on_toolButtonEditInventTreeCategory_clicked()
{
    auto dlg = new DialogSelectInvenTreeCategory(m_wizard->partApi(), m_invenTreeTargetCategoryPk, this);
    connect(dlg, &DialogSelectInvenTreeCategory::categorySelected, this, [=](InvenTree::Category category) {
        ui->labelInvenTreeSelectedCategory->setText(category.getName());
        ui->labelInvenTreeSelectedCategory->setToolTip(category.getPathstring());
        m_invenTreeTargetCategoryPk = category.getPk();
        checkCategorySelection(category);
        emit completeChanged();
        dlg->close();
    });
    dlg->show();
}

bool WizardPagePartDetails::validatePage()
{
    return m_invenTreeTargetCategoryPk != -1 || m_wizard->m_selectedPart.existingPk() != 0;
}

bool WizardPagePartDetails::isComplete() const
{
    return m_invenTreeTargetCategoryPk != -1 || m_wizard->m_selectedPart.existingPk() != 0;
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
