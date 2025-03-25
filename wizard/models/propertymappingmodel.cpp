#include "propertymappingmodel.h"

PropertyMappingModel::PropertyMappingModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

void PropertyMappingModel::setPart(::SupplierPart *part)
{
    beginResetModel();
    m_lines.clear();
    for (const auto &prop : part->properties()) {
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
    emit dataChanged(index(row, Value), index(row, Unit));
}

void PropertyMappingModel::setParameterToSave(int row, bool save)
{
    m_lines[row].setValueAction(save ? SaveValue : SkipValue);
    emit dataChanged(index(row, Action), index(row, Action));
}

QString PropertyMappingModel::propertyName(int row) const
{
    return m_lines[row].supplierPartProperty.m_name;
}

void PropertyMappingModel::setTemplateTargetCategory(int row, bool save, int pk, const QString &name)
{
    m_lines[row].setTemplateAction(save ? SaveTemplateToCategory : NoTemplate, pk, name);
    emit dataChanged(index(row, Action), index(row, Action));
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
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (static_cast<Columns>(index.column())) {
        case Name:
            return m_lines.at(index.row()).supplierPartProperty.m_name;
        case Value:
            return m_lines.at(index.row()).supplierPartProperty.m_value;
        case Action:
            return m_lines.at(index.row()).actionText();
        case Col_Invalid:
            break;
        case Unit:
            return m_lines.at(index.row()).supplierPartProperty.m_unit;
        }
    } else if (role == SaveRole) {
        return m_lines.at(index.row()).valueAction() == SaveValue;
    } else if (role == TemplateCategoryPkRole) {
        return m_lines.at(index.row()).templateTargetCategoryPk();
    } else if (role == TemplateCategoryNameRole) {
        return m_lines.at(index.row()).templateTargetCategoryName();
    }
    return QVariant();
}

bool PropertyMappingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        switch (static_cast<PropertyMappingModel::Columns>(index.column())) {
        case Name:
            m_lines[index.row()].supplierPartProperty.m_name = value.toString();
            break;
        case Value:
            m_lines[index.row()].supplierPartProperty.m_value = value.toString();
            break;
        case Unit:
            m_lines[index.row()].supplierPartProperty.m_unit = value.toString();
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}

QVariant PropertyMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<Columns>(section)) {
        case Name:
            return tr("Name");
        case Value:
            return tr("Value");
        case Unit:
            return tr("Unit");
        case Action:
            return tr("Action");
        case Col_Invalid:
            break;
        }
    }
    return QVariant();
}

Qt::ItemFlags PropertyMappingModel::flags(const QModelIndex &index) const
{
    if (index.column() == Name || index.column() == Value || index.column() == Unit)
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QString PropertyMappingLine::actionText() const
{
    switch (m_valueAction) {
    case PropertyMappingModel::SkipValue:
        return QObject::tr("Do not save this parameter");
    case PropertyMappingModel::SaveValue:
        if (m_templateAction == PropertyMappingModel::NoTemplate) {
            return QObject::tr("Save as parameter");
        } else {
            auto catName = m_templateTargetCategoryName;
            if (m_templateTargetCategoryPk == 0)
                catName = QObject::tr("global");
            return QObject::tr("Save as parameter, create parameter template to the %1 category").arg(catName);
        }
    }
    return QString();
}

PropertyMappingModel::TemplateAction PropertyMappingLine::templateAction() const
{
    return m_templateAction;
}

void PropertyMappingLine::setTemplateAction(PropertyMappingModel::TemplateAction newTemplateAction, int pk, const QString &name)
{
    if (newTemplateAction == PropertyMappingModel::SaveTemplateToCategory && m_valueAction != PropertyMappingModel::SaveValue)
        m_valueAction = PropertyMappingModel::SaveValue;

    m_templateAction = newTemplateAction;
    m_templateTargetCategoryPk = pk;
    m_templateTargetCategoryName = name;
}

int PropertyMappingLine::templateTargetCategoryPk() const
{
    return m_templateTargetCategoryPk;
}

QString PropertyMappingLine::templateTargetCategoryName() const
{
    return m_templateTargetCategoryName;
}

PropertyMappingModel::ValueAction PropertyMappingLine::valueAction() const
{
    return m_valueAction;
}

void PropertyMappingLine::setValueAction(PropertyMappingModel::ValueAction newValueAction)
{
    m_valueAction = newValueAction;
    if (newValueAction != PropertyMappingModel::SaveValue) {
        m_templateAction = PropertyMappingModel::NoTemplate;
        m_templateTargetCategoryPk = -1;
        m_templateTargetCategoryName.clear();
    }
}
