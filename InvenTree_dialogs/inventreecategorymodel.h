#pragma once

#include "gen_src/client/PartApi.h"
#include <QAbstractItemModel>

class InvenTreeCategoryItem : public QObject
{
    Q_OBJECT
public:
    InvenTreeCategoryItem(InvenTree::PartApi *api, InvenTree::Category category, InvenTreeCategoryItem *parentItem = nullptr);
    InvenTree::Category categoryData;

    void appendChild(InvenTreeCategoryItem *child);

    InvenTreeCategoryItem *child(int row) {return m_childItems.at(row);}
    InvenTreeCategoryItem *childByPk(int pk)
    {
        for (auto child : std::as_const(m_childItems)) {
            if (child->categoryData.getPk() == pk)
                return child;
        }

        return nullptr;
    }

    int childCount() const {return m_childItems.count();}
    int row() const;
    InvenTreeCategoryItem *parentItem() {return m_parentItem;}

    void fetchChilds();
    bool areChildsFetched() const;

    QModelIndex index() const;
    void setIndex(const QModelIndex &newIndex);

    QModelIndex findIndexOfCategory(int pk, bool *found = nullptr) const;

    bool hasChildPk(const int pk) const;

    void setParentItem(InvenTreeCategoryItem *newParentItem);

private:
    QList<InvenTreeCategoryItem *> m_childItems;
    InvenTreeCategoryItem *m_parentItem = nullptr;
    InvenTree::PartApi *m_api = nullptr;
    bool m_childsFetched = false;
    bool m_fetchInProgress = false;
    QModelIndex m_index;

private slots:
    void subCategoriesReceieved(InvenTree::PaginatedCategoryList summary);

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
        CategoryDefaultLocation,
    };
    explicit InvenTreeCategoryModel(InvenTree::PartApi *api, QObject *parent = nullptr);
    ~InvenTreeCategoryModel();

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool hasChildren(const QModelIndex &parent) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    void fetchParentCategory(int pk);

    void setVisiblePk(int pk);

    InvenTree::Category getCategoryByIndex(const QModelIndex &index);

private slots:
    void childItemsFetched(int childCount);
    void parentCategoryFetchedForSelection(InvenTree::Category categoryData);

private:
    InvenTreeCategoryItem *m_rootItem = nullptr;
    InvenTree::PartApi *m_api = nullptr;
    bool m_topLevelCategoriesFetched = false;
    bool m_populatetreeToSelectedCategory = false;

    int m_preSelectedPk = -1;

    void populateParentsRecursivelyToTop(int pk);
    QList<int> m_categoryListToPopulate;

signals:
    void dataFetched();
    void requestExpand(const QModelIndex & index);
    void requestSelection(const QModelIndex & index);
};
