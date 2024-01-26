#include "inventreepartimportwizard.h"
#include "qmenu.h"
#include "ui_inventreepartimportwizard.h"

#include "wizard/models/propertymappingmodel.h"
#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/wizardpagestockandpricing.h"

#include "InvenTree_dialogs/dialogselectinventreelocation.h"

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
}

InvenTreePartImportWizard::~InvenTreePartImportWizard()
{
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
    if (id == InvenTreeData) {
        // update the unit suffix
        m_stockAndPricingPage->update();
    }
}

void InvenTreePartImportWizard::on_tableViewPropertyMapping_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu = QMenu();

    auto skipAction = contextMenu.addAction(tr("Skip"));
    skipAction->setCheckable(true);


    bool showSplit = true;
    for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows()) {
        if (!m_propertyModel->rowUnitSplittable(row.row())) {
            showSplit = false;
            break;
        }
    }

    QAction *splitAction = nullptr;
    if (showSplit) {
        splitAction = contextMenu.addAction(tr("Split unit"));
        contextMenu.addAction(splitAction);
    }

    auto selectedAction = contextMenu.exec(ui->tableViewPropertyMapping->mapToGlobal(pos));
    if (!selectedAction)
        return;

    if (selectedAction == splitAction) {
        for (auto row : ui->tableViewPropertyMapping->selectionModel()->selectedRows()) {
            m_propertyModel->splitRowUnit(row.row());
        }
    }

}


void InvenTreePartImportWizard::on_tableViewAttachmentMapping_customContextMenuRequested(const QPoint &pos)
{

}


void InvenTreePartImportWizard::on_toolButtonEditInventTreeCategory_clicked()
{
    auto dlg = new DialogSelectInvenTreeLocation(m_api, this);
    connect(dlg, &DialogSelectInvenTreeLocation::categorySelected, this, [=](int pk, const QString &categoryName, const QString &categoryPath) {
        ui->labelInvenTreeCategory->setText(categoryName);
        ui->labelInvenTreeCategory->setToolTip(categoryPath);
        m_invenTreeTargetCategoryPk = pk;
        dlg->close();
    });
    dlg->show();
}

