#include "dialogselectinventreecategory.h"
#include "ui_dialogselectinventreecategory.h"

#include <QMessageBox>

DialogSelectInvenTreeCategory::DialogSelectInvenTreeCategory(InvenTree::PartApi *api, int selectedPk, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeCategory)
{
    ui->setupUi(this);

    m_model = new InvenTreeCategoryModel(api, this);
    m_model->setVisiblePk(selectedPk);
    ui->treeViewCategory->setModel(m_model);

    m_settings.beginGroup("DialogSelectInvenTreeCategory");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();
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
                                                              "Please select an another non-structural category"));
        return;
    }
    emit categorySelected(m_model->data(index, InvenTreeCategoryModel::PkRole).toInt(),
                          m_model->data(index, Qt::DisplayRole).toString(),
                          m_model->data(index, InvenTreeCategoryModel::CategoryPathRole).toString());
}

void DialogSelectInvenTreeCategory::setFilterForNonStructural(bool newFilterForNonStructural)
{
    m_filterForNonStructural = newFilterForNonStructural;
}
