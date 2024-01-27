#include "inventreepartimportwizard.h"
#include "qmenu.h"
#include "ui_inventreepartimportwizard.h"

#include "wizard/models/propertymappingmodel.h"
#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/wizardpagestockandpricing.h"

#include "InvenTree_dialogs/dialogselectinventreecategory.h"

InvenTreePartImportWizard::InvenTreePartImportWizard(InvenTree::PartApi *api, QWidget *parent)
    : QWizard(parent),
    ui(new Ui::InvenTreePartImportWizard),
    m_api(api)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_startPage = new WizardPageSupplierDataEnter(this);
    setPage(1, m_startPage);

    m_stockAndPricingPage = new WizardPageStockAndPricing(this);
    setPage(6, m_stockAndPricingPage);
    m_propertyModel = new PropertyMappingModel(this);
    ui->tableViewPropertyMapping->setModel(m_propertyModel);

    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Name, QHeaderView::ResizeToContents);
    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Value, QHeaderView::ResizeToContents);
    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Action, QHeaderView::Stretch);

    m_settings.beginGroup("InvenTreePartImportWizard");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();
}

InvenTreePartImportWizard::~InvenTreePartImportWizard()
{
    m_settings.beginGroup("InvenTreePartImportWizard");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();
    delete ui;
}

void InvenTreePartImportWizard::setSelectedPart(SupplierPart &part)
{
    m_selectedPart = part;

    ui->lineEditIPN->setText(m_selectedPart.name());
    ui->lineEditPartName->setText(m_selectedPart.name());
    ui->labelSupplierCategory->setText(m_selectedPart.categoryName());
    ui->textEditDescription->setText(m_selectedPart.description());
    ui->lineEditUnit->setText(m_selectedPart.unit());

    m_propertyModel->loadFromSupplierPart(m_selectedPart);
    m_stockAndPricingPage->update();
}

InvenTreePartImportWizardPage::InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent) :
    QWizardPage(parent),
    m_wizard(parent)
{

}

void InvenTreePartImportWizard::on_InvenTreePartImportWizard_currentIdChanged(int id)
{
    if (id == PropertyMapping) {
        // update the unit suffix
        m_stockAndPricingPage->update();
    }
}

void InvenTreePartImportWizard::on_tableViewPropertyMapping_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu = QMenu();
    bool showSplit = true;
    bool allToSave = true;
    int templateCategoryId = -1;
    bool allShareSameTemplateCategoryId = true;
    QString templateCategoryName;
    for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows()) {
        if (!m_propertyModel->rowUnitSplittable(row.row()))
            showSplit = false;

        if (!m_propertyModel->data(row, PropertyMappingModel::SaveRole).toBool())
            allToSave = false;

        if (templateCategoryId == -1) {
            templateCategoryId = m_propertyModel->data(row, PropertyMappingModel::TemplateCategoryPkRole).toInt();
            templateCategoryName = m_propertyModel->data(row, PropertyMappingModel::TemplateCategoryNameRole).toString();
        } else {
            if (templateCategoryId != m_propertyModel->data(row, PropertyMappingModel::TemplateCategoryPkRole).toInt())
                allShareSameTemplateCategoryId = false;
        }

    }

    QAction *splitAction = nullptr;
    if (showSplit) {
        splitAction = contextMenu.addAction(tr("Split unit"));
        contextMenu.addAction(splitAction);
    }

    QAction *saveAction = contextMenu.addAction(tr("Save as parameter"));
    saveAction->setCheckable(true);
    saveAction->setChecked(allToSave);

    QAction *templateAction = nullptr;
    if (allToSave) {
        auto templateMenu = contextMenu.addMenu(tr("Template creation"));
        QString templateActionString;
        if (allShareSameTemplateCategoryId && templateCategoryId != -1) {
            templateActionString = tr("Unassign template creation from the %1 category").arg(templateCategoryName);
        } else {
            templateActionString = tr("Select category for template creation");
        }
        templateAction = templateMenu->addAction(templateActionString);
    }


    /// Anddd action....
    auto selectedAction = contextMenu.exec(ui->tableViewPropertyMapping->mapToGlobal(pos));
    if (!selectedAction)
        return;

    if (selectedAction == splitAction) {
        for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows())
            m_propertyModel->splitRowUnit(row.row());
    } else if (selectedAction == saveAction) {
        for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows())
            m_propertyModel->setParameterToSave(row.row(), saveAction->isChecked());
    } else if (selectedAction == templateAction) {
        if (allShareSameTemplateCategoryId && templateCategoryId != -1) {
            // unassign from the current category
            for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows())
                m_propertyModel->setTemplateTargetCategory(row.row(), false);
        } else {
            auto dlg = new DialogSelectInvenTreeCategory(m_api, 0, this);
            dlg->setWindowTitle(tr("Select category for the parameter template"));
            connect(dlg, &DialogSelectInvenTreeCategory::categorySelected, this, [=](int pk, const QString &name, const QString &path) {
                Q_UNUSED(path)
                for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows())
                    m_propertyModel->setTemplateTargetCategory(row.row(), true, pk, name);
                dlg->close();
            });
            dlg->show();
        }
    }
}

void InvenTreePartImportWizard::on_tableViewAttachmentMapping_customContextMenuRequested(const QPoint &pos)
{

}

void InvenTreePartImportWizard::on_toolButtonEditInventTreeCategory_clicked()
{
    auto dlg = new DialogSelectInvenTreeCategory(m_api, m_invenTreeTargetCategoryPk, this);
    connect(dlg, &DialogSelectInvenTreeCategory::categorySelected, this, [=](int pk, const QString &categoryName, const QString &categoryPath) {
        ui->labelInvenTreeCategory->setText(categoryName);
        ui->labelInvenTreeCategory->setToolTip(categoryPath);
        m_invenTreeTargetCategoryPk = pk;
        dlg->close();
    });
    dlg->show();
}

