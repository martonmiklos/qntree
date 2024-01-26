#include "propertymappingmodel.h"

PropertyMappingModel::PropertyMappingModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

void PropertyMappingModel::loadFromSupplierPart(const SupplierPart &part)
{
    beginResetModel();
    m_lines.clear();
    for (const auto &prop : part.properties()) {
        PropertyMappingLine line;
        line.supplierPartProperty = prop;
        m_lines.append(line);
    }
    endResetModel();
}

bool PropertyMappingModel::rowUnitSplittable(int row) const
{
    if (row < m_lines.count())
        return m_lines.at(row).supplierPartProperty.splittable();
    return false;
}

void PropertyMappingModel::splitRowUnit(int row)
{
    m_lines[row].supplierPartProperty.splitUnit();
    emit dataChanged(index(row, Value), index(row, Value));
}

int PropertyMappingModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_lines.count();
}

int PropertyMappingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return Col_Invalid;
}

QVariant PropertyMappingModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (static_cast<Columns>(index.column())) {
        case Name:
            return m_lines.at(index.row()).supplierPartProperty.m_name;
        case Value:
            return m_lines.at(index.row()).supplierPartProperty.valueString();
        case Action:
            return m_lines.at(index.row()).actionText();
        case Col_Invalid:
            break;
        }
    }
    return QVariant();
}

QVariant PropertyMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<Columns>(section)) {
        case Name:
            return tr("Name");
        case Value:
            return tr("Value");
        case Action:
            return tr("Action");
        case Col_Invalid:
            break;
        }
    }
    return QVariant();
}

QString PropertyMappingLine::actionText() const
{
    return "foo";
}
