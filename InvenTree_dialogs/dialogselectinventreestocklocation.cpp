#include "dialogselectinventreestocklocation.h"
#include "ui_dialogselectinventreestocklocation.h"

#include <QMessageBox>

DialogSelectInvenTreeStockLocation::DialogSelectInvenTreeStockLocation(InvenTree::StockApi *api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeStockLocation)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    m_model = new InvenTreeStockLocationModel(api, this);
    ui->treeViewStockLocations->setModel(m_model);

    m_settings.beginGroup("DialogSelectInvenTreeStockLocation");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();
}

DialogSelectInvenTreeStockLocation::~DialogSelectInvenTreeStockLocation()
{
    m_settings.beginGroup("DialogSelectInvenTreeStockLocation");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();
    delete ui;
}

void DialogSelectInvenTreeStockLocation::on_treeViewStockLocations_doubleClicked(const QModelIndex &index)
{
    if (m_filterForNonStructural && m_model->data(index, InvenTreeStockLocationModel::IsStructuralRole).toBool()) {
        QMessageBox::warning(this, tr("Unable to select"), tr("The %1 location is structural and cannot be selected.\n"
                                                              "Please select an another non-structural location")
                                                               .arg(m_model->data(index, Qt::DisplayRole).toString()));
        return;
    }
    emit stockLocationSelected(m_model->data(index, InvenTreeStockLocationModel::PkRole).toInt(),
                          m_model->data(index, Qt::DisplayRole).toString(),
                               m_model->data(index, InvenTreeStockLocationModel::LocationPathRole).toString());}

void DialogSelectInvenTreeStockLocation::setFilterForNonStructural(bool newFilterForNonStructural)
{
    m_filterForNonStructural = newFilterForNonStructural;
}

