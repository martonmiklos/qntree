#pragma once

#include "gen_src/client/PartApi.h"
#include <QAbstractItemModel>

class PartListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        Col_IPN,
        Col_Name,
        Col_Description,
        Col_Invalid
    };

    explicit PartListModel(InvenTree::PartApi *api, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    InvenTree::Part partAt(int index);

    void setFilter(const QString &filterValue);

private:
    InvenTree::PartApi *m_api = nullptr;
    QList<InvenTree::Part> m_parts;
    quint32 m_offset = 0;

private slots:
    void partListReceived(InvenTree::PaginatedPartList summary);
};
