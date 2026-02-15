#pragma once

#include "gen_src/client/SupplierPriceBreak.h"
#include "supplier/supplierpart.h"
#include <QAbstractTableModel>

class PricebreaksModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        Col_Qty,
        Col_Price,
        Col_Invalid
    };
    explicit PricebreaksModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setPart(SupplierPart *newPart);

    qreal getPriceForQuantity(qreal quantity, QString *currency = nullptr);
private:
    SupplierPart *m_part = nullptr;
};
