#pragma once

#include "gen_src/client/PartApi.h"
#include <QAbstractItemModel>

class InvenTreeCategoryItem : public QObject
{
    Q_OBJECT
public:
    InvenTreeCategoryItem(InvenTree::PartApi *api, InvenTree::Category category, InvenTreeCategoryItem *parentItem = nullptr);
    InvenTree::Category categoryData;

    void appendChild(InvenTreeCategoryItem *child) {m_childItems.append(child);}

    InvenTreeCategoryItem *child(int row) {return m_childItems.at(row);}
    int childCount() const {return m_childItems.count();}
    int row() const;
    InvenTreeCategoryItem *parentItem() {return m_parentItem;}

    void fetchChilds();
    bool areChildsFetched() const;

    QModelIndex index() const;
    void setIndex(const QModelIndex &newIndex);

    QModelIndex findIndexOfCategory(int pk, bool *found = nullptr) const;

private:
    QList<InvenTreeCategoryItem *> m_childItems;
    InvenTreeCategoryItem *m_parentItem = nullptr;
    InvenTree::PartApi *m_api = nullptr;
    bool m_childsFetched = false;
    bool m_fetchInProgress = false;
    QModelIndex m_index;

private slots:
    void subCategoriesReceieved(InvenTree::PaginatedCategoryList summary);
    void subCategoriesReceieveError(InvenTree::PaginatedCategoryTreeList summary, QNetworkReply::NetworkError error_type, const QString &error_str);

signals:
    void childsFetched(int childCount);
};

class InvenTreeCategoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Roles {
        IsStructuralRole = Qt::UserRole + 1,
        PkRole,
        CategoryPathRole,
    };
    explicit InvenTreeCategoryModel(InvenTree::PartApi *api, QObject *parent = nullptr);
    ~InvenTreeCategoryModel();

    void addCategory(int pk);

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

    void addPk(int pk);

    void setVisiblePk(int pk);

private slots:
    void itemsChildsFetched(int childCount);

private:
    InvenTreeCategoryItem *m_rootItem = nullptr;
};
