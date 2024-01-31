#include "supplierattachmentsmodel.h"

#include "supplier/supplierpart.h"

#include <QLocale>

SupplierAttachmentsModel::SupplierAttachmentsModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant SupplierAttachmentsModel::headerData(int section,
                                              Qt::Orientation orientation,
                                              int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (static_cast<Columns>(section)) {
        case Col_FileName:
            return tr("Attachment name");
        case Col_Size:
            return tr("Size");
        case Col_Action:
            return tr("Action");
        default:
            break;
        }
    }
    return QVariant();
}

int SupplierAttachmentsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (!m_part)
        return 0;
    return m_part->attachments().count();
}

int SupplierAttachmentsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Col_Invalid;
}

QVariant SupplierAttachmentsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!m_part)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (static_cast<Columns>(index.column())) {
        case Col_FileName:
            return m_part->attachments().at(index.row()).url.fileName();
        case Col_Size: {
            QLocale locale;
            return  locale.formattedDataSize(m_part->attachments().at(index.row()).sizeInBytes);
        }
        case Col_Action:
            //return m_actionMap.find(&(m_part->attachments().at(index.row()))) ? tr("Save") : tr("Skip");
        case Col_Invalid:
            break;
        }
    } else if (role == Role_Url) {
        return m_part->attachments().at(index.row()).url;
    } else if (role == Role_Size) {
        return m_part->attachments().at(index.row()).sizeInBytes;
    }
    return QVariant();
}

void SupplierAttachmentsModel::setPart(SupplierPart *part)
{
    beginResetModel();
    m_actionMap.clear();
    m_part = part;
    endResetModel();
}

