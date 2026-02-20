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

    auto a = m_part->attachmentAt(index.row());
    if (role == Qt::DisplayRole) {
        switch (static_cast<Columns>(index.column())) {
        case Col_FileName:
            if (a->hasOtherLang)
                return a->filename() + " (" + a->language + ")";
            return a->filename();
        case Col_Size: {
            QLocale locale;
            return  locale.formattedDataSize(a->sizeInBytes);
        }
        case Col_Action:
            return a->upload ? tr("Save") : tr("Skip");
        case Col_Invalid:
            break;
        case Col_Comment:
            return a->comment;
        }
    } else if (role == Qt::EditRole) {
        if (index.column() == Col_Action) {
            return a->upload ? "save" : "skip";
        } else if (index.row() == Col_Comment) {
            return a->comment;
        }
    } else if (role == Role_Url) {
        return a->url;
    } else if (role == Role_Size) {
        return a->sizeInBytes;
    } else if (role == Role_Save) {
        return a->upload;
    }
    return QVariant();
}

bool SupplierAttachmentsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (index.column() == Col_Action) {
            auto a = m_part->attachmentAt(index.row());
            a->upload = value.toBool();
            emit dataChanged(index, index, QList<int>({Qt::DisplayRole, Qt::EditRole}));
            return true;
        } else if (index.column() == Col_Comment) {
            m_part->setAttachmentComment(index.row(), value.toString());
            emit dataChanged(index, index, QList<int>({Qt::DisplayRole, Qt::EditRole}));
            return true;
        }
    }
    return false;
}

Qt::ItemFlags SupplierAttachmentsModel::flags(const QModelIndex &index) const
{
    auto ret = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == Col_Action || index.column() == Col_Comment)
        ret |= Qt::ItemIsEditable;
    return ret;
}

void SupplierAttachmentsModel::setPart(SupplierPart *part)
{
    beginResetModel();
    m_part = part;
    endResetModel();
}

bool SupplierAttachmentsModel::hasSaveable() const
{
    for (const auto &a : m_part->attachments())
        if (a.upload)
            return true;
    return false;
}

