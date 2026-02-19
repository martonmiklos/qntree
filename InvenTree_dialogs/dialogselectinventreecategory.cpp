#include "dialogselectinventreecategory.h"
#include "ui_dialogselectinventreecategory.h"

#include <QMessageBox>

DialogSelectInvenTreeCategory::DialogSelectInvenTreeCategory(InvenTree::PartApi *api, int selectedPk, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeCategory)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    m_model = new InvenTreeCategoryModel(api, this);
    connect(m_model, &InvenTreeCategoryModel::requestExpand, ui->treeViewCategory, [=](const QModelIndex &index) {
        ui->treeViewCategory->setExpanded(index, true);
    });

    connect(m_model, &InvenTreeCategoryModel::requestSelection, ui->treeViewCategory, [=](const QModelIndex &index) {
        ui->treeViewCategory->setCurrentIndex(index);
        ui->treeViewCategory->scrollTo(index);
    });

    ui->treeViewCategory->setModel(m_model);
    connect(m_model, &InvenTreeCategoryModel::dataFetched, this, [=]() {
        ui->labelError->clear();
    });

    m_model->setVisiblePk(selectedPk);

    m_settings.beginGroup("DialogSelectInvenTreeCategory");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();

    connect(api, &InvenTree::PartApi::partCategoryListSignalError, this, [=](InvenTree::PaginatedCategoryList, QNetworkReply::NetworkError, const QString &error_str) {
        ui->labelError->setText(error_str);
    });
}

DialogSelectInvenTreeCategory::~DialogSelectInvenTreeCategory()
{
    m_settings.beginGroup("DialogSelectInvenTreeCategory");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();
    delete ui;
}

void DialogSelectInvenTreeCategory::on_treeViewCategory_doubleClicked(const QModelIndex &index)
{
    if (m_filterForNonStructural && m_model->data(index, InvenTreeCategoryModel::IsStructuralRole).toBool()) {
        QMessageBox::warning(this, tr("Unable to select"), tr("The %1 category is structural and cannot be selected.\n"
                                                              "Please select an another non-structural category").arg(m_model->data(index, Qt::DisplayRole).toString()));
        return;
    }
    emit categorySelected(m_model->getCategoryByIndex(index));
}

void DialogSelectInvenTreeCategory::setFilterForNonStructural(bool newFilterForNonStructural)
{
    m_filterForNonStructural = newFilterForNonStructural;
}
