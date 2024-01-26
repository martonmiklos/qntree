#include "dialogselectinventreelocation.h"
#include "ui_dialogselectinventreelocation.h"

#include <QMessageBox>

DialogSelectInvenTreeLocation::DialogSelectInvenTreeLocation(InvenTree::PartApi *api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeLocation)
{
    ui->setupUi(this);

    m_model = new InvenTreeCategoryModel(api, this);
    ui->treeViewCategory->setModel(m_model);
}

DialogSelectInvenTreeLocation::~DialogSelectInvenTreeLocation()
{
    delete ui;
}

void DialogSelectInvenTreeLocation::on_treeViewCategory_doubleClicked(const QModelIndex &index)
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

void DialogSelectInvenTreeLocation::setFilterForNonStructural(bool newFilterForNonStructural)
{
    m_filterForNonStructural = newFilterForNonStructural;
}
