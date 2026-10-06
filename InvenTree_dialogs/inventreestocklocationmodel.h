#pragma once

#include "gen_src/client/StockApi.h"

#include <QAbstractItemModel>

class InvenTreeStockLocationItem : public QObject
{
    Q_OBJECT
public:
    InvenTreeStockLocationItem(InvenTree::StockApi *api, InvenTree::Location location, InvenTreeStockLocationItem *parentItem = nullptr);
    InvenTree::Location locationData;

    void appendChild(InvenTreeStockLocationItem *child);

    InvenTreeStockLocationItem *child(int row) {return m_childItems.at(row);}
    InvenTreeStockLocationItem *childByPk(int pk);
    int childCount() const {return m_childItems.count();}
    int row() const;
    InvenTreeStockLocationItem *parentItem() {return m_parentItem;}

    void fetchChilds();
    bool areChildsFetched() const;

    QModelIndex index() const;
    void setIndex(const QModelIndex &newIndex);

    QModelIndex findIndexOfLocation(int pk, bool *found = nullptr) const;
    bool hasChildPk(int pk) const;
    void setParentItem(InvenTreeStockLocationItem *newParentItem);

private:
    QList<InvenTreeStockLocationItem *> m_childItems;
    InvenTreeStockLocationItem *m_parentItem = nullptr;
    InvenTree::StockApi *m_api = nullptr;
    bool m_childsFetched = false;
    bool m_fetchInProgress = false;
    QModelIndex m_index;

private slots:
    void subLocationsReceieved(InvenTree::PaginatedLocationList summary);

signals:
    void childsFetched(int childCount);
};


class InvenTreeStockLocationModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Roles {
        IsStructuralRole = Qt::UserRole + 1,
        PkRole,
        LocationPathRole,
    };
    explicit InvenTreeStockLocationModel(InvenTree::StockApi *api, QObject *parent = nullptr);
    ~InvenTreeStockLocationModel();

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool hasChildren(const QModelIndex &parent) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    void setSelectedPk(int pk);
    void setVisiblePk(int pk) { setSelectedPk(pk); }

private slots:
    void itemsChildsFetched(int childCount);
    void parentLocationFetchedForSelection(InvenTree::Location locationData);

private:
    void populateParentsRecursivelyToTop(int pk);
    void fetchParentLocation(int pk);

    InvenTreeStockLocationItem *m_rootItem = nullptr;
    InvenTree::StockApi *m_api = nullptr;
    bool m_topLevelLocationsFetched = false;
    bool m_populatetreeToSelectedLocation = false;
    int m_preSelectedPk = -1;
    QList<int> m_locationListToPopulate;

signals:
    void dataFetched();
    void requestExpand(const QModelIndex &index);
    void requestSelection(const QModelIndex &index);
};
