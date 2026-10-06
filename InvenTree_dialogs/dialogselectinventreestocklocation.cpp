#include "dialogselectinventreestocklocation.h"
#include "ui_dialogselectinventreestocklocation.h"

#include <QMessageBox>

DialogSelectInvenTreeStockLocation::DialogSelectInvenTreeStockLocation(InvenTree::StockApi *api, QWidget *parent,
                                                                       int selectedPk)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeStockLocation)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    m_model = new InvenTreeStockLocationModel(api, this);
    ui->treeViewStockLocations->setModel(m_model);
    connect(m_model, &InvenTreeStockLocationModel::requestExpand, ui->treeViewStockLocations,
            [this](const QModelIndex &index) { ui->treeViewStockLocations->setExpanded(index, true); });
    connect(m_model, &InvenTreeStockLocationModel::requestSelection, ui->treeViewStockLocations,
            [this](const QModelIndex &index) {
                ui->treeViewStockLocations->setCurrentIndex(index);
                ui->treeViewStockLocations->scrollTo(index);
            });
    connect(m_model, &InvenTreeStockLocationModel::dataFetched, this,
            [this] { ui->labelError->clear(); });

    m_settings.beginGroup("DialogSelectInvenTreeStockLocation");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();

    connect(api, &InvenTree::StockApi::stockLocationListSignalError, this,
            [this](InvenTree::PaginatedLocationList, QNetworkReply::NetworkError, const QString &error) {
                ui->labelError->setText(error);
            });
    connect(ui->lineEditFilterSelector, &InventreeStockLocationPathLineEdit::locationSelected,
            this, [this](quint32 pk) { m_model->setSelectedPk(pk); });

    if (selectedPk != 0)
        m_model->setSelectedPk(selectedPk);
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
                               m_model->data(index, InvenTreeStockLocationModel::LocationPathRole).toString());
}

void DialogSelectInvenTreeStockLocation::setFilterForNonStructural(bool newFilterForNonStructural)
{
    m_filterForNonStructural = newFilterForNonStructural;
}
