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
        Col_Comment,
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
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setPart(::SupplierPart *part);

private:
    QList<const SupplierAttachment*> m_attachmentsToSave;
    ::SupplierPart *m_part = nullptr;


};
