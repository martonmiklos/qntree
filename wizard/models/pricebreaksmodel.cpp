#include "pricebreaksmodel.h"

PricebreaksModel::PricebreaksModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant PricebreaksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Col_Qty:
            return tr("Qty");
        case Col_Price:
            return tr("Price");
        }
    }
    return QVariant();
}

int PricebreaksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (!m_part)
        return 0;
    return m_part->priceRanges().count();
}

int PricebreaksModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Col_Invalid;
}

QVariant PricebreaksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (m_part != nullptr) {
        if (role == Qt::DisplayRole) {
            if (index.column() == Col_Qty) {
                return QString("%1+").arg(m_part->priceRanges().at(index.row()).qtyMin);
            } else if (index.column() == Col_Price) {
                return QString("%1 %2")
                    .arg(m_part->priceRanges().at(index.row()).price)
                    .arg(m_part->priceRanges().at(index.row()).currency);
            }
        }
    }
    return QVariant();
}

void PricebreaksModel::setPart(SupplierPart *newPart)
{
    beginResetModel();
    m_part = newPart;
    endResetModel();
}

qreal PricebreaksModel::getPriceForQuantity(qreal quantity, QString *currency)
{
    for (int i = m_part->priceRanges().count(); i>0; i--) {
        auto pr = m_part->priceRanges().at(i-1);
        if (quantity >= pr.qtyMin) {
            if (currency)
                *currency = pr.currency;
            return pr.price;
        }
    }
    return 0.0;
}
