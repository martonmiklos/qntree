#pragma once

#include "supplier/supplierpart.h"
#include "supplier/supplierpartproperty.h"

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
        Unit,
        Action,
        Col_Invalid,
    };

    enum ValueAction {
        SkipValue,
        SaveValue
    };

    enum TemplateAction {
        NoTemplate,
        SaveTemplateToCategory,
    };

    enum Roles {
        SaveRole = Qt::UserRole + 1,
        TemplateCategoryPkRole,
        TemplateCategoryNameRole,
    };

    explicit PropertyMappingModel(QObject *parent = nullptr);
    void setPart(::SupplierPart *part);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool rowUnitSplittable(int row) const;
    void splitRowUnit(int row);

    void setParameterToSave(int row, bool save);
    QString propertyName(int row) const;

    void setTemplateTargetCategory(int row, bool save, int pk = -1, const QString &name = QString());

private:
    QList<PropertyMappingLine> m_lines;    
};

class PropertyMappingLine
{
public:
    PropertyMappingLine() = default;
    QString actionText() const;
    SupplierPartProperty supplierPartProperty;

    PropertyMappingModel::ValueAction valueAction() const;
    void setValueAction(PropertyMappingModel::ValueAction newValueAction);

    PropertyMappingModel::TemplateAction templateAction() const;
    void setTemplateAction(PropertyMappingModel::TemplateAction newTemplateAction, int pk = -1, const QString &name = QString());

    int templateTargetCategoryPk() const;
    QString templateTargetCategoryName() const;

private:
    PropertyMappingModel::ValueAction m_valueAction = PropertyMappingModel::SaveValue;
    PropertyMappingModel::TemplateAction m_templateAction = PropertyMappingModel::NoTemplate;

    int m_templateTargetCategoryPk = -1;
    QString m_templateTargetCategoryName;

};
