#pragma once

#include <QAbstractTableModel>

class SupplierPart;
class SupplierAttachment;

class SupplierAttachmentsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        Col_FileName,
        Col_Size,
        Col_Action,
        Col_Invalid
    };

    enum Roles {
        Role_Url = Qt::UserRole + 1,
        Role_Size,
    };

    explicit SupplierAttachmentsModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setPart(::SupplierPart *part);

private:
    QMap<const SupplierAttachment*, bool> m_actionMap;
    ::SupplierPart *m_part = nullptr;
};
