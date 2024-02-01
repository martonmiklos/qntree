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
            return m_attachmentsToSave.indexOf(&(m_part->attachments().at(index.row()))) != -1 ? tr("Save") : tr("Skip");
        case Col_Invalid:
            break;
        }
    } else if (role == Qt::EditRole && index.column() == Col_Action) {
        return m_attachmentsToSave.indexOf(&(m_part->attachments().at(index.row()))) != -1 ? "save" : "skip";
    } else if (role == Role_Url) {
        return m_part->attachments().at(index.row()).url;
    } else if (role == Role_Size) {
        return m_part->attachments().at(index.row()).sizeInBytes;
    }
    return QVariant();
}

bool SupplierAttachmentsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole && index.column() == Col_Action) {
        if (value.toString() == "save") {
            if (m_attachmentsToSave.indexOf(&m_part->attachments().at(index.row())) == -1) {
                m_attachmentsToSave.append(&m_part->attachments().at(index.row()));
            }
        } else {
            m_attachmentsToSave.removeAll(&m_part->attachments().at(index.row()));
        }
        emit dataChanged(index, index, QList<int>({Qt::DisplayRole, Qt::EditRole}));
        return true;
    }
    return false;
}

Qt::ItemFlags SupplierAttachmentsModel::flags(const QModelIndex &index) const
{
    auto ret = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == Col_Action)
        ret |= Qt::ItemIsEditable;
    return ret;
}

void SupplierAttachmentsModel::setPart(SupplierPart *part)
{
    beginResetModel();
    m_attachmentsToSave.clear();
    m_part = part;
    endResetModel();
}

