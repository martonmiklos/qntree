#pragma once

#include <QDialog>
#include <QSettings>

#include "InvenTree_dialogs/inventreestocklocationmodel.h"

namespace Ui {
class DialogSelectInvenTreeStockLocation;
}

class DialogSelectInvenTreeStockLocation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectInvenTreeStockLocation(InvenTree::StockApi *api, QWidget *parent = nullptr);
    ~DialogSelectInvenTreeStockLocation();

    void setFilterForNonStructural(bool newFilterForNonStructural);

private slots:
    void on_treeViewStockLocations_doubleClicked(const QModelIndex &index);

signals:
    void stockLocationSelected(int pk, const QString &locationName, const QString &locationPath);

private:
    Ui::DialogSelectInvenTreeStockLocation *ui;
    bool m_filterForNonStructural = true;

    InvenTreeStockLocationModel *m_model = nullptr;
    QSettings m_settings;
};
