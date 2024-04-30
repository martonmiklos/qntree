#include "wizardpagepartparameters.h"
#include "InvenTree_dialogs/dialogselectinventreecategory.h"
#include "inventreepartimportwizard.h"
#include "qmenu.h"
#include "ui_wizardpagepartparameters.h"
#include "wizard/models/propertymappingmodel.h"

WizardPagePartParameters::WizardPagePartParameters(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPagePartParameters)
{
    ui->setupUi(this);

    m_propertyModel = new PropertyMappingModel(this);
    ui->tableViewPropertyMapping->setModel(m_propertyModel);

    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Name, QHeaderView::ResizeToContents);
    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Value, QHeaderView::ResizeToContents);
    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Action, QHeaderView::Stretch);
}

WizardPagePartParameters::~WizardPagePartParameters()
{
    delete ui;
}

void WizardPagePartParameters::setSelectedPart(SupplierPart *newSelectedPart)
{
    InvenTreePartImportWizardPage::setSelectedPart(newSelectedPart);
    m_propertyModel->setPart(newSelectedPart);
}

void WizardPagePartParameters::on_tableViewPropertyMapping_customContextMenuRequested(const QPoint &pos)
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
            auto dlg = new DialogSelectInvenTreeCategory(m_wizard->partApi(), 0, this);
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
