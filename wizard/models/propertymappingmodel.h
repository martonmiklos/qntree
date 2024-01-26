#pragma once

#include "supplier/supplierpart.h"
#include <QAbstractTableModel>
#include <QObject>


class PropertyMappingLine;

class PropertyMappingModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        Name,
        Value,
        Action,
        Col_Invalid,
    };

    enum ValueAction {
        Skip,
        Save
    };

    enum TemplateAction {
        NoTemplate,
        SaveTemplateToCategory,
    };

    explicit PropertyMappingModel(QObject *parent = nullptr);
    void loadFromSupplierPart(const SupplierPart &part);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool rowUnitSplittable(int row) const;
    void splitRowUnit(int row);

private:
    QList<PropertyMappingLine> m_lines;
};

class PropertyMappingLine
{
public:
    PropertyMappingLine() = default;
    QString actionText() const;
    SupplierPartProperty supplierPartProperty;
    int targetCategoryPk = -1;
    PropertyMappingModel::ValueAction m_valueAction = PropertyMappingModel::Save;
    PropertyMappingModel::TemplateAction m_templateAction = PropertyMappingModel::SaveTemplateToCategory;
};
