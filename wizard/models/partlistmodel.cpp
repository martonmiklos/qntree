#include "partlistmodel.h"

PartListModel::PartListModel(InvenTree::PartApi *api, QObject *parent)
    : QAbstractTableModel(parent),
    m_api(api)
{

}

int PartListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_parts.count();
}

int PartListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Col_Invalid;
}

QVariant PartListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (static_cast<Columns>(index.column())) {
        case Col_IPN:
            return m_parts.at(index.row()).getIpn();
        case Col_Name:
            return m_parts.at(index.row()).getName();
        case Col_Description:
            return m_parts.at(index.row()).getDescription();
        case Col_Invalid:
            break;
        }
    } else if (role == Qt::ToolTipRole && index.column() == Col_IPN) {
        return m_parts.at(index.row()).getPk();
    }
    return QVariant();
}

QVariant PartListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (static_cast<Columns>(section)) {
        case Col_IPN:
            return tr("IPN");
        case Col_Name:
            return tr("Name");
        case Col_Description:
            return tr("Description");
        case Col_Invalid:
            break;
        }
    }
    return QVariant();
}

InvenTree::Part PartListModel::partAt(int index)
{
    return m_parts.at(index);
}

void PartListModel::setFilter(const QString &filterValue)
{
    beginResetModel();
    connect(m_api, &InvenTree::PartApi::partListSignal, this, &PartListModel::partListReceived);

    m_api->partList(InvenTree::OptionalParam<QString>(), // ipn
                    InvenTree::OptionalParam<QString>(),
                    InvenTree::OptionalParam<bool>(), // active
                    InvenTree::OptionalParam<qint32>(), // ancestor
                    InvenTree::OptionalParam<bool>(), // assembly
                    InvenTree::OptionalParam<bool>(), // bom_valid
                    InvenTree::OptionalParam<bool>(), // component
                    InvenTree::OptionalParam<qint32>(), // convert_from
                    InvenTree::OptionalParam<QDate>(), // created_after
                    InvenTree::OptionalParam<QDate>(), // created_before
                    InvenTree::OptionalParam<qint32>(), // default_location
                    InvenTree::OptionalParam<bool>(), // depleted_stock
                    InvenTree::OptionalParam<qint32>(), // exclude_tree
                    InvenTree::OptionalParam<bool>(), // has_ipn
                    InvenTree::OptionalParam<bool>(), // has_pricing
                    InvenTree::OptionalParam<bool>(), // has_revisions
                    InvenTree::OptionalParam<bool>(), // has_stock
                    InvenTree::OptionalParam<bool>(), // has_units
                    InvenTree::OptionalParam<qint32>(), // in_bom_for
                    InvenTree::OptionalParam<bool>(), // is_revision
                    InvenTree::OptionalParam<bool>(), // is_template
                    InvenTree::OptionalParam<qint32>(25), // limit
                    InvenTree::OptionalParam<bool>(), // locked
                    InvenTree::OptionalParam<bool>(), // low_stock
                    InvenTree::OptionalParam<QString>(), // name_regex
                    InvenTree::OptionalParam<qint32>(m_offset), // offset
                    InvenTree::OptionalParam<QString>(), // ordering
                    InvenTree::OptionalParam<bool>(), // purchaseable
                    InvenTree::OptionalParam<qint32>(), // revision_of
                    InvenTree::OptionalParam<bool>(), // salable
                    InvenTree::OptionalParam<QString>(filterValue), // search
                    InvenTree::OptionalParam<bool>(), // starred
                    InvenTree::OptionalParam<bool>(), // stock_to_build
                    InvenTree::OptionalParam<bool>(), // stocktake
                    InvenTree::OptionalParam<QString>(), // tags_name
                    InvenTree::OptionalParam<QString>(), // tags_slug
                    InvenTree::OptionalParam<bool>(), // trackable
                    InvenTree::OptionalParam<bool>(), // unallocated_stock
                    InvenTree::OptionalParam<qint32>(), // variant_of
                    InvenTree::OptionalParam<bool>() // r_virtual
                    );
}

void PartListModel::partListReceived(InvenTree::PaginatedPartList summary)
{
    m_parts.clear();
    m_parts = summary.getResults();
    disconnect(m_api, nullptr, this, nullptr);
    endResetModel();
}
