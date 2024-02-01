#include "inventreepartimportwizard.h"
#include "qmenu.h"
#include "ui_inventreepartimportwizard.h"

#include "wizard/models/propertymappingmodel.h"
#include "wizard/wizardpagesupplierdataenter.h"
#include "wizard/wizardpagestockandpricing.h"
#include "wizard/models/supplierattachmentactiondelegate.h"

#include "InvenTree_dialogs/dialogselectinventreecategory.h"

InvenTreePartImportWizard::InvenTreePartImportWizard(InvenTree::PartApi *api,
                                                     InvenTree::StockApi *stockApi,
                                                     QWidget *parent)
    : QWizard(parent),
    ui(new Ui::InvenTreePartImportWizard),
    m_partApi(api)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_startPage = new WizardPageSupplierDataEnter(this);
    setPage(1, m_startPage);

    m_stockAndPricingPage = new WizardPageStockAndPricing(stockApi, this);
    setPage(6, m_stockAndPricingPage);
    m_propertyModel = new PropertyMappingModel(this);
    ui->tableViewPropertyMapping->setModel(m_propertyModel);

    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Name, QHeaderView::ResizeToContents);
    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Value, QHeaderView::ResizeToContents);
    ui->tableViewPropertyMapping->horizontalHeader()->setSectionResizeMode(PropertyMappingModel::Columns::Action, QHeaderView::Stretch);

    m_settings.beginGroup("InvenTreePartImportWizard");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();

    m_attachmentsModel = new SupplierAttachmentsModel(this);
    ui->tableViewAttachmentMapping->setModel(m_attachmentsModel);

    ui->tableViewAttachmentMapping->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableViewAttachmentMapping->setItemDelegateForColumn(SupplierAttachmentsModel::Col_Action, new SupplierAttachmentActionDelegate(ui->tableViewAttachmentMapping));

    m_networkAccessManager = new QNetworkAccessManager(this);
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
    ui->labelSupplierCategory->setText(m_selectedPart.categoryName());
    ui->textEditDescription->setText(m_selectedPart.description());
    ui->lineEditUnit->setText(m_selectedPart.unit());
    
    m_propertyModel->setPart(&m_selectedPart);
    m_attachmentsModel->setPart(&m_selectedPart);
    m_stockAndPricingPage->setPart(&m_selectedPart);

    m_stockAndPricingPage->update();

    ui->labelPartImage->setPixmap(QPixmap::fromImage(m_selectedPart.image()));
}

InvenTreePartImportWizardPage::InvenTreePartImportWizardPage(InvenTreePartImportWizard *parent) :
    QWizardPage(parent),
    m_wizard(parent)
{

}

void InvenTreePartImportWizard::on_InvenTreePartImportWizard_currentIdChanged(int id)
{
    if (id == ParameterMapping) {
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
            auto dlg = new DialogSelectInvenTreeCategory(m_partApi, 0, this);
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
    auto dlg = new DialogSelectInvenTreeCategory(m_partApi, m_invenTreeTargetCategoryPk, this);
    connect(dlg, &DialogSelectInvenTreeCategory::categorySelected, this, [=](int pk, const QString &categoryName, const QString &categoryPath) {
        ui->labelInvenTreeCategory->setText(categoryName);
        ui->labelInvenTreeCategory->setToolTip(categoryPath);
        m_invenTreeTargetCategoryPk = pk;
        dlg->close();
    });
    dlg->show();
}


void InvenTreePartImportWizard::on_labelPartImage_customContextMenuRequested(const QPoint &pos)
{
    // TOOD add a right click menu to change the product image
}


void InvenTreePartImportWizard::on_tableViewAttachmentMapping_clicked(const QModelIndex &index)
{
    if (index.column() == SupplierAttachmentsModel::Col_FileName) {
        auto fn = m_attachmentsModel->data(m_attachmentsModel->index(index.row(), SupplierAttachmentsModel::Col_FileName), Qt::DisplayRole).toString();
        fn = QDir::tempPath() + QDir::separator() + fn;
        QFileInfo fi(fn);
        if (fi.exists() && fi.size() == m_attachmentsModel->data(index, SupplierAttachmentsModel::Role_Size)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
        } else {
            QNetworkRequest req;
            req.setUrl(m_attachmentsModel->data(index, SupplierAttachmentsModel::Role_Url).toUrl());
            req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
            // TODO make user agent settable
            req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
            auto reply = m_networkAccessManager->get(req);
            connect(reply, &QNetworkReply::finished, this, [=]() {
                QFile file(fn);
                if (file.open(QFile::WriteOnly)) {
                    file.write(reply->readAll());
                }
                file.close();
                QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
            });
        }
    }
}

